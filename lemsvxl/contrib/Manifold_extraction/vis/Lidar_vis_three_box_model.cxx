#include <vgui/vgui.h>
#include <vgui/vgui_menu.h>
#include <vgui/vgui_dialog.h>
#include <vgui/vgui_style.h>
#include <vgui/vgui_style_sptr.h>
#include <vgui/vgui_command.h>
#include <vgui/vgui_shell_tableau.h>
#include <vgui/vgui_composite_tableau.h>
#include <vgui/vgui_grid_tableau.h>
#include <vgui/vgui_viewer2D_tableau.h>
#include <vgui/vgui_projection_inspector.h>

#include <vgl/vgl_point_2d.h>
#include <vgl/vgl_point_3d.h>
#include <vgl/vgl_box_2d.h>
#include <vgl/vgl_box_3d.h>

#include <vcl_fstream.h>
#include <vnl/vnl_double_3.h>
#include <vpgl/vpgl_perspective_camera.h>

#include <bgui3d/bgui3d.h>
#include <bgui3d/bgui3d_file_io.h>
#include <bgui3d/bgui3d_examiner_tableau.h>
#include <bgui3d/bgui3d_project2d_tableau.h>
#include <bgui/bgui_vtol2D_tableau.h>
#include <bgui/bgui_image_tableau.h>

#include <vsol/vsol_point_2d.h>
#include <vsol/vsol_point_2d_sptr.h>
#include <vsol/vsol_line_2d.h>
#include <vsol/vsol_rectangle_2d.h>

#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoTransform.h>
#include <Inventor/nodes/SoMaterial.h>
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

void set_bounding_box_hood(const vgl_point_3d<double>& min_pt,const vgl_point_3d<double>& max_pt)
    {bounding_box_hood_.set_min_point(min_pt);
bounding_box_hood_.set_max_point(max_pt);
    }

void set_bounding_box_cab(const vgl_point_3d<double>& min_pt,const vgl_point_3d<double>& max_pt)
    {bounding_box_cab_.set_min_point(min_pt);
bounding_box_cab_.set_max_point(max_pt);
    }

void set_bounding_box_bed(const vgl_point_3d<double>& min_pt,const vgl_point_3d<double>& max_pt)
    {bounding_box_bed_.set_min_point(min_pt);
bounding_box_bed_.set_max_point(max_pt);
    }
void set_hood_mean(const SbVec3f& mean)
    {
    hood_mean_ =  mean;
    }
void set_cab_mean(const SbVec3f& mean)
    {
    cab_mean_ =  mean;
    }

void set_bed_mean(const SbVec3f& mean)
    {
    bed_mean_ =  mean;
    }
vgl_box_3d<double> bounding_box_hood()
    {
    return bounding_box_hood_;
    }

vgl_box_3d<double> bounding_box_cab()
    {
    return bounding_box_cab_;
    }

vgl_box_3d<double> bounding_box_bed()
    {
    return bounding_box_bed_;
    }

//bgui3d_examiner_tableau examiner_tableau()
//    {
//    return btab_;
//    }

void read_file();

void read_vrml_file();

void save_selection();

void save_bounding_box();

void draw_rect(vsol_rectangle_2d const& rect,vgui_style_sptr style);

// bgui_vtol2D_tableau_sptr btab_;

    private:
        static data_manager* instance_;

        void init();
        data_manager(){}

    SoBaseColor* p_colors_;
    SoCoordinate3* p_coords_;
    SbVec3f hood_mean_;
    SbVec3f cab_mean_;
    SbVec3f bed_mean_;

    vcl_vector<unsigned int> selection_;
    vgl_box_3d<double> bounding_box_hood_;
    vgl_box_3d<double> bounding_box_cab_;
    vgl_box_3d<double> bounding_box_bed_;
    
    // bgui3d_examiner_tableau btab_;

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
        unsigned int val;
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
            fh >> val;
            colors[i][0] /= 255.0f;
            colors[i][1] /= 255.0f;
            colors[i][2] /= 255.0f;
            ++i;
            }
    vcl_cout << "max intensity: "<<val<<vcl_endl;
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

