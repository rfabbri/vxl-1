// This is lemsvxlsrc/contrib/bwm_lidar/algo/flimap_reader.cxx

//:
// \file
// \brief I/O interface for the flimap data
//
// \author Ibrahim Eden, (ieden@lems.brown.edu)
// \date 12/16/2007
//      
// \endverbatim

#include <vcl_iostream.h>
#include <vcl_fstream.h>

#include "all_includes.h"
#include "flimap_reader.h"

//: I/O interface for reading an input .xyz file
void flimap_reader::read_flimap_file(vcl_string flimap_file_path, vnl_matrix<double>& points){
  vcl_ifstream file_inp;
  file_inp.clear();
  file_inp.open(flimap_file_path.c_str());

  char line_string_char[256];

  vcl_vector<vcl_string> temp_pts;

  file_inp.getline(line_string_char,256);
  file_inp.getline(line_string_char,256);

  vcl_vector<vnl_double_2x3> all_nums;

  while(file_inp.eof()!=1){
    // Read one line at a time. First 3 entries are the world coordinates and next 3 entries are the RGB color elements.
    vnl_double_2x3 curr_nums;
    char temp;
    file_inp >> curr_nums[0][0] >> temp;
    file_inp >> curr_nums[0][1] >> temp;
    file_inp >> curr_nums[0][2] >> temp;
    file_inp >> curr_nums[1][0] >> temp;
    file_inp >> curr_nums[1][1] >> temp;
    file_inp >> curr_nums[1][2];
    file_inp.getline(line_string_char,256);
    all_nums.push_back(curr_nums);
  }
  points.set_size(all_nums.size(),6);

  for (unsigned i=0; i<all_nums.size(); i++) {
      points(i,0) = all_nums[i][0][0];
      points(i,1) = all_nums[i][0][1];
      points(i,2) = all_nums[i][0][2];
      points(i,3) = all_nums[i][1][0];
      points(i,4) = all_nums[i][1][1];
      points(i,5) = all_nums[i][1][2];
  }

  all_nums.clear();
}
