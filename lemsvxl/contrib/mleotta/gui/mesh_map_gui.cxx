#include <vgui/vgui.h>
#include <vgui/vgui_utils.h>
#include <vgui/vgui_dialog.h>
#include <vgui/vgui_menu.h>
#include <vgui/vgui_shell_tableau.h>
#include <vgui/vgui_viewer2D_tableau.h>
#include <vgui/vgui_image_tableau.h>
#include <vgui/vgui_composite_tableau.h>
#include <vgui/vgui_grid_tableau.h>
#include <vgui/vgui_easy2D_tableau.h>

#include <vul/vul_arg.h>
#include <vcl_ios.h>

#include <bgui/bgui_selector_tableau.h>
#include <bgui3d/bgui3d.h>
#include <bgui3d/bgui3d_project2d_tableau.h>
#include <bgui3d/bgui3d_examiner_tableau.h>
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoShapeHints.h>
#include <Inventor/nodes/SoMaterial.h>
#include <Inventor/nodes/SoCoordinate3.h>
#include <Inventor/nodes/SoTextureCoordinate2.h>
#include <Inventor/nodes/SoTexture2.h>
#include <Inventor/nodes/SoIndexedFaceSet.h>

#include <vil/vil_new.h>

#include <vidl/vidl_frame.h>
#include <vidl/vidl_image_list_istream.h>
#include <vidl/gui/vidl_gui_param_dialog.h>
#include <dbpro/dbpro_delay.h>
#include <dbpro/dbpro_try_option.h>
#include <dbpro/dbpro_executive.h>
#include <dbpro/dbpro_basic_processes.h>
#include <dbvidl2/pro/dbvidl2_source.h>
#include <dbvidl2/pro/dbvidl2_frame_to_resource.h>
#include <dbvidl2/pro/dbvidl2_sink.h>
#include <dbvidl2/pro/dbvidl2_resource_to_frame.h>

#include <vpgl/vpgl_perspective_camera.h>
#include <vgl/vgl_line_3d_2_points.h>
#include <vgl/vgl_plane_3d.h>
#include <vgl/vgl_distance.h>
#include <vgl/vgl_intersection.h>

#include <vnl/vnl_double_3x4.h>
#include <vnl/vnl_double_3x3.h>
#include <vnl/vnl_double_3.h>

#include <dbpro/vis/dbpro_run_tableau.h>

#include <imesh/imesh_fileio.h>
#include <imesh/algo/imesh_project.h>
#include <modrec/modrec_feature_3d.h>

#include <vgui/vgui_style.h>
#include <dbdet/vis/dbdet_keypoint_soview2D.h>
#include <dbdet/dbdet_keypoint_sptr.h>
#include <dbdet/dbdet_keypoint.h>
#include <dbdet/dbdet_lowe_keypoint.h>
#include <vsl/vsl_vector_io.h>

#include "set_file_commands.h"
#include <dbil/vis/dbil_image_observer.h>

#include <dbcll/dbcll_rnn_agg_clustering.h>
#include <dbcll/dbcll_euclidean_cluster.h>


class make_codebook_command : public vgui_command
{
  public:
    make_codebook_command(const dbpro_process_sptr& pro) : process_(pro) {}
    
