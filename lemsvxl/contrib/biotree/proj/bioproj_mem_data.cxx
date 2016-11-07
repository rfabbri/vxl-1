#include "bioproj_mem_data.h"
#include <vcl_cstddef.h>
#include <vul/vul_file.h>
#include <vul/vul_file_iterator.h>
#include <vcl_algorithm.h>
#include <vcl_cstdlib.h>
#include <vcl_string.h>
#include <vcl_vector.h>
#include <vcl_fstream.h>
#include <vil/vil_image_resource_sptr.h>
#include <vil/vil_load.h>
#include <vil/vil_image_view.h>
#include <vcl_cstdio.h>

// Look for a set of filenames that match the glob spec in filename
// The globbing format expects only '#' to represent numbers.
// Do not use "*" or "?"
// All "#" should be in one contiguous group.
static void parse_globbed_filenames(const vcl_string & input,
                                    vcl_vector<vcl_string> &filenames)
{
  filenames.clear();
  vcl_string filename = input;

  // Avoid confusing globbing functions
  if (filename.find("*") != filename.npos) return;
  if (filename.find("?") != filename.npos) return;

  // Check that all the #s are in a single group.
  vcl_size_t start = filename.find_first_of("#");
  if (start == filename.npos) return;
  vcl_size_t end = filename.find_first_not_of("#", start);
  if (filename.find_first_of("#",end) != filename.npos) return;
  if (end == filename.npos) end = filename.length();
  for (vcl_size_t i=start, j=start; i!=end; ++i, j+=12)
    filename.replace(j,1,"[0123456789]");


  // Search for the files
  for (vul_file_iterator fit(filename); fit; ++fit)
    filenames.push_back(fit());


  if (filenames.empty()) return;

  start = (start + filenames.front().size()) - input.size();
  end = (end + filenames.front().size()) - input.size();

  // Put them all in numeric order.
  vcl_sort(filenames.begin(), filenames.end());

  // Now discard non-contiguously numbered files.
  long count = vcl_atol(filenames.front().substr(start, end-start).c_str());
  vcl_vector<vcl_string>::iterator it=filenames.begin()+1;
  while (it != filenames.end())
  {
    if (vcl_atol(it->substr(start, end-start).c_str()) != ++count)
      break;
    ++it;
  }
  filenames.erase(it, filenames.end());
}

bioproj_mem_data::bioproj_mem_data(vcl_string file_path, int nviews, 
                                   int sensor_width, int sensor_height,
                                   int grid_w, int grid_h, int grid_d)
: nviews_(nviews), sensor_width_(sensor_width), sensor_height_(sensor_height),
  grid_w_(grid_w), grid_h_(grid_h), grid_d_(grid_d)
{
  // parse filenames
  parse_globbed_filenames(file_path, filenames_);
  // allocate the responses data structure and set to zero
  grid_.resize(grid_w_, grid_h_, grid_d_);
  grid_.fill(0.0);
}
