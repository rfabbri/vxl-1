#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif

#include <imgr/imgr_scan_resource_io.h>
#include <vcl_cstdio.h>
#include <vcl_cstring.h>
#include <vcl_cstdlib.h>
#include <vcl_cstddef.h>
#include <vcl_compiler.h>
#include <vcl_algorithm.h>
#include <vul/vul_file.h>
#include <vul/vul_file_iterator.h>
#include <vil/vil_load.h>
#include <vil/vil_copy.h>
#include <imgr/imgr_scan_resource.h>

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


void parse_multiple_filenames(const vcl_string & input,
                              vcl_vector<vcl_string> &filenames)
{
  filenames.clear();
  unsigned start=0;
  for (unsigned i=0; i != input.size(); ++i)
  {
    if (input[i]==';')
    {
      filenames.push_back(input.substr(start, i-start));
      start=i+1;
    }
  }
  filenames.push_back(input.substr(start, input.size() - start));
}

imgr_scan_resource_sptr
imgr_scan_resource_io::read_resource(xscan_scan const& scan)
{
  vcl_vector<vcl_string> filenames;
  vcl_string filename = scan.image_file_path();
  parse_multiple_filenames(filename, filenames);

  for (unsigned i=0; i<filenames.size(); ++i)
    if (!vul_file::exists(filenames[i]))
    {
      filenames.clear();
      break;
    }


  if (filenames.empty() || filenames.size()==1)
    parse_globbed_filenames(filename, filenames);

  if (filenames.empty()) return 0;


  //A hack for the moment to increase the number of files for WINDOWS
#if defined(VCL_WIN32)
vcl_cout << '\n'<< "Max number of open files has been reset from " << _getmaxstdio();
_setmaxstdio(2048);
#endif
  vcl_vector<vil_image_resource_sptr> rescs(filenames.size());
  // load all the slices
  for (unsigned i=0; i<filenames.size(); ++i)
  {
    //DEBUG
//  vcl_cout << "["<<i<<"]  "<< filenames[i].c_str() <<'\n';
  
  vil_image_resource_sptr resc  =
      vil_load_image_resource(filenames[i].c_str());
    rescs[i]=resc;
  }

  return new imgr_scan_resource(scan, rescs);
}
