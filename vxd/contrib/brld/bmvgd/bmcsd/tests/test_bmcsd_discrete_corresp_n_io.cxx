#include <testlib/testlib_test.h>
#include <bmcsd/bmcsd_discrete_corresp_3.h>
#include <bmcsd/bmcsd_discrete_corresp_n.h>
#include <iostream>
#include <limits>
#include <set>
#include <vnl/vnl_math.h>

#include <iterator>
#include <algorithm>

static const double tolerance=std::numeric_limits<double>::epsilon()*100;

static void test_discrete_corresp_n();



//: Currently tests both IO and other functions of bmcsd_discrete_corresp_3
MAIN( test_discrete_corresp_n_io )
{
  START ("bmcsd_discrete_corresp_n io");

  test_discrete_corresp_n();

  SUMMARY();
}

void
test_discrete_corresp_n()
{
  std::vector<unsigned> npts;
  npts.push_back(7);
  npts.push_back(8);
  npts.push_back(8);
  bmcsd_discrete_corresp_n gt(npts);
  
  bmcsd_ntuplet tup(3);

  tup[0] = 1; tup[1] = 2; tup[2] = 3; gt.l_.put(tup,bmcsd_match_attribute());
  tup[0] = 2; tup[1] = 2; tup[2] = 2; gt.l_.put(tup,bmcsd_match_attribute());
  tup[0] = 2; tup[1] = 3; tup[2] = 3; gt.l_.put(tup,bmcsd_match_attribute());

  std::cout << gt << std::endl;

//  double inf = std::numeric_limits<double>::infinity();
  bmcsd_discrete_corresp_n cp(npts);
  tup[0] = 1; tup[1] = 2; tup[2]= 3; cp.l_.put(tup,bmcsd_match_attribute(false,0));
  tup[0] = 2; tup[1] = 2; tup[2]= 2; cp.l_.put(tup,bmcsd_match_attribute(false,23.2));

  // Equality operator

  TEST("Equality operator", cp == gt, false);
  TEST("Equality operator", cp == cp, true);


  //----- BINARY I/O ----------------------------------------------------------

  std::cout << "Initial data:" << std::endl;
  std::cout << cp;

  // ------ Writing -------
  vsl_b_ofstream bfs_out("bmcsd_discrete_corresp_n.tmp");
  TEST("Created bmcsd_discrete_corresp_n.tmp for writing", (!bfs_out), false);
  vsl_b_write(bfs_out, cp);
  bfs_out.close();

  // ------ Reading -------

  bmcsd_discrete_corresp_n cp_in;

  vsl_b_ifstream bfs_in("bmcsd_discrete_corresp_n.tmp");
  TEST("Opened bmcsd_discrete_corresp_n.tmp for reading", (!bfs_in), false);
  vsl_b_read(bfs_in, cp_in);
  TEST("Finished reading file successfully", (!bfs_in), false);
  bfs_in.close();

  TEST("cp_out == cp_in", cp, cp_in);

  std::cout << "Recovered data:" << std::endl;
  std::cout << cp_in;
}



