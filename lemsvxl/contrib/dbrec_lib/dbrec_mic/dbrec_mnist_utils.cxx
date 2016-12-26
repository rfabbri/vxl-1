//:
// \file
// \author Ozge C Ozcanli (ozge@lems.brown.edu)
// \date 06/22/09
//
//
#include "dbrec_mnist_utils.h"

#include <vcl_iostream.h>
#include <vsl/vsl_binary_io.h>
#include <vil/vil_save.h>
#include <vil/vil_load.h>
#include <vil/vil_flip.h>
#include <vil/vil_rotate.h>
#include <vil/vil_image_view.h>
#include <vcl_vector.h>
#include <bsta/bsta_histogram.h>
#include <bsta/bsta_joint_histogram.h>
#include <bsta/vis/bsta_svg_tools.h>

void convert_mnist_files(const vcl_string& path, int cnt, const vcl_string& label_filename, const vcl_string& out_path)
{
  vcl_cout << "reading label file in " << path << vcl_endl;
  vcl_vector<int> labels(cnt, 0);
  vcl_vector<int> cnts(10, 0);
  vcl_ifstream ifs(label_filename.c_str());
  for (int i = 0; i < cnt; i++) {
    int lbl;
    ifs >> lbl;
    cnts[lbl]++;
    labels[i] = lbl;
  }
  for (unsigned i = 0; i < 10; i++) 
    vcl_cout << "class: " << i << " cnt: " << cnts[i] << vcl_endl;
  vcl_vector<int> counters(10, 0);
  for (int i = 0; i < cnt; i++) {
    vcl_stringstream ss; ss << i+1;
    vcl_string name = path + "img_"+ss.str()+".png";
    vil_image_view<vxl_byte> img = vil_load(name.c_str());
    //: flip the image
    vil_image_view<vxl_byte> img_new(img.nj(), img.ni(), img.nplanes());
    vil_rotate_image(img,img_new,90.0);
    vil_image_view<vxl_byte> img_new_flipped = vil_flip_lr(img_new);
    vcl_stringstream sss; sss << labels[i] << "_" << counters[labels[i]] << ".png";
    counters[labels[i]]++;
    vcl_string out_name = out_path + "img_"+sss.str();
    vil_save(img_new_flipped, out_name.c_str());
  }
}

void read_stat_file_gamma(const vcl_string& file, int gamma_interval, int gamma_range, const vcl_string out_path)
{
  vcl_ifstream ifs(file.c_str());

  //: prepare two gamma histograms [-gamma_interval/2, gamma_range-gamma_interval/2] and [0,gamma_range]
  int gamma_nbins = gamma_range/gamma_interval;
  bsta_histogram<float> h_gamma1(-gamma_interval/2.0f,gamma_range-gamma_interval/2.0f,gamma_nbins);
  bsta_histogram<float> h_gamma2(0,(float)gamma_range,gamma_nbins);

  while (true) {
    float mag; int d, gamma, rho; 
    ifs >> mag;  if (ifs.eof()) break;
    ifs >> d;    
    ifs >> rho; 
    ifs >> gamma; 
    //vcl_cout << " read mag: " << mag << " d: " << d << " rho: " << rho << " gamma: " << gamma << vcl_endl;

    h_gamma2.upcount(gamma, mag);
    if (gamma > gamma_range-gamma_interval/2.0f)
      h_gamma1.upcount(gamma_range-gamma, mag);
    else
      h_gamma1.upcount(gamma, mag);

  }
  vcl_stringstream g_int_str; g_int_str << gamma_interval;
  vcl_string name1 = out_path + "_gamma_interval_"+g_int_str.str()+"_hist1.svg";
  write_svg<float>(h_gamma1, name1, 600.0f, 600.0f, 30.0f, 15.0f);
  vcl_string name2 = out_path + "_gamma_interval_"+g_int_str.str()+"_hist2.svg";
  write_svg<float>(h_gamma2, name2, 600.0f, 600.0f, 30.0f, 15.0f);

  ifs.close();
}
void read_stat_file_d_rho(const vcl_string& file, int gamma_min, int gamma_max, int gamma_range, int d_interval, int d_range, int rho_interval, int rho_range, const vcl_string out_path)
{
  vcl_ifstream ifs(file.c_str());
  
  int d_nbins = d_range/d_interval;

  //: prepare two rho histograms [-rho_interval/2, rho_range-rho_interval/2] and [0,rho_range]
  int rho_nbins = rho_range/rho_interval;
  
  bsta_joint_histogram<float> h1(-rho_interval/2.0f,rho_range-rho_interval/2.0f,rho_nbins, 0.0, d_range, d_nbins);
  bsta_joint_histogram<float> h2(0,(float)rho_range,rho_nbins, 0.0, d_range, d_nbins);

  int gamma_interval = gamma_max - gamma_min;

  while (true) {
    float mag; int d, gamma, rho; 
    ifs >> mag;  if (ifs.eof()) break;
    ifs >> d;    
    ifs >> rho; 
    ifs >> gamma; 
    //vcl_cout << " read mag: " << mag << " d: " << d << " rho: " << rho << " gamma: " << gamma << vcl_endl;

    if (gamma_min < 0) {
      if ((gamma > gamma_range-gamma_interval/2.0f && gamma <= gamma_range) || (gamma > 0 && gamma <= gamma_max)) {
        h2.upcount(rho, mag, d, 0.0f);
        if (rho > rho_range-rho_interval/2.0f)
          h1.upcount(rho_range-rho, mag, d, 0.0f);
        else
          h1.upcount(rho, mag, d, 0.0f);
      }
    } else {
      if (gamma > gamma_min && gamma <= gamma_max) {
        h2.upcount(rho, mag, d, 0.0f);
        if (rho > rho_range-rho_interval/2.0f)
          h1.upcount(rho_range-rho, mag, d, 0.0f);
        else
          h1.upcount(rho, mag, d, 0.0f);
      }
    }
  }

  ifs.close();

  vcl_stringstream r_int_str; r_int_str << rho_interval;
  vcl_stringstream g_min_str; g_min_str << gamma_min;
  vcl_stringstream g_max_str; g_max_str << gamma_max;
  vcl_string name1 = out_path + "_gamma_min_"+g_min_str.str()+"_max_"+g_max_str.str()+"_rho_interval_"+r_int_str.str()+"_hist1.vrml";
  vcl_ofstream ofs(name1.c_str());
  h1.print_to_vrml(ofs);
  ofs.close();
  vcl_string name2 = out_path + "_gamma_min_"+g_min_str.str()+"_max_"+g_max_str.str()+"_rho_interval_"+r_int_str.str()+"_hist2.vrml";
  vcl_ofstream ofs2(name2.c_str());
  h2.print_to_vrml(ofs2);
  ofs2.close();
  
}

