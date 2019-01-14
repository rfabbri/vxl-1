// This is vidpro/process/vidpro_load_cem_process.h

#include <iostream>
#include <fstream>
#include <cassert>

#include <algorithm>
#include <vidpro/process/vidpro_load_cem_process.h>
#include <vsol/vsol_polyline_2d.h>
#include <vsol/vsol_polyline_2d_sptr.h>
#include <vsol/vsol_point_2d.h>
#include <vnl/vnl_math.h>
#include <vul/vul_file.h>
#include <vul/vul_file_iterator.h>

vidpro_load_cem_process::vidpro_load_cem_process() : bpro_process(), num_frames_(0)
{
  if( !parameters()->add( "Input file <filename...>" , "-ceminput" , bpro_filepath("Choose a file or a directory","*.cem") ))
  {
    std::cerr << "ERROR: Adding parameters in " __FILE__ << std::endl;
  }
}


//: Clone the process
bpro_process*
vidpro_load_cem_process::clone() const
{
  return new vidpro_load_cem_process(*this);
}


std::vector< std::string > vidpro_load_cem_process::get_input_type()
{
  std::vector< std::string > to_return;
  // no input type required
  to_return.clear();
  return to_return;
}

std::vector< std::string > vidpro_load_cem_process::get_output_type()
{
  std::vector< std::string > to_return;
  // output type
  to_return.push_back( "vsol2D" );
  return to_return;
}

bool vidpro_load_cem_process::execute()
{
  bpro_filepath input;
  parameters()->get_value( "-ceminput" , input);
  std::string input_file_path = input.path;

  int num_of_files = 0;

  output_data_.clear();

  // make sure that input_file_path is sane
  if (input_file_path == "") { return false; }

  //std::cout << vul_file::dirname(input_file_path);

  // test if fname is a directory
  if (vul_file::is_directory(input_file_path))
  {
    vul_file_iterator fn=input_file_path+"/*.cem";
    for ( ; fn; ++fn) 
    {
      std::string input_file = fn();
  
      vidpro_vsol2D_storage_sptr new_cem = loadCEM(input_file);
      output_data_.push_back(std::vector< bpro_storage_sptr > (1,new_cem));
      num_of_files++;
    }

    //this is the number of frames to be outputted
    num_frames_ = num_of_files;
  }
  else {
    std::string input_file = input_file_path;

    vidpro_vsol2D_storage_sptr new_cem = loadCEM(input_file);
    output_data_.push_back(std::vector< bpro_storage_sptr > (1,new_cem));
    num_frames_ = 1;
  }

  //reverse the order of the objects so that they come out in the right order
  std::reverse(output_data_.begin(),output_data_.end());

  return true;
}

vidpro_vsol2D_storage_sptr vidpro_load_cem_process::loadCEM (std::string filename)
{
  double x, y;
  char lineBuffer[1024];
  int numContours, numTotalEdges, numEdges;
  int ix, iy;
  double idir, iconf, dir, conf;

  // new vector to store the contours
  std::vector< vsol_spatial_object_2d_sptr > contours;

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
        sscanf(lineBuffer," [%d, %d]\t%lf\t%lf\t[%lf, %lf]\t%lf\t%lf",&(ix), &(iy),
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

  // create the output storage class
  vidpro_vsol2D_storage_sptr output_vsol = vidpro_vsol2D_storage_new();
  output_vsol->add_objects(contours, filename);

  std::cout << "Loaded: " << filename.c_str() << ".\n";

  return output_vsol;
}
