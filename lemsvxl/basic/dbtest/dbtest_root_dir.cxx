// This is basic/dbtest/dbtest_root_dir.cxx


//:
// \file



#include "dbtest_root_dir.h"

//#include <cstdlib>
#include <iostream>

// The following should have been created automatically by the
// configuration scripts from dbtest_where_root_dir.h.in
// We need to check for its existence and if it doesn't exist - do something else.

#ifdef DBTEST_WHERE_ROOT_DIR_H_EXISTS
#include <dbtest_where_root_dir.h>

//: Return source root directory of LEMSVXL source
std::string dbtest_root_dir()
{
  return std::string(DBTEST_SOURCE_ROOT_DIR);
}
#else
//: Return source root directory of LEMSVXL source
std::string dbtest_root_dir()
{
  std::cerr << "ERROR: dbtest_root_dir() Unable to retrieve root directory.\n"
           << "Missing header file dbtest_where_root_dir.h. Please check configuration of dbtest.\n";
  return std::string("");
}

#endif
