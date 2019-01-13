#ifndef bmcsd_util_txx_
#define bmcsd_util_txx_

#include "bmcsd_util.h"
#include <sstream>

// Most non-template implementations are in bmcsd_util_defs.cxx

template <typename T>  void bmcsd_util::
parse_num_list(const std::string &stringlist, std::vector<T> *values)
{
  std::istringstream istream;
  istream.str(stringlist);

  T value;
  while ( istream >> value )
    values->push_back(value);
}

#undef BMCSD_UTIL_INSTANTIATE
#define BMCSD_UTIL_INSTANTIATE(T) \
template void bmcsd_util::parse_num_list(const std::string &stringlist, std::vector<T> *values);

#endif // bmcsd_util_txx_