void data_manager::read_vrml_file()
    {
    vgui_dialog vrml_dlg("Load vrml file");
    static vcl_string filename = "", ext = "*.wrl";

    vrml_dlg.file("vrml file",ext, filename);

    if(!vrml_dlg.ask())
        return;

    // read the file into a scene graph
    SoNode* root = bgui3d_import_file(filename);
    root->ref();

    bgui3d_examiner_tableau_new btab(root);
    // wrap the scenegraph in an examiner tableau
    /*bgui3d_examiner_tableau_sptr btab = new bgui3d_examiner_tableau(data_manager::instance()->examiner_tableau());
    btab->set_scene_root(root);*/

    root->getBoundingBox(SoGetBoundingBoxAction

    socoordinate3 coord;

   // p_coords_->point.setValues(0,
        

    root->unref();
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

void data_manager::save_bounding_box()
    {
    double c_x,min_z_hood,min_z_cab,min_z_bed,max_y_hood,min_y_cab,max_y_cab,min_y_bed;
    double centroid_y_cab,centroid_y_bed,centroid_z_bed,centroid_z_cab;

    vgui_dialog save_dlg("Save bounding box");
    static vcl_string filename = " ", ext = "*.txt";
    bool vrml;

    save_dlg.file("File",ext, filename);
    save_dlg.checkbox("as vrml",vrml);

    if(!save_dlg.ask())
        return;

    vgl_box_3d<double>bb_hood = data_manager::instance()->bounding_box_hood();
    vgl_box_3d<double>bb_cab = data_manager::instance()->bounding_box_cab();
    vgl_box_3d<double>bb_bed = data_manager::instance()->bounding_box_bed();


    SbVec3f offset = cab_mean_ - hood_mean_;

    double x1,y1,z1,x2,y2,z2;

    x1 = offset[0] + bb_cab.min_x();
    y1 = offset[1] + bb_cab.min_y();
    z1 = offset[2] + bb_cab.min_z();
    x2 = offset[0] + bb_cab.max_x();
    y2 = offset[1] + bb_cab.max_y();
    z2 = offset[2] + bb_cab.max_z();

    vgl_point_3d<double> min_pt(x1,y1,z1);
    vgl_point_3d<double> max_pt(x2,y2,z2);

    bb_cab.set_min_point(min_pt);
    bb_cab.set_max_point(max_pt);

    offset = bed_mean_ - hood_mean_;

    x1 = offset[0] + bb_bed.min_x();
    y1 = offset[1] + bb_bed.min_y();
    z1 = offset[2] + bb_bed.min_z();
    x2 = offset[0] + bb_bed.max_x();
    y2 = offset[1] + bb_bed.max_y();
    z2 = offset[2] + bb_bed.max_z();

    vgl_point_3d<double> min_pt_b(x1,y1,z1);
    vgl_point_3d<double> max_pt_b(x2,y2,z2);

    bb_bed.set_min_point(min_pt_b);
    bb_bed.set_max_point(max_pt_b);

 /*   c_x = bb_hood.centroid_x();

    bb_cab.set_centroid_x(c_x);
    bb_bed.set_centroid_x(c_x);*/

    bb_cab.set_min_x(bb_hood.min_x());
    bb_cab.set_max_x(bb_hood.max_x());

     bb_bed.set_min_x(bb_hood.min_x());
    bb_bed.set_max_x(bb_hood.max_x());

    min_z_hood = bb_hood.min_z();
    min_z_cab = bb_cab.min_z();
    min_z_bed = bb_bed.min_z();

    centroid_z_cab = bb_cab.centroid_z();
    centroid_z_bed = bb_bed.centroid_z();

    bb_cab.set_centroid_z(centroid_z_cab + min_z_hood - min_z_cab);
    bb_bed.set_centroid_z(centroid_z_bed + min_z_hood - min_z_bed);

    max_y_hood = bb_hood.max_y();
   
    min_y_bed = bb_bed.min_y();
   
    centroid_y_cab = bb_cab.centroid_y();
    centroid_y_bed = bb_bed.centroid_y();

    min_y_cab = bb_cab.min_y();

    bb_cab.set_centroid_y(centroid_y_cab + max_y_hood - min_y_cab);
     
    max_y_cab = bb_cab.max_y();

    bb_bed.set_centroid_y(centroid_y_bed + max_y_cab - min_y_bed);

    vcl_vector<vgl_point_3d<double> >centroids;

    vcl_cout << "bb_hood "<< bb_hood << vcl_endl; 
    vcl_cout << "bb_cab "<< bb_cab << vcl_endl; 
    vcl_cout << "bb_bed "<< bb_bed << vcl_endl; 


    centroids.push_back(bb_hood.centroid());
    centroids.push_back(bb_cab.centroid());
    centroids.push_back(bb_bed.centroid());

    vcl_vector<vgl_box_3d<double> >b_boxes;

    b_boxes.push_back(bb_hood);
    b_boxes.push_back(bb_cab);
    b_boxes.push_back(bb_bed);

    vcl_ofstream ofstr(filename.c_str());

    if (vrml)
        {
        ofstr <<      "#VRML V2.0 utf8\n";
        ofstr <<      "Background { \n";
        ofstr <<      "    skyColor [ 0 0 0 ]\n";
        ofstr <<      "    groundColor [ 0 0 0 ]\n";
        ofstr <<      "}\n";
        ofstr <<      "PointLight {\n";
        ofstr <<      "    on FALSE\n";
        ofstr <<      "    intensity 1 \n";
        ofstr <<      "ambientIntensity 0 \n";
        ofstr <<      "color 1 1 1 \n";
        ofstr <<      "location 0 0 0 \n";
        ofstr <<      "attenuation 1 0 0 \n";
        ofstr <<      "radius 100  \n";
        ofstr <<      "}\n";

        for (unsigned i=0;i<centroids.size();i++)
            {
            ofstr <<      " Transform { \n";
            ofstr <<      "translation" << " " <<centroids[i].x() << " " << centroids[i].y() << " " << centroids[i].z() << "\n";
            ofstr << "children [ \n";
            ofstr << "Shape { \n";
            ofstr << " appearance Appearance{ \n";
            ofstr << "   material Material \n";
            ofstr << "    { \n";
            ofstr << "      diffuseColor 1 0 0 \n";
            ofstr << "      transparency 0 \n";
            ofstr << "    } \n";
            ofstr << "  } \n";
            ofstr << " geometry Box \n";
            ofstr << "{ \n";
            ofstr << "  size " << b_boxes[i].width() <<" "<< b_boxes[i].height()<<" " << b_boxes[i].depth() <<" \n";
            ofstr << "   } \n";
            ofstr << "  } \n";
            ofstr << " ] \n";
            ofstr << "} \n";
            }
        }

    else
        {
        ofstr << "bounding box hood: "<< vcl_endl;
        ofstr << bb_hood << vcl_endl;
        ofstr << "bounding box cab: "<< vcl_endl;
        ofstr << bb_cab << vcl_endl;
        ofstr << "bounding box bed: "<< vcl_endl;
        ofstr << bb_bed << vcl_endl;
        }

    }

void data_manager::draw_rect(vsol_rectangle_2d const& rect,vgui_style_sptr style)
    {
    vsol_line_2d_sptr line_1 = new vsol_line_2d(rect.p0(),rect.p1());
    vsol_line_2d_sptr line_2 = new vsol_line_2d(rect.p1(),rect.p2());
    vsol_line_2d_sptr line_3 = new vsol_line_2d(rect.p2(),rect.p3());
    vsol_line_2d_sptr line_4 = new vsol_line_2d(rect.p3(),rect.p0());

    //btab_->add_vsol_line_2d(line_1,style);
    //btab_->add_vsol_line_2d(line_2,style);
    //btab_->add_vsol_line_2d(line_3,style);
    //btab_->add_vsol_line_2d(line_4,style);

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


        float ix, iy;
        vgui_projection_inspector().window_to_image_coordinates(e.wx, e.wy, ix, iy);
        //vgui_projection_inspector().print(vcl_cout);

        double x_min,y_min,z_min,x_max,y_max,z_max;

        float x,y,z;



        if(e.type == vgui_MOUSE_MOTION){
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
        {
        selection.push_back(i);
        }
    }

double min_x =1e50,min_y =1e50,min_z=1e50,max_x=-1e50,max_y=-1e50,max_z=-1e50;
int pt_num;

vcl_string txt_file = "C:\\Lidar_data\\vehicles\\b_box_points.txt";

vcl_ofstream ofstr(txt_file.c_str());

SbVec3f mean(0,0,0);

for(unsigned int i=0; i<selection.size(); ++i){
    unsigned int j = selection[i];
    const SbVec3f& pt = coords->point[j];
    mean += pt/selection.size();
    }
// mean /= selection.size();

for (int i =0;i<selection.size();i++)
    {
    pt_num = selection[i];
    const SbVec3f& pt = coords->point[pt_num] - mean;

    ofstr << pt[0] <<" "<< pt[1] << " " <<pt[2] << vcl_endl;

    if (min_x > pt[0])
        min_x = pt[0];
    if (min_y > pt[1])
        min_y = pt[1];
    if (min_z > pt[2])
        min_z = pt[2];
    if (max_x < pt[0])
        max_x = pt[0];
    if (max_y < pt[1])
        max_y = pt[1];
    if (max_z < pt[2])
        max_z = pt[2];

    }

vgl_point_3d<double>min_pt(min_x,min_y,min_z);
vgl_point_3d<double>max_pt(max_x,max_y,max_z);

vgui_dialog bounding_box_dlg("check the bounding box type");

bool b_box_hood = 0,b_box_cab = 0,b_box_bed = 0;

bounding_box_dlg.checkbox("bounding box hood",b_box_hood);
bounding_box_dlg.checkbox("bounding box cab",b_box_cab);
bounding_box_dlg.checkbox("bounding box bed",b_box_bed);

if (!bounding_box_dlg.ask())
return true;

if (b_box_hood)
    {
    data_manager::instance()->set_bounding_box_hood(min_pt,max_pt);
    data_manager::instance()->set_hood_mean(mean);
    vcl_cout << "bounding box hood: " <<vcl_endl;

    }

if (b_box_cab)
    {
    data_manager::instance()->set_bounding_box_cab(min_pt,max_pt);
    data_manager::instance()->set_cab_mean(mean);
    vcl_cout << "bounding box cab: " <<vcl_endl;
    }

if (b_box_bed)
    {
    data_manager::instance()->set_bounding_box_bed(min_pt,max_pt);
    data_manager::instance()->set_bed_mean(mean);
    vcl_cout << "bounding box bed: " <<vcl_endl;

    }
vcl_cout << "min point" << min_pt << vcl_endl;
vcl_cout << "max point" << max_pt << vcl_endl;
vcl_cout << mean[0] << " " << mean[1] << " " << mean[2] << vcl_endl;

data_manager::instance()->set_selection(selection);
vgui::out << "selected "<<selection.size()<<" points\n";
vcl_cout<< "selected "<<selection.size()<<" points\n";



vgui_style_sptr style_hood = vgui_style::new_style(1.0f, 0.0f, 1.0f, 1, 3);
vgui_style_sptr style_cab = vgui_style::new_style(1.0f, 0.0f, 1.0f, 1, 3);
vgui_style_sptr style_bed = vgui_style::new_style(1.0f, 0.0f, 1.0f, 1, 3);

/* vgl_box_2d<double>hood_proj = vpgl_project::project_bounding_box(cam, data_manager::instance()->bounding_box_hood());
vgl_box_2d<double>cab_proj = vpgl_project::project_bounding_box(cam, data_manager::instance()->bounding_box_cab());
vgl_box_2d<double>bed_proj = vpgl_project::project_bounding_box(cam, data_manager::instance()->bounding_box_bed());*/

vsol_point_2d_sptr center_hood = new vsol_point_2d(0,0);
vsol_point_2d_sptr center_cab = new vsol_point_2d(20,20);
vsol_point_2d_sptr center_bed = new vsol_point_2d(40,40);

center_hood->set_x(0);
center_hood->set_y(0);

center_cab->set_x(20);
center_cab->set_y(20);

center_bed->set_x(40);
center_bed->set_y(40);


vsol_rectangle_2d hood_proj(center_hood,20,20,0,true);
vsol_rectangle_2d cab_proj(center_cab,200,200,0,true);
vsol_rectangle_2d bed_proj(center_bed,400,400,0,true);

data_manager::instance()->draw_rect(hood_proj,style_hood);
data_manager::instance()->draw_rect(cab_proj,style_cab);
data_manager::instance()->draw_rect(bed_proj,style_bed);

// data_manager::instance()->btab_->post_redraw();

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

vgui_dialog load_dlg("load file");
  static vcl_string filename = "", ext = "*.wrl";
  
  load_dlg.file("File",ext, filename);
  
  if(!load_dlg.ask())
    return 0;

  // initialize bgui_3d
  bgui3d_init();

  // read the file into a scene graph
  SoNode* root = bgui3d_import_file(filename);
  root->ref();
  
  // wrap the scenegraph in an examiner tableau
  bgui3d_examiner_tableau_new examine_tab(root);
  root->unref();


   /* SoSeparator *root = new SoSeparator;
    root->ref();
    buildScene(root);
    bgui3d_examiner_tableau_new examine_tab(root);
    root->unref();*/

    vgui_menu file_menu;
    file_menu.add("Open File", new vgui_command_simple<data_manager>
        (data_manager::instance(), &data_manager::read_file));
    file_menu.add("Open vrml File", new vgui_command_simple<data_manager>
        (data_manager::instance(), &data_manager::read_vrml_file));
    file_menu.add("Save Selection", new vgui_command_simple<data_manager>
        (data_manager::instance(), &data_manager::save_selection));
    file_menu.add("Save 3 box model", new vgui_command_simple<data_manager>
        (data_manager::instance(), &data_manager::save_bounding_box));


    vgui_menu menu_bar;
    menu_bar.add("File", file_menu);

    select_points_tableau_new select1_tab(examine_tab);

    /*bgui_vtol2D_tableau_sptr btab = bgui_vtol2D_tableau_new(examine_tab);


    vgui_viewer2D_tableau_sptr vtab =  vgui_viewer2D_tableau_new(btab);*/


    vgui_composite_tableau_new comp1_tab(examine_tab,select1_tab);

    // Put a shell tableau at the top of our tableau tree.
    vgui_shell_tableau_new shell(comp1_tab);

    // Create a window, add the tableau and show it on screen.
    return vgui::run(shell, 1000, 1000, menu_bar);

    }


