// This is bsold/bsold_file_io.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif

//:
// \file


#include "bsold_file_io.h"

#include <iostream>
#include <cstring>
#include <vsol/vsol_point_2d.h>
#include <vsol/vsol_polygon_2d.h>
#include <vsol/vsol_polyline_2d.h>
#include <vsol/vsol_line_2d.h>
#include <vsol/vsol_line_2d_sptr.h>

#include <vnl/vnl_math.h>
#include <vul/vul_psfile.h>
#include <vul/vul_file.h>
#include <vil/vil_image_view.h>
#include <vil/vil_image_resource.h>
#include <vil/vil_image_resource_sptr.h>
#include <vil/vil_image_view_base.h>

#include <bsold/bsold_config.h>

#ifdef HAS_BOOST
#include <boost/iostreams/filtering_stream.hpp>
#include <boost/iostreams/copy.hpp>
#include <boost/iostreams/filter/gzip.hpp>
#endif


// -----------------------------------------------------------------------------
//: Load a .CON file and save results to a vector of points and whether the 
// contour is closed. Return false if loading fails
bool bsold_load_con_file(char const* filename, 
                         std::vector<vsol_point_2d_sptr >& points, 
                         bool & is_closed)
{
  // open file for reading
  std::ifstream infp(filename, std::ios::in);
  if (!infp) 
  {
    std::cerr << " Error opening file  " << filename << std::endl;
    return false;
  }

  // check header
  char line_buffer[2000]; //200
  infp.getline(line_buffer,2000);
  if (std::strncmp(line_buffer,"CONTOUR",7)) 
  {
    std::cerr << "Invalid File " << filename << std::endl
             << "Should be CONTOUR " << line_buffer << std::endl;
    return false;
  }

  // check whether contour is open or closed
  char open_flag[2000];
  infp.getline(open_flag,2000);
  if (!std::strncmp(open_flag,"OPEN",4))
    is_closed = false;
  else if (!std::strncmp(open_flag,"CLOSE",5))
    is_closed = true;
  else
  {
    std::cerr << "Invalid File " << filename << std::endl
             << "Should be OPEN/CLOSE " << open_flag << std::endl;
    return false;
  }


  // read coordinates of the points
  int num_pts;
  infp >> num_pts;

  // vector to store the points
  points.clear();
  points.reserve(num_pts);
  for (int i=0;i<num_pts;i++) 
  {
    float x,y;
    infp >> x >> y;
    vsol_point_2d_sptr new_pt = new vsol_point_2d (x,y);
    points.push_back(new_pt);
  }

  //close file
  infp.close();

  return true;
}


 
// ----------------------------------------------------------------------------
//: Load a .CON file and return a smart pointer to vsol_spatial_object_2d
// This can be either a polyline or a polygon. Use cast function to get the
// exact type
vsol_spatial_object_2d_sptr bsold_load_con_file(char const* filename)
{  
  std::vector<vsol_point_2d_sptr > points;
  bool is_closed;
  if (!bsold_load_con_file(filename, points, is_closed))
    return 0;

  
  // create a polyline or a polygon depending on whether the contour
  // file is labelled OPEN or CLOSE
  if (is_closed) 
  {
    return new vsol_polygon_2d (points);
  }
  else 
  {
    return new vsol_polyline_2d (points);
  }
}


// ----------------------------------------------------------------------------
//: Save a polyline to a .CON file. Return false if saving fails
bool bsold_save_con_file(char const* filename, vsol_polyline_2d_sptr polyline)
{
  std::vector<vsol_point_2d_sptr > pts;
  pts.reserve(polyline->size());
  for (unsigned int i=0; i<polyline->size(); ++i)
  {
    pts.push_back(polyline->vertex(i));
  }
  return bsold_save_con_file(filename, pts, false);
}


// ----------------------------------------------------------------------------
//: Save a polygon to a .CON file. Return false if saving fails
bool bsold_save_con_file(char const* filename, vsol_polygon_2d_sptr polygon)
{
  std::vector<vsol_point_2d_sptr > pts;
  pts.reserve(polygon->size());
  for (unsigned int i=0; i<polygon->size(); ++i)
  {
    pts.push_back(polygon->vertex(i));
  }
  return bsold_save_con_file(filename, pts, true);
}


