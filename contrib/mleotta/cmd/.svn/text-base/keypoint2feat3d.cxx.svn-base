
#include <vul/vul_arg.h>
#include <vcl_ios.h>


#include <dbpro/dbpro_executive.h>
#include <dbpro/dbpro_basic_processes.h>
#include <dbpro/dbpro_ios_processes.h>
#include <dbpro/dbpro_vsl_processes.h>

#include <vpgl/vpgl_perspective_camera.h>
#include <vgl/vgl_line_3d_2_points.h>
#include <vgl/vgl_plane_3d.h>
#include <vgl/vgl_distance.h>
#include <vgl/vgl_intersection.h>

#include <vnl/vnl_double_3x4.h>
#include <vnl/vnl_double_3x3.h>
#include <vnl/vnl_double_3.h>

#include <imesh/imesh_fileio.h>
#include <imesh/algo/imesh_project.h>
#include <modrec/modrec_feature_3d.h>

#include <dbdet/dbdet_keypoint_sptr.h>
#include <dbdet/dbdet_keypoint.h>
#include <dbdet/dbdet_lowe_keypoint.h>
#include <dbdet/dbdet_surf_keypoint.h>
#include <vsl/vsl_vector_io.h>


template <unsigned dim>
void backproject_keypoints(const vpgl_perspective_camera<double>& camera,
                           const imesh_mesh& mesh,
                           const vcl_vector<dbdet_keypoint_sptr>& keypoints,
                           vcl_vector<modrec_desc_feature_3d<dim> >& features,
                           vcl_vector<bool>& on_surface,
                           double radius = 8.0)
{

  vcl_vector<vgl_point_2d<double> > verts2d;
  imesh_project_verts(mesh.vertices<3>(), camera, verts2d);

  vgl_point_3d<double> c = camera.get_camera_center();
  typedef vcl_vector<dbdet_keypoint_sptr>::const_iterator kitr;
  for(kitr k=keypoints.begin(); k<keypoints.end(); ++k){
    vgl_point_3d<double> pt_3d;
    int tri_idx = imesh_project_onto_mesh(mesh, mesh.faces().normals(), verts2d,
                                          camera, **k, pt_3d);
    if(tri_idx >= 0){
      vgl_vector_3d<double> ray = pt_3d - c;
      vnl_double_3 z(-ray.x(), -ray.y(), -ray.z());
      z.normalize();

      double s = radius * (*k)->scale();
      double o = (*k)->orientation();
      vgl_homg_point_2d<double> pt2((*k)->x()+s*vcl_cos(o), (*k)->y()+s*vcl_sin(o));
      vgl_line_3d_2_points<double> line = camera.backproject(pt2);
      vgl_point_3d<double> pt_3d2 = vgl_intersection(line,vgl_plane_3d<double>(ray,pt_3d));
      s = vgl_distance(pt_3d, pt_3d2);
      ray = pt_3d2 - pt_3d;
      vnl_double_3 x(ray.x(), ray.y(), ray.z());
      x.normalize();
      assert(dot_product(x,z) < 1e-8);
      assert((*k)->descriptor().size() == dim);
      vnl_double_3x3 R; R.set_column(0,x);  R.set_column(1,vnl_cross_3d(z,x));  R.set_column(2,z);
      features.push_back(modrec_desc_feature_3d<dim>(pt_3d, vgl_rotation_3d<double>(R),
                                                     s, (*k)->descriptor()));
      on_surface.push_back(true);
    }
    else
      on_surface.push_back(false);
  }

}

template <unsigned dim>
class mesh_proj_filter : public dbpro_filter
{
  public:
    //: Execute this process
    dbpro_signal execute()
    {
      assert(input_type_id(0) == typeid(vcl_vector<dbdet_keypoint_sptr>));
      vcl_vector<dbdet_keypoint_sptr> keypoints = 
      input<vcl_vector<dbdet_keypoint_sptr> >(0);

      assert(input_type_id(1) == typeid(vnl_double_3x4));
      vnl_double_3x4 cam = input<vnl_double_3x4>(1);

      assert(input_type_id(2) == typeid(imesh_mesh));
      imesh_mesh mesh = input<imesh_mesh>(2);

      vpgl_perspective_camera<double> camera;
      vpgl_perspective_decomposition(cam,camera);

      vcl_vector<modrec_desc_feature_3d<dim> > features;
      vcl_vector<bool> on_surface;
      backproject_keypoints(camera,mesh,keypoints,features,on_surface);


      output(0, features);
      output(1, on_surface);

      return DBPRO_VALID;
    }
};


