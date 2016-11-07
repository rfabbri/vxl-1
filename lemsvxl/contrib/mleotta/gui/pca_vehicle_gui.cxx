#include <vgui/vgui.h>
#include <vgui/vgui_window.h>
#include <vgui/vgui_adaptor.h>
#include <vgui/vgui_shell_tableau.h>
#include <vgui/vgui_grid_tableau.h>
#include <vgui/vgui_easy2D_tableau.h>
#include <vgui/vgui_viewer2D_tableau.h>
#include <vgui/vgui_image_tableau.h>
#include <vgui/vgui_dialog.h>
#include <vgui/vgui_error_dialog.h>
#include <vgui/vgui_menu.h>

#include <bgui3d/bgui3d.h>
#include <bgui3d/bgui3d_project2d_tableau.h>

#include <vil/vil_image_resource.h>
#include <vil/vil_load.h>

#include <vgl/vgl_area.h>

#include <imesh/imesh_fileio.h>
#include <imesh/imesh_mesh.h>
#include <imesh/imesh_detection.h>
#include <imesh/imesh_operations.h>
#include <imesh/algo/imesh_project.h>
#include <modrec/modrec_vehicle_parts.h>

#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoCone.h>
#include <Inventor/nodes/SoMaterial.h>
#include <Inventor/nodes/SoTransform.h>
#include <Inventor/nodes/SoTranslation.h>
#include <Inventor/nodes/SoShapeHints.h>
#include <Inventor/nodes/SoCoordinate3.h>
#include <Inventor/nodes/SoIndexedFaceSet.h>

#include <Inventor/nodes/SoMaterialBinding.h> 
#include <Inventor/nodes/SoIndexedLineSet.h>
#include <Inventor/nodes/SoVertexProperty.h>
#include <bgui3d/bgui3d_examiner_tableau.h>


class pca_vehicle_manager : public vgui_wrapper_tableau
{
  public:
    static pca_vehicle_manager *instance();
    void init();

    typedef vcl_map<vcl_string, vgl_polygon<double> > part_map;

    //: access to the window
    vgui_window* get_window(){return win_;}
    void set_window(vgui_window* win){win_=win;}

    void load_mesh();
    void load_image();
    void load_parts();

    void draw_texmap();
    void draw_parts();

  private:
    void build_mesh_node();
    void build_parts_node(const vcl_vector<vcl_vector<vgl_point_3d<double> > >& parts3d);
    void project_parts(const vcl_vector<vcl_vector<vgl_point_3d<double> > >& parts3d);

    static pca_vehicle_manager *instance_;
    vgui_window* win_;

    SoSeparator* mesh_node_;
    SoSeparator* parts_node_;

    imesh_mesh mesh_;
    part_map parts_;
    vil_image_resource_sptr img_;
    vpgl_perspective_camera<double> cam_;

    bgui3d_examiner_tableau_sptr exam_tab_;
    vgui_easy2D_tableau_sptr proj_tab_;
    vgui_image_tableau_sptr image_tab_;
    vgui_easy2D_tableau_sptr tex_tab_;
    vgui_grid_tableau_sptr grid_;
};

//static manager instance
pca_vehicle_manager *pca_vehicle_manager::instance_ = 0;

//===============================================================
//: The singleton pattern - only one instance of the manager can occur
//==============================================================
pca_vehicle_manager *pca_vehicle_manager::instance()
{
  if (!instance_)
  {
    instance_ = new pca_vehicle_manager();
    instance_->init();
  }
  return pca_vehicle_manager::instance_;
}