// ----------------------------------------------------------------------------
//: Save a set of ordered points to a .CON file. Return false if saving fails
bool bsold_save_con_file(char const* filename, 
                         const std::vector<vsol_point_2d_sptr >& pts,
                         bool is_closed)
{
  // 1) create the file
  //std::ifstream infp(filename, std::ios::in);
  std::ofstream outfp(filename, std::ios::out);
  if (!outfp)
  {
    std::cout << " Error writing file  " << filename << std::endl;
    return false;
  }

  // 2) start writing out the contour to the file
  
  // header
  outfp << "CONTOUR" << std::endl;

  // The contour can either be a polyline producing an open contour 
  // or a polygon producing a close contour          
  if (is_closed)
  {
    outfp << "CLOSE" << std::endl;
  }
  else
  {
    outfp << "OPEN" << std::endl;
  }

  // Number of points
  outfp << pts.size() << std::endl;

  // Write coordinates of the points
  for (unsigned int i=0; i<pts.size(); ++i)
  {
    vsol_point_2d_sptr pt = pts[i];
    outfp <<pt->x() << " " << pt->y() << std::endl;
  }

  // 3) close the file
  outfp.close();

  return true;
}


#ifdef HAS_BOOST
static bool bsold_load_cem_gzip(std::vector< vsol_spatial_object_2d_sptr >& contours, std::string filename);
static bool bsold_save_cem_gzip(std::vector< vsol_spatial_object_2d_sptr >& vsol_list, std::string filename);
#endif

static bool bsold_load_cem_ascii(std::vector< vsol_spatial_object_2d_sptr >& contours, std::string filename);
static bool bsold_save_cem_ascii(std::vector< vsol_spatial_object_2d_sptr >& vsol_list, std::string filename);

bool bsold_load_cem(std::vector< vsol_spatial_object_2d_sptr >& contours, std::string filename)
{
  std::string ext = vul_file::extension(filename);
  std::cerr << "bsold_load_cem attempt at loading " << filename << "with ext" << ext << std::endl;;

  if (ext == ".gz") {
    std::cerr << "XXX Gzip file\n";
#ifdef HAS_BOOST
    std::cerr << "XXX HAS_BOOST, YEAH\n";
    return bsold_load_cem_gzip(contours, filename);
#else
    std::cerr << "Error: .gz compressed file was provided, but boost wasn't found\n";
#endif
  } else
    return bsold_load_cem_ascii(contours, filename);
}

bool bsold_save_cem(std::vector< vsol_spatial_object_2d_sptr >& vsol_list, std::string filename)
{
  std::string ext = vul_file::extension(filename);

  if (ext == ".gz") {
#ifdef HAS_BOOST
    return bsold_save_cem_gzip(vsol_list, filename);
#else
    std::cerr << "Error: .gz compressed filename was provided, but boost wasn't found\n";
#endif
  } else
    return bsold_save_cem_ascii(vsol_list, filename);
}