    void execute()
    {
      dbpro_b_istream_source_base* source = 
        dynamic_cast<dbpro_b_istream_source_base*>(process_.ptr());
      if(!source || !source->is->is().good())
        return;
      
      vcl_vector<vcl_vector<dbdet_keypoint_sptr> > kypts;
      vcl_istream::pos_type pos = source->is->is().tellg();
      vcl_cout << "file position = " << pos << vcl_endl;
      source->is->is().seekg(6,vcl_ios_base::beg);
      vcl_cout << "new file position = " << source->is->is().tellg() << vcl_endl;
      while(source->is->is().good()){
        vcl_vector<dbdet_keypoint_sptr> k;
        vsl_b_read(*source->is, k);
        kypts.push_back(k);
        source->is->is().peek();
      }
      source->is->is().seekg(pos);
      source->is->is().clear();
      vcl_cout << "read " << kypts.size() << vcl_endl;
      
      vcl_vector<vnl_vector_fixed<double,128> > features;
      for(unsigned i=0; i<kypts.size(); ++i){
        for(unsigned j=0; j<kypts[i].size(); ++j){
          dbdet_lowe_keypoint* k = static_cast<dbdet_lowe_keypoint*>(kypts[i][j].ptr());
          features.push_back(k->descriptor());
        }
      }
      vcl_cout << "num features: " << features.size() << vcl_endl;
      
      vcl_vector<dbcll_cluster_sptr> clusters = dbcll_init_euclidean_clusters(features);
      dbcll_remainder_heap remain(clusters.begin(), clusters.end());
      clusters.clear();
      dbcll_rnn_agg_clustering(remain, clusters, -.2);
      vcl_cout << "num clusters = " << clusters.size() << vcl_endl;
      vcl_vector<dbcll_cluster_sptr> good_clusters;
      for(unsigned i=0; i<clusters.size(); ++i){
        const dbcll_euclidean_cluster<128>* c = 
          static_cast<const dbcll_euclidean_cluster<128>*>(clusters[i].ptr());
        if(c->size() > 1)
          good_clusters.push_back(clusters[i]);
      }
      vcl_cout << "num good clusters = " << good_clusters.size() << vcl_endl;
      for(unsigned i=0; i<good_clusters.size(); ++i){
        const dbcll_euclidean_cluster<128>* c = 
          static_cast<const dbcll_euclidean_cluster<128>*>(good_clusters[i].ptr());
        vcl_cout << "size: "<< c->size()
        <<  "  var: "
        << c->var() << vcl_endl;
      }
    }
    
    dbpro_process_sptr process_;
};

//===============================================================

class dbgui_screengrab_filter : public dbpro_filter
{
  public:
    //: Execute this process
    dbpro_signal execute()
    {
      vgui::run_till_idle();
      vil_image_view<vxl_byte> screen = vgui_utils::colour_buffer_to_view();

      output(0, vil_new_image_resource_of_view(screen));
      return DBPRO_VALID;
    }
};


class keypoint_observer: public dbpro_observer
{
  public:
    keypoint_observer(const vgui_easy2D_tableau_sptr& t)
    : tab(t) {}

    //: Called by the process when the data is ready
    virtual bool notify(const dbpro_storage_sptr& data, unsigned long)
    {
      assert(tab);
      assert(data);
      tab->clear();
      if(data->info() == DBPRO_VALID){
        assert(data->type_id() == typeid(vcl_vector<dbdet_keypoint_sptr>));
        vcl_vector<dbdet_keypoint_sptr> keypoints =
          data->data<vcl_vector<dbdet_keypoint_sptr> >();
        
        vgui_style_sptr line_style = vgui_style::new_style(1.0f , 1.0f , 0.0f , 1.0f , 1.0f);
        vgui_style_sptr point_style = vgui_style::new_style(0.0f , 1.0f , 0.0f , 4.0f , 1.0f);
        for( vcl_vector< dbdet_keypoint_sptr >::const_iterator itr = keypoints.begin();
            itr != keypoints.end();  ++itr ){
          if(*itr){
            dbdet_keypoint_soview2D* obj = new dbdet_keypoint_soview2D(*itr,false);
            tab->add( obj );
            obj->set_style( line_style );
            obj->set_point_style( point_style );
          }
        }

      }
      
      tab->post_redraw();
      return true;
    }
    vgui_easy2D_tableau_sptr tab;
};

class mesh_observer: public dbpro_observer
{
  public:
    mesh_observer(const bgui3d_project2d_tableau_sptr& pt,
                  const bgui3d_examiner_tableau_sptr& et)
    : proj_tab(pt), exam_tab(et) 
    { 
      SoNode* root = build_SoNode();
      proj_tab->set_scene_root(root); 
      exam_tab->set_scene_root(root);
      root->unref();
    }
    