//======================================================================
//: Initialize
//======================================================================
void pca_vehicle_manager::init()
{
  mesh_node_ = new SoSeparator;
  mesh_node_->ref();
  parts_node_ = new SoSeparator;
  mesh_node_->ref();
  SoSeparator* root = new SoSeparator;
  root->ref();
  root->addChild(mesh_node_);
  root->addChild(parts_node_);

  exam_tab_ = bgui3d_examiner_tableau_new(root);
  root->unref();

  proj_tab_ = vgui_easy2D_tableau_new();
  image_tab_ = vgui_image_tableau_new();
  tex_tab_ = vgui_easy2D_tableau_new();
  grid_ = vgui_grid_tableau_new(1,2);
  grid_->set_uses_paging_events(false);
  vgui_grid_tableau_new sub_grid(2,1);
  sub_grid->set_uses_paging_events(false);
  grid_->add_at(sub_grid,0,0);
  vgui_composite_tableau_new comp(proj_tab_,image_tab_);
  grid_->add_at(vgui_viewer2D_tableau_new(comp),0,1);
  sub_grid->add_at(exam_tab_,1,0);
  vgui_viewer2D_tableau_new tex_viewer(tex_tab_);
  tex_viewer->zoomin(300,0,0);
  tex_viewer->center_image(0,0);
  sub_grid->add_at(tex_viewer,0,0);
  vgui_shell_tableau_sptr shell = vgui_shell_tableau_new(grid_);

  tex_tab_->set_line_width(1.0f);
  tex_tab_->set_foreground(0.5f,0.5f,0.5f);
  float x[4],y[4];
  x[0] = 0.0f; y[0] = 0.0f;
  x[1] = 1.0f; y[1] = 0.0f;
  x[2] = 1.0f; y[2] = 1.0f;
  x[3] = 0.0f; y[3] = 1.0f;
  tex_tab_->add_polygon(4,x,y);
  tex_tab_->post_redraw();
  this->add_child(shell);

  vgl_point_3d<double> c(2,2,2);
  vpgl_calibration_matrix<double> K(1200,vgl_point_2d<double>(512,386));
  cam_ = vpgl_perspective_camera<double>(K,c,vgl_rotation_3d<double>());
  cam_.look_at(vgl_homg_point_3d<double>(0,0,0.5));
}

//=========================================================================
//: load a mesh
//=========================================================================
void pca_vehicle_manager::load_mesh()
{
  vgui_dialog load_mesh_dlg("Load Mesh");
  static vcl_string filename = "";
  static vcl_string ext = "*.*";
  load_mesh_dlg.file("Mesh Filename:", ext, filename);
  if (!load_mesh_dlg.ask())
    return;

  if(!imesh_read(filename,mesh_)){
    vgui_error_dialog(("Could not load file: "+filename).c_str());
    return;
  }

  imesh_triangulate(mesh_);
  mesh_.compute_face_normals();
  mesh_.build_edge_graph();


  if(mesh_.has_tex_coords())
  {
    draw_texmap();
  }
  else{
    vcl_cerr << "No texture coordinates"<<vcl_endl;
  }


  build_mesh_node();
  exam_tab_->post_redraw();
}

//=========================================================================
//: draw the texture map
//=========================================================================
void pca_vehicle_manager::draw_texmap()
{
  const vcl_vector<vgl_point_2d<double> >& tc = mesh_.tex_coords();
  if(mesh_.has_tex_coords() == imesh_mesh::TEX_COORD_ON_VERT)
  {
    tex_tab_->clear();
    tex_tab_->set_line_width(1.0);
    tex_tab_->set_foreground(0.5f,0.5f,0.5f);
    const imesh_face_array_base& faces = mesh_.faces();
    for(unsigned int i=0; i<faces.size(); ++i)
    {
      const unsigned int num_verts = faces.num_verts(i);
      float x[num_verts], y[num_verts];
      vgl_polygon<float> poly(1);
      for(unsigned int j=0; j<num_verts; ++j)
      {
        x[j] = tc[faces(i,j)].x();
        y[j] = tc[faces(i,j)].y();
        poly.push_back(x[j],y[j]);
      }
      if(vgl_area_signed(poly) > 0.0)
        tex_tab_->add_polygon(num_verts,x,y);
    }
    tex_tab_->post_redraw();
  }
  else{
    vcl_cerr << "Texutre coordinates per corner not supported"<<vcl_endl;
  }
}

//=========================================================================
//: load an image 
//=========================================================================
void pca_vehicle_manager::load_image()
{
  vgui_dialog load_image_dlg("Load Image");
  static vcl_string image_filename = "";
  static vcl_string ext = "*.*";
  load_image_dlg.file("Image Filename:", ext, image_filename);
  if (!load_image_dlg.ask())
    return;

  img_ = vil_load_image_resource(image_filename.c_str());
  image_tab_->set_image_resource(img_);
}

//=========================================================================
//: Vehicle surface parts
//=========================================================================
void pca_vehicle_manager::load_parts()
{
  vgui_dialog load_parts_dlg("Load Parts");
  static vcl_string filename = "";
  static vcl_string ext = "*.*";
  load_parts_dlg.file("Parts Filename:", ext, filename);
  if (!load_parts_dlg.ask())
    return;

  parts_ = modrec_read_vehicle_parts(filename);
  draw_parts();

}

