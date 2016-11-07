// This is mleotta/cmd/learn_patch_basis.cxx

#include <vul/vul_arg.h>
#include <vcl_iomanip.h>

#include "cluster_io.h"

#include <vnl/vnl_vector.h>
#include <vnl/vnl_matrix.h>
#include <vnl/algo/vnl_generalized_eigensystem.h>
#include <vnl/vnl_matlab_filewrite.h>


vnl_vector<double> patch_to_vector(const vil_image_view<vxl_byte>& patch)
{
  vnl_vector<double> v(768);
  for(unsigned p=0; p<3; ++p)
    for(unsigned j=0; j<16; ++j)
      for(unsigned i=0; i<16; ++i)
        v(p*256+j*16+i) = patch(i,j,p)/255.0;

  return v;
}


vil_image_view<vxl_byte> vector_to_patch(const vnl_vector<double>& v)
{
  double minv = -1.0;
  double maxv = 1.0;
  vil_image_view<vxl_byte> patch(16,16,3);
  for(unsigned p=0; p<3; ++p)
    for(unsigned j=0; j<16; ++j)
      for(unsigned i=0; i<16; ++i){
        double val = (v(p*256+j*16+i)-minv)*255.0/(maxv-minv);
        if(val <0.0) val = 0.0;
        if(val > 255.0) val = 255.0;
        patch(i,j,p) = static_cast<vxl_byte>(val);
      }

  return patch;
}



// The Main Function
int main(int argc, char** argv)
{
  vul_arg<vcl_string>  a_patch_file("-patches", "path to patches", "");
  vul_arg<vcl_string>  a_on_surf("-on_surf", "path to \"on surface\" flags", "");
  vul_arg<vcl_string>  a_clust_file("-clusters", "path to clusters index file", "");
  vul_arg_parse(argc, argv);

  vcl_vector<vcl_vector<unsigned> > clusters_idx;
  read_clusters(a_clust_file(), clusters_idx);

  unsigned long num_clusters = 0, num2_clusters = 0;
  for(unsigned i=0; i<clusters_idx.size(); ++i){
    unsigned s = clusters_idx[i].size();
    num_clusters += s;
    num2_clusters += s*s;
  }
  vcl_cout << "num features: "<< num_clusters << vcl_endl;

  vcl_vector<vil_image_view<vxl_byte> > patches;
  read_patches(a_patch_file(),patches);

  vcl_cout << "read "<<patches.size()<< " patches" << vcl_endl;

  if(a_on_surf.set()){
    // read on surface flags
    vcl_vector<bool> on_surf;
    read_on_surface_flags(a_on_surf(),on_surf);
    vcl_cout << "read "<<on_surf.size()<<" flags"<<vcl_endl;
    // keep only patches that are on surface (i.e. have a 3d feature)
    vcl_vector<vil_image_view<vxl_byte> > new_patches;
    for(unsigned i=0; i<on_surf.size(); ++i)
      if(on_surf[i])
        new_patches.push_back(patches[i]);
    patches.swap(new_patches);

    vcl_cout << "kept "<<patches.size()<< " patches" << vcl_endl;
  }


  double n1 = static_cast<double>(num_clusters);
  double n2 = static_cast<double>(num2_clusters);
  double mix = -(n1*n1-n2)/(n2-n1);
  double alpha = 2.0/(n2 - n1) + mix*2.0/(n1*n1-n2);
  double beta = - mix*2.0/(n1-n2/n1);


  vcl_cout << "alpha = "<< alpha << " beta = "<<beta << " mix = "<<mix<<vcl_endl;
  vcl_cout << "num clusters = "<< clusters_idx.size() << vcl_endl;

  vnl_vector<double> mean(768,0.0);
  vnl_matrix<double> Sw(768,768,0.0);
  vnl_matrix<double> Sb(768,768,0.0);
  
  vcl_vector<vnl_vector<double> > c_means(clusters_idx.size(),vnl_vector<double>(768,0.0));
  for(unsigned i=0; i<clusters_idx.size(); ++i){
    const vcl_vector<unsigned>& c = clusters_idx[i];
    vnl_vector<double>& c_mean = c_means[i];
    const unsigned c_size = c.size();
    //double c_alpha = alpha*c_size + beta;
    vcl_vector<vnl_vector<double> > c_data(c.size());
    for(unsigned j=0; j<c_size; ++j){
      c_data[j] = patch_to_vector(patches[c[j]]);
      c_mean += c_data[j];
    }
    mean += c_mean;
    if(c_size ==1)
      continue;

    c_mean /= c_size;
    for(unsigned j=0; j<c_size; ++j){
      vnl_vector<double> v = c_data[j] - c_mean;
      //scatter += c_alpha*outer_product(v,v);
      //fast scaled outer product increment
      // lower triangular part only
      for(unsigned k=0; k<768; ++k)
        for(unsigned l=0; l<=k; ++l)
          Sw(k,l) += v(k)*v(l);
    }
    if(i%100 == 0)
      vcl_cout << "processed within "<< i << vcl_endl;
  }
  mean /= num_clusters;

  for(unsigned i=0; i<clusters_idx.size(); ++i){
    vnl_vector<double> v = c_means[i]-mean;
    const unsigned c_size = clusters_idx[i].size();
    //double c_beta = beta * clusters_idx[i].size();
    //scatter += c_beta*outer_product(v,v);
    //fast scaled outer product increment
    // lower triangular part only
    for(unsigned k=0; k<768; ++k)
      for(unsigned l=0; l<=k; ++l)
        Sb(k,l) += c_size*v(k)*v(l);
    if(i%100 == 0)
      vcl_cout << "processed between "<< i << vcl_endl;
  }

  // copy lower triangular to upper triangular
  for(unsigned k=0; k<768; ++k){
    for(unsigned l=k+1; l<768; ++l){
      Sw(k,l) = Sw(l,k);
      Sb(k,l) = Sb(l,k);
    }
  }

  vcl_cout << "Solving Eigensystem" << vcl_endl;
  vnl_generalized_eigensystem ES(Sb,Sw);

  const vnl_matrix<double>& V = ES.V;

  vnl_vector<double> sv = ES.D.diagonal();

  vnl_matlab_filewrite mf("basis.mat");
  mf.write(Sw,"Sw");
  mf.write(Sb,"Sb");
  mf.write(V,"V");
  mf.write(sv,"d");

  vcl_vector<vil_image_view<vxl_byte> > basis_patches;
  for(unsigned n=0; n<768; ++n){
    vcl_cout << sv(768-n-1) << vcl_endl;
    vnl_vector<double> v = V.get_column(768-n-1);
    basis_patches.push_back(vector_to_patch(v));
  }

  write_patch_image("basis.png",basis_patches);

  return 0;
}
