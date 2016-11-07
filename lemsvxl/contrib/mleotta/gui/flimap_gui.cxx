#include <vgui/vgui.h>
#include <vgui/vgui_menu.h>
#include <vgui/vgui_dialog.h>
#include <vgui/vgui_command.h>
#include <vgui/vgui_shell_tableau.h>
#include <vgui/vgui_composite_tableau.h>
#include <vgui/vgui_grid_tableau.h>
#include <vgui/vgui_viewer2D_tableau.h>
#include <vgui/vgui_projection_inspector.h>

#include <vcl_fstream.h>
#include <vnl/vnl_double_3.h>
#include <vpgl/vpgl_perspective_camera.h>

#include <bgui3d/bgui3d.h>
#include <bgui3d/bgui3d_examiner_tableau.h>
#include <bgui3d/bgui3d_project2d_tableau.h>
#include <Inventor/nodes/SoSeparator.h>

#include <Inventor/nodes/SoTransform.h>
#include <Inventor/nodes/SoMaterialBinding.h>
#include <Inventor/nodes/SoBaseColor.h>
#include <Inventor/nodes/SoCoordinate3.h>
#include <Inventor/nodes/SoIndexedFaceSet.h>
#include <Inventor/nodes/SoPointSet.h>
#include <Inventor/nodes/SoDirectionalLight.h>
#include <Inventor/nodes/SoDrawStyle.h>
#include <Inventor/nodes/SoLightModel.h>

#include <Inventor/VRMLnodes/SoVRMLCoordinate.h>
#include <Inventor/VRMLnodes/SoVRMLIndexedFaceSet.h>
#include <Inventor/VRMLnodes/SoVRMLPointSet.h>
#include <Inventor/VRMLnodes/SoVRMLColor.h>
#include <Inventor/VRMLnodes/SoVRMLShape.h>
#include <Inventor/actions/SoWriteAction.h>


class data_manager
{
public:
  //: Access the static instance of the tableau manager
  static data_manager * instance();

  SoBaseColor* point_colors() const { return p_colors_; }
  SoCoordinate3* point_coords() const { return p_coords_; }

  vcl_vector<unsigned int> selection() const { return selection_; }

  void set_selection(const vcl_vector<unsigned int>& s) { selection_ = s; }

  void read_file();

  void save_selection();

private:
  static data_manager* instance_;

  void init();
  data_manager(){}

  SoBaseColor* p_colors_;
  SoCoordinate3* p_coords_;
  vcl_vector<unsigned int> selection_;
};


data_manager* data_manager::instance_ = 0;


//: Access the static instance of the tableau manager
data_manager * data_manager::instance()
{
  if (!instance_){
    instance_ = new data_manager();
    instance_->init();
  }
  return data_manager::instance_;
}

void data_manager::init()
{
  p_colors_ = new SoBaseColor();
  p_colors_->ref();

  p_coords_ = new SoCoordinate3();
  p_coords_->ref();
}