    //: Called by the process when the data is ready
    virtual bool notify(const dbpro_storage_sptr& data, unsigned long)
    {
      assert(proj_tab);
      assert(exam_tab);
      assert(data);
      
      if(data->info() != DBPRO_VALID)
        return true;
      
      if(data->type_id() == typeid(vnl_double_3x4))
        update_camera(data->data<vnl_double_3x4>());
      else if(data->type_id() == typeid(imesh_mesh))
        update_mesh(data->data<imesh_mesh>());
      else if(data->type_id() == typeid(vil_image_resource_sptr))
        update_image(data->data<vil_image_resource_sptr>());
      else
        assert(false && "invalid data type");
        
      proj_tab->post_redraw();
      exam_tab->post_redraw();
      return true;
    }
  
    void update_camera(const vnl_double_3x4& cam)
    {
      proj_tab->set_camera(vpgl_proj_camera<double>(cam));
    }
    
    void update_image(const vil_image_resource_sptr& img)
    {
      vil_image_view<vxl_byte> image = img->get_view();
      texture->image.setValue(SbVec2s(image.ni(),image.nj()),1,
                               image.top_left_ptr(),SoSFImage::NO_COPY);
    }
  
    void update_mesh(const imesh_mesh& mesh)
    {
      typedef imesh_vertex_array<3>::const_iterator vitr;
      const imesh_vertex_array<3>& verts3d = mesh.vertices<3>();
      unsigned int idx = 0;
      for(vitr v = verts3d.begin(); v!=verts3d.end(); ++v)
      {
        coords->point.set1Value(idx++, SbVec3f((*v)[0], (*v)[1], (*v)[2]));
      }

      typedef vcl_vector<vgl_point_2d<double> >::const_iterator uv_itr;
      idx = 0;
      for(uv_itr uv = mesh.tex_coords().begin(); uv!=mesh.tex_coords().end(); ++uv)
      {
        tex_coords->point.set1Value(idx++, uv->x(), uv->y());
      }

      const imesh_face_array_base& faces = mesh.faces();
      idx = 0;
      for(unsigned int f=0; f<faces.size(); ++f)
      {
        for(unsigned int i=0; i<faces.num_verts(f); ++i)
          ifs->coordIndex.set1Value(idx++, faces(f,i));
        ifs->coordIndex.set1Value(idx++, -1);
      }

    }
  
    SoNode* build_SoNode()
    {
      SoSeparator *root = new SoSeparator;
      root->ref();
      
      //root->addChild(mesh_xform_);
      
      SoSeparator *group = new SoSeparator;
      root->addChild(group);
      
#if 0     
      SoShapeHints * hints = new SoShapeHints;
      group->addChild( hints );
      hints->vertexOrdering = SoShapeHints::COUNTERCLOCKWISE;
      hints->shapeType = SoShapeHints::SOLID;
      hints->faceType = SoShapeHints::CONVEX;
  
      // Add a red material
      SoMaterial *myMaterial = new SoMaterial;
      myMaterial->diffuseColor.setValue(1.0f, 0.0f, 0.0f);  // Red
      myMaterial->transparency.setValue(0.5);
      group->addChild(myMaterial);
#endif
      
      // texture
      texture = new SoTexture2;
      texture->ref();
      group->addChild(texture);
      
      // coordset
      coords = new SoCoordinate3;
      coords->ref();
      group->addChild( coords );
      
      // texture coordinates
      tex_coords = new SoTextureCoordinate2;
      tex_coords->ref();
      group->addChild( tex_coords );
      
      // indexed face set
      ifs = new SoIndexedFaceSet;
      ifs->ref();
      group->addChild( ifs );
    
      return root;
    }
  
    bgui3d_project2d_tableau_sptr proj_tab;
    bgui3d_examiner_tableau_sptr exam_tab;
    SoIndexedFaceSet * ifs;
    SoTextureCoordinate2 * tex_coords;
    SoCoordinate3 * coords;
    SoTexture2 *texture;
};


class read_mesh_command : public vgui_command
{
  public:
    read_mesh_command(const dbpro_process_sptr& pro) : process_(pro) {}
    
