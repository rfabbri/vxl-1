#include <cali/cali_conic_info.h>

#include <vsol/vsol_conic_2d.h>

short cali_conic_info::version() const {
  return 1;
}

void cali_conic_info::b_write(vsl_b_ostream &os) const  {
  vsl_b_write(os, version());
  vsl_b_write(os, scan_start);
  vsl_b_write(os, scan_interval);
  //vsl_b_write(os, this->conic_list);

  vsl_b_write(os, (int)this->conic_list.size());
  for(unsigned int i=0;i<this->conic_list.size();i++){
    vsl_b_write(os, (int)this->conic_list[i].size());
    for(unsigned int j=0;j<this->conic_list[i].size();j++)
      conic_list[i][j]->b_write(os);
  }
}

void cali_conic_info::b_read(vsl_b_istream &is){
  int size, set_size;
  short ver;
  
  vcl_vector<vcl_vector<vsol_conic_2d_sptr> > list;

  if (!is)
    return;
  vsl_b_read(is, ver);
  switch(ver)
  {
  case 1:
     vsl_b_read(is, scan_start);
     vsl_b_read(is, scan_interval);
     vsl_b_read(is, size);
     list.resize(size);
     for(int i=0;i<size;i++){
       vsl_b_read(is, set_size);
       for(int j=0;j<set_size;j++){
        vsol_conic_2d_sptr conic = new vsol_conic_2d();
        conic->b_read(is);
        list[i].push_back(conic);
      }
    }
    conic_list = list;
    break;

  default:
    vcl_cerr << "I/O ERROR: calib_conic_info::b_read(vsl_b_istream&)\n"
             << "           Unknown version number "<< ver << '\n';
    is.is().clear(vcl_ios::badbit); // Set an unrecoverable IO error on stream*/
    return;
  }
 }