// Read the ASCII point file
void data_manager::read_file()
{
  vgui_dialog vrml_dlg("Load ASCII LIDAR file");
  static vcl_string filename = "", ext = "*.xyz";
  static unsigned int start = 0;
  static unsigned int n_pts = 500000;
  vrml_dlg.file("LIDAR file",ext, filename);
  vrml_dlg.field("start",start);
  vrml_dlg.field("num point",n_pts);
  if(!vrml_dlg.ask())
    return;

  vcl_ifstream fh(filename.c_str());
  if(fh.is_open()){
    //check for lvcs
    char c;
    c = fh.get();
    if(c=='#')
      {
        double lat=0, lon=0, elev=0;
        vcl_string buf;
        fh >> buf;//skip lvcs
        fh >> buf;
        if(buf=="lat:")
          fh >> lat;
        fh >> buf;
        if(buf=="lon:")
          fh >> lon;
        fh >> buf;
        if(buf=="elev:")
          fh >> elev;
        vcl_cout << "\nGeographic Origin: lat:" << lat << " lon:" 
                 << lon << " elev(meters):" << elev << '\n';
        fh.ignore(1024,'\n');
      }
    else
      fh.unget();

    // find the first line starting with a digit
    c = fh.get();
    while(c < '0' || c > '9')
    {
      fh.ignore(1024,'\n');
      c = fh.get();
    }
    fh.unget();

    unsigned int val;
    double time;
    unsigned i=0;
    for(unsigned j=0; j<start; ++j)
      fh.ignore(1024,'\n');

    float (*coords)[3] = new float[n_pts][3];
    float (*colors)[3] = new float[n_pts][3];
    while(i < n_pts && fh >> coords[i][0]){
      fh.ignore(1,',');
      fh >> coords[i][1]; fh.ignore(1,',');
      fh >> coords[i][2]; fh.ignore(1,',');
      fh >> colors[i][0]; fh.ignore(1,',');
      fh >> colors[i][1]; fh.ignore(1,',');
      fh >> colors[i][2]; fh.ignore(1,',');
      fh >> val; fh.ignore(1,','); // intensity
      fh >> time; fh.ignore(1,','); // time
      fh >> val; fh.ignore(1,','); // scan
      fh >> val; fh.ignore(1,','); // beam
      fh >> val;                   // return
      colors[i][0] /= 255.0f;
      colors[i][1] /= 255.0f;
      colors[i][2] /= 255.0f;
      ++i;
    }
    vcl_cout << "last point: "<<i<<vcl_endl;
    p_coords_->point.setValues(0, i, coords);
    p_coords_->point.deleteValues(i);
    p_colors_->rgb.setValues(0, i, colors);
    p_colors_->rgb.deleteValues(i);
    p_colors_->rgb.touch();
    p_coords_->point.touch();

    delete [] coords;
    delete [] colors;
  }
  selection_.clear();
}

// Save the selection as an ASCII point file
void data_manager::save_selection()
{
  vgui_dialog save_dlg("Save Selection");
  static vcl_string filename = "", ext = "*.xyz";
  static bool vrml = false, recenter=false;
  save_dlg.file("File",ext, filename);
  save_dlg.checkbox("Save as VRML",vrml);
  save_dlg.checkbox("Recenter",recenter);
  if(!save_dlg.ask())
    return;

  SbVec3f mean(0,0,0);
  if(recenter){
    for(unsigned int i=0; i<selection_.size(); ++i){
      unsigned int j = selection_[i];
      const SbVec3f& pt = p_coords_->point[j];
      mean += pt;
    }
    mean /= selection_.size();
  }

  if(vrml){
    SoVRMLShape *newroot = new SoVRMLShape;
    newroot->ref();

    SoVRMLColor* vrml_color = new SoVRMLColor;
    vrml_color->color.connectFrom(&p_colors_->rgb);

    SoVRMLCoordinate* vrml_coord = new SoVRMLCoordinate;
    vrml_coord->point.connectFrom(&p_coords_->point);

    //SoVRMLIndexedFaceSet* points = new SoVRMLIndexedFaceSet;
    SoVRMLPointSet* points = new SoVRMLPointSet;
    points->coord.setValue(vrml_coord);
    points->color.setValue(vrml_color);
    //points->coordIndex.setValue();
    //points->colorPerVertex.setValue(true);
    newroot->geometry.setValue(points);

    for(unsigned int i=0; i<selection_.size(); ++i){
      unsigned int j = selection_[i]; 
      vrml_color->color.set1Value(i,p_colors_->rgb[j]);
      vrml_coord->point.set1Value(i,p_coords_->point[j]-mean);
    }

    SoOutput out;
    out.openFile(filename.c_str());
    out.setHeaderString("#VRML V2.0 utf8");
    SoWriteAction wra(&out);
    wra.apply(newroot);
    out.closeFile();

    newroot->unref();
  }
  else{
    vcl_ofstream fh(filename.c_str());
    fh.precision(10);
    if(fh.is_open()){
      for(unsigned int i=0; i<selection_.size(); ++i){
        unsigned int j = selection_[i];
        SbVec3f pt = p_coords_->point[j]-mean;
        const SbVec3f& cl = p_colors_->rgb[j];
        unsigned int r = static_cast<unsigned int>(cl[0]*255.0);
        unsigned int g = static_cast<unsigned int>(cl[1]*255.0);
        unsigned int b = static_cast<unsigned int>(cl[2]*255.0);
        fh << pt[0]<<','<<pt[1]<<','<<pt[2]<<','
           << r<<','<<g<<','<<b<<",0\n";
      }
    }
  }
}