//: Read surf keypoints from an ifstream
class dbpro_surf_ifstream_source : public dbpro_ifstream_source_base
{
  public:
    dbpro_surf_ifstream_source(const vcl_string& filename) { open(filename.c_str()); }

    bool open(const vcl_string& filename)
    {
      ifs.open(filename.c_str());
      return ifs.is_open();
    }

    //: Execute the process
    dbpro_signal execute()
    {
      if(ifs.eof()){
        return DBPRO_EOS;
      }
      unsigned d,n;
      ifs >> d >> n;
      if(ifs.eof()){
        return DBPRO_EOS;
      }
      vcl_cout << "size: " << n<<vcl_endl;
      assert(d == 64);
      vcl_vector<dbdet_keypoint_sptr> keypoints;
      for(unsigned i=0; i<n; ++i){
        dbdet_surf_keypoint* s = new dbdet_surf_keypoint;
        ifs >> *s;
        keypoints.push_back(s);
      }
      output(0, keypoints);
      return DBPRO_VALID;
    }

    vcl_ifstream ifs;
};



int main(int argc, char** argv)
{
  vul_arg<vcl_string>  a_keypoints("-keypoints", "path to keypoints", "");
  vul_arg<bool>        a_surf("-surf", "expect surf keypoints (in ASCII)", false);
  vul_arg<vcl_string>  a_cameras("-cameras", "path to cameras", "");
  vul_arg<vcl_string>  a_mesh("-mesh", "path to mesh", "");
  vul_arg<vcl_string>  a_feat3d("-feat3d", "path to feature 3d output", "");
  vul_arg<vcl_string>  a_on_surf("-on_surf", "path to \"on surface\" flag output", "");
  vul_arg_parse(argc, argv);


  imesh_mesh default_mesh;
  if(a_mesh.set()){
     imesh_read_ply2(a_mesh(), default_mesh);
  }


  typedef vcl_vector<dbdet_keypoint_sptr> keypoint_vector;
  vsl_add_to_binary_loader(dbdet_keypoint());
  vsl_add_to_binary_loader(dbdet_lowe_keypoint());

  dbpro_executive graph;
#ifndef NDEBUG
  graph.enable_debug();
#endif

  if(a_surf.set()){
    graph["keypoint_src"]= new dbpro_surf_ifstream_source(a_keypoints());
    typedef vcl_vector<modrec_desc_feature_3d<64> > feat_vector;
    graph["proj_mesh"]   = new mesh_proj_filter<64>();
    graph["feat_3d_snk"] = new dbpro_b_ostream_sink<feat_vector>(a_feat3d());
  }else{
    graph["keypoint_src"]= new dbpro_b_istream_source<keypoint_vector>(a_keypoints());
    typedef vcl_vector<modrec_desc_feature_3d<128> > feat_vector;
    graph["proj_mesh"]   = new mesh_proj_filter<128>();
    graph["feat_3d_snk"] = new dbpro_b_ostream_sink<feat_vector>(a_feat3d());
  }


  graph["on_surf_snk"] = new dbpro_b_ostream_sink<vcl_vector<bool> >(a_on_surf());
  graph["camera_src"]  = new dbpro_ifstream_list_source<vnl_double_3x4>(a_cameras());
  graph["mesh_src"] = new dbpro_static_source<imesh_mesh>(default_mesh);



  //===========================================================================
  // Make the graph connections
  //===========================================================================

  graph["on_surf_snk"] ->connect_input(0,graph["proj_mesh"],1);
  graph["feat_3d_snk"] ->connect_input(0,graph["proj_mesh"],0);
  graph["proj_mesh"]   ->connect_input(0,graph["keypoint_src"],0);
  graph["proj_mesh"]   ->connect_input(1,graph["camera_src"],0);
  graph["proj_mesh"]   ->connect_input(2,graph["mesh_src"],0);


  //===========================================================================
  // Run the processing graph
  //===========================================================================

  graph.init();
  graph.run_all();

  return 0;
}
