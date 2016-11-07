// This is mleotta/cmd/cluster2patchimg.cxx

#include <vul/vul_arg.h>
#include <vcl_iomanip.h>

#include "cluster_io.h"




// The Main Function
int main(int argc, char** argv)
{
  vul_arg<vcl_string>  a_patch_file("-patches", "path to patches", "");
  vul_arg<vcl_string>  a_on_surf("-on_surf", "path to \"on surface\" flags", "");
  vul_arg<vcl_string>  a_clust_file("-clusters", "path to clusters index file", "");
  vul_arg<int>         a_range_min("-min", "minimum cluster index", 1);
  vul_arg<int>         a_range_max("-max", "minimum cluster index", 20);
  vul_arg_parse(argc, argv);

  vcl_vector<vcl_vector<unsigned> > clusters_idx;
  read_clusters(a_clust_file(), clusters_idx);

  unsigned max = 0;
  for(unsigned i=0; i<clusters_idx.size(); ++i){
    for(unsigned j=0; j<clusters_idx[i].size(); ++j){
      if(clusters_idx[i][j] > max)
        max = clusters_idx[i][j];
    }
  }
  vcl_cout << "num features: "<< max+1 << vcl_endl;

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

  for(unsigned n=a_range_min(); n<=a_range_max(); ++n){
    vcl_stringstream name;
    name << "patch" << vcl_setfill('0') << vcl_setw(2) << n << ".png";

    vcl_vector<vil_image_view<vxl_byte> > matches;
    vcl_vector<unsigned> c = *(clusters_idx.end()-n);
    for(unsigned i=0; i<c.size(); ++i){
      matches.push_back(patches[c[i]]);
    }

    write_patch_image(name.str(),matches);

  }

  return 0;
}
