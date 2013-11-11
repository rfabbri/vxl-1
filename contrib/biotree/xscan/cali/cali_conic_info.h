#ifndef cali_conic_info_h
#define cali_conic_info_h

#include <vsol/vsol_conic_2d_sptr.h>
#include <vsl/vsl_vector_io.h>
#include <vsl/vsl_list_io.h>
#include <vsl/vsl_binary_io.h>
#include <vcl_vector.h>
#include <vcl_list.h>

class cali_conic_info {
private: 
  vcl_vector<vcl_vector<vsol_conic_2d_sptr> > conic_list;
  
  int scan_start;
  int scan_interval;
public:
  cali_conic_info(){};
  cali_conic_info(vcl_vector<vcl_vector<vsol_conic_2d_sptr> >  const &list, 
    int start, 
    int interval) :
  conic_list(list), scan_start(start), scan_interval(interval){};
  
  ~cali_conic_info(){};

  vcl_vector<vcl_vector<vsol_conic_2d_sptr> > get_conic_list() {return conic_list;}

  short version() const;

  void  b_write(vsl_b_ostream &os) const;

  void b_read(vsl_b_istream &is);
};
#endif
