// This is 
//:
// \file

// This is a batch edge linking test program written for CVPR 07
// it takes in a text file with a list of image filenames in each line.
// It computes generic edges and thirdorder edges using fixed parameters that
// were pre-selected from preliminary tests to examine the effect of the parameter settings (sel_examine_params.exe)

#include "sel_batch_compute_contourmap.h"

#include <vul/vul_file.h>
#include <vnl/vnl_math.h>
#include <vnl/vnl_random.h>
#include <iostream>
#include <algorithm>
#include <fstream>
#include <cstdio>

#include <vil/vil_load.h>
#include <vil/vil_image_view.h>
#include <vil/vil_convert.h>

#include <vsol/vsol_polyline_2d.h>
#include <vsol/vsol_polyline_2d_sptr.h>
#include <vsol/vsol_point_2d.h>

#include <pdf1d/pdf1d_calc_mean_var.h>

#include <dbdet/sel/dbdet_edgel.h>
#include <dbdet/edge/dbdet_edgemap_sptr.h>
#include <dbdet/edge/dbdet_edgemap.h>

#include <dbdet/sel/dbdet_sel_base.h>
#include <dbdet/sel/dbdet_sel_sptr.h>
#include <dbdet/sel/dbdet_curve_model.h>
#include <dbdet/sel/dbdet_sel.h>
#include <dbdet/sel/dbdet_sel_utils.h>

#include <dbdet/edge/dbdet_gen_edge_det.h>
#include <dbdet/edge/dbdet_third_order_edge_det.h>

//global random number generator
vnl_random vrand;

int main(int argc, char** argv)
{
  if (argc < 2) {
    std::cout << "USAGE: sel_batch_compute_contourmap <image_list_filename> \n";
    return 1;
  }

  //go over each of the files in the list
  char lineBuffer[1024];
  
  //1)If file open fails, return.
  std::ifstream infp(argv[1], std::ios::in);

  if (!infp){
    std::cout << " Error opening file  " << argv[1] << std::endl;
    return false;
  }

  while (infp.getline(lineBuffer,1024)){

    //ignore comment lines and empty lines
    if (strlen(lineBuffer)<2 || lineBuffer[0]=='#')
      continue;

    //get the image filename
    std::string image_filename(lineBuffer);

    //load image
    vil_image_view<vxl_byte> image  = vil_load(image_filename.c_str());

    std::string out_filename(vul_file::strip_extension(vul_file::basename(image_filename)));

    //compute pixel generic edges
    dbdet_edgemap_sptr edgemap1 = compute_generic_edges(image, 0, out_filename);
    link_edges(edgemap1, 3.0, 7, 0.3, 15, out_filename+"_gen_0");

    //compute generic edges
    dbdet_edgemap_sptr edgemap2 = compute_generic_edges(image, 1, out_filename);
    link_edges(edgemap2, 2.0, 5, 0.2, 15, out_filename+"_gen_1");

    //compute third_order edges
    dbdet_edgemap_sptr edgemap3 = compute_third_order_edges(image, out_filename);
    link_edges(edgemap3, 2.0, 5, 0.1, 5, out_filename+"_TO");
 
  }
  infp.close();

  return 0; 
}

dbdet_edgemap_sptr compute_generic_edges(vil_image_view<vxl_byte>& image, int N, std::string filename)
{
  //compute edges
  double sigma = 1;
  double threshold = 5;
  //int N=1;

  dbdet_edgemap_sptr edgemap = dbdet_detect_generic_edges(image, sigma, threshold, N);

  //save edgemap
  char out_edge_filename[300];
  std::sprintf(out_edge_filename, "%s_gen_%d_%f.edg", filename.c_str(), N, sigma);
  saveEDG(std::string(out_edge_filename), edgemap);

  return edgemap;
}

dbdet_edgemap_sptr compute_third_order_edges(vil_image_view<vxl_byte>& image, std::string filename)
{
  //compute edges
  double sigma = 1;
  double threshold = 5;
  int N=1;

  dbdet_edgemap_sptr edgemap = dbdet_detect_third_order_edges(image, sigma, threshold, N);

  //save edgemap
  char out_edge_filename[300];
  std::sprintf(out_edge_filename, "%s_TO_%d_%f.edg", filename.c_str(), N, sigma);
  saveEDG(std::string(out_edge_filename), edgemap);

  return edgemap;
}

void link_edges(dbdet_edgemap_sptr edgemap, double nrad, unsigned maxN, double dx_sel, double dt_deg, std::string filename)
{
  //parameters
  //double nrad = 2.0;
  //unsigned maxN = 5;
  //double dx_sel=0.3;
  //double dt_deg=30.0;

  double dt_sel = dt_deg*vnl_math::pi/180.0;

  //first we need to clear the edgemap of any curvelets it might have formed before
  edgemap->clear_all_curvelets();

  dbdet_sel_sptr edge_linker = new dbdet_sel<dbdet_ES_curve_model>(edgemap, nrad, dt_sel, dx_sel); 
  edge_linker->build_curvelets_greedy(maxN, true);//extra greedy
  edge_linker->construct_the_link_graph(4, 2);//link quads and higher
  edge_linker->extract_image_contours();
  edge_linker->report_stats();//report stats

  //save linked edges
  char out_cem_filename[500];
  std::sprintf(out_cem_filename, "%s_%f_%f_%d.cem", filename.c_str(), dx_sel, dt_deg, maxN);
  saveCEM(out_cem_filename, edge_linker);
  
}

