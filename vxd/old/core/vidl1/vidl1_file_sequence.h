// This is core/vidl1/vidl1_file_sequence.h
#ifndef vidl1_file_sequence_h_
#define vidl1_file_sequence_h_
//:
// \file
// \author awf@robots.ox.ac.uk
// \date 18 Dec 01
// copied by l.e.galup from oxl/oxp/oxp_bunch_of_files
// 10-18-02

#include <cstdio>
#include <vector>
#include <string>

class vidl1_file_sequence
{
  typedef unsigned long offset_t;
 public:
  vidl1_file_sequence() {}
  vidl1_file_sequence(std::string const& fmt) { open(fmt); }
  ~vidl1_file_sequence() { close(); }

  bool open(std::string const& fmt);
  void close();

  bool seek(offset_t to);
  offset_t tell() const;
  offset_t read(void*, offset_t);
  bool ok() const { return current_file_index != -1; }

 private:
  int current_file_index;
  std::vector<std::string> filenames;
  std::vector<std::FILE*> fps;
  std::vector<offset_t> filesizes;
  std::vector<offset_t> start_byte;
};

#endif   // vidl1_file_sequence_h_