// Construct a simple scene
void buildScene(SoGroup *root)
{

  root->addChild(new SoDirectionalLight);


  {
    SoSeparator *group = new SoSeparator;
    data_manager* manager = data_manager::instance();

    // draw style
    SoDrawStyle * dstyle = new SoDrawStyle;
    dstyle->style.setValue( SoDrawStyle::POINTS );
    dstyle->pointSize.setValue( 2 );
    group->addChild( dstyle );

    // light model
    SoLightModel * lmodel = new SoLightModel;
    lmodel->model.setValue( SoLightModel::BASE_COLOR );
    group->addChild( lmodel );

    // color
    SoBaseColor * bcolor = manager->point_colors();
    group->addChild( bcolor );

    // material binding
    SoMaterialBinding * matbind = new SoMaterialBinding;
    matbind->value.setValue( SoMaterialBinding::PER_VERTEX );
    group->addChild( matbind );

    // coordset
    SoCoordinate3 * coords = manager->point_coords();
    group->addChild( coords );

    // pointset
    SoPointSet * pointset = new SoPointSet;
    group->addChild( pointset );


    root->addChild(group);
  }

}


class select_points_tableau : public vgui_tableau
{
public:
  select_points_tableau(const bgui3d_tableau_sptr& tab3d)
    : draw_mode_(false), x1(0), y1(0), last_x(0), last_y(0), tab3d_(tab3d) {}

