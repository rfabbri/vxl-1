// This is mleotta/cmd/cluster2mat.cxx

#include <vul/vul_arg.h>
#include <vbl/vbl_triple.h>
#include <vcl_iomanip.h>

#include <modrec/modrec_feature_3d.h>

#include <vnl/vnl_double_3.h>
#include <vnl/vnl_matlab_filewrite.h>

#include "cluster_io.h"


// The Main Function
int main(int argc, char** argv)
{
  vul_arg<vcl_string>  a_feat_file("-feat3d", "path to 3d features", "");
  vul_arg<vcl_string>  a_clust_file("-clusters", "path to clusters index file", "");
  vul_arg_parse(argc, argv);

  vcl_vector<modrec_desc_feature_3d<128> > features;
  typedef vbl_triple<unsigned,unsigned,unsigned> utriple;
  vcl_vector<utriple> idx_array;  
  read_features(a_feat_file(), features, idx_array);

  vcl_cout << "read " << features.size() << vcl_endl;

  vcl_vector<vcl_vector<unsigned> > clusters_idx;
  read_clusters(a_clust_file(), clusters_idx);

  for(unsigned n=1; n<=20; ++n){
    vcl_vector<unsigned> c = *(clusters_idx.end()-n);
    vcl_stringstream name;
    name <<"features" << vcl_setfill('0') << vcl_setw(2) <<  n << ".mat";
    vnl_matlab_filewrite mfs(name.str().c_str());
    vnl_matrix<double> desc_M(c.size(),128);
    vnl_matrix<double> pos_M(c.size(),3);
    vnl_matrix<double> ornt_M(c.size(),9);
    vnl_vector<double> scale_v(c.size());
    for(unsigned i=0; i<c.size(); ++i){
      const modrec_desc_feature_3d<128>& f = features[c[i]];
      desc_M.set_row(i,f.descriptor());
      vnl_double_3x3 R = f.orientation().as_matrix();
      ornt_M.set_row(i,vnl_vector<double>(R.data_block(),9));
      pos_M.set_row(i,vnl_double_3(f.position().x(),f.position().y(),f.position().z()));
      scale_v[i] = f.scale();
    }
    mfs.write(desc_M,"D");
    mfs.write(ornt_M,"R");
    mfs.write(pos_M,"p");
    mfs.write(scale_v,"s");

  }


  return 0;
}