// -----------------------------------------------------------------------------
// Load .CEM file
bool bsold_load_cem_ascii(std::vector< vsol_spatial_object_2d_sptr >& contours, std::string filename)
{
  float x, y;
  char lineBuffer[1024];
  int numContours, numTotalEdges, numEdges;
  int ix, iy;
  double idir, iconf, dir, conf;

  //1)If file open fails, return.
  std::ifstream infp(filename.c_str(), std::ios::in);

  if (!infp){
    std::cout << " Error opening file  " << filename.c_str() << std::endl;
    return false;
  }

  //2)Read in each line
  while (infp.getline(lineBuffer,1024)) {

    //ignore comment lines and empty lines
    if (strlen(lineBuffer)<2 || lineBuffer[0]=='#')
      continue;

    //read the line with the contour count info
    if (!strncmp(lineBuffer, "CONTOUR_COUNT=", sizeof("CONTOUR_COUNT=")-1)){
      sscanf(lineBuffer,"CONTOUR_COUNT=%d",&(numContours));
      //std::cout << numContours << std::endl;
      continue;
    }

    //read the line with the edge count info
    if (!strncmp(lineBuffer, "TOTAL_EDGE_COUNT=", sizeof("TOTAL_EDGE_COUNT=")-1)){
      sscanf(lineBuffer,"TOTAL_EDGE_COUNT=%d",&(numTotalEdges));
      //std::cout << numTotalEdges << std::endl;
      continue;
    }

    //read the beginning of a contour block
    if (!strncmp(lineBuffer, "[BEGIN CONTOUR]", sizeof("[BEGIN CONTOUR]")-1)){

      //discarding other information for now...should really be outputting edgels
      std::vector< vsol_point_2d_sptr > points;

      infp.getline(lineBuffer,1024);
      sscanf(lineBuffer,"EDGE_COUNT=%d",&(numEdges));
      //std::cout << numEdges << std::endl;

      for (int j=0; j< numEdges; j++){
        //the rest should have data that goes into the current contour
        infp.getline(lineBuffer,1024);
        sscanf(lineBuffer," [%d, %d]\t%f\t%f\t[%f, %f]\t%f\t%f",&(ix), &(iy),
              &(idir), &(iconf), &(x), &(y), &(dir), &(conf));

        //VJ's current CEM is in degrees rather than radians so need to convert
        dir += 90;
        dir *= vnl_math::pi/180;

        vsol_point_2d_sptr newPt = new vsol_point_2d (x,y);
        points.push_back(newPt);
      }

      infp.getline(lineBuffer,1024);

    while (strncmp(lineBuffer, "[END CONTOUR]", sizeof(" [END CONTOUR]")-1)) {

      infp.getline(lineBuffer,1024);

    }
      //if (strncmp(lineBuffer, "[END CONTOUR]", sizeof(" [END CONTOUR]")-1))
        //assert(false);

      vsol_polyline_2d_sptr newContour = new vsol_polyline_2d (points);
      contours.push_back(newContour->cast_to_spatial_object());
    }
  }
  infp.close();

  std::cout << "Loaded: " << filename.c_str() << ".\n";
  return true;
}

