#include "vehicle_model_vis_manager.h"
#include <vcl_cstdlib.h> // for vcl_exit()
#include <vcl_iostream.h>
#include <vcl_fstream.h>
#include <vgui/vgui.h>
#include <vgui/vgui_find.h>
#include <vgui/vgui_tableau.h>
#include <vgui/vgui_dialog.h>
#include <vgui/vgui_style_sptr.h>
#include <vgui/vgui_adaptor.h>
#include <vgui/vgui_style.h>
#include <vgui/vgui_deck_tableau.h>
#include <vgui/vgui_easy2D_tableau.h>
#include <vgui/vgui_viewer2D_tableau.h>
#include <vgui/vgui_shell_tableau.h>
#include <vgui/vgui_range_map_params.h>
#include <vgui/vgui_image_tableau.h>
#include <vgui/vgui_projection_inspector.h>
#include <bgui/bgui_image_tableau.h>
#include <bgui/bgui_vtol2D_tableau.h>
#include <bgui/bgui_picker_tableau.h>
#include <vil/vil_image_view.h>
#include <vil/vil_image_resource.h>
#include <vil/vil_load.h>
#include <vil/vil_stream_fstream.h>
#include <vil/vil_pixel_format.h>
#include <vil/vil_convert.h>
#include <vul/vul_file_iterator.h>
#include <vsol/vsol_line_2d.h>
#include <Lie_group_operations.h>


vehicle_model_vis_manager *vehicle_model_vis_manager::instance_ = 0;

vehicle_model_vis_manager *vehicle_model_vis_manager::instance()
    {
    if (!instance_)
        {
        instance_ = new vehicle_model_vis_manager();
        instance_->init();
        }
    return vehicle_model_vis_manager::instance_;
    }

//-----------------------------------------------------------
// constructors/destructor
//
vehicle_model_vis_manager::vehicle_model_vis_manager():vgui_wrapper_tableau()
    {
    }

vehicle_model_vis_manager::~vehicle_model_vis_manager()
    {
    }

//: Set up the tableaux
void vehicle_model_vis_manager::init()
    {
    itab_ = bgui_image_tableau_new();
    btab_ = bgui_vtol2D_tableau_new(itab_);
    ptab_ = bgui_picker_tableau_new(btab_);
    vgui_viewer2D_tableau_sptr vtab =  vgui_viewer2D_tableau_new(ptab_);
    vgui_shell_tableau_sptr shell = vgui_shell_tableau_new(vtab);
    this->add_child(shell);
    range_params_ = new vgui_range_map_params(0.0, 65535.0, 1.0, false);
    }

void vehicle_model_vis_manager::quit()
    {
    vcl_exit(1);
    }

void vehicle_model_vis_manager::load_image()
    {
    vgui_dialog load_dlg("Load the image");
    load_dlg.set_ok_button("LOAD");
    load_dlg.set_cancel_button("CANCEL");

    static vcl_string image_filename = "C:\\pradeep\\Manifold_extraction\\3_box_model\\256x256\\Acura_NSX_92_068-000-no_sun.png";
    static vcl_string ext = "*.*";

    load_dlg.file("File name:", ext,image_filename);

    if (!load_dlg.ask())
        return;

    img_ = vil_load_image_resource(image_filename.c_str());

    if (!img_)
        {
        vcl_cout << "Null image resource - couldn't load from "
            << image_filename << '\n';
        return;
        }

    vgui_range_map_params_sptr rmps = range_params(img_);
    if (itab_)
        {
        itab_->set_image_resource(img_, rmps);
        itab_->post_redraw();
        return;
        }

    vcl_cout << "In bmvv_cal_manager::load_image_file() - null tableau\n";

    }


vgui_range_map_params_sptr vehicle_model_vis_manager::
range_params(vil_image_resource_sptr const& image)
    {
    float gamma = 1.0;
    bool invert = false;
    bool gl_map = false;
    bool cache = true;
    unsigned min = 0, max = 255;
    if (image->pixel_format()==VIL_PIXEL_FORMAT_BYTE)
        {
        vil_image_view<unsigned char> temp = image->get_view();
        unsigned char vmin=0, vmax=255;
        vil_math_value_range<unsigned char>(temp, vmin, vmax);
        min = static_cast<unsigned>(vmin);
        max = static_cast<unsigned>(vmax);
        return  new vgui_range_map_params(min, max, gamma, invert,
            gl_map, cache);
        }
    if (image->pixel_format()==VIL_PIXEL_FORMAT_UINT_16)
        {
        vil_image_view<unsigned short> temp = image->get_view();
        unsigned short vmin=0, vmax=60000;
        vil_math_value_range<unsigned short>(temp, vmin, vmax);
        min = static_cast<unsigned>(vmin);
        max = static_cast<unsigned>(vmax);
        gl_map = true;
        return  new vgui_range_map_params(min, max, gamma, invert,
            gl_map, cache);
        }
    vcl_cout << "Image pixel format not handled\n";
    return new vgui_range_map_params(0, 255, gamma, invert,
        gl_map, cache);
    }