  //: Handle all events sent to this tableau.
  bool handle(const vgui_event& e)
  {
    if(e.type == vgui_DRAW){
      if(draw_mode_ && !(x1==0 && y1==0) ){
        glColor3f(1,0,1);
        glBegin(GL_LINE_LOOP);
        glVertex2f(x1, y1);
        glVertex2f(x1, last_y);
        glVertex2f(last_x, last_y);
        glVertex2f(last_x, y1);
        glEnd();
      }


      SoCoordinate3 * coords = data_manager::instance()->point_coords();
      vcl_auto_ptr<vpgl_proj_camera<double> > cam = tab3d_->camera();
      bool inverted_camera = (dynamic_cast<bgui3d_project2d_tableau*>(tab3d_.ptr()) == 0);
      vpgl_perspective_camera<double>* pcam = cam->cast_to_perspective_camera();
      vcl_vector<unsigned int> selection = data_manager::instance()->selection();
      int num_pts = selection.size();

      glColor3f(0,1,0);
      glPointSize(2.0);
      glBegin(GL_POINTS);
      for(int i=0; i<num_pts; ++i){
        unsigned int j = selection[i];
        const SbVec3f& pt = coords->point[j];
        vgl_homg_point_3d<double> X(pt[0],pt[1],pt[2]);
        if(pcam && inverted_camera == pcam->is_behind_camera(X)){
          vgl_homg_point_2d<double> x = (*cam)(X);
          if(!x.ideal()){
            glVertex2d(x.x()/x.w(), x.y()/x.w());
          }
        }
      }
      glEnd();

      return true;
    }


    //vgui_projection_inspector().print(vcl_cout);

    if(e.type == vgui_MOUSE_MOTION){
      float ix, iy;
      vgui_projection_inspector().window_to_image_coordinates(e.wx, e.wy, ix, iy);
      last_x = ix;
      last_y = iy;

      return false;
    }

    if(e.type == vgui_KEY_PRESS && e.key == 's'){
      draw_mode_ = true;
      x1 = y1 = 0;
      return true;
    }
    if(e.type == vgui_MOUSE_DOWN && draw_mode_){
      x1 = last_x;
      y1 = last_y;
      return true;
    }
    if(e.type == vgui_MOUSE_UP && draw_mode_){
      draw_mode_ = false;  
      vcl_auto_ptr<vpgl_proj_camera<double> > cam = tab3d_->camera();
      bool inverted_camera = (dynamic_cast<bgui3d_project2d_tableau*>(tab3d_.ptr()) == 0);

      vpgl_perspective_camera<double>* pcam = cam->cast_to_perspective_camera();

      vnl_double_3x4 P = cam->get_matrix();
      if(inverted_camera){
        P *= -1;
      }
      vnl_double_4 P1 = P.get_row(0);
      vnl_double_4 P2 = P.get_row(1);
      vnl_double_4 P3 = P.get_row(2);

      if(x1 > last_x) vcl_swap(x1,last_x);
      if(y1 > last_y) vcl_swap(y1,last_y);

      vnl_double_4 B1 = P1 - double(x1)*P3;
      vnl_double_4 B2 = double(last_x)*P3 - P1;
      vnl_double_4 B3 = P2 - double(y1)*P3;
      vnl_double_4 B4 = double(last_y)*P3 - P2;

      SoCoordinate3 * coords = data_manager::instance()->point_coords();
      int num_pts = coords->point.getNum();
      vcl_vector<unsigned int> selection;
      for(int i=0; i<num_pts; ++i){
        const SbVec3f& pt = coords->point[i];
        vnl_double_4 X(pt[0],pt[1],pt[2],1);

        if(pcam && inverted_camera != pcam->is_behind_camera(vgl_homg_point_3d<double>(pt[0],pt[1],pt[2],1)))
          continue;
        if(dot_product(B1,X) > 0 && dot_product(B2,X) > 0 &&
           dot_product(B3,X) > 0 && dot_product(B4,X) > 0)
          selection.push_back(i);
      }

      data_manager::instance()->set_selection(selection);
      vgui::out << "selected "<<selection.size()<<" points\n";
      return true;
    }
    return false;
  }

  bool draw_mode_;
  float x1, y1;
  float last_x, last_y;
  bgui3d_tableau_sptr tab3d_;

};

typedef vgui_tableau_sptr_t<select_points_tableau> select_points_tableau_sptr;

//: Create a smart-pointer to a select_points_tableau tableau.
struct select_points_tableau_new : public select_points_tableau_sptr
{
  typedef select_points_tableau_sptr base;

  //: Constructor - make a tableau 
  select_points_tableau_new(const bgui3d_tableau_sptr& tab3d)
    : base(new select_points_tableau(tab3d)) { }
};



int main(int argc, char** argv)
{
  // initialize vgui
  vgui::init(argc, argv);

  // initialize bgui_3d
  bgui3d_init();


  SoSeparator *root = new SoSeparator;
  root->ref();
  buildScene(root);
  bgui3d_examiner_tableau_new examine_tab(root);
  root->unref();

  select_points_tableau_new select1_tab(examine_tab);
  vgui_composite_tableau_new comp1_tab(examine_tab,select1_tab);

  // Put a shell tableau at the top of our tableau tree.
  vgui_shell_tableau_new shell(comp1_tab);

  vgui_menu file_menu;
  file_menu.add("Open File", new vgui_command_simple<data_manager>
                (data_manager::instance(), &data_manager::read_file));
  file_menu.add("Save Selection", new vgui_command_simple<data_manager>
                (data_manager::instance(), &data_manager::save_selection));


  vgui_menu menu_bar;
  menu_bar.add("File", file_menu);


  // Create a window, add the tableau and show it on screen.
  return vgui::run(shell, 1000, 1000, menu_bar);
}
