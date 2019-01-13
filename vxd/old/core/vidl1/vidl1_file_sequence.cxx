// This is core/vidl1/vidl1_file_sequence.cxx
#include "vidl1_file_sequence.h"

#include <cassert>
#include <fstream>
#include <iostream>

#include <vul/vul_awk.h>
#include <vul/vul_file.h>
#include <vul/vul_sprintf.h>

bool vidl1_file_sequence::open(std::string const& fmt)
{
  current_file_index = -1;

  // fmt could be a "lst" file, or a single vob or mpg or a %d list
  int l = fmt.size();
  if (l > 4 && fmt.substr(l-4) == ".lst") {
    std::ifstream f(fmt.c_str());
    assert(f.good());
    for (vul_awk awk(f); awk; ++awk) {
      if (awk.NF() > 0) {
        std::cerr << awk[0] << std::endl;
        filenames.push_back(awk[0]);
      }
    }
  }
  else if (fmt.find('%') != std::string::npos) {
    // Assume a list.  Could start from some low number... Could glob.  hmmm.
    bool found_one = false;
    for (int i = 0; ; ++i) {
      std::string const& buf = vul_sprintf(fmt.c_str(), i);
      offset_t s = (offset_t)vul_file::size(buf);
      if (s > 0) {
        found_one = true;
        filenames.push_back(buf);
        filesizes.push_back(s);
      }
      else {
        // If got at least one so far, then bail now
        if (found_one)
          break;
        else {
          // If not found one yet, and still only tried < 10, then
          // keep trying.
          if (i > 10)
            break;
        }
      }
    }
  }
  else {
    // No %, not .lst: assume it's an mpeg/vob
    filenames.push_back(fmt);
  }

  unsigned int n = filenames.size();

  if (n == 0) {
    std::cerr << "vidl1_file_sequence: ERROR: Could not turn [" << fmt << "] into a list of files\n";
    return false;
  }

  // Fill in sizes if not done already
  if (filesizes.size() < n) {
    filesizes.resize(n);
    for (unsigned int i = 0; i < n; ++i) {
      offset_t s = (offset_t)vul_file::size(filenames[i].c_str());
      if (s == 0)
        std::cerr << "WARNING: Zero size file [" << filenames[i] << "]\n";
      filesizes[i] = s;
    }
  }

  // Set up file ptr etc.
  current_file_index = 0;

  // Fill start_bytes
  start_byte.resize(n);
  start_byte[0] = 0L;
  for (unsigned int i = 1; i < filenames.size(); ++i)
    start_byte[i] = start_byte[i-1] + filesizes[i-1];

  // Open them all
  fps.resize(n);
  for (unsigned int i = 0; i < filenames.size(); ++i) {
    std::string const& fn = filenames[i];
    fps[i] = std::fopen(fn.c_str(), "rb");
    if (!fps[i]) {
      std::cerr << "vidl1_file_sequence::open(): ERROR: Could not open [" << fn << "]\n";
      current_file_index = -1;
      return false;
    }
  }

  // Summarize:
  std::cerr << "files: sizeof(offset_t) = " << sizeof(offset_t) << '\n';
  for (unsigned int i = 0; i < n; ++i)
    std::cerr << "   " << filenames[i] << "  " << start_byte[i] << '\n';
  std::cerr << '\n';

  return true;
}

bool vidl1_file_sequence::seek(offset_t to)
{
  int newindex = -1;
  for (unsigned int i = 1; i < filesizes.size(); ++i)
    if (start_byte[i] > to) {
      newindex = i-1;
      break;
    }

  if (newindex == -1) {
    int i = filesizes.size() - 1;
    // Know start_byte[i] <= to
    if (to < start_byte[i] + filesizes[i])
      newindex = i;
  }

  if (newindex == -1) {
    std::cerr << "vidl1_file_sequence::seek(): ERROR: Could not seek to [" << to << "]\n";
    return false;
  }

  current_file_index = newindex;

  offset_t file_ptr = to - start_byte[current_file_index];
  std::cerr << " si = " << start_byte[current_file_index] << " to = " << to << '\n';
  assert(file_ptr < filesizes[current_file_index]);

  return 0 <= std::fseek(fps[current_file_index], file_ptr, SEEK_SET);
}

vidl1_file_sequence::offset_t vidl1_file_sequence::tell() const
{
  return start_byte[current_file_index] + ftell(fps[current_file_index]);
}

vidl1_file_sequence::offset_t vidl1_file_sequence::read(void* buf, offset_t len)
{
  offset_t space_left_in_this_file = filesizes[current_file_index] - ftell(fps[current_file_index]);

  offset_t bytes_from_curr = len;
  offset_t bytes_from_next = 0L;
  if (space_left_in_this_file < len) {
    bytes_from_curr = space_left_in_this_file;
    bytes_from_next = len - space_left_in_this_file;
  }

  if (bytes_from_next == 0)
    return std::fread(buf, 1, len, fps[current_file_index]);

  offset_t n1 = std::fread(buf, 1, bytes_from_curr, fps[current_file_index]);
  if (n1 < bytes_from_curr)
    // First read stopped short, don't even bother with next one
    return n1;
  if ((unsigned int)(current_file_index+1) == fps.size())
    // First was OK, and we've run out of files
    return n1;

  // First read was OK.  Advance to next file.
  ++current_file_index;
  std::fseek(fps[current_file_index], 0L, SEEK_SET); // need to seek(0) since we may have read from this file before.
  int n2 = std::fread((unsigned char*)buf + n1, 1, bytes_from_next, fps[current_file_index]);
  return n1 + n2;
}

void vidl1_file_sequence::close()
{
  for (unsigned int i = 0; i < fps.size(); ++i)
    std::fclose(fps[i]);
}