void vehicle_model_vis_manager::draw_rect(vsol_rectangle_2d const& rect,vgui_style_sptr style)
    {
    vsol_line_2d_sptr line_1 = new vsol_line_2d(rect.p0(),rect.p1());
    vsol_line_2d_sptr line_2 = new vsol_line_2d(rect.p1(),rect.p2());
    vsol_line_2d_sptr line_3 = new vsol_line_2d(rect.p2(),rect.p3());
    vsol_line_2d_sptr line_4 = new vsol_line_2d(rect.p3(),rect.p0());

    btab_->add_vsol_line_2d(line_1,style);
    btab_->add_vsol_line_2d(line_2,style);
    btab_->add_vsol_line_2d(line_3,style);
    btab_->add_vsol_line_2d(line_4,style);

    }

void vehicle_model_vis_manager::display_model(vehicle_model M,vgui_style_sptr style)
    {
    this->draw_rect(M.engine(),style);
    this->draw_rect(M.body(),style);
    this->draw_rect(M.rear(),style);


    //bgui_vsol_soview2D_polygon* sov1 =  btab_->add_vsol_polygon_2d(M.engine().cast_to_polygon(),style);
    // sov1->set_selectable(false);
    //
    //   bgui_vsol_soview2D_polygon* sov2 =  btab_->add_vsol_polygon_2d(M.body().cast_to_polygon(),style);
    // sov2->set_selectable(false);
    //
    //  bgui_vsol_soview2D_polygon* sov3 =  btab_->add_vsol_polygon_2d(M.rear().cast_to_polygon(),style);
    // sov3->set_selectable(false);

    btab_->post_redraw();
    }

void vehicle_model_vis_manager::pick_model()
    {

    btab_->clear();
    vgui_style_sptr style = vgui_style::new_style(1.0f, 0.0f, 1.0f, 1, 3);

    /*float* engine_x1,* engine_y1,* engine_x2,* engine_y2,* body_x1,* body_y1,* body_x2,* body_y2,
    *rear_x1,*rear_y1,*rear_x2,*rear_y2;*/
    float engine_x1,engine_y1,engine_x2,engine_y2,body_x1,body_y1,body_x2,body_y2,
        rear_x1,rear_y1,rear_x2,rear_y2;

    ptab_->pick_box(&engine_x1,&engine_y1,&engine_x2,&engine_y2);
    ptab_->pick_box(&body_x1,&body_y1,&body_x2,&body_y2);
    ptab_->pick_box(&rear_x1,&rear_y1,&rear_x2,&rear_y2);

    //forcing the connectivity along the x axis for the 3 box model
    body_x1 = engine_x2;
    rear_x1 = body_x2;

    vsol_point_2d_sptr P1_eng = new vsol_point_2d(double(engine_x1),double(engine_y1));
    vsol_point_2d_sptr P2_eng = new vsol_point_2d((double)engine_x2,(double)engine_y1);
    vsol_point_2d_sptr P3_eng = new vsol_point_2d((double)engine_x2,(double)engine_y2);
    vsol_point_2d_sptr P4_eng = new vsol_point_2d((double)engine_x1,(double)engine_y2);


    vsol_point_2d_sptr P1_bod = new vsol_point_2d((double)body_x1,(double)body_y1);
    vsol_point_2d_sptr P2_bod = new vsol_point_2d((double)body_x2,(double)body_y1);
    vsol_point_2d_sptr P3_bod = new vsol_point_2d((double)body_x2,(double)body_y2);
    vsol_point_2d_sptr P4_bod = new vsol_point_2d((double)body_x1,(double)body_y2);


    vsol_point_2d_sptr P1_rear = new vsol_point_2d((double)rear_x1,(double)rear_y1);
    vsol_point_2d_sptr P2_rear = new vsol_point_2d((double)rear_x2,(double)rear_y1);
    vsol_point_2d_sptr P3_rear = new vsol_point_2d((double)rear_x2,(double)rear_y2);
    vsol_point_2d_sptr P4_rear = new vsol_point_2d((double)rear_x1,(double)rear_y2);

    vsol_rectangle_2d engine(P1_eng,P2_eng,P3_eng,P4_eng);
    vsol_rectangle_2d body(P1_bod,P2_bod,P3_bod,P4_bod);
    vsol_rectangle_2d rear(P1_rear,P2_rear,P3_rear,P4_rear);

    vehicle_model M(engine,body,rear);

    ref_model_.set_model(engine,body,rear);

    vcl_cout << M << vcl_endl;
    vcl_cout << ref_model_ << vcl_endl;

    btab_->clear();

    this->display_model(ref_model_,style);
    // this->save_ref_model();

    }

