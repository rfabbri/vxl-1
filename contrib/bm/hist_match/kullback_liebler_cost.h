//: Brandon Mayer
//: Implementation of Kullback-Libler cost function for
//: Histogram matching.
#include <vnl/vnl_cost_function.h>
#include <vcl_math.h>

class kullback_libler_cost: public vnl_cost_function 
{ 
public:
 kullback_libler_cost(int num_unknowns,vnl_vector<double> ref_hist,vnl_vector<double> hist2):
  vnl_cost_function(num_unknowns),ref_hist_(ref_hist),hist2_(hist2){}
  
  ~kullback_libler_cost(){}

  //: The cost Function:
  double f(vnl_vector<double> const & x)
  {
    double c = 0;
    //: x[0] = gain
    //: x[1] = offset

    vnl_vector<double> aprx_hist;
    aprx_hist = hist1*x[0] + x[1];
    for(unsigned i = 0; i < hist1.size(); ++i)
    {
      c = c + (aprx_hist[i]*log10(aprx_hist[i]/hist2[i]))
    }
    
    
  }
 protected:
  vnl_vector<double> ref_hist_, hist2_;
};
