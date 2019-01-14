#include "mw_app.h"
#include "epip_app.h"
#include <string>
#include <sstream>
#include <iostream>

#include <mw/mw_point_matcher.h>

int
main(int argc, char **argv)
{

//   read_cam("curr/p1010049.jpg","curr/p1010053.jpg");
//   std::string dir, pref;
//   mw_get_prefix("curr/p1010046.jpg",&dir,&pref);
//   point_transfer_app();
//   test_point_reconstruct_rig();
//   test_point_reconstruct();
//  test_k_formula_circle();
//   example_project_and_reconstruct();
//   trinocular_match();

//  test_formulas_circle();
//  test_formulas_helix();
//  test_formulas_space_curve1();


//  test_geometry_numerics();
//  test_geometry_numerics2();

  unsigned n_iter=1, n_iter_positional=0;
  bool circular=false;

  if (argc > 1) {
    {
    std::istringstream istream;
    istream.str(argv[1]);

    istream >> n_iter;
    }

    if (argc > 2) {
      std::istringstream istream;
      istream.str(argv[2]);
      istream >> n_iter_positional;
      if (argc > 3) {
        circular = true;
      }
    }
    
  }
    
  std::cout << "Executing " << n_iter << " descent iterations\n";
  std::cout << "and " << n_iter_positional << " positional (smoothing) descent iterations\n";

  if (!circular)
    test_geometry_numerics2_2(n_iter,n_iter_positional);
  else
    arc_positional_descent_test(n_iter,n_iter_positional);

  return 0;
}
