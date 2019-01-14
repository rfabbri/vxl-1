#ifndef cali_artf_corresponder_h
#define cali_artf_corresponder_h

#include <vsol/vsol_conic_2d_sptr.h>
#include <vil/vil_image_resource_sptr.h>
#include <vector>
#include <vnl/vnl_vector.h>
#include <vgl/vgl_point_2d.h>
#include <vgl/vgl_sphere_3d.h>
#include <string>
#include <utility>
#include <iostream>
#include <fstream>
#include <cali/cali_param.h>

typedef std::vector<std::vector<vsol_conic_2d_sptr> > conic_vector_set;

class cali_artf_corresponder
{
public:
    cali_param par_;
  cali_artf_corresponder(void);

  cali_artf_corresponder(cali_param par);


  ~cali_artf_corresponder(void);
  
  double ellipse_angle(vsol_conic_2d_sptr e) const;
  // generates a vector of conic set pairs
  std::vector<std::pair<conic_vector_set *, conic_vector_set *> > 
    gen_corresp(std::vector<vil_image_resource_sptr> img_vector, 
    std::string file_base, int interval);
  

  void diff_corresp_set(std::vector<std::vector<vsol_conic_2d > > first, 
                        std::vector<conic_vector_set> second, 
                        vnl_vector<double> &diff_vector);

  void robust_corresp_set(std::vector<std::vector<vsol_conic_2d> > first, 
                                             std::vector<conic_vector_set> second, 
                                             vnl_vector<double> &diff_vector);

  // finding out the conic correspondences neglecting the ones corresponding to the masked balls 

  void masked_corresp_set(std::vector<std::vector<vsol_conic_2d> > first, 
                                             std::vector<conic_vector_set> second, 
                                             vnl_vector<double> &diff_vector);

  void verbose_error_summary(std::vector<std::vector<vsol_conic_2d> > first, 
                                             conic_vector_set second, 
                                             vnl_vector<double> &diff_vector);
void new_correspondence(const std::vector<std::vector<vsol_conic_2d> > &synthetic_conics, 
                                             const conic_vector_set &found_conics, 
                                             vnl_vector<double> &diff_vector) ;




  void manual_corresp_set(std::vector<std::vector<vsol_conic_2d > > first,vnl_vector<double> &diff_vector);

  conic_vector_set fit_conics(vil_image_resource_sptr img);

  // sorting the conics along the axis of the cylinder artifact 

  conic_vector_set axis_orientation_sort(conic_vector_set second);

  void save_conics_bin(conic_vector_set conics, std::string file_name);

  conic_vector_set read_conics_bin(std::string file_name);

  static void print(conic_vector_set &list);

  std::ostream& print(std::ostream&  s, conic_vector_set const &list);

  static std::string gen_read_fname(std::string file_base, int i);

  static std::string gen_write_fname(std::string fname, int i);

  conic_vector_set combine_conics(std::vector<vsol_conic_2d_sptr> conics) const;
protected:
    

  std::vector<std::pair<conic_vector_set *, conic_vector_set *> > correspondences;



  conic_vector_set order_conics(conic_vector_set &conics) const;

  conic_vector_set reverse_order_conics(conic_vector_set &conics );

  std::ofstream fstream;


};

#endif