#ifdef HAS_BOOST
// -----------------------------------------------------------------------------
// Load .CEM file compressed with zlib, gzip style.
// TODO: use templating and/or istream inheritance to avoid duplicating almost
// identical code to bsold_load_cem_ascii
bool bsold_load_cem_gzip(std::vector< vsol_spatial_object_2d_sptr >& contours, std::string filename)
{
  float x, y;
  char lineBuffer[1024];
  int numContours, numTotalEdges, numEdges;
  int ix, iy;
  double idir, iconf, dir, conf;

  //1)If file open fails, return.
  std::ifstream infp_orig(filename.c_str(), std::ios::in | std::ios::binary);

  if (!infp_orig){
    std::cout << " Error opening file  " << filename.c_str() << std::endl;
    return false;
  }

  boost::iostreams::filtering_istream infp;
  infp.push(boost::iostreams::gzip_decompressor());
  infp.push(infp_orig);

  //2)Read in each line
  while (infp.getline(lineBuffer,1024)) {

    //ignore comment lines and empty lines
    if (strlen(lineBuffer)<2 || lineBuffer[0]=='#')
      continue;

    //read the line with the contour count info
    if (!strncmp(lineBuffer, "CONTOUR_COUNT=", sizeof("CONTOUR_COUNT=")-1)){
      sscanf(lineBuffer,"CONTOUR_COUNT=%d",&(numContours));
      //std::cout << numContours << std::endl;
      continue;
    }

    //read the line with the edge count info
    if (!strncmp(lineBuffer, "TOTAL_EDGE_COUNT=", sizeof("TOTAL_EDGE_COUNT=")-1)){
      sscanf(lineBuffer,"TOTAL_EDGE_COUNT=%d",&(numTotalEdges));
      //std::cout << numTotalEdges << std::endl;
      continue;
    }

    //read the beginning of a contour block
    if (!strncmp(lineBuffer, "[BEGIN CONTOUR]", sizeof("[BEGIN CONTOUR]")-1)){

      //discarding other information for now...should really be outputting edgels
      std::vector< vsol_point_2d_sptr > points;

      infp.getline(lineBuffer,1024);
      sscanf(lineBuffer,"EDGE_COUNT=%d",&(numEdges));
      //std::cout << numEdges << std::endl;

      for (int j=0; j< numEdges; j++){
        //the rest should have data that goes into the current contour
        infp.getline(lineBuffer,1024);
        sscanf(lineBuffer," [%d, %d]\t%f\t%f\t[%f, %f]\t%f\t%f",&(ix), &(iy),
              &(idir), &(iconf), &(x), &(y), &(dir), &(conf));

        //VJ's current CEM is in degrees rather than radians so need to convert
        dir += 90;
        dir *= vnl_math::pi/180;

        vsol_point_2d_sptr newPt = new vsol_point_2d (x,y);
        points.push_back(newPt);
      }

      infp.getline(lineBuffer,1024);

    while (strncmp(lineBuffer, "[END CONTOUR]", sizeof(" [END CONTOUR]")-1)) {

      infp.getline(lineBuffer,1024);

    }
      //if (strncmp(lineBuffer, "[END CONTOUR]", sizeof(" [END CONTOUR]")-1))
        //assert(false);

      vsol_polyline_2d_sptr newContour = new vsol_polyline_2d (points);
      contours.push_back(newContour->cast_to_spatial_object());
    }
  }

  std::cout << "Loaded: " << filename.c_str() << ".\n";
  return true;
}
#endif //! HAS_BOOST

// -----------------------------------------------------------------------------
//: Save .CEM file
bool bsold_save_cem_ascii(std::vector< vsol_spatial_object_2d_sptr >& vsol_list, std::string filename)
{
  //1)If file open fails, return.
  std::ofstream outfp(filename.c_str(), std::ios::out);

  if (!outfp){
    std::cout << " Error opening file  " << filename.c_str() << std::endl;
    return false;
  }

  // output header information
   outfp <<"# CONTOUR_EDGE_MAP : Logical-Linear + Shock_Grouping"<<std::endl;
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
   
  
  for (unsigned int b = 0 ; b < vsol_list.size() ; b++ )
  {
    if( vsol_list[b]->cast_to_curve())
    {
      if( vsol_list[b]->cast_to_curve()->cast_to_polyline() )
      {
        outfp <<"[BEGIN CONTOUR]"<<std::endl;
        outfp <<"EDGE_COUNT="<< vsol_list[b]->cast_to_curve()->cast_to_polyline()->size() <<std::endl;
        
        for (unsigned int i=0; i<vsol_list[b]->cast_to_curve()->cast_to_polyline()->size();i++)
        {
          vsol_point_2d_sptr pt = vsol_list[b]->cast_to_curve()->cast_to_polyline()->vertex(i);
          //output as subpixel contours
          // [%d, %d]\t%lf\t%lf\t[%lf, %lf]\t%lf\t%lf
          outfp <<" [0, 0]  0.0  0.0  [" << pt->x() << ", " << pt->y() << "]  0.0  0.0"  << std::endl;
        }
        outfp <<"[END CONTOUR]"<<std::endl<<std::endl;
      }
    } else if (vsol_list[b]->cast_to_region()) {
      if (vsol_list[b]->cast_to_region()->cast_to_polygon() )
      {
        outfp <<"[BEGIN CONTOUR]"<<std::endl;
        outfp <<"EDGE_COUNT="<< vsol_list[b]->cast_to_region()->cast_to_polygon()->size()+1 <<std::endl;
        
        for (unsigned int i=0; i<vsol_list[b]->cast_to_region()->cast_to_polygon()->size();i++)
        {
          vsol_point_2d_sptr pt = vsol_list[b]->cast_to_region()->cast_to_polygon()->vertex(i);
          //output as subpixel contours
          // [%d, %d]\t%lf\t%lf\t[%lf, %lf]\t%lf\t%lf
          outfp <<" [0, 0]  0.0  0.0  [" << pt->x() << ", " << pt->y() << "]  0.0  0.0"  << std::endl;
        }
        vsol_point_2d_sptr pt = vsol_list[b]->cast_to_region()->cast_to_polygon()->vertex(0);  // print the first point once more to include last edge
        outfp <<" [0, 0]  0.0  0.0  [" << pt->x() << ", " << pt->y() << "]  0.0  0.0"  << std::endl;

        outfp <<"[END CONTOUR]"<<std::endl<<std::endl;
      }
    }
  }
  
  return true;
}