//adjust the reference model by taking some parameters as input from the user like 
//scaling of the engine,body and the rear and the x direction translation for the engine
//and the rear
void vehicle_model_vis_manager::adjust_model()
    {
    vgui_dialog params_dlg("params for adjusting the model");
    params_dlg.set_ok_button("OK");
    params_dlg.set_cancel_button("CANCEL");

    static double s1x = 1,t1x =0,s1y = 1,t1y = 0,s2x = 1,s2y = 1,s3x = 1,s3y = 1,t3x = 0,t3y = 0;

    params_dlg.field("s1x",s1x);
    params_dlg.field("s1y",s1y);
    params_dlg.field("s2x",s2x);
    params_dlg.field("s2y",s2y);
    params_dlg.field("s3x",s3x);
    params_dlg.field("s3y",s3y);
    params_dlg.field("t1x",t1x);
    params_dlg.field("t1y",t1y);
    params_dlg.field("t3x",t1x);
    params_dlg.field("t3y",t1y);

    if (!params_dlg.ask())
        return;


    vnl_matrix<double> M1(3,3,0.0),M2(3,3,0.0),M3(3,3,0.0);

    set_transformation_matrices(M1,M2,M3,ref_model_,s1x,t1x,s1y,t1y,s2x,s2y,s3x,s3y,t3x,t3y);

    vehicle_model new_model = ref_model_;

    new_model.transform_model(M1,M2,M3);

    vcl_cout << ref_model_ << vcl_endl;
    vcl_cout << new_model << vcl_endl;

    vgui_style_sptr style = vgui_style::new_style(1.0f, 1.0f, 1.0f, 1, 3);
    btab_->clear();

    adjusted_model_.set_model(new_model.engine(),new_model.body(),new_model.rear());

    this->display_model(new_model,style);

    }

void vehicle_model_vis_manager::translate_model()
    {
    vgui_dialog params_dlg("params for translating the model");
    params_dlg.set_ok_button("OK");
    params_dlg.set_cancel_button("CANCEL");

    static double tx = 0,ty = 0;

    params_dlg.field("tx",tx);
    params_dlg.field("ty",ty);


    if (!params_dlg.ask())
        return;


    vnl_matrix<double> M1(3,3,0.0),M2(3,3,0.0),M3(3,3,0.0);

    M1.put(0,0,1);
    M1.put(1,1,1);
    M1.put(2,2,1);
    M1.put(0,2,tx);
    M1.put(1,2,ty);

    M2.put(0,0,1);
    M2.put(1,1,1);
    M2.put(2,2,1);
    M2.put(0,2,tx);
    M2.put(1,2,ty);

    M3.put(0,0,1);
    M3.put(1,1,1);
    M3.put(2,2,1);
    M3.put(0,2,tx);
    M3.put(1,2,ty);

    vehicle_model new_model = ref_model_;

    new_model.transform_model(M1,M2,M3);

    vcl_cout << ref_model_ << vcl_endl;
    vcl_cout << new_model << vcl_endl;

    vgui_style_sptr style = vgui_style::new_style(1.0f, 1.0f, 1.0f, 1, 3);
    btab_->clear();

    adjusted_model_.set_model(new_model.engine(),new_model.body(),new_model.rear());

    this->display_model(new_model,style);

    }

void vehicle_model_vis_manager::save_ref_model()
    {
    vgui_dialog save_dlg("save the model");
    save_dlg.set_ok_button("SAVE");
    save_dlg.set_cancel_button("CANCEL");

    static vcl_string fname = "C:\\pradeep\\Manifold_extraction\\3_box_model\\ref_model.txt";

    static vcl_string ext = "*.*";
    save_dlg.file("File name:", ext,fname);

    if (!save_dlg.ask())
        return;

    vcl_cout << ref_model_ << vcl_endl;

    vcl_ofstream ofst(fname.c_str());
    ofst << ref_model_ << vcl_endl;

    ofst.close();
    }

void vehicle_model_vis_manager::save_adjusted_model()
    {
    vgui_dialog save_dlg("save the model");
    save_dlg.set_ok_button("SAVE");
    save_dlg.set_cancel_button("CANCEL");

    static vcl_string fname = "C:\\pradeep\\Manifold_extraction\\3_box_model\\adjusted_model.txt";

    static vcl_string ext = "*.*";
    save_dlg.file("File name:", ext,fname);

    if (!save_dlg.ask())
        return;

    vcl_cout << adjusted_model_ << vcl_endl;

    vcl_ofstream ofst(fname.c_str());
    ofst << adjusted_model_ << vcl_endl;

    ofst.close();
    }

