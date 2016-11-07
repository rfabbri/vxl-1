// This is mleotta/cmd/cluster2vrml.cxx

#include <vul/vul_arg.h>
#include <vcl_iomanip.h>
#include <vbl/vbl_triple.h>

#include <modrec/modrec_feature_3d.h>
#include <modrec/io/modrec_io_desc_feature_3d.h>

#include "cluster_io.h"


void write_vrml_feature(vcl_ostream& os, const modrec_feature_3d& f, unsigned view)
{
  double colors[][3] = {{1,0,0}, {.5,0,0}, {1,1,0}, {1,.5, 0}, {.5, 1, 0}, {.5, .5, 0},
    {0,1,0}, {0,.5,0}, {0,0,1}, {0,0,.5}, {.5,0,.5}, {0, .5, .5}, {.5, 0, 1}};
  //vcl_cout << "Z = " << f.orientation() * vgl_vector_3d<double>(0,0,1) << vcl_endl;
  vgl_point_3d<double> pos = f.position();
  vnl_vector_fixed<double, 3> axis = f.orientation().axis();
  double angle = f.orientation().angle();
  double length = f.scale()*2;
  os << "Transform {\n"
     << "   translation " << pos.x()<<' '<<pos.y()<<' '<<pos.z()<<'\n'
     << "   rotation " << axis[0]<<' '<<axis[1]<<' '<<axis[2]<<' '<<angle<<'\n'
     << "   children [\n"
     << "      Shape {\n"
     << "         appearance Appearance {\n"
     << "            material Material {\n"
     << "               specularColor 1 1 1\n"
     << "               diffuseColor 1 1 0\n"
     << "               ambientIntensity 0\n"
     << "               transparency 0\n"
     << "               shininess 0.2\n"
     << "            }\n"
     << "         }\n"
     << "         geometry Sphere { radius 0.02 }\n"
     << "      }\n"
     << "      Transform {\n"
     << "         translation " << 0<<' '<<0<<' '<<length/2<<'\n'
     << "         rotation " << 1<<' '<<0<<' '<<0<<' '<<1.570796326794897<<'\n'
     << "         children [\n"
     << "            Shape {\n"
     << "               appearance Appearance {\n"
     << "                  material Material {\n"
     << "                     specularColor 1 1 1\n"
     << "                     diffuseColor "<<colors[view][0]<<' '<<colors[view][1]<<' '<<colors[view][2]<<'\n'
     << "                     ambientIntensity 0\n"
     << "                     transparency 0\n"
     << "                     shininess 0.2\n"
     << "                  }\n"
     << "               }\n"
     << "               geometry Cylinder { height "<<length<<" radius 0.01 }\n"
     << "            }\n"
     << "         ]\n"
     << "      }\n"
     << "      Transform {\n"
     << "         translation " << length/4<<' '<<0<<' '<<length<<'\n'
     << "         rotation " << 0<<' '<<0<<' '<<1<<' '<<1.570796326794897<<'\n'
     << "         children [\n"
     << "            Shape {\n"
     << "               appearance Appearance {\n"
     << "                  material Material {\n"
     << "                     specularColor 1 1 1\n"
     << "                     diffuseColor 1 0 1\n"
     << "                     ambientIntensity 0\n"
     << "                     transparency 0\n"
     << "                     shininess 0.2\n"
     << "                  }\n"
     << "               }\n"
     << "               geometry Cylinder { height "<<length/2<<" radius 0.01 }\n"
     << "            }\n"
     << "         ]\n"
     << "      }\n"
     << "   ]\n"
     << "}\n\n";

}


template <unsigned dim>
void write_vrml(const vcl_string& feat_file,
                const vcl_string& clust_file, int min, int max)
{
  vcl_vector<modrec_desc_feature_3d<dim> > features;
  typedef vbl_triple<unsigned,unsigned,unsigned> utriple;
  vcl_vector<utriple> idx_array;  
  read_features(feat_file, features, idx_array);

  vcl_cout << "read " << features.size() << vcl_endl;

  vcl_vector<vcl_vector<unsigned> > clusters_idx;
  read_clusters(clust_file, clusters_idx);


  for(unsigned n=min; n<=max; ++n){
    vcl_stringstream name;
    name << "features" << vcl_setfill('0') << vcl_setw(2) << n << ".wrl";
    vcl_ofstream ofs(name.str().c_str());
    vcl_vector<unsigned> c = *(clusters_idx.end()-n);
    ofs << "#VRML V2.0 utf8\n\n";
    for(unsigned i=0; i<c.size(); ++i){
      write_vrml_feature(ofs, features[c[i]], idx_array[c[i]].first);
    }
    ofs.close();

  }
}

// The Main Function
int main(int argc, char** argv)
{
  vul_arg<vcl_string>  a_feat_file("-feat3d", "path to 3d features", "");
  vul_arg<vcl_string>  a_clust_file("-clusters", "path to clusters index file", "");
  vul_arg<int>         a_range_min("-min", "minimum cluster index", 1);
  vul_arg<int>         a_range_max("-max", "minimum cluster index", 20);
  vul_arg<unsigned>    a_dim("-dim", "feature descriptor dimension", 128);
  vul_arg_parse(argc, argv);

  switch(a_dim()){
  case 64:
    {
      write_vrml<64>(a_feat_file(), a_clust_file(), a_range_min(), a_range_max());
    }
    break;
  case 128:
    {
      write_vrml<128>(a_feat_file(), a_clust_file(), a_range_min(), a_range_max());
    }
    break;
  default:
      vcl_cerr << "features with dimension "<<a_dim()<<" not supported" << vcl_endl;
  }



  return 0;
}