#ifdef HAS_BOOST
// -----------------------------------------------------------------------------
//: Save .CEM file
bool bsold_save_cem_gzip(std::vector< vsol_spatial_object_2d_sptr >& vsol_list, std::string filename)
{
  //1)If file open fails, return.
  std::ofstream outfp_orig(filename.c_str(), std::ios::out | std::ios::binary);

  if (!outfp_orig){
    std::cout << " Error opening file  " << filename.c_str() << std::endl;
    return false;
  }

  boost::iostreams::filtering_ostream outfp;
  outfp.push(boost::iostreams::gzip_compressor());
  outfp.push(outfp_orig);

  // output header information
   outfp <<"# CONTOUR_EDGE_MAP : Logical-Linear + Shock_Grouping"<<std::endl;
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
   
  
  for (unsigned int b = 0 ; b < vsol_list.size() ; b++ )
  {
    if( vsol_list[b]->cast_to_curve())
    {
      if( vsol_list[b]->cast_to_curve()->cast_to_polyline() )
      {
        outfp <<"[BEGIN CONTOUR]"<<std::endl;
        outfp <<"EDGE_COUNT="<< vsol_list[b]->cast_to_curve()->cast_to_polyline()->size() <<std::endl;
        
        for (unsigned int i=0; i<vsol_list[b]->cast_to_curve()->cast_to_polyline()->size();i++)
        {
          vsol_point_2d_sptr pt = vsol_list[b]->cast_to_curve()->cast_to_polyline()->vertex(i);
          //output as subpixel contours
          // [%d, %d]\t%lf\t%lf\t[%lf, %lf]\t%lf\t%lf
          outfp <<" [0, 0]  0.0  0.0  [" << pt->x() << ", " << pt->y() << "]  0.0  0.0"  << std::endl;
        }
        outfp <<"[END CONTOUR]"<<std::endl<<std::endl;
      }
    } else if (vsol_list[b]->cast_to_region()) {
      if (vsol_list[b]->cast_to_region()->cast_to_polygon() )
      {
        outfp <<"[BEGIN CONTOUR]"<<std::endl;
        outfp <<"EDGE_COUNT="<< vsol_list[b]->cast_to_region()->cast_to_polygon()->size()+1 <<std::endl;
        
        for (unsigned int i=0; i<vsol_list[b]->cast_to_region()->cast_to_polygon()->size();i++)
        {
          vsol_point_2d_sptr pt = vsol_list[b]->cast_to_region()->cast_to_polygon()->vertex(i);
          //output as subpixel contours
          // [%d, %d]\t%lf\t%lf\t[%lf, %lf]\t%lf\t%lf
          outfp <<" [0, 0]  0.0  0.0  [" << pt->x() << ", " << pt->y() << "]  0.0  0.0"  << std::endl;
        }
        vsol_point_2d_sptr pt = vsol_list[b]->cast_to_region()->cast_to_polygon()->vertex(0);  // print the first point once more to include last edge
        outfp <<" [0, 0]  0.0  0.0  [" << pt->x() << ", " << pt->y() << "]  0.0  0.0"  << std::endl;

        outfp <<"[END CONTOUR]"<<std::endl<<std::endl;
      }
    }
  }
  return true;
}
#endif //!HAS_BOOST