//=========================================================================
//: draw the vehicle parts
//=========================================================================
void pca_vehicle_manager::draw_parts()
{
  tex_tab_->set_line_width(2.0);
  tex_tab_->set_foreground(1.0f,0.0f,0.0f);
  for(part_map::const_iterator itr=parts_.begin(); itr!=parts_.end(); ++itr)
  {
    const vcl_vector<vgl_point_2d<double> >& poly = itr->second[0];
    float x[poly.size()], y[poly.size()];
    for(unsigned int i=0; i<poly.size(); ++i)
    {
      x[i] = poly[i].x();
      y[i] = poly[i].y();
    }
    tex_tab_->add_polygon(poly.size(),x,y);
  }
  tex_tab_->post_redraw();

  if(mesh_.has_tex_coords())
  {
    imesh_mesh mesh = imesh_submesh_from_faces(mesh_,mesh_.faces().group_face_set("body"));
    imesh_triangulate(mesh);
    mesh.build_edge_graph();
    tex_tab_->set_point_radius(3.0);
    tex_tab_->set_foreground(0.0f,1.0f,0.0f);
    vcl_vector<vcl_vector<vgl_point_3d<double> > > parts3d;
    for(part_map::const_iterator itr=parts_.begin(); itr!=parts_.end(); ++itr)
    {
      const vcl_vector<vgl_point_2d<double> >& poly = itr->second[0];

      parts3d.push_back( vcl_vector<vgl_point_3d<double> >() );
      vcl_vector<vgl_point_2d<double> > pts_uv;
      vcl_vector<unsigned long> idx;
      vcl_vector<int> map_back;
      imesh_project_texture_to_barycentric(mesh,poly,pts_uv,idx,map_back);
      for(unsigned int i=0; i<pts_uv.size(); ++i)
      {
        unsigned int fidx = mesh.half_edges()[idx[i]].face_index();
        vgl_point_2d<double> p2 = imesh_project_barycentric_to_texture(mesh,pts_uv[i],fidx);
        tex_tab_->add_point(p2.x(),p2.y());
        vgl_point_3d<double> p3 = imesh_project_barycentric_to_mesh(mesh,pts_uv[i],fidx);
        parts3d.back().push_back(p3);
      }
    }
    tex_tab_->post_redraw();


    build_parts_node(parts3d);
    project_parts(parts3d);
    exam_tab_->post_redraw();
  }
}


void pca_vehicle_manager::build_mesh_node()
{
  mesh_node_->removeAllChildren();

  // coordset
  SoCoordinate3 * coords = new SoCoordinate3;
  mesh_node_->addChild( coords );
  typedef imesh_vertex_array<3>::const_iterator vitr;
  const imesh_vertex_array<3>& verts3d = mesh_.vertices<3>();
  unsigned int idx = 0;
  for(vitr v = verts3d.begin(); v!=verts3d.end(); ++v)
  {
    coords->point.set1Value(idx++, SbVec3f((*v)[0], (*v)[1], (*v)[2]));
  }


  // indexed face set
  SoIndexedFaceSet * ifs = new SoIndexedFaceSet;
  mesh_node_->addChild( ifs );
  const imesh_face_array_base& faces = mesh_.faces();
  idx = 0;
  for(unsigned int f=0; f<faces.size(); ++f)
  {
    for(unsigned int i=0; i<faces.num_verts(f); ++i)
      ifs->coordIndex.set1Value(idx++, faces(f,i));
    ifs->coordIndex.set1Value(idx++, -1);
  }

  exam_tab_->view_all();
}


void pca_vehicle_manager::build_parts_node(const vcl_vector<vcl_vector<vgl_point_3d<double> > >& parts3d)
{
  parts_node_->removeAllChildren();

  // material
  SoMaterial* material = new SoMaterial;
  parts_node_->addChild(material);
  material->diffuseColor.setValue(SbColor(1.0f, 0.0f, 0.0f));

  // coordset
  SoCoordinate3 * coords = new SoCoordinate3;
  parts_node_->addChild( coords );

  // indexed face set
  SoIndexedLineSet * ils = new SoIndexedLineSet;
  parts_node_->addChild( ils );

  unsigned int cidx = 0, iidx=0;
  for(unsigned int i=0; i<parts3d.size(); ++i)
  {
    unsigned int fidx = cidx;
    const vcl_vector<vgl_point_3d<double> >& part = parts3d[i];
    for(unsigned int j=0; j<part.size(); ++j)
    {
      const vgl_point_3d<double>& pt = part[j];
      ils->coordIndex.set1Value(iidx++, cidx);
      coords->point.set1Value(cidx++, SbVec3f(pt.x(),pt.y(),pt.z()));
    }
    ils->coordIndex.set1Value(iidx++, fidx);
    ils->coordIndex.set1Value(iidx++, -1);
  }

  exam_tab_->view_all();
}