    void execute()
    {
      dbpro_static_source<imesh_mesh>* source =
        dynamic_cast< dbpro_static_source<imesh_mesh>*>(process_.ptr());
      if(source){
        vgui_dialog file_dlg("Select Input File");
        vcl_string regexp("*.ply2"), filename;
        file_dlg.inline_file("Mesh File (PLY2)",regexp,filename);
        vcl_string regexp2("*.uv2"), filename2;
        file_dlg.inline_file("Texture File (UV2)",regexp2,filename2);
        if(!file_dlg.ask())
          return;
        
        if(filename != ""){
          imesh_read_ply2(filename, source->data);
          if(filename2 != ""){
            imesh_read_uv2(filename2, source->data);
          }
        }
      }
    }
    
    dbpro_process_sptr process_;
};


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
      
      vcl_vector<vgl_point_2d<double> > verts2d;
      imesh_project_verts(mesh.vertices<3>(), camera, verts2d);
      
      vcl_vector<modrec_desc_feature_3d<128> > features;
      vgl_point_3d<double> c = camera.get_camera_center();
      for(unsigned int i=0; i<keypoints.size(); ++i){
        vgl_point_3d<double> pt_3d;
        int tri_idx = imesh_project_onto_mesh(mesh, mesh.faces().normals(), verts2d,
                                              camera, *keypoints[i], pt_3d);
        if(tri_idx >= 0){
          dbdet_lowe_keypoint* k = static_cast<dbdet_lowe_keypoint*>(keypoints[i].ptr());
          vgl_vector_3d<double> ray = pt_3d - c; 
          vnl_double_3 z(-ray.x(), -ray.y(), -ray.z());
          z.normalize();
          
          double s = 8.0 * k->scale();
          double o = k->orientation();
          vgl_homg_point_2d<double> pt2(k->x()+s*vcl_cos(o), k->y()+s*vcl_sin(o));
          vgl_line_3d_2_points<double> line = camera.backproject(pt2);
          vgl_point_3d<double> pt_3d2 = vgl_intersection(line,vgl_plane_3d<double>(ray,pt_3d));
          s = vgl_distance(pt_3d, pt_3d2);
          ray = pt_3d2 - pt_3d;
          vnl_double_3 x(ray.x(), ray.y(), ray.z());
          x.normalize();
          assert(dot_product(x,z) < 1e-8);
          vnl_double_3x3 R; R.set_column(0,x);  R.set_column(1,vnl_cross_3d(z,x));  R.set_column(2,z);
          features.push_back(modrec_desc_feature_3d<128>(pt_3d, vgl_rotation_3d<double>(R), 
                                                       s, k->descriptor()));
        }
      }
      
      output(0, features);
      
      
      return DBPRO_VALID;
    }
};


class texture_proj_filter : public dbpro_filter
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
      
      vcl_vector<vgl_point_2d<double> > verts2d;
      imesh_project_verts(mesh.vertices<3>(), camera, verts2d);
      
      vcl_vector<vgl_point_2d<double> > tex_coords;
      for(unsigned int i=0; i<keypoints.size(); ++i){
        vgl_point_2d<double> pt_uv;
        int tri_idx = imesh_project_onto_mesh_texture(mesh, verts2d, camera,
                                                      *keypoints[i], pt_uv);
        if(tri_idx >= 0)
          tex_coords.push_back(pt_uv);
      }

      output(0, tex_coords);
      
      
      return DBPRO_VALID;
    }
};


class hist_image_filter : public dbpro_filter
{
  public:
    hist_image_filter() : image(1024,1024) {}
  
    //: Execute this process
    dbpro_signal execute()
    {
      assert(input_type_id(0) == typeid(vcl_vector<vgl_point_2d<double> >));
      vcl_vector<vgl_point_2d<double> > tex_coords = 
        input<vcl_vector<vgl_point_2d<double> > >(0);
      
      unsigned w = image.ni();
      unsigned h = image.nj();
      
      for(unsigned int k=0; k<tex_coords.size(); ++k){
        unsigned i = static_cast<unsigned>(vcl_floor(w*tex_coords[k].x()));
        unsigned j = static_cast<unsigned>(vcl_floor(h*tex_coords[k].y()));
        if(i < w && j < h)
          image(i,j) += 16;
        else
          vcl_cout << "bad tex point: " << i << ", "<< j << vcl_endl;
      }
      
      output(0, vil_new_image_resource_of_view(image));
      
      return DBPRO_VALID;
    }
    vil_image_view<vxl_byte> image;
};