void vehicle_model_vis_manager::load_ref_model()
    {
    vgui_dialog load_dlg("load the reference model");
    load_dlg.set_ok_button("LOAD");
    load_dlg.set_cancel_button("CANCEL");

    static vcl_string fname = "C:\\pradeep\\Manifold_extraction\\3_box_model\\ref_model.txt";

    static vcl_string ext = "*.*";
    load_dlg.file("File name:", ext,fname);

    if (!load_dlg.ask())
        return;

    vcl_ifstream ifst(fname.c_str());
    ifst >> ref_model_ ;

    vcl_cout << ref_model_ ;

    vgui_style_sptr style = vgui_style::new_style(1.0f, 0.0f, 1.0f, 1, 3);

    btab_->clear();
    this->display_model(ref_model_,style);

    ifst.close();
    }

//align the intrinsic mean wrt reference model after taking scale values
//as input....for the pickup trucks,all the images are at different scale
//but intrinsc mean is calculated at common scale...so convert the 
//intrinsic mean to the scale of the image and then align it with the 
//reference model 

void vehicle_model_vis_manager::load_intrinsic_mean_model()
    {
    vgui_dialog load_dlg("load the intrinsic mean model");
    load_dlg.set_ok_button("LOAD");
    load_dlg.set_cancel_button("CANCEL");

    static vcl_string fname = "C:\\pradeep\\Manifold_extraction\\3_box_model\\intrinsic_mean_model.txt";
    static double sx = 1,sy = 1;
    int i;

    static vcl_string ext = "*.*";
    load_dlg.file("File name:", ext,fname);
    load_dlg.field("sx",sx);
    load_dlg.field("sy",sy);

    if (!load_dlg.ask())
        return;

    vcl_ifstream ifst(fname.c_str());
    ifst >> intrinsic_mean_model_ ;

    vcl_cout << intrinsic_mean_model_;

    vnl_matrix<double> M1(3,3,0.0),M2(3,3,0.0),M3(3,3,0.0);

    for (i=0;i<3;i++)
        {
        if (i==0)
            {
            M1.put(i,i,sx);
            M2.put(i,i,sx);
            M3.put(i,i,sx);
            }
        else if (i==1)
            {
            M1.put(i,i,sy);
            M2.put(i,i,sy);
            M3.put(i,i,sy);
            }
        else
            {
            M1.put(i,i,1);
            M2.put(i,i,1);
            M3.put(i,i,1);

            }

        }
    //first scale the model to bring it to the scale of the reference model

    intrinsic_mean_model_.transform_model(M1,M2,M3);

    
    double tx = ref_model_.body().centroid()->x() - intrinsic_mean_model_.body().centroid()->x();
    double ty = ref_model_.body().centroid()->y() - intrinsic_mean_model_.body().centroid()->y();

     for (i=0;i<2;i++)
        {
            M1.put(i,i,1);
            M2.put(i,i,1);
            M3.put(i,i,1);
        }
    M1.put(0,2,tx);
    M1.put(1,2,ty);
    M2.put(0,2,tx);
    M2.put(1,2,ty);
    M3.put(0,2,tx);
    M3.put(1,2,ty);

    // align the intrinsic mean model to the reference model
    intrinsic_mean_model_.transform_model(M1,M2,M3);

    vgui_style_sptr style = vgui_style::new_style(1.0f, 0.0f, 0.0f, 1, 3);

    this->display_model(intrinsic_mean_model_,style);

    ifst.close();
    }

void vehicle_model_vis_manager::set_range_params()
    {

    static double min = 0, max = 65535;
    static float gamma = 1.0;
    static bool invert = false;
    vgui_dialog range_dlg("Set Range Map Params");

    if(range_params_ != 0){
        min = range_params_->min_L_;
        max = range_params_->max_L_;
        }

range_dlg.field("Range min:", min);
range_dlg.field("Range max:", max);
range_dlg.field("Gamma:", gamma);
range_dlg.checkbox("Invert:", invert);
if (!range_dlg.ask())
return;

range_params_ = new vgui_range_map_params(min, max, gamma, invert);

itab_->set_mapping(range_params_);

    }


// Handle all events for this tableau.
bool vehicle_model_vis_manager::handle(vgui_event const &e)
    {
    static bool button_down = false;
    if (e.type == vgui_BUTTON_DOWN)
        button_down = true;
    else if (e.type == vgui_BUTTON_UP)
        button_down = false;


    return base::handle(e);
    }