void pca_vehicle_manager::project_parts(const vcl_vector<vcl_vector<vgl_point_3d<double> > >& parts3d)
{
  proj_tab_->clear();
  proj_tab_->set_line_width(1.0);
  proj_tab_->set_foreground(1.0f,0.0f,0.0f);
  for(unsigned int i=0; i<parts3d.size(); ++i)
  {
    const vcl_vector<vgl_point_3d<double> >& part = parts3d[i];
    float x[part.size()], y[part.size()];
    for(unsigned int j=0; j<part.size(); ++j)
    {
      vgl_point_2d<double> pt = cam_(vgl_homg_point_3d<double>(part[j]));
      x[j] = pt.x();
      y[j] = pt.y();
    }
    proj_tab_->add_polygon(part.size(),x,y);
  }


  proj_tab_->set_foreground(0.0f,1.0f,0.0f);

#if 0
  vcl_vector<bool> edge = imesh_detect_contours(mesh_, cam_.camera_center());
  vcl_vector<vcl_vector<unsigned int> > loops;
  imesh_trace_half_edge_loops(mesh_.half_edges(),edge,loops);
  for(unsigned int i=0; i<2*mesh_.num_edges(); ++i)
  {
    if(!edge[i])
      continue;
    const imesh_half_edge& he = mesh_.half_edges()[i];
    const imesh_half_edge& he2 = mesh_.half_edges()[he.pair_index()];
    vgl_point_3d<double> pt3 = mesh_.vertices<3>()[he.vert_index()];
    vgl_point_2d<double> pt1 = cam_(vgl_homg_point_3d<double>(pt3));
    pt3 = mesh_.vertices<3>()[he2.vert_index()];
    vgl_point_2d<double> pt2 = cam_(vgl_homg_point_3d<double>(pt3));
    proj_tab_->add_line(pt1.x(),pt1.y(),pt2.x(),pt2.y());
  }
#else
  vcl_vector<vcl_vector<unsigned int> > edge_loops =
      imesh_detect_contour_generator(mesh_, cam_.camera_center());
  for(unsigned int i=0; i<edge_loops.size(); ++i)
  {
    const vcl_vector<unsigned int>& loop = edge_loops[i];
    float x[loop.size()], y[loop.size()];
    for(unsigned int j=0; j<loop.size(); ++j)
    {
      const imesh_half_edge& he = mesh_.half_edges()[loop[j]];
      vgl_point_3d<double> pt3 = mesh_.vertices<3>()[he.vert_index()];
      vgl_point_2d<double> pt = cam_(vgl_homg_point_3d<double>(pt3));
      x[j] = pt.x();
      y[j] = pt.y();
    }
    proj_tab_->add_polygon(loop.size(),x,y);
  }
#endif
}

//=======================================================================================

class pca_vehicle_menus
{
  public:
    static void quit_callback();
    static void load_image_callback();
    static void load_camera_callback();
    static void load_mesh_callback();
    static void load_parts_callback();
    static vgui_menu get_menu();
  private:
    pca_vehicle_menus() {}
};

void pca_vehicle_menus::load_mesh_callback()
{
  pca_vehicle_manager::instance()->load_mesh();
}

void pca_vehicle_menus::load_image_callback()
{
  pca_vehicle_manager::instance()->load_image();
}

void pca_vehicle_menus::load_parts_callback()
{
  pca_vehicle_manager::instance()->load_parts();
}


//menus definition
vgui_menu pca_vehicle_menus::get_menu()
{
  vgui_menu menubar;
  vgui_menu menufile;

  //file menu entries
  menufile.add( "Load Image",  load_image_callback);
  //menufile.add( "Load Camera", load_camera_callback);
  menufile.add( "Load Mesh",   load_mesh_callback);
  menufile.add( "Load Parts",  load_parts_callback);

  //Top level menu layout
  menubar.add( "File", menufile);

  return menubar;
}


//======================================================================
//: Main function
//======================================================================
int main(int argc, char** argv)
{

  // initialize vgui
  vgui::init(argc, argv);

  // initialize bgui_3d
  bgui3d_init();

  vgui_menu menubar = pca_vehicle_menus::get_menu();
  unsigned w = 1000, h = 800;

  vcl_string title = "PCA Vehicle GUI";
  vgui_window* win = vgui::produce_window(w, h, menubar, title);
  win->get_adaptor()->set_tableau(pca_vehicle_manager::instance());
  win->set_statusbar(true);
  win->show();
  pca_vehicle_manager::instance()->set_window(win);
  pca_vehicle_manager::instance()->post_redraw();

  vgui::run();
  return 0;

}
