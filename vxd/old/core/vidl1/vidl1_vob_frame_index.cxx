// This is core/vidl1/vidl1_vob_frame_index.cxx
#include "vidl1_vob_frame_index.h"
//
// this file has been copied from oxl/oxp
// author:  AWF
// copied by l.e.galup
// 10-18-02

#include <iostream>
#include <cassert>
#include <cstdio>
#include <fstream>
#include <vul/vul_awk.h>

bool vidl1_vob_frame_index::load(std::string const& filename)
{
  std::vector<vidl1_vob_frame_index_entry> tmp;

  std::ifstream f(filename.c_str(), std::ios::binary);
  if (!f.good())
  {
    std::cerr << "vidl1_vob_frame_index: Cannot read IDX file ["<< filename <<"]\n";
    return false;
  }
  vul_awk awk(f);
  std::string tag(awk[0]);
  const int MPEG_IDX = 1;
  const int LBA = 2;
  int idx_type = 0;
  if (tag == "MPEG_IDX") 
    idx_type = MPEG_IDX;
  else if (tag == "LBA")
    idx_type = LBA;
  else
    std::cerr << "vidl1_vob_frame_index: WARNING: unknown type [" << awk[0] << "]\n";

  for (int frame=0; awk; ++awk, ++frame)
  {
    // Skip comment and ----- lines
    vidl1_vob_frame_index_entry e;
    if (idx_type == LBA && std::sscanf(awk.line(), " %x | %d", &e.lba, &e.frame) == 2)
      tmp.push_back(e);
    int dummy;
    if (idx_type == MPEG_IDX && std::sscanf(awk.line(), " %x %x", &e.lba, &dummy) == 2)
    {
      e.frame = frame;
      tmp.push_back(e);
    }
  }
  l = tmp;

  // assert that l is sorted by frame
  for (unsigned int i = 0; i+1 < l.size(); ++i)
    assert(l[i+1].frame > l[i].frame);
  std::cerr << "Loaded " << l.size() << " entries from [" << filename << "]\n";
  if (l.size() == 0)
    std::cerr << "WARNING: No index entries -- all seeks from start\n";
  return true;
}

int vidl1_vob_frame_index::frame_to_lba_of_prev_I_frame(int f, int* f_actual)
{
  int lo = 0;
  int hi = l.size()-1;
  if (hi < 0 || f < l[lo].frame || f > l[hi].frame) {
    std::cerr << "urk: frame " << f << " out of IDX range\n";
    return -1;
  }
  while (lo < hi-1)
  {
    int half = (lo + hi) / 2;
    int f_half = l[half].frame;
    if (f < f_half)
      hi = half;
    else if (f > f_half)
      lo = half;
    else {
      lo = half;
      break;
    }
  }
  // std::fprintf(stderr, "vidl1_vob_frame_index: [%5d %5d] -> [%5d %5d]\n", lo, hi, l[lo].frame, l[hi].frame);
  if (f_actual)
    *f_actual = l[lo].frame;
  return l[lo].lba;
}
