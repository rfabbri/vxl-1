//// \file
// \brief basic data structures to represent edges and store information about their matches
// \author vj (vj@lems.brown.edu)
// \date   9/22/2004
// \verbatim
// Modifications
// \endverbatim

#ifndef dbetrk_edge_h_
#define dbetrk_edge_h_
#include <vcl_iostream.h>
#include <vbl/vbl_ref_count.h>
#include <vdgl/vdgl_digital_curve_sptr.h>
#include <vgl/vgl_point_2d.h>
#include <vtol/vtol_edge_2d_sptr.h>
#include <vcl_vector.h>
#include <vsl/vsl_binary_io.h>
#include <vcl_utility.h>
#include <dbetrk/dbetrk_edge_sptr.h>

class edge_arc;
class dbetrk_edge:public vbl_ref_count
{
public:
    dbetrk_edge();
    ~dbetrk_edge();

   vtol_edge_2d_sptr parent_curve_;
   //: set the point 
   void setpoint(vgl_point_2d<double> point){point_=point; }
   //: set theta
   void settheta(double theta){theta_=theta;}
   //: set grad
   void setgrad(double grad){grad_=grad;}

   //: get orientation of the edge
   double gettheta(){return theta_;}
   //: get the gradient of the edge
   double getgrad(){return grad_;}

   void print_summary(vcl_ostream& os) const;
   vgl_point_2d<double> point_;

   double pcolor[3];
   double ncolor[3];

   int frame_;

   vcl_vector<edge_arc * > in_edges;
   vcl_vector<edge_arc * > out_edges;

   //vcl_vector<vcl_pair<double,vcl_vector<vcl_pair<dbetrk_edge_sptr,dbetrk_edge_sptr> > > >  particles;


   vcl_vector<vcl_vector<dbetrk_edge_sptr>  > particles;
  vcl_vector<double>  prob_particles;

   //: Binary save self to stream.
  void b_write(vsl_b_ostream &os) const;

  //: Binary load self from stream.
  void b_read(vsl_b_istream &is);

  //: Return IO version number;
  short version() const;

protected:
    

    double grad_;
    double theta_;
    
};
//: Print a summary of the egde node data to a stream
vcl_ostream& operator<< (vcl_ostream& os, dbetrk_edge const & k);
void vsl_b_write(vsl_b_ostream & os, const dbetrk_edge p);
void vsl_b_read(vsl_b_istream &is, dbetrk_edge &p);

#endif
