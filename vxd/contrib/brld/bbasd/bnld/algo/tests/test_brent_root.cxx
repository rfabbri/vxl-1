#include <testlib/testlib_test.h>
#include <bnld/algo/bnld_brent_root.h>
#include <iostream>
#include <vnl/vnl_math.h>



class linear_fnt: public vnl_cost_function
{
public:
  linear_fnt(double a, double b): vnl_cost_function(1), a_(a), b_(b){};
  virtual ~linear_fnt(){};
  double f(vnl_vector<double> const& x)
  {
    double t = x[0];
    return (a_*t + b_);
  }
protected:
  double a_;
  double b_;
};

MAIN( test_brent_root )
{
   START ("newton_root");
   linear_fnt f0(2, 50);
   double x1 = 0;
   double x2 = 1;
  
   bnld_brent_root root_solver(f0);
   double root = -1000;
   root_solver.solve(-100, 100, root);

   std::cout << "Function f(x) = 2x + 50\n";
   std::cout << "Root x0 = " << root << std::endl;

   TEST("solve f(x) = ", std::abs(root+25)<1e-12, true); 
   SUMMARY();
}