int main(int argc, char** argv)
{
  
  vul_arg<vcl_string>  a_images("-images", "path to images", "");
  vul_arg<vcl_string>  a_keypoints("-keypoints", "path to keypoints", "");
  vul_arg<vcl_string>  a_cameras("-cameras", "path to cameras", "");
  vul_arg<vcl_string>  a_mesh("-mesh", "path to mesh", "");
  vul_arg<vcl_string>  a_tex_coords("-tex_coords", "path to texture coordinates", "");
  vul_arg<vcl_string>  a_feat3d("-feat3d", "path to feature 3d output", "");
  vul_arg_parse(argc, argv);
  
  vidl_istream_sptr default_vidl2_istream = NULL;
  if(a_images.set()){
    default_vidl2_istream = new vidl_image_list_istream(a_images());
    if (default_vidl2_istream && !default_vidl2_istream->is_open()) {
      default_vidl2_istream = NULL;
    }
  }
  
  imesh_mesh default_mesh;
  if(a_mesh.set()){
     imesh_read_ply2(a_mesh(), default_mesh);
     if(a_tex_coords.set()){
        imesh_read_uv2(a_tex_coords(), default_mesh);
     }
  }

  
  typedef vcl_vector<dbdet_keypoint_sptr> keypoint_vector;
  vsl_add_to_binary_loader(dbdet_keypoint());
  vsl_add_to_binary_loader(dbdet_lowe_keypoint());

  dbpro_executive graph;
#ifndef NDEBUG
  graph.enable_debug();
#endif
  graph["source"]      = new dbvidl2_source(default_vidl2_istream);
  graph["to_resource"] = new dbvidl2_frame_to_resource(dbvidl2_frame_to_resource::REQUIRE_WRAP,
                                                       VIL_PIXEL_FORMAT_BYTE,VIDL_PIXEL_COLOR_UNKNOWN,
                                                       dbvidl2_frame_to_resource::PLANES,
                                                       dbvidl2_frame_to_resource::REUSE_MEMORY);

  graph["null_sink"]   = new dbpro_null_sink();
  
  graph["keypoint_src"]= new dbpro_b_istream_source<keypoint_vector>(a_keypoints());
  
  graph["camera_src"]  = new dbpro_ifstream_list_source<vnl_double_3x4>(a_cameras());
  
  graph["mesh_src"] = new dbpro_static_source<imesh_mesh>(default_mesh);
  
  // Screen capture processes
  graph["screen_grab"] = new dbgui_screengrab_filter();
  graph["to_frame"]    = new dbvidl2_resource_to_frame();
  graph["sink"]        = new dbvidl2_sink(NULL);
  
  graph["proj_tex"]    = new texture_proj_filter();
  graph["hist_img"]    = new hist_image_filter();
  
  typedef vcl_vector<modrec_desc_feature_3d<128> > feat_vector;
  graph["proj_mesh"]   = new mesh_proj_filter();
  graph["feat_3d_snk"] = new dbpro_b_ostream_sink<feat_vector>(a_feat3d());

  // Binary streaming processes
  //graph["binary_if_source"]  = new dbpro_ifb_source();
  graph["track_read_try"]    = new dbpro_try_option();


  //===========================================================================
  // Make the graph connections
  //===========================================================================
  
  graph["null_sink"]   ->connect_input(0,graph["hist_img"],0);
  graph["null_sink"]   ->connect_input(1,graph["to_resource"],0);
  graph["hist_img"]    ->connect_input(0,graph["proj_tex"],0);
  graph["proj_tex"]    ->connect_input(0,graph["keypoint_src"],0);
  graph["proj_tex"]    ->connect_input(1,graph["camera_src"],0);
  graph["proj_tex"]    ->connect_input(2,graph["mesh_src"],0);
  graph["to_resource"] ->connect_input(0,graph["source"],0);
  
  graph["feat_3d_snk"] ->connect_input(0,graph["proj_mesh"],0);
  graph["proj_mesh"]   ->connect_input(0,graph["keypoint_src"],0);
  graph["proj_mesh"]   ->connect_input(1,graph["camera_src"],0);
  graph["proj_mesh"]   ->connect_input(2,graph["mesh_src"],0);

  // Screen capture links
  graph["sink"]        ->connect_input(0,graph["to_frame"],0);
  graph["to_frame"]    ->connect_input(0,graph["screen_grab"],0);
  graph["screen_grab"] ->connect_input(0,graph["to_resource"],0);

  //===========================================================================
  // Make the GUI
  //===========================================================================
  
  // initialize vgui
  vgui::init(argc, argv);
  
  // initialize bgui_3d
  bgui3d_init();

  bgui_selector_tableau_new selector_tab;
  
  vgui_image_tableau_new image_tab;
  selector_tab->add(image_tab, "image");
  graph["to_resource"]->add_output_observer(0,new dbil_image_observer(image_tab));
  
  vgui_easy2D_tableau_new keypoint_tab;
  selector_tab->add(keypoint_tab, "keypoints");
  graph["keypoint_src"]->add_output_observer(0,new keypoint_observer(keypoint_tab));
  
  bgui3d_project2d_tableau_new proj_tab;
  bgui3d_examiner_tableau_new examine_tab;
  selector_tab->add(proj_tab, "mesh projection");
  dbpro_observer_sptr mesh_obs = new mesh_observer(proj_tab,examine_tab);
  graph["mesh_src"]->add_output_observer(0, mesh_obs);
  graph["camera_src"]->add_output_observer(0, mesh_obs);
  graph["hist_img"]->add_output_observer(0, mesh_obs);

  //vgui_viewer2D_tableau_new view_tab(selector_tab);
  
  vgui_image_tableau_new texture_tab;
  graph["hist_img"]->add_output_observer(0,new dbil_image_observer(texture_tab));

  vgui_grid_tableau_new grid_tab(2,1);
  vgui_grid_tableau_new sub_grid_tab(1,2);
  sub_grid_tab->add_at(examine_tab, 0,0);
  sub_grid_tab->add_at(vgui_viewer2D_tableau_new(texture_tab), 0,1);
  grid_tab->add_at(vgui_viewer2D_tableau_new(selector_tab), 0,0);
  grid_tab->add_at(sub_grid_tab, 1,0);

  dbpro_run_tableau_sptr dbpro_tab = dbpro_run_tableau_new(graph);

  //vgui_composite_tableau_new comp_tab(dbpro_tab, view_tab);
  vgui_composite_tableau_new comp_tab(dbpro_tab, grid_tab);


  // Put a shell tableau at the top of our tableau tree.
  vgui_shell_tableau_new shell(comp_tab);

  vgui_menu menu_bar, file_menu, video_menu, data_menu, cmd_menu;
  video_menu.add("Input", new set_istream_command(graph["source"]));
  video_menu.add("Output", new set_ostream_command(graph["sink"]));
  file_menu.add("Open Video Stream",video_menu);

  data_menu.add("Keypoint Input", new set_b_istream_command(graph["keypoint_src"]));
  data_menu.add("Camera Input", new set_ifstream_command(graph["camera_src"]));
  data_menu.add("Mesh File", new read_mesh_command(graph["mesh_src"]));
  data_menu.add("Feature 3D Output", new set_b_ostream_command(graph["feat_3d_snk"]));
  file_menu.add("Open Data Stream",data_menu);
  
  cmd_menu.add("Build Codebook", new make_codebook_command(graph["keypoint_src"]));
  menu_bar.add("Commands",cmd_menu);
  
  menu_bar.add("Streams",file_menu);

  // Create a window, add the tableau and show it on screen.
  return vgui::run(shell, 1000, 800, menu_bar);
}