#if 0
void
test_discrete_corresp_3()
{
  std::cout << "\nTesting statistics ----------------------------------------\n";
  bmcsd_discrete_corresp_3 gt(7,8,8);

  gt.l_.put(1,2,3,bmcsd_match_attribute());
  gt.l_.put(2,2,2,bmcsd_match_attribute());
  gt.l_.put(2,3,3,bmcsd_match_attribute());
  gt.l_.put(5,3,7,bmcsd_match_attribute());
  gt.l_.put(4,4,4,bmcsd_match_attribute());
  gt.l_.put(3,4,4,bmcsd_match_attribute());


  double inf = std::numeric_limits<double>::infinity();
  bmcsd_discrete_corresp_3 cp(7,8,8);
  cp.l_.put(1,2,3,bmcsd_match_attribute(false,0));
  cp.l_.put(2,2,2,bmcsd_match_attribute(false,23.2));
  cp.l_.put(4,5,6,bmcsd_match_attribute(false,4.45));
  cp.l_.put(4,4,4,bmcsd_match_attribute(false,29.8));
  cp.l_.put(4,7,1,bmcsd_match_attribute(false,10.33));

  //: Infinite cost doesn't count.
  cp.l_.put(5,3,7,bmcsd_match_attribute(false,inf));

  //: empty g-t doesn't count.
  cp.l_.put(6,7,4,bmcsd_match_attribute(false,10.33));

  // compare
  cp.compare_and_print(&gt);

  /* 
     # Points in image 1 having correct candidates ranked among top 50% matches:
        71.4286% (5 out of 7).
     # Points in image 1 having correct candidates in top 5 matches:
        85.7143% (6 out of 7).
  */


  //----------------------------------------------
  // Test for new function that gives % correspondences having cost above right
  // ones


  unsigned n_correct, n_valid;
  cp.number_of_correct_triplets(n_correct, n_valid, &gt);

  std::cout << "n_correct: " << n_correct << " n_valid: " << n_valid << std::endl;
  TEST("number_of_correct_triplets:  n_correct",n_correct,3);
  TEST("number_of_correct_triplets:  n_valid",n_valid,5);

  std::vector<bool> p0s, p1s, p2s;
  gt.participating_points(p0s,p1s,p2s);

  std::cout << "[ ";
  for (unsigned i=0; i <p0s.size(); ++i)
    std::cout << p0s[i] << " "; 
  std::cout << "]\n";



  //: All triplets are correct if we compare ground-truth against itself
  gt.number_of_correct_triplets(n_correct, n_valid, &gt);
  TEST("number_of_correct_triplets ground-truth must be 100%:",n_correct,n_valid);

  // Equality operator

  TEST("Equality operator", cp == gt, false);
  TEST("Equality operator", cp == cp, true);

  //----- Hashed fast access ---------------------------------------------------


  TEST("Isnt Hashed",cp.is_hashed(),false);
  cp.hash();
  TEST("Is Hashed",cp.is_hashed(),true);

  const std::set<triplet_uuu> &s4xx = cp.triplets(0,4);

  std::cout << "All triplets in cp whose first index is 4: ";
  std::copy(s4xx.begin(), s4xx.end(), std::ostream_iterator<trip_uuu>(std::cout, " "));
  std::cout << std::endl;


  gt.hash();
  std::set<triplet_uuu> sx44;

  gt.triplets(1,4,2,4,sx44);

  std::cout << "All triplets in gt whose second and third indices are both 4: ";
  std::copy(sx44.begin(), sx44.end(), std::ostream_iterator<trip_uuu>(std::cout, " "));
  std::cout << std::endl;


  //----- BINARY I/O ----------------------------------------------------------

  std::cout << "Initial data:" << std::endl;
  std::cout << cp;

  // ------ Writing -------
  vsl_b_ofstream bfs_out("bmcsd_discrete_corresp_3.tmp");
  TEST("Created bmcsd_discrete_corresp_3.tmp for writing", (!bfs_out), false);
  vsl_b_write(bfs_out, cp);
  bfs_out.close();

  // ------ Reading -------

  bmcsd_discrete_corresp_3 cp_in;

  vsl_b_ifstream bfs_in("bmcsd_discrete_corresp_3.tmp");
  TEST("Opened bmcsd_discrete_corresp_3.tmp for reading", (!bfs_in), false);
  vsl_b_read(bfs_in, cp_in);
  TEST("Finished reading file successfully", (!bfs_in), false);
  bfs_in.close();

  TEST("cp_out == cp_in", cp, cp_in);

  std::cout << "Recovered data:" << std::endl;
  std::cout << cp_in;

}

void 
test_set_triplet()
{ // A brief sets refresher:
  typedef vbl_triple<unsigned,unsigned,unsigned> trip_uuu;
  std::set<trip_uuu > s1;

  s1.insert(trip_uuu(1,2,3));
  s1.insert(trip_uuu(3,2,1));
  s1.insert(trip_uuu(1,2,4));

  std::cout << "Set 1: ";
  std::copy(s1.begin(), s1.end(), std::ostream_iterator<trip_uuu>(std::cout, " "));
  std::cout << std::endl;

  std::set<trip_uuu > s2;
  s2.insert(trip_uuu(4,1,1));
  s2.insert(trip_uuu(3,2,1));
  s2.insert(trip_uuu(69,1,1));
  s2.insert(trip_uuu(1,2,3));

  std::cout << "Set 2: ";
  std::copy(s2.begin(), s2.end(), std::ostream_iterator<trip_uuu>(std::cout, " "));
  std::cout << std::endl;

  std::set<trip_uuu> s3;
  std::set_intersection(s1.begin(),s1.end(), s2.begin(), s2.end(), inserter(s3, s3.begin()));

  std::cout << "Set 3 (Set 1 intersection with Set 2): ";
  std::copy(s3.begin(), s3.end(), std::ostream_iterator<trip_uuu>(std::cout, " "));
  std::cout << std::endl;
}
#endif
