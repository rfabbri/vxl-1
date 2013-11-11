// This is mleotta/cmd/cluster2codebook.cxx

#include <vul/vul_arg.h>
#include <vbl/vbl_triple.h>

#include <modrec/modrec_feature_3d.h>
#include <modrec/modrec_codeword.h>

#include <dbcll/dbcll_euclidean_cluster.h>
#include <dbcll/dbcll_rnn_agg_clustering.h>

#include <vnl/vnl_double_3.h>

#include "cluster_io.h"

#include <modrec/io/modrec_io_codeword.h>


bool less_num_members(const dbcll_cluster_sptr& c1, 
                      const dbcll_cluster_sptr& c2)
{
  if(c1->size() == c2->size())
    return c1->key_index() < c2->key_index();
  return c1->size() < c2->size();
}



// The Main Function
int main(int argc, char** argv)
{
  vul_arg<vcl_string>  a_feat_file("-feat3d", "path to 3d features", "");
  vul_arg<vcl_string>  a_clust_file("-clusters", "path to clusters index file", "");
  vul_arg<vcl_string>  a_code_file("-codebook", "path to output codebook file", "");
  vul_arg_parse(argc, argv);


  vcl_vector<modrec_desc_feature_3d<128> > features;
  typedef vbl_triple<unsigned,unsigned,unsigned> utriple;
  vcl_vector<utriple> idx_array;
  read_features(a_feat_file(), features, idx_array);

  vcl_cout << "read " << features.size() << " features"<<vcl_endl;

  vcl_vector<vcl_vector<unsigned> > clusters_idx;
  read_clusters(a_clust_file(), clusters_idx);

  vcl_cout << "read " << clusters_idx.size() << " clusters"<<vcl_endl;

  vcl_vector<modrec_codeword<128> > codebook;
  vcl_vector<unsigned> sizes;

  for(unsigned i=0; i<clusters_idx.size(); ++i){
    //if(clusters_idx[i].size() < 10)
    //  continue;


    vcl_vector<unsigned> ci = clusters_idx[i];
    vnl_double_3 mpos(0,0,0);
    vnl_double_3 mrot(0,0,0);
    vnl_vector_fixed<double,128> mdesc(0.0);
    double ms = 0.0;
    double vdesc = 0.0, vpos = 0.0, vrot = 0.0, vs = 0.0;
    for(unsigned j=0; j<ci.size(); ++j){
      const modrec_desc_feature_3d<128>& f = features[ci[j]];
      vgl_point_3d<double> pt = f.position();
      vnl_double_3 pos(pt.x(),pt.y(),pt.z());
      vnl_double_3 rot = f.orientation().as_rodrigues();
      ms += f.scale();
      mdesc += f.descriptor();
      mpos += pos;
      mrot += rot;
      vdesc += f.descriptor().squared_magnitude();
      vpos += pos.squared_magnitude();
      vrot += rot.squared_magnitude();
      vs += f.scale()*f.scale();
    }
    mpos /= ci.size();
    mrot /= ci.size();
    mdesc /= ci.size();
    ms /= ci.size();
    vpos -= ci.size() * mpos.squared_magnitude(); vpos /= ci.size();
    vrot -= ci.size() * mrot.squared_magnitude(); vrot /= ci.size();
    vdesc -= ci.size() * mdesc.squared_magnitude(); vdesc /= ci.size();
    vs -= ci.size() * ms*ms; vs /= ci.size();

    //vcl_cout << vdesc<<", "<<vpos<<", "<<vrot<<", "<< vs<<vcl_endl;

    vgl_point_3d<double> p(mpos[0],mpos[1],mpos[2]);
    vgl_rotation_3d<double> r(mrot);
    codebook.push_back(modrec_codeword<128>(p,vpos,r,vrot,ms,vs,mdesc,vdesc));
    sizes.push_back(ci.size());

  }


  vsl_b_ofstream os(a_code_file());
  vsl_b_write(os,codebook);
  vsl_b_write(os,sizes);
  os.close();


  return 0;
}
