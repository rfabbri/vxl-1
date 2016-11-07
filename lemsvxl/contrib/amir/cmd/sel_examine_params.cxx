// This is 
//:
// \file

#include "sel_examine_params.h"

#include <vnl/vnl_math.h>
#include <vnl/vnl_random.h>
#include <vcl_iostream.h>
#include <vcl_algorithm.h>
#include <vcl_fstream.h>
#include <vcl_cstdio.h>

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
  if (argc < 8) {
    vcl_cout << "USAGE: sel_examine_params <image file name> <out_cem_name> <gen=1, third order=2> <interp> <lin=0, cc=1, es=2> <nrad> <N_group>\n";
    return 1;
  }

  //get the image filename
  vcl_string image_filename(argv[1]);

  //load image
  vil_image_view<vxl_byte> image  = vil_load(image_filename.c_str());

  //compute edges
  double sigma = 1;
  double threshold = 5;
  int N = atoi(argv[4]);
  dbdet_edgemap_sptr edgemap;
  
  //detector option
  int det_opt = atoi(argv[3]);
  if (det_opt==1)
    edgemap = dbdet_detect_generic_edges(image, sigma, threshold, N);
  else if (det_opt==2)
    edgemap = dbdet_detect_third_order_edges(image, sigma, threshold, N);
  else
    return 1;

  //save edgemap
  char out_edge_filename[300];
  vcl_sprintf(out_edge_filename, "%s_%d_%f.edg", argv[2], 1, sigma);

  saveEDG(vcl_string(out_edge_filename), edgemap);

  //perform linking with various parameters

  //parameters
  int cmodel = atoi(argv[5]);
  double nrad = (double) atoi(argv[6]); //2.0
  unsigned maxN = atoi(argv[7]); //5

  //for one step debug
  //double dx_sel=0.3;
  //double dt_deg=30.0;

  for (double dx_sel=0.05; dx_sel<0.4; dx_sel+=0.05){
    for (double dt_deg=0.0; dt_deg<45.0; dt_deg+=5.0){

      double dt_sel = dt_deg*vnl_math::pi/180.0;

      //first we need to clear the edgemap of any curvelets it might have formed before
      edgemap->clear_all_curvelets();

      dbdet_sel_sptr edge_linker;
      switch (cmodel){
        case 0:
          edge_linker = new dbdet_sel<dbdet_linear_curve_model>(edgemap, nrad, dt_sel, dx_sel);
          break;
        case 1:
          edge_linker = new dbdet_sel<dbdet_CC_curve_model>(edgemap, nrad, dt_sel, dx_sel);
          break;
        case 2:
          edge_linker = new dbdet_sel<dbdet_ES_curve_model>(edgemap, nrad, dt_sel, dx_sel);
          break;
      }
       
      edge_linker->build_curvelets_greedy(maxN, true);//extra greedy

      //also look at the contours formed with different sized curvelets
      for (int nn=4; nn<=maxN; nn++){
        edge_linker->construct_the_link_graph(nn, 2);
        edge_linker->extract_image_contours();
        edge_linker->report_stats();//report stats

        //form vsol polylines from the chains
        vcl_vector< vsol_spatial_object_2d_sptr > image_curves;
        for (unsigned i=0; i<edge_linker->edgel_chains().size(); i++){
          //only keep the longer contours
          if (edge_linker->edgel_chains()[i]->edgels.size() >= 3){
            vcl_vector<vsol_point_2d_sptr> pts;
            for (unsigned j=0; j<edge_linker->edgel_chains()[i]->edgels.size(); j++)
              pts.push_back(new vsol_point_2d(edge_linker->edgel_chains()[i]->edgels[j]->pt));
            vsol_polyline_2d_sptr new_curve = new vsol_polyline_2d(pts);
            image_curves.push_back(new_curve->cast_to_spatial_object());
          }
        }

        //save linked edges
        char out_cem_filename[500];
        vcl_sprintf(out_cem_filename, "%s_%f_%f_%d_%d.cem", argv[2], dx_sel, dt_deg, maxN, nn);

        saveCEM(out_cem_filename, image_curves);
      }
    }
  }

  return 0; 
}