bool saveEDG(std::string filename, dbdet_edgemap_sptr edgemap)
{
  //1) If file open fails, return.
  std::ofstream outfp(filename.c_str(), std::ios::out);

  if (!outfp){
    std::cout << " Error opening file  " << filename.c_str() << std::endl;
    return false;
  }

  //2) write out the header block
  outfp << "# EDGE_MAP " << std::endl << std::endl;
  outfp << "# Format :  [Pixel_Pos]  Pixel_Dir Pixel_Conf  [Sub_Pixel_Pos] Sub_Pixel_Dir Sub_Pixel_Conf" << std::endl;
  outfp << std::endl;
  outfp << "WIDTH=" << edgemap->width() << std::endl;
  outfp << "HEIGHT=" << edgemap->height() << std::endl;
  outfp << "EDGE_COUNT=" << edgemap->num_edgels  << std::endl;
  outfp << std::endl << std::endl;

  //write out all the edgels
  dbdet_edgemap_const_iter it = edgemap->edge_cells.begin();
  for (; it!=edgemap->edge_cells.end(); it++){
    for (unsigned j=0; j<(*it).size(); j++){
      dbdet_edgel* edgel = (*it)[j];

      double x = edgel->pt.x();
      double y = edgel->pt.y();

      int ix = (int) std::floor(x);
      int iy = (int) std::floor(y);
      
      double idir = edgel->tangent, iconf = edgel->strength, dir= edgel->tangent, conf= edgel->strength;
      
      outfp << "[" << ix << ", " << iy << "]    " << idir << " " << iconf << "   [" << x << ", " << y << "]   " << dir << " " << conf << std::endl;
    }
  }

  outfp.close();

  return true;
}

bool saveCEM (std::string filename, dbdet_sel_sptr edge_linker)
{ 
  // 0) form vsol polylines from the chains
  std::vector< vsol_spatial_object_2d_sptr > image_curves;
  for (unsigned i=0; i<edge_linker->edgel_chains().size(); i++){
    //only keep the longer contours
    if (edge_linker->edgel_chains()[i]->edgels.size() >= 3){
      std::vector<vsol_point_2d_sptr> pts;
      for (unsigned j=0; j<edge_linker->edgel_chains()[i]->edgels.size(); j++)
        pts.push_back(new vsol_point_2d(edge_linker->edgel_chains()[i]->edgels[j]->pt));
      vsol_polyline_2d_sptr new_curve = new vsol_polyline_2d(pts);
      image_curves.push_back(new_curve->cast_to_spatial_object());
    }
  }

  //1)If file open fails, return.
  std::ofstream outfp(filename.c_str(), std::ios::out);

  if (!outfp){
    std::cout << " Error opening file  " << filename.c_str() << std::endl;
    return false;
  }

  std::cout << "Saving " << filename.c_str() << " ..." << std::endl;

  // output header information
   outfp <<"# CONTOUR_EDGE_MAP "<<std::endl;
   outfp <<"# .cem files"<<std::endl;
   outfp <<"#"<<std::endl;
   outfp <<"# Format :"<<std::endl;
   outfp <<"# Each contour block will consist of the following"<<std::endl;
   outfp <<"# [BEGIN CONTOUR]"<<std::endl;
   outfp <<"# EDGE_COUNT=num_of_edges"<<std::endl;
   outfp <<"# [Pixel_Pos]  Pixel_Dir Pixel_Conf  [Sub_Pixel_Pos] Sub_Pixel_Dir Sub_Pixel_Conf "<<std::endl;
   outfp <<"# ..."<<std::endl;
   outfp <<"# ..."<<std::endl;
   outfp <<"# [END CONTOUR]"<<std::endl;

  outfp<<std::endl;

  // Note: this count is currently missing
   outfp <<"CONTOUR_COUNT="<< std::endl;
   outfp <<"TOTAL_EDGE_COUNT="<<std::endl;

  // parse through all the vsol classes and save curve objects only
  for (unsigned int b = 0 ; b < image_curves.size() ; b++ )
  {
    if( image_curves[b]->cast_to_curve())
    {
      if( image_curves[b]->cast_to_curve()->cast_to_polyline() )
      {
        outfp <<"[BEGIN CONTOUR]"<<std::endl;
        outfp <<"EDGE_COUNT="<< image_curves[b]->cast_to_curve()->cast_to_polyline()->size() <<std::endl;
        
        for (unsigned int i=0; i<image_curves[b]->cast_to_curve()->cast_to_polyline()->size();i++)
        {
          vsol_point_2d_sptr pt = image_curves[b]->cast_to_curve()->cast_to_polyline()->vertex(i);
          //output as subpixel contours
          // [%d, %d]\t%lf\t%lf\t[%lf, %lf]\t%lf\t%lf
          outfp <<" [0, 0]  0.0  0.0  [" << pt->x() << ", " << pt->y() << "]  0.0  0.0"  << std::endl;
        }
        outfp <<"[END CONTOUR]"<<std::endl<<std::endl;
      }
    }
  }
  
  //close file
  outfp.close();
  
  return true;
}

