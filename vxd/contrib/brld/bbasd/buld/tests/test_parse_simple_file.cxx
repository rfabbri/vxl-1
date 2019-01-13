// \file
// \author Ricardo Fabbri
//
#include <testlib/testlib_test.h>
#include <iostream>
#include <fstream>
#include <buld/buld_parse_simple_file.h>

void test_parse_number_lists()
{

  {
  std::ofstream ofp("number_lists_tmp.txt", std::ios::out);

  if (!ofp) {
    std::cerr << "Error in test, unable to open file" << std::endl;
    return;
  }

  ofp << "# Example of a file\n"
    << "0 2 1 3 4 5\n"
    << "0 2 1 3 5\n";
  }

  std::vector<std::vector<int> > nl;
  bool retval = buld_parse_number_lists("number_lists_tmp.txt", nl);

  TEST("retval", retval, true);
  TEST("num lines", nl.size(), 2u);

  int num_lines_gt[2] = { 6, 5 };

  for (unsigned i=0; i < nl.size(); ++i) {
    std::cout << "Line [" << i << "]:\n" << nl[i].size() << std::endl;
    TEST("num element in line", nl[i].size(), num_lines_gt[i]);
    for (unsigned k=0; k < nl[i].size(); ++k) {
      std::cout << nl[i][k] << ":";
    }
    std::cout << std::endl;
  }
}

MAIN( test_parse_simple_file )
{
   START ("Parse Simple File");

   test_parse_number_lists();
   
   SUMMARY();
}