bool saveEDG(vcl_string filename, dbdet_edgemap_sptr edgemap)
{
  //1) If file open fails, return.
  vcl_ofstream outfp(filename.c_str(), vcl_ios::out);

  if (!outfp){
    vcl_cout << " Error opening file  " << filename.c_str() << vcl_endl;
    return false;
  }

  //2) write out the header block
  outfp << "# EDGE_MAP " << vcl_endl << vcl_endl;
  outfp << "# Format :  [Pixel_Pos]  Pixel_Dir Pixel_Conf  [Sub_Pixel_Pos] Sub_Pixel_Dir Sub_Pixel_Conf" << vcl_endl;
  outfp << vcl_endl;
  outfp << "WIDTH=" << edgemap->width() << vcl_endl;
  outfp << "HEIGHT=" << edgemap->height() << vcl_endl;
  outfp << "EDGE_COUNT=" << edgemap->num_edgels  << vcl_endl;
  outfp << vcl_endl << vcl_endl;

  //write out all the edgels
  dbdet_edgemap_const_iter it = edgemap->edge_cells.begin();
  for (; it!=edgemap->edge_cells.end(); it++){
    for (unsigned j=0; j<(*it).size(); j++){
      dbdet_edgel* edgel = (*it)[j];

      double x = edgel->pt.x();
      double y = edgel->pt.y();

      int ix = (int) vcl_floor(x);
      int iy = (int) vcl_floor(y);
      
      double idir = edgel->tangent, iconf = edgel->strength, dir= edgel->tangent, conf= edgel->strength;
      
      outfp << "[" << ix << ", " << iy << "]    " << idir << " " << iconf << "   [" << x << ", " << y << "]   " << dir << " " << conf << vcl_endl;
    }
  }

  outfp.close();

  return true;
}

bool saveCEM (vcl_string filename, vcl_vector< vsol_spatial_object_2d_sptr > & vsol_list)
{ 
  //1)If file open fails, return.
  vcl_ofstream outfp(filename.c_str(), vcl_ios::out);

  if (!outfp){
    vcl_cout << " Error opening file  " << filename.c_str() << vcl_endl;
    return false;
  }

  vcl_cout << "Saving " << filename.c_str() << " ..." << vcl_endl;

  // output header information
   outfp <<"# CONTOUR_EDGE_MAP "<<vcl_endl;
   outfp <<"# .cem files"<<vcl_endl;
   outfp <<"#"<<vcl_endl;
   outfp <<"# Format :"<<vcl_endl;
   outfp <<"# Each contour block will consist of the following"<<vcl_endl;
   outfp <<"# [BEGIN CONTOUR]"<<vcl_endl;
   outfp <<"# EDGE_COUNT=num_of_edges"<<vcl_endl;
   outfp <<"# [Pixel_Pos]  Pixel_Dir Pixel_Conf  [Sub_Pixel_Pos] Sub_Pixel_Dir Sub_Pixel_Conf "<<vcl_endl;
   outfp <<"# ..."<<vcl_endl;
   outfp <<"# ..."<<vcl_endl;
   outfp <<"# [END CONTOUR]"<<vcl_endl;

  outfp<<vcl_endl;

  // Note: this count is currently missing
   outfp <<"CONTOUR_COUNT="<< vcl_endl;
   outfp <<"TOTAL_EDGE_COUNT="<<vcl_endl;

  // parse through all the vsol classes and save curve objects only
  for (unsigned int b = 0 ; b < vsol_list.size() ; b++ )
  {
    if( vsol_list[b]->cast_to_curve())
    {
      if( vsol_list[b]->cast_to_curve()->cast_to_polyline() )
      {
        outfp <<"[BEGIN CONTOUR]"<<vcl_endl;
        outfp <<"EDGE_COUNT="<< vsol_list[b]->cast_to_curve()->cast_to_polyline()->size() <<vcl_endl;
        
        for (unsigned int i=0; i<vsol_list[b]->cast_to_curve()->cast_to_polyline()->size();i++)
        {
          vsol_point_2d_sptr pt = vsol_list[b]->cast_to_curve()->cast_to_polyline()->vertex(i);
          //output as subpixel contours
          // [%d, %d]\t%lf\t%lf\t[%lf, %lf]\t%lf\t%lf
          outfp <<" [0, 0]  0.0  0.0  [" << pt->x() << ", " << pt->y() << "]  0.0  0.0"  << vcl_endl;
        }
        outfp <<"[END CONTOUR]"<<vcl_endl<<vcl_endl;
      }
    }
  }
  
  //close file
  outfp.close();
  
  return true;
}

