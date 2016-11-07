#ifndef cali_artf_corresponder_h
#define cali_artf_corresponder_h

#include <vsol/vsol_conic_2d_sptr.h>
#include <vil/vil_image_resource_sptr.h>
#include <vcl_vector.h>
#include <vnl/vnl_vector.h>
#include <vgl/vgl_point_2d.h>
#include <vgl/vgl_sphere_3d.h>
#include <vcl_string.h>
#include <vcl_utility.h>
#include <vcl_iostream.h>
#include <vcl_fstream.h>
#include <cali/cali_param.h>

typedef vcl_vector<vcl_vector<vsol_conic_2d_sptr> > conic_vector_set;

class cali_artf_corresponder
{
public:
    cali_param par_;
  cali_artf_corresponder(void);

  cali_artf_corresponder(cali_param par);


  ~cali_artf_corresponder(void);
  
  double ellipse_angle(vsol_conic_2d_sptr e) const;
  // generates a vector of conic set pairs
  vcl_vector<vcl_pair<conic_vector_set *, conic_vector_set *> > 
    gen_corresp(vcl_vector<vil_image_resource_sptr> img_vector, 
    vcl_string file_base, int interval);
  

  void diff_corresp_set(vcl_vector<vcl_vector<vsol_conic_2d > > first, 
                        vcl_vector<conic_vector_set> second, 
                        vnl_vector<double> &diff_vector);

  void robust_corresp_set(vcl_vector<vcl_vector<vsol_conic_2d> > first, 
                                             vcl_vector<conic_vector_set> second, 
                                             vnl_vector<double> &diff_vector);

  // finding out the conic correspondences neglecting the ones corresponding to the masked balls 

  void masked_corresp_set(vcl_vector<vcl_vector<vsol_conic_2d> > first, 
                                             vcl_vector<conic_vector_set> second, 
                                             vnl_vector<double> &diff_vector);

  void verbose_error_summary(vcl_vector<vcl_vector<vsol_conic_2d> > first, 
                                             conic_vector_set second, 
                                             vnl_vector<double> &diff_vector);
void new_correspondence(const vcl_vector<vcl_vector<vsol_conic_2d> > &synthetic_conics, 
                                             const conic_vector_set &found_conics, 
                                             vnl_vector<double> &diff_vector) ;




  void manual_corresp_set(vcl_vector<vcl_vector<vsol_conic_2d > > first,vnl_vector<double> &diff_vector);

  conic_vector_set fit_conics(vil_image_resource_sptr img);

  // sorting the conics along the axis of the cylinder artifact 

  conic_vector_set axis_orientation_sort(conic_vector_set second);

  void save_conics_bin(conic_vector_set conics, vcl_string file_name);

  conic_vector_set read_conics_bin(vcl_string file_name);

  static void print(conic_vector_set &list);

  vcl_ostream& print(vcl_ostream&  s, conic_vector_set const &list);

  static vcl_string gen_read_fname(vcl_string file_base, int i);

  static vcl_string gen_write_fname(vcl_string fname, int i);

  conic_vector_set combine_conics(vcl_vector<vsol_conic_2d_sptr> conics) const;
protected:
    

  vcl_vector<vcl_pair<conic_vector_set *, conic_vector_set *> > correspondences;



  conic_vector_set order_conics(conic_vector_set &conics) const;

  conic_vector_set reverse_order_conics(conic_vector_set &conics );

  vcl_ofstream fstream;


};

#endif
