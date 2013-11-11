//:
// \file
// \author Dongjin Han


#include <vcl_cstdlib.h> // for vcl_exit()
#include <vcl_string.h>
#include <vcl_ostream.h>
#include <vcl_sstream.h>
#include <vcl_fstream.h>
#include <vcl_cassert.h>
#include <vcl_cmath.h>
#include <vcl_iostream.h>
#include <vcl_cstdio.h> //vcl_sprintf//

#include <vgui/vgui.h>
#include <vgui/vgui_dialog.h>
#include <vgui/vgui_macro.h>
#include <vgui/vgui_find.h>

#include <vgui/vgui_style.h>

#include <vul/vul_file.h>
#include <vul/vul_file_iterator.h>

#include <vgui/vgui_adaptor.h>
#include <vgui/vgui_easy3D_tableau.h>
#include <vgui/vgui_soview3D.h> // for vgui_lineseg3D
#include <vgui/vgui_viewer3D_tableau.h>
#include <vgui/vgui_image_tableau.h>
#include <vgui/vgui_easy2D_tableau.h>
#include <vgui/vgui_composite_tableau.h>
#include <vgui/vgui_composite_tableau_sptr.h>
#include <vgui/vgui_viewer2D_tableau.h>
#include <vgui/vgui_grid_tableau.h>
#include <vgui/vgui_shell_tableau.h>


#include <vgl/vgl_polygon.h>
#include <vgl/vgl_homg_point_2d.h>
#include <vgl/algo/vgl_homg_operators_2d.h>
#include <vil1/vil1_load.h>
#include <vil1/vil1_save.h>
#include "rc3d_menus.h"
#include "rc3d_windows_frame.h"
//#include "rc3d_image_util.h"

#include <bgui/bgui_image_tableau.h>
#include <bgui/bgui_vtol2D_tableau.h>
#include <bgui/bgui_picker_tableau.h>
#include <bgui/bgui_picker_tableau_sptr.h>
#include <bgui/bgui_vtol2D_tableau_sptr.h>

#include <sdet/sdet_detector_params.h>
#include <sdet/sdet_detector.h>
#include <vil1/vil1_load.h>
#include <brip/brip_vil1_float_ops.h>
#include <brct/brct_algos.h>

#include <vdgl/vdgl_digital_curve.h>
#include <vdgl/vdgl_interpolator.h>
#include <vdgl/vdgl_interpolator_linear.h>
#include <vdgl/vdgl_edgel_chain.h>
#include <vsol/vsol_curve_2d.h>
//#include <vsol/vsol_curve_2d_sptr.h>
// pilou's line
//#include <bdgl/bdgl_curve_description.h>
//#include <bdgl/bdgl_curve_tracker.h>
//#include <bdgl/bdgl_curve_tracker_primitive.h>
//#include <bdgl/bdgl_curve_matcher.h>
//#include <bdgl/bdgl_curve_algs.h>

#include <vgl/vgl_distance.h>
#include <vgl/vgl_point_2d.h>
#include <vgl/vgl_point_3d.h>
#include <vgl/vgl_homg_point_2d.h>
#include <vgl/vgl_homg_line_2d.h>
#include <vgl/algo/vgl_homg_operators_2d.h>
#include <mvl/FMatrix.h>
#include <vnl/vnl_math.h> // for pi
#include <vnl/vnl_inverse.h>
#include <vnl/vnl_double_2.h>
#include <vnl/vnl_double_3x4.h>

#include <osl/osl_edgel_chain.h>
#include <osl/osl_fit_lines.h>
#include <osl/osl_edge.h>
#include <osl/osl_fit_lines_params.h>

rc3d_windows_frame *rc3d_windows_frame::instance_ = 0;

//===============================================================
//: The singleton pattern - only one instance of the manager can occur
//==============================================================
rc3d_windows_frame *rc3d_windows_frame::instance()
{
   if (!instance_)
   {
      instance_ = new rc3d_windows_frame();
      instance_->init();
   }
   
   return rc3d_windows_frame::instance_;
}

//==================================================================
//: constructors/destructor
//==================================================================
rc3d_windows_frame::rc3d_windows_frame() : vgui_wrapper_tableau()
{
}

rc3d_windows_frame::~rc3d_windows_frame()
{
}

//======================================================================
//: set up the tableaux at each grid cell
//  the vtol2D_tableaux have been initialized in the constructor
//======================================================================
void rc3d_windows_frame::init()
{

  con_flag_=false;
        BATCH_=false;
  front_windshield_fine_adjust_=false;
        W_=50.0;E_=10.0;


   initialize_d3_probe();
   initialize_probe();


         
         wheel_count_=4;wheel_detector_=false;slope_detector_=false;  well_detector_on_=false;
 //vcl_cout<<"hello"<<vcl_endl;
   epi_flag=false;
   centered_flag_=false;
   init_3_cam_flag_=false;
   dots_in_cube_flag_=false;
   how_many_dots_flag_=true;
   yellow_cube_=true;
   // wheel well probe activation  6-20-2004
   special_flag_wheel_well_1_=false;
   special_flag_wheel_well_2_=false;

   how_many_dots_=10;

   iview_=4;  //image frame number + 1(3d tableau)
   grid_ = vgui_grid_tableau_new(iview_,1);
   grid_->set_grid_size_changeable(true);
   unsigned int col=0, row = 0;
   // add 3D tableau
   
   vgui_easy3D_tableau_new tab3d;
   tab_3d_ = tab3d;
   tab_3d_->set_point_radius(2);
   // Add a point at the origin
   tab_3d_->set_foreground(1,1,1);
   tab_3d_->add_point(0,0,0);
   
   tab_3d_->set_point_radius(5);
    // Add a line in the xaxis:
   tab_3d_->set_foreground(1,0,0);
   tab_3d_->add_line(1,0,0, 4,0,0);
   tab_3d_->add_point(4,0,0);
   // Add a line in the yaxis:
   tab_3d_->set_foreground(0,1,0);
   tab_3d_->add_line(0,1,0, 0,4,0);
   tab_3d_->add_point(0,4,0);
   // Add a line in the zaxis:
   tab_3d_->set_foreground(0,0,1);
   tab_3d_->add_line(0,0,1, 0,0,4);
   tab_3d_->add_point(0,0,4);
   vgui_viewer3D_tableau_sptr v3d = vgui_viewer3D_tableau_new(tab_3d_);
   //grid_->add_at(v3d, iview_-1, row);
   
   // initialize the easy 2d grid

   //for (unsigned int col1=0, row1=0; col1<iview_-1; ++col1) {
   for (unsigned int col1=0, row1=0; col1<iview_; ++col1) {
      vgui_image_tableau_sptr tab_img = bgui_image_tableau_new();
      //img_2d_.push_back(tab_img);
         
      bgui_vtol2D_tableau_sptr btab = bgui_vtol2D_tableau_new(tab_img);
      vtol_tabs_.push_back(btab);

        bgui_picker_tableau_new picker(btab);

      //bgui_picker_tableau_sptr picker = bgui_picker_tableau_new(btab);
      tabs_picker_.push_back(picker);
      
      vgui_viewer2D_tableau_sptr v2d = vgui_viewer2D_tableau_new(picker);
      grid_->add_at(v2d, col1, row1);
   
      
   }
   
   
   vgui_shell_tableau_sptr shell = vgui_shell_tableau_new(grid_);
   this->add_child(shell);
   //this->post_redraw();
   
}

//=========================================================================
//: make an event handler
// note that we have to get an adaptor and set the tableau to receive events
// this handler does nothing but is a place holder for future event processing
// For now, just pass the events down to the child tableaux
//=========================================================================
bool rc3d_windows_frame::handle(const vgui_event &e)
{
   return this->child.handle(e);
}


void rc3d_windows_frame::quit()
{
   //clean_up();
   vcl_exit(1);
}

void rc3d_windows_frame::clean_up()
{
   if (kalman_)
      delete kalman_;   
   if (e_)
      delete e_;
}



/*void rc3d_windows_frame::remove_curve2d()
{
   predicted_curves_2d_.clear();
   curves_2d_.clear();
   this->post_redraw();
}*/

void rc3d_windows_frame::add_curve3d(vcl_vector<vgl_point_3d<double> >& pts)
{
   int size = pts.size();
   if(size > 1){
      curves_3d_.resize(size-1);
      instance_->tab_3d_->set_foreground(1, 1, 1);
      for (int i=0; i<size-1; i++)
      {
         vgl_point_3d<double>& s = pts[i];
         vgl_point_3d<double>& e = pts[i+1];
         vgui_lineseg3D* l = instance_->tab_3d_->add_line(s.x(), s.y(), s.z(), e.x(), e.y(), e.z());
         //instance_->tab_3d_->add_point(s.x(), s.y(), s.z());
         curves_3d_[i] = l;
      }
      instance_->post_redraw();
   }
}

//////
void rc3d_windows_frame::remove_curve3d()
{
   int size = curves_3d_.size();
   for (int i=0; i<size; i++)
      instance_->tab_3d_->remove(curves_3d_[i]);
   curves_3d_.clear();
   instance_->tab_3d_->clear();
   tab_3d_->set_point_radius(2);
   // Add a point at the origin
   tab_3d_->set_foreground(1,1,1);
   tab_3d_->add_point(0,0,0);
   
   tab_3d_->set_point_radius(5);
    // Add a line in the xaxis:
   tab_3d_->set_foreground(1,0,0);
   tab_3d_->add_line(1,0,0, 4,0,0);
   tab_3d_->add_point(4,0,0);
   // Add a line in the yaxis:
   tab_3d_->set_foreground(0,1,0);
   tab_3d_->add_line(0,1,0, 0,4,0);
   tab_3d_->add_point(0,4,0);
   // Add a line in the zaxis:
   tab_3d_->set_foreground(0,0,1);
   tab_3d_->add_line(0,0,1, 0,0,4);
   tab_3d_->add_point(0,0,4);
   this->post_redraw();

   //
   centered_flag_=false;
}

//////
/*
void rc3d_windows_frame::init_kalman()
{
   vcl_vector<vgl_point_2d<double> > c2d;
   
   if (!e_)
   {
      assert(lines_.size()>=2);
      //init_epipole();
   }
   
   if (kalman_ == 0)
      vcl_cout<<"rc3d_windows_frame::kalman_ not created yet\n";
   else
      kalman_->init();
   
   // add the curve in the first view
   c2d = kalman_->get_pre_observes();
   ////add_curve2d(c2d);
   
   //update the display.
   vcl_vector<vgl_point_3d<double> > c3d = kalman_->get_local_pts();
   add_curve3d(c3d);
   
   // add the curve in the second view
   c2d = kalman_->get_cur_observes();
   ////add_curve2d(c2d);
   
   instance_->post_redraw();
}
*/
/*void rc3d_windows_frame::go()
{
remove_curve3d();
kalman_->inc();

  // add current data
  vcl_vector<vgl_point_2d<double> > c2d = kalman_->get_cur_observes();
  //// add_curve2d(c2d);
  
   //add 3D resoult
   vcl_vector<vgl_point_3d<double> > c3d = kalman_->get_local_pts();
   add_curve3d(c3d);
   
     show_epipole();
     this->post_redraw();
     }
*/

/*void rc3d_windows_frame::show_predicted_curve()
{
vnl_matrix<double> pts = kalman_->get_predicted_curve();
int num_points = pts.columns();
vcl_vector<vgl_point_2d<double> > curve(num_points);

  for (int i=0; i<num_points; i++)
  curve[i]= vgl_point_2d<double>(pts[0][i], pts[1][i]);
  
   add_predicted_curve2d(curve);
   //add_curve2d(curve);
   this->post_redraw();
   }
   
*/

//////////////////////////////////////////////////
void rc3d_windows_frame::load_image()
{
        static bool greyscale = true;
        vgui_dialog load_image_dlg("Load Image");
        static vcl_string image_filename = "";
         static vcl_string con_filename = "";
   static vcl_string ext = "*.*";
         static bool con_flag=false;
   load_image_dlg.file("Image Filename:", ext, image_filename);
   load_image_dlg.checkbox("greyscale ", greyscale);
         load_image_dlg.file("CON Filename:", ext, con_filename);
   load_image_dlg.checkbox("CON file?",con_flag);
         
         
         if (!load_image_dlg.ask())
                return;
                 
                if (!con_flag) {
                         vil1_image temp = vil1_load(image_filename.c_str());
                         if (greyscale)
                         {
                                 vil1_memory_image_of<unsigned char> temp1 =
                                         brip_vil1_float_ops::convert_to_grey(temp);
                                 img_ = temp1;
                         }
                         else
                                 img_ = temp;
                         
                         bgui_vtol2D_tableau_sptr btab = this->get_selected_vtol2D_tableau();
                         if (btab)
                         {
                                 vgui_image_tableau_sptr itab = btab->get_image_tableau();
                                 vcl_cout<<"\n";
                                 itab->set_image(img_);
                                 
                                 itab->post_redraw();
                                 return;
                         }
                         vcl_cout << "In load_image() - null tableau\n";
                         
                         return;
                 }
                 /*
                 bool greyscale = false;
                 vgui_dialog load_image_dlg("Load Image");
                 static vcl_string image_filename = "";
                 static vcl_string ext = "*.*";
                 load_image_dlg.file("Image Filename:", ext, image_filename);
                 load_image_dlg.checkbox("greyscale ", greyscale);
   if (!load_image_dlg.ask())
    return;
   img_ = vil1_load(image_filename.c_str());
   if (img_2d_)
   {
    img_2d_->set_image(img_);
    instance_->post_redraw();
    status_info_ = image_filename;
    status_info_ += "\n";
   }
   else
    vcl_cout << "In rc3d_windows_frame::load_image() - null tableau\n";
   */
                int index=100;
                bsol_intrinsic_curve_2d_sptr curve_2d = new bsol_intrinsic_curve_2d;
                curve_2d->readCONFromFile(con_filename.c_str() );
                vcl_cout<<curve_2d->size()<<vcl_endl;
                vcl_cout<<curve_2d->x(index)<<" "<<curve_2d->y(index)<<vcl_endl;

}
void rc3d_windows_frame::load_image_file(vcl_string image_filename, bool greyscale, unsigned col, unsigned row)
{
  vil1_image temp= vil1_load(image_filename.c_str());
  if (!temp) {
    vcl_cout<<"file reading eeror"<<vcl_endl;
    return;
  }
  bgui_vtol2D_tableau_sptr btab = this->get_vtol2D_tableau_at(col, row);


        //****************  fix bug
        //if (col<3)
  (fore_ground_rgb_[col]).set(temp);
        // 9-13-2005 bug bug bug

  if (greyscale)
   {
      vil1_memory_image_of<unsigned char> temp1 =
         brip_vil1_float_ops::convert_to_grey(temp);
      img_ = temp1;


      vil1_memory_image_of<float>  temp1_float =
         brip_vil1_float_ops::convert_to_float(temp1);
      if (col<4)
      (fore_ground_[col]).set(temp1_float);
   }
  //if (greyscale) {
  //  vil_convert_planes_to_grey( temp , greyimage );
  //}
   else
      img_ = temp;
  if (btab)
  {
    vgui_image_tableau_sptr itab = btab->get_image_tableau();

    itab->set_image(img_);
    return;
  }
  vcl_cout << "In bmvv_multiview_manager::load_image_file() - null tableau\n";
}

/////////// 7-19-2005
void rc3d_windows_frame::add_vertex(int iframe,float x,float y, int size, int color) 
{
        
        for (unsigned i=floor(x-size/2.0);i<x+size/2.0;i++) 
                for (unsigned j=floor(y-size/2.0);j<y+size/2.0;j++) {
                        
                        if ((i-x)*(i-x)+(j-y)*(j-y)>size*size/4.0) continue;

                        //////bug  9-13-2005
                        //if (i<0||i>1023||j<0||j>767) return;
                        if (i<0||i>1279||j<0||j>719) return;
                        if (color==0) //red
                        {
                                (fore_ground_rgb_[iframe])(i,j).r=255;
                                (fore_ground_rgb_[iframe])(i,j).g=0;
                                (fore_ground_rgb_[iframe])(i,j).b=0;
                        }
                        if (color==1) //green
                        {
                                (fore_ground_rgb_[iframe])(i,j).r=0;
                                (fore_ground_rgb_[iframe])(i,j).g=255;
                                (fore_ground_rgb_[iframe])(i,j).b=0;
                        }
                        if (color==2) //blue
                        {
                                (fore_ground_rgb_[iframe])(i,j).r=0;
                                (fore_ground_rgb_[iframe])(i,j).g=0;
                                (fore_ground_rgb_[iframe])(i,j).b=255;
                        }
                        
                }

return;
}

void rc3d_windows_frame::save_images() 
{
        vcl_string filename;
        filename=batch_cam_filename_;
        //static vil1_memory_image_of<vil1_rgb<unsigned char> >
        //    convert_to_rgb(vil1_memory_image_of<float> const & image);
        char f11[10];
        vcl_sprintf(f11,"%03d",batch_frame_1_);
        vcl_string outname1=filename+"_"+f11+".png";
                
        //vil1_memory_image_of<vil1_rgb<unsigned char> > image_rgb=
        //    brip_vil1_float_ops::convert_to_rgb((fore_ground_[i]));
                
        vil1_save(fore_ground_rgb_[0],outname1.c_str());
                
        vcl_sprintf(f11,"%03d",batch_frame_2_);
        vcl_string outname2=filename+"_"+f11+".png";
                
                
        vil1_save(fore_ground_rgb_[1],outname2.c_str());
                
        vcl_sprintf(f11,"%03d",batch_frame_3_);
        vcl_string outname3=filename+"_"+f11+".png";
                
                
        vil1_save(fore_ground_rgb_[2],outname3.c_str());
                
}












//8-3-2004 For Vorl
void rc3d_windows_frame::load_image_file(vcl_string image_filename, bool greyscale,  unsigned col)
{
  vil1_image temp= vil1_load(image_filename.c_str());
   if (greyscale)
   {
      vil1_memory_image_of<unsigned char> temp1 =
         brip_vil1_float_ops::convert_to_grey(temp);
      img_ = temp1;
      vil1_memory_image_of<float>  temp1_float =
         brip_vil1_float_ops::convert_to_float(temp1);
      (fore_ground_[col]).set(temp1_float);
   }
   else
      img_ = temp;
 }
/*
void rc3d_windows_frame::show_epipole()
{
instance_->easy_2d_->set_foreground(1, 0, 0);
//vgl_point_2d<double> e = kalman_->get_cur_epipole();

  vcl_cout<<"\n epipole ("<<e.x() <<'\t'<<e.y()<<")\n";
  instance_->easy_2d_->add_point(e.x(), e.y());
}*/


/*
void rc3d_windows_frame::init_epipole()
{
if (!e_)
e_ = new vgl_point_2d<double>;

  assert(lines_.size() >= 2);
  vgl_homg_point_2d<double> epipole = vgl_homg_operators_2d<double>::lines_to_point(lines_);
  
   vcl_cout<<"epipole = ("<<epipole.x()<<' ' << epipole.y() << ' '<< epipole.w()<<")\n";
   
     vgl_point_2d<double> pt(epipole);
     e_ -> set(pt.x(), pt.y());
     
      kalman_->init_epipole(pt.x(), pt.y());
      
        vcl_stringstream ss;
        ss<<pt.x()<<' '<<pt.y();
        status_info_ += ss.str();
        }
*/

//===================================================================
//: pick a point in the left image and show the corresponding epipolar
//  line in the right image
//===================================================================
void rc3d_windows_frame::show_epipolar_line()
{
  //this->clear_display();
  vgui::out << "pick point in left image\n";
  unsigned int col=0, row=0;//left image
  //bgui_picker_tableau_sptr pkt = this->get_picker_tableau_at(col, row);
  bgui_picker_tableau_sptr pkt = this->get_selected_picker_tableau();
  if (!pkt)
  {
    vcl_cout << "In rc3d_multiview_manager::show_epipolar_line() - null tableau\n";
    return;
  }
  float x = 0, y=0;
  pkt->pick_point(&x, &y);
  vgui::out << "p(" << x << ' ' << y << ")\n";
  vcl_cout << "p(" << x << ' ' << y << ")\n";

  //col = 1;//right image
  
  
  //temporary test for FMatrix
  if (!epi_flag) {
     vcl_cout<<"epipole needed!\n"<<vcl_endl;;
     return;
  }
  vnl_double_3x3 F;
  F[0][0] = 0;     F[0][1] = -epi_[2];  F[0][2] = epi_[1];
  F[1][0] = epi_[2];  F[1][1] = 0;      F[1][2] = -epi_[0];
  F[2][0] = -epi_[1]; F[2][1] = epi_[0];   F[2][2] = 0;
  
  FMatrix FM(F);
  //FMatrix f = this->test_fmatrix();
  vgl_homg_point_2d<double> pl(x,y);
  vgl_homg_line_2d<double> lr = FM.image2_epipolar_line(pl);
  //end test
  for (unsigned i=0;i<iview_-1;i++) {
     bgui_vtol2D_tableau_sptr v2D = this->get_vtol2D_tableau_at(i,row);
     if (v2D)
     {
        v2D->add_infinite_line(lr.a(), lr.b(), lr.c());
        v2D->post_redraw();
     }
  }
}

void rc3d_windows_frame::creat_line()
{
   float x1=0, y1=0, x2=0, y2=0;
   vcl_cout<<"pick a line\n";
   tab_picker_->pick_line(&x1, &y1, &x2, &y2);
   vgl_homg_point_2d<double> p1(x1, y1, 1), p2(x2, y2, 1);
   vgl_homg_line_2d<double> l(p1, p2);
   lines_.push_back(l);
   vtol_tabs_[0]->add_infinite_line( y1-y2, x2-x1, x1*y2 - x2*y1 );
   instance_->post_redraw();
}


void rc3d_windows_frame::save_status()
{
   vgui_dialog save_file_dlg("save status");
   static vcl_string filename = "";
   static vcl_string ext = "*.*";
   save_file_dlg.file("file name", ext, filename);
   if (!save_file_dlg.ask())
      return;
   
   if (filename != "")
   {
      vcl_ofstream of(filename.c_str());
      //of << status_info_;
      of<<epi_;
   }
}

void rc3d_windows_frame::load_status()
{
   vgui_dialog load_file_dlg("load status");
   static vcl_string filename = "C:/E3D/REAL/ICPR2004/Probe_Depot/epi_1new_for_debug_van.txt";
   static vcl_string ext = "*.*";
   load_file_dlg.file("file name", ext, filename);
   if (!load_file_dlg.ask())
      return;
   
   if (filename != "")
   {
      vcl_ifstream in(filename.c_str());
      
   //// epi ..........   
      double x, y,z;
      in >> x >> y>>z;
      epi_[0]=x;
      epi_[1]=y;
      epi_[2]=z;
      vcl_cout<<"ur epipole is..:"<<epi_<<vcl_endl;
      instance_->vtol_tabs_[0]->add_point(epi_[0]/epi_[2], epi_[1]/epi_[2]);
      epi_flag=true;
      /*if (!e_)
      e_ = new vgl_point_2d<double>;
      e_ -> set(x, y);*/
      
      /*kalman_->init_epipole(x, y);
      vcl_stringstream ss;
      ss << x <<' '<<y;
      status_info_ += ss.str();*/
   }
}


void rc3d_windows_frame::vd_edges()
{
   static bool agr = false;
   static bool clear = true;
   
   static sdet_detector_params dp;
   dp.junctionp=false;
   vgui_dialog vd_dialog("VD Edges");
   vd_dialog.field("Gaussian sigma", dp.smooth);
   vd_dialog.field("Noise Threshold", dp.noise_multiplier);
   vd_dialog.checkbox("Automatic Threshold", dp.automatic_threshold);
   vd_dialog.checkbox("Agressive Closure", agr);
   vd_dialog.checkbox("Compute Junctions", dp.junctionp);
   vd_dialog.checkbox("Clear", clear);
   if (!vd_dialog.ask())
      return;
   if (agr)
      dp.aggressive_junction_closure=1;
   else
      dp.aggressive_junction_closure=0;
   sdet_detector det(dp);
   
   bgui_vtol2D_tableau_sptr btab = this->get_selected_vtol2D_tableau();
   
   if (btab)
   {
      vgui_image_tableau_sptr itab = btab->get_image_tableau();
      img_ = itab->get_image();
   }
   else
   {
      vcl_cout << "In rc3d_WINDOWS_FRAME::vd_edges() - null tableau\n";
      return;
   }
   det.SetImage(img_);
   det.DoContour();
   vcl_vector<vtol_edge_2d_sptr>* edges = det.GetEdges();
   
   //display the edges
   if (btab&&edges)
   {
      if (clear)
         btab->clear_all();
      btab->add_edges(*edges, true);
      btab->post_redraw();
   }
   else
   {
      vcl_cout << "In rc3d_windows_frame::vd_edges() - null edges or null tableau\n";
      return;
   }
}
// 5-12-2004 background modeling
void rc3d_windows_frame::vd_back_ground()
{

}
// 3-3-2004 to find occluding contour ..
void rc3d_windows_frame::vd_subtract(){

   static bool agr = false;
   static bool clear = true;
   
   static sdet_detector_params dp;
   static int sub_image= 0;
   static float subtraction_threshold=2.0;
   static bool smoothing_flag=false;
   static bool show_subtracted_image=false;
   static bool use_back_ground_image=true;
   static float smoothing_gauss_sigma=4.0;

   dp.junctionp=false;
   vgui_dialog vd_dialog("VD Edges");
   vd_dialog.field("Gaussian sigma", dp.smooth);
   vd_dialog.field("Noise Threshold", dp.noise_multiplier);
   vd_dialog.checkbox("Automatic Threshold", dp.automatic_threshold);
   vd_dialog.checkbox("Agressive Closure", agr);
   vd_dialog.checkbox("Compute Junctions", dp.junctionp);
   vd_dialog.checkbox("Clear", clear);
   vd_dialog.checkbox("Show Subtracted Image?",show_subtracted_image);
   vd_dialog.field("subtraction image",sub_image);
   vd_dialog.field("subtraction threshold",subtraction_threshold);
   vd_dialog.checkbox("Smoothing for subtraction?",smoothing_flag);
   vd_dialog.field("Smoothing Gauss Sigma",smoothing_gauss_sigma);
   vd_dialog.checkbox("use back_ground_?",use_back_ground_image);
   vil1_image img_sub;

   if (!vd_dialog.ask())
      return;
   if (agr)
      dp.aggressive_junction_closure=1;
   else
      dp.aggressive_junction_closure=0;
   sdet_detector det(dp);
   
   bgui_vtol2D_tableau_sptr btab = this->get_selected_vtol2D_tableau();
   
    vgui_image_tableau_sptr itab;
   if (btab)
   {
      itab = btab->get_image_tableau();
      img_ = itab->get_image();
   }
   else
   {
      vcl_cout << "In rc3d_WINDOWS_FRAME::vd_edges() - null tableau\n";
      return;
   }
   bgui_vtol2D_tableau_sptr btab_sub=this->get_vtol2D_tableau_at(sub_image,0);
    if (btab_sub)
   {
      vgui_image_tableau_sptr itab_sub = btab_sub->get_image_tableau();
      img_sub = itab_sub->get_image();
   }
   else
   {
      vcl_cout << "In rc3d_WINDOWS_FRAME::vd_edges_sub() - null tableau\n";
      return;
   }

    vil1_memory_image_of<unsigned char> img_sub1 =
         brip_vil1_float_ops::convert_to_grey(img_sub);

   vil1_memory_image_of<float> temp1 =
         brip_vil1_float_ops::convert_to_float(img_sub1);


   vil1_memory_image_of<unsigned char> img1_ =
         brip_vil1_float_ops::convert_to_grey(img_);

   vil1_memory_image_of<float> temp11=
      brip_vil1_float_ops::convert_to_float(img1_);

    
   
   vil1_memory_image_of<float> img_subtracted;
   if (use_back_ground_image) {
      if ( !back_ground_) {
         vcl_cout<<"null back_ground_"<<vcl_endl;
         return;
      }
      img_subtracted=
         brip_vil1_float_ops::difference(temp11,back_ground_);
   }

   else {
      img_subtracted=brip_vil1_float_ops::difference(temp11,temp1);
   }
   // take absolute value
   if (show_subtracted_image) {
      itab->set_image(img_subtracted);
   }
   //vil1_memory_image_of<float> output;
   int w = img_subtracted.width(), h = img_subtracted.height();
   //output.resize(w,h);
   for (int y = 0; y < h ; y++)
      for (int x = 0; x < w ; x++)
      {
         //vil1_rgb<float> rgb = image(x,y);
         if (img_subtracted(x,y)<0) img_subtracted(x,y)=-img_subtracted(x,y);
         //img_subtracted(x,y)=vcl_fabs(img_subtracted(x,y));
         //vcl_cout<<x<<" "<<y<<" "<<img_subtracted(x,y)<<vcl_endl;
         if (img_subtracted(x,y)>subtraction_threshold)
            img_subtracted(x,y)=200;
         else img_subtracted(x,y)=0.0;
         //vcl_cout<<x<<" "<<y<<" "<<img_subtracted(x,y)<<vcl_endl;
         //output(x,y) = (float)rgb.grey();
      }

   int neighborhood_size=2;
   if (smoothing_flag) {
      
      for (int y=0+neighborhood_size; y<h-neighborhood_size; y++)
         for (int x =0+neighborhood_size; x<w-neighborhood_size; x++) {
            float sum=0.0;
            for ( int y_nbh=-neighborhood_size;y_nbh<neighborhood_size+1;y_nbh++) {
               for ( int x_nbh=-neighborhood_size;x_nbh<neighborhood_size+1;x_nbh++) {
                  if (y_nbh==0 && x_nbh==0) ;
                  else {
                     sum+=img_subtracted(x+x_nbh, y+y_nbh);
                  }
                  
               }
            }

            if (sum>200*(vcl_pow(2*neighborhood_size+1,2.0)-1)*.6) img_subtracted(x,y)=200.0;
            if (sum<200*(vcl_pow(2*neighborhood_size+1,2.0)-1)*.4) img_subtracted(x,y)=0.0;
         }
   }


   det.SetImage(img_subtracted);
   det.DoContour();
   vcl_vector<vtol_edge_2d_sptr>* edges = det.GetEdges();
   
   //display the edges
   if (btab&&btab_sub&&edges)
   {
      if (clear)
         btab->clear_all();
      btab->add_edges(*edges, true);
      btab->post_redraw();
   }
   else
   {
      vcl_cout << "In rc3d_windows_frame::vd_edges() - null edges or null tableau\n";
      return;
   }
}
//void rc3d_windows_frame::filling_hole(vil1_memory_image_of<float> const & image, int interval) {
//   int w = image.width(), h = image.height();
//   //output.resize(w,h);
//   for (int y = 0; y < h ; y+=interval)
//      for (int x = 0; x < w ; x+=interval) {
         
   
void rc3d_windows_frame::auto_vd_edges(unsigned col, unsigned row)
{
   static bool agr = false;
   static bool clear = true;
   
   static sdet_detector_params dp;
   dp.junctionp=false;
   dp.aggressive_junction_closure=0;
   dp.noise_multiplier=2.5;
   sdet_detector det(dp);
      
    bgui_vtol2D_tableau_sptr btab = this->get_vtol2D_tableau_at(col, row);   
   //bgui_vtol2D_tableau_sptr btab = this->get_selected_vtol2D_tableau();
   
   if (btab)
   {
      vgui_image_tableau_sptr itab = btab->get_image_tableau();
      img_ = itab->get_image();
   }
   else
   {
      vcl_cout << "In bmvv_multiview_manager::vd_edges() - null tableau\n";
      return;
   }
   det.SetImage(img_);
   det.DoContour();
   vcl_vector<vtol_edge_2d_sptr>* edges = det.GetEdges();
   
   //display the edges
   if (btab&&edges)
   {
      if (clear)
         btab->clear_all();
      btab->add_edges(*edges, true);
      btab->post_redraw();
   }
   else
   {
      vcl_cout << "In rc3d_windows_frame::vd_edges() - null edges or null tableau\n";
      return;
   }
}
void rc3d_windows_frame::auto_vd_edges()
{
   static bool agr = false;
   static bool clear = true;
   
   static sdet_detector_params dp;
   dp.junctionp=false;
   dp.aggressive_junction_closure=0;
   sdet_detector det(dp);
    //bgui_vtol2D_tableau_sptr btab = this->get_vtol2D_tableau_at(col, row);   
   bgui_vtol2D_tableau_sptr btab = this->get_selected_vtol2D_tableau();
   
   if (btab)
   {
      vgui_image_tableau_sptr itab = btab->get_image_tableau();
      img_ = itab->get_image();
   }
   else
   {
      vcl_cout << "In bmvv_multiview_manager::vd_edges() - null tableau\n";
      return;
   }
   det.SetImage(img_);
   det.DoContour();
   vcl_vector<vtol_edge_2d_sptr>* edges = det.GetEdges();
   
   //display the edges
   if (btab&&edges)
   {
      if (clear)
         btab->clear_all();
      btab->add_edges(*edges, true);
      btab->post_redraw();
   }
   else
   {
      vcl_cout << "In rc3d_windows_frame::vd_edges() - null edges or null tableau\n";
      return;
   }
}


//4-27-04 Dongjin Han ..
void rc3d_windows_frame::vd_edges_map()
{

   static sdet_detector_params dp;
   dp.junctionp=false;
   dp.aggressive_junction_closure=0;
   //sdet_detector det(dp);
    //bgui_vtol2D_tableau_sptr btab = this->get_vtol2D_tableau_at(col, row);   
   unsigned int col,row;

   static bool agr = false;
   static bool clear = true;
   
   static float threshold =1;
   static int N=10;
   static bool show_map=true;
   dp.junctionp=false;
   vgui_dialog vd_dialog("VD Edges");
   vd_dialog.field("Gaussian sigma", dp.smooth);
   vd_dialog.field("Noise Threshold", dp.noise_multiplier);
   vd_dialog.checkbox("Automatic Threshold", dp.automatic_threshold);
   vd_dialog.checkbox("Agressive Closure", agr);
   vd_dialog.checkbox("Compute Junctions", dp.junctionp);
   vd_dialog.checkbox("Clear", clear);
   vd_dialog.field("threshold",threshold);
   vd_dialog.field("kernel size",N);
    vd_dialog.checkbox("show map?", show_map);

   if (!vd_dialog.ask())
      return;
   if (agr)
      dp.aggressive_junction_closure=1;
   else
      dp.aggressive_junction_closure=0;
   sdet_detector det(dp);

   grid_->get_last_selected_position(&col, &row);
   bgui_vtol2D_tableau_sptr btab = this->get_selected_vtol2D_tableau();
   vgui_image_tableau_sptr itab;
   if (btab)
   {
       itab= btab->get_image_tableau();
      img_ = itab->get_image();
   }
   else
   {
      vcl_cout << "In bmvv_multiview_manager::vd_edges() - null tableau\n";
      return;
   }
   det.SetImage(img_);
   det.DoContour();
   vcl_vector<vtol_edge_2d_sptr>* edges = det.GetEdges();
   vcl_vector<vgui_soview*> sovs_all;
   //display the edges
   if (btab&&edges)
   {
      if (clear)
         btab->clear_all();
      btab->add_edges(*edges, true);
      btab->post_redraw();
      sovs_all= (btab)->get_all();
   }
   else
   {
      vcl_cout << "In rc3d_windows_frame::vd_edges() - null edges or null tableau\n";
      return;
   }

     vil1_memory_image_of<unsigned char> img_unsigned_char =
         brip_vil1_float_ops::convert_to_grey(img_);
    vil1_memory_image_of<float> map=
       brip_vil1_float_ops::convert_to_float(img_unsigned_char);

    
    //vil1_memory_image_of<float> output;
    int w = map.width(), h = map.height();
    //output.resize(w,h);
    /*for (int y = 0; y < h ; y++) {
       for (int x = 0; x < w ; x++)
       {
          map(x,y)=0.0;
       }
    }*/
    map.fill(0.0);
    
    //copy 
    (maps_[col]).set(map);
    
    
    vgui_soview* sov;
    vdgl_edgel_chain_sptr  ec;
    
    vdgl_digital_curve_sptr dc;
    unsigned int xi,yi;
    
    for (unsigned int i = 0; i<sovs_all.size(); i++)
    {
       sov= sovs_all[i];
       int id=sovs_all[i]->get_id();
       
       // interested only in edge..not vertex..etc
       if (sov->type_name()!="bgui_vtol_soview2D_edge") continue;
       
       vtol_edge_2d_sptr e = (btab)->get_mapped_edge(id);
       
       vsol_curve_2d_sptr c = e->curve();
       dc = c->cast_to_vdgl_digital_curve();
       
       vdgl_interpolator_sptr interp = dc->get_interpolator();
       ec = interp->get_edgel_chain();
       
       
       //int x0_index = bdgl_curve_algs:: closest_point(ec,x1 , y1);
       for (int x0_index=0;x0_index<(*ec).size();x0_index++) {
          //float a=(x-(*ec)[x0_index].get_x())*(x-(*ec)[x0_index].get_x())+
          //    (y-(*ec)[x0_index].get_y())*(y-(*ec)[x0_index].get_y());
          
          xi=floor((*ec)[x0_index].get_x());
          yi=floor((*ec)[x0_index].get_y());
          float theta=(*ec)[x0_index].get_theta();
          (maps_[col])(xi,yi)=theta;       
          map(xi,yi)++;      
          
       }
    }
    
    

    
   vbl_array_2d<float> kernel(N,N,1.0);
    vil1_memory_image_of<float> map1=
             brip_vil1_float_ops::convolve(map,kernel);   

   for (int y = 0; y < h ; y++) {
      for (int x = 0; x < w ; x++) {
         
         //xi=floor((*ec)[x0_index].get_x());
         //yi=floor((*ec)[x0_index].get_y());
         
         if (map1(x,y)>threshold) map1(x,y)=125.0;
         
         
      }
    }
   maps_search_[col].set(map1);
    if (show_map) {
      itab->set_image(map1);
   }
    grid_->post_redraw();
    
}

//5-7-04 Dongjin Han .. multiple windows..
void rc3d_windows_frame::vd_edges_map_all_in()
{

  
   static sdet_detector_params dp;
   dp.junctionp=false;
   dp.aggressive_junction_closure=0;
   //sdet_detector det(dp);
    //bgui_vtol2D_tableau_sptr btab = this->get_vtol2D_tableau_at(col, row);   
   unsigned int col,row;

   static bool agr = false;
   static bool clear = true;
   
   static float threshold =1;
   static int N=10;
   static bool show_map=false;
   dp.junctionp=false;
   //dp.smooth=2.0;
   dp.noise_multiplier=2.0;
   vgui_dialog vd_dialog("VD Edges");
   vd_dialog.field("Gaussian sigma", dp.smooth);
   vd_dialog.field("Noise Threshold", dp.noise_multiplier);
   vd_dialog.checkbox("Automatic Threshold", dp.automatic_threshold);
   vd_dialog.checkbox("Agressive Closure", agr);
   vd_dialog.checkbox("Compute Junctions", dp.junctionp);
   vd_dialog.checkbox("Clear", clear);
   vd_dialog.field("threshold",threshold);
   vd_dialog.field("kernel size",N);
         vd_dialog.checkbox("show map?", show_map);
         
         // batch mode
   if (!BATCH_)
         {
                 if (!vd_dialog.ask())
                         return;
         }
                 
         if (agr)
                 dp.aggressive_junction_closure=1;
         else
                 dp.aggressive_junction_closure=0;
                 
                 //vcl_cout<<dp<<vcl_endl;
   //grid_->get_last_selected_position(&col, &row);
   //bgui_vtol2D_tableau_sptr btab = this->get_selected_vtol2D_tableau();
   vgui_image_tableau_sptr itab;
   
   for (unsigned it=0;it<3;it++) 
   {
      bgui_vtol2D_tableau_sptr btab=get_vtol2D_tableau_at(it,0);
      if (btab)
      {
         itab= btab->get_image_tableau();
         img_ = itab->get_image();
      }
      else
      {
         vcl_cout << "In bmvv_multiview_manager::vd_edges() - null tableau\n";
         return;
      }
      //if (it==0) vil1_save(img_,"brrc.png");
      sdet_detector det(dp);
      det.SetImage(img_);
      det.DoContour();
      vcl_vector<vtol_edge_2d_sptr>* edges = det.GetEdges();
      vcl_vector<vgui_soview*> sovs_all;
//      vcl_cout<<edges->size()<<vcl_endl;

      //display the edges
      if (btab&&edges)
      {
         if (clear) {
            btab->clear_all();
         }
                                 //vgui_style_sptr  style = vgui_style::new_style(0,1,0,2, 1);
         //btab->add_edges(*edges, true,style);
                                 btab->add_edges(*edges, true);

                                 
                                 //btab->set_edgel_curve_style( style);
         btab->post_redraw();
         sovs_all= (btab)->get_all();
         vcl_cout<<sovs_all.size()<<vcl_endl;
       vcl_cout<<edges->size()<<vcl_endl;

      }
      else
      {
         vcl_cout << "In rc3d_windows_frame::vd_edges() - null edges or null tableau\n";
         return;
      }
      
      vil1_memory_image_of<unsigned char> img_unsigned_char =
         brip_vil1_float_ops::convert_to_grey(img_);
      vil1_memory_image_of<float> map=
         brip_vil1_float_ops::convert_to_float(img_unsigned_char);
      
      
      //vil1_memory_image_of<float> output;
      int w = map.width(), h = map.height();
      //output.resize(w,h);
   
      map.fill(0.0);
      
      //copy
      
      //6-10-2004 it=col?
      col=it;
      (maps_[col]).set(map);
      
      
      vgui_soview* sov;
      vdgl_edgel_chain_sptr  ec;
      
      vdgl_digital_curve_sptr dc;
      unsigned int xi,yi;
      
      for (unsigned int i = 0; i<sovs_all.size(); i++)
      {
         sov= sovs_all[i];
         int id=sovs_all[i]->get_id();
         
         // interested only in edge..not vertex..etc
         if (sov->type_name()!="bgui_vtol_soview2D_edge") continue;
         
         vtol_edge_2d_sptr e = (btab)->get_mapped_edge(id);
         
         vsol_curve_2d_sptr c = e->curve();
         dc = c->cast_to_vdgl_digital_curve();
         
         vdgl_interpolator_sptr interp = dc->get_interpolator();
         ec = interp->get_edgel_chain();
         
         
         //int x0_index = bdgl_curve_algs:: closest_point(ec,x1 , y1);
         for (int x0_index=0;x0_index<(*ec).size();x0_index++) {
            //float a=(x-(*ec)[x0_index].get_x())*(x-(*ec)[x0_index].get_x())+
            //    (y-(*ec)[x0_index].get_y())*(y-(*ec)[x0_index].get_y());
            
            if (!veh_cons_[it].contains((*ec)[x0_index].get_x(),(*ec)[x0_index].get_y() )) continue;

            xi=floor((*ec)[x0_index].get_x());
            yi=floor((*ec)[x0_index].get_y());
            float theta=(*ec)[x0_index].get_theta();
            (maps_[col])(xi,yi)=theta;       
            map(xi,yi)++;      
            
         }
      }
      
      
      
      
      vbl_array_2d<float> kernel(N,N,1.0);
      vil1_memory_image_of<float> map1=
         brip_vil1_float_ops::convolve(map,kernel);   
      
      for (int y = 0; y < h ; y++) {
         for (int x = 0; x < w ; x++) {
            
            //xi=floor((*ec)[x0_index].get_x());
            //yi=floor((*ec)[x0_index].get_y());
            
            if (map1(x,y)>threshold) map1(x,y)=125.0;
            
            
         }
      }
      maps_search_[col].set(map1);
      if (show_map) {
         itab->set_image(map1);
      }
      
      grid_->post_redraw();
   }
}

//8-3-2004 For Vorl
//5-7-04 Dongjin Han .. multiple windows..
void rc3d_windows_frame::vd_edges_map_all_in_Vorl()
{

   static sdet_detector_params dp;
   dp.junctionp=false;
   dp.aggressive_junction_closure=0;
   //sdet_detector det(dp);
    //bgui_vtol2D_tableau_sptr btab = this->get_vtol2D_tableau_at(col, row);   
   unsigned int col,row;

   static bool agr = false;
   static bool clear = true;
   
   static float threshold =1;
   static int N=5;
   static bool show_map=false;
   dp.junctionp=false;
   //dp.smooth=2.0;
   dp.noise_multiplier=2.0;
   /*vgui_dialog vd_dialog("VD Edges");
   vd_dialog.field("Gaussian sigma", dp.smooth);
   vd_dialog.field("Noise Threshold", dp.noise_multiplier);
   vd_dialog.checkbox("Automatic Threshold", dp.automatic_threshold);
   vd_dialog.checkbox("Agressive Closure", agr);
   vd_dialog.checkbox("Compute Junctions", dp.junctionp);
   vd_dialog.checkbox("Clear", clear);
   vd_dialog.field("threshold",threshold);
   vd_dialog.field("kernel size",N);
    vd_dialog.checkbox("show map?", show_map);
*/
   //if (!vd_dialog.ask())
   //   return;
   if (agr)
      dp.aggressive_junction_closure=1;
   else
      dp.aggressive_junction_closure=0;
   
   vil1_memory_image_of<float> map=fore_ground_[0];
   int w = map.width(), h = map.height();
      //output.resize(w,h);
   
      map.fill(0.0);
   
   for (unsigned col=0;col<3;col++) 
   {
      (maps_[col]).set(map);
      maps_[col].fill(0.0);
      map.fill(0.0);

      sdet_detector det(dp);
      det.SetImage(fore_ground_[col]);
      det.DoContour();
      vcl_vector<vtol_edge_2d_sptr>* edges = det.GetEdges();
      
      
      
      if (!edges)
         return;
      
      // pass the edges
      vsol_curve_2d_sptr c;
      vdgl_digital_curve_sptr dc;
      vdgl_interpolator_sptr interp;
      vdgl_edgel_chain_sptr  ec;
      vcl_vector<vtol_edge_2d_sptr> ecl;
      ecl.clear();
      
      for (unsigned int i=0; i<edges->size(); i++)
      {
         c  = (*edges)[i]->curve();
         dc = c->cast_to_vdgl_digital_curve();
         //if (dc->length()>tp.min_length_of_curves)
         //  ecl.push_back((*edges)[i]);
         vdgl_interpolator_sptr interp = dc->get_interpolator();
         ec = interp->get_edgel_chain();
         
         
         
         for (int x0_index=0;x0_index<(*ec).size();x0_index++) {
            //float a=(x-(*ec)[x0_index].get_x())*(x-(*ec)[x0_index].get_x())+
            //    (y-(*ec)[x0_index].get_y())*(y-(*ec)[x0_index].get_y());
            
            int xi=floor((*ec)[x0_index].get_x());
            int yi=floor((*ec)[x0_index].get_y());
            float theta=(*ec)[x0_index].get_theta();
            (maps_[col])(xi,yi)=theta;       
            map(xi,yi)++;      
            
         }
      }
      vbl_array_2d<float> kernel(N,N,1.0);
      vil1_memory_image_of<float> map1=
         brip_vil1_float_ops::convolve(map,kernel);   
      int w1 = map1.width(), h1 = map1.height();
      for (int y = 0; y < h1 ; y++) {
         for (int x = 0; x < w1 ; x++) {
            
            //xi=floor((*ec)[x0_index].get_x());
            //yi=floor((*ec)[x0_index].get_y());
            
            if (map1(x,y)>threshold) map1(x,y)=125.0;
            
            
         }
      }
      maps_search_[col].set(map1);
      
   }
}
   
     
     
     
     
     
     
     
     
     
     
     




//4-29-2004 Dongjin Han ..
bool rc3d_windows_frame::map_check(double ix,double iy,double iz)
{

    float count=0.0;
   for (unsigned int i_frame=0;i_frame<3;i_frame++) {
      int w = maps_search_[i_frame].width(),
         h = maps_search_[i_frame].height();
      //vnl_matrix<double> temp=back_project(i_frame,ix,iy,iz);
     vnl_matrix<double> temp=back_project_direct(i_frame,ix,iy,iz);
      if (temp[0][0]<0) continue;
      if (temp[0][0]>w-1) continue;
      if (temp[1][0]<0) continue;
      if (temp[1][0]>h-1) continue;
      count+=(maps_search_[i_frame])(temp[0][0],temp[1][0]);
   }
   if (count==0)
      return false;
   else return true;
}


// REAd back ground...
//void rc3d_windows_frame::back_ground_read() 
//{

   
//}
// 7-9-2004 Back ground modeling
void rc3d_windows_frame::back_ground() {
   vgui_dialog dlg("Read backgound file's");
   static vcl_string filename = "C:/CVPR2004/carback.txt";
   static vcl_string ext = "*.*";
   vcl_string str1,str_filename;
    dlg.file("read file name", ext, filename);
   
   if (!dlg.ask()) { 
      vcl_cout<<"Background FIle Read Fail!" <<vcl_endl;
      return;
   }
   
   vnl_double_4x4 RT(0.0);
   //double x,y,z,t;
   double temp=0;
   vnl_double_3x4 Camera(0.0);
   vcl_vector<vnl_double_3x4 > Cam_List;
   int total_camera_number=-1;
   int start_number=0,end_number=0;
   if (filename != "")
      {
         vcl_ifstream in(filename.c_str());
         vcl_cout<<"reading camera file "<< filename.c_str()<<vcl_endl;
         in>> total_camera_number;
         for (unsigned j=0;j<10;j++) {
            in>>str1;
            if (j==2) {
               str_filename=str1;
            }
            if (j==4) {
               start_number =vcl_atoi(str1.c_str());   
            }
            if (j==6) {
               end_number=vcl_atoi(str1.c_str());
            }
            vcl_cout<<str1<<vcl_endl;
         }
         //read R-T matrix...
         for (unsigned i=0;i<4;i++) {
            for (unsigned j=0;j<4;j++) {
               in >> temp;
//               RT[i][j]=temp;
            }
         }
//         vcl_cout<<RT<<vcl_endl;
         //BB_RT_matt_=RT;
         for (unsigned i=0;i<total_camera_number;i++) {
            Camera.fill(0.0);
            in>>str1>>str1;
            vcl_cout<<str1<<vcl_endl;
            for (unsigned j=0;j<3; j++) {
               for (unsigned k=0;k<4;k++) {
                  in >> temp;
                  Camera[j][k]=temp;
               }
            }
            //Cam_List.push_back(Camera);
            vcl_cout<<Camera<<vcl_endl;
         }
      }
   vcl_string f1,f2,f3;
   
   char tf[10], f11[10]; 
   
   bool greyscale=true;
   int w,h;
   
   vcl_vector <vil1_memory_image_of<unsigned char> > image_list;
   for (unsigned i=0;i<total_camera_number;i++) {
      int fram1 = start_number + i ;

      vcl_sprintf(f11,"%03d",fram1);

      f1=str_filename + "/00" + f11+".png";
      vcl_cout<<f1<<vcl_endl;
      //load_image_file("C:/E3D/REAL/Aq2_1_left_brown_U_white_van/00016.tiff", true, 2,0);
      //load_image_file(f1, true, 0,0);
      vil1_image temp = vil1_load(f1.c_str());
      
      
      vil1_memory_image_of<unsigned char> temp1 =
         brip_vil1_float_ops::convert_to_grey(temp);
      //img_ = temp1;
      w=temp1.width();
      h=temp1.height();
      image_list.push_back(temp1);
   }
   (back_ground_).set(image_list[0]);
   back_ground_.fill(0);
   vcl_vector <float> v;
   for (unsigned x=0;x<w;x++) {
      //vcl_cout<<x<<" ";
      for (unsigned y=0;y<h;y++) {
         for (unsigned i=0;i<total_camera_number;i++) {
            
            
            v.push_back((image_list[i])(x,y));
            
         }
         float v_back=back_ground_clustering(v);
         back_ground_(x,y)=v_back;
         v.clear();

      }
   }
   bgui_vtol2D_tableau_sptr btab=this->get_vtol2D_tableau_at(3,0);
    if (btab)
   {
      vgui_image_tableau_sptr itab = btab->get_image_tableau();
      itab->set_image(back_ground_);
      //img_sub = itab_sub->get_image();
   }
   else
   {
      vcl_cout << "In rc3d_WINDOWS_FRAME::back_ground() - null tableau\n";
      return;
   }

   grid_->post_redraw();



}
#include <vcl_algorithm.h>
struct smalla
{
   bool operator() (float  x, float y)
   { return x > y; }
};

float rc3d_windows_frame::back_ground_clustering(vcl_vector <float> v)
{

   float v0,v1;

   vcl_sort(v.begin(),v.end(),smalla());
//   for (unsigned i=0;i<v.size();i++)
//      vcl_cout<<i<<": "<<v[i]<<vcl_endl;
      
   float threshold=5.0;
   v0=v[0]; //init
   float count0=0;
   for (unsigned i=1;i<v.size();i++) {
      
      if (vcl_abs(v[i]-v0)<threshold)
      {   count0++;
         v0=(v[i]+(count0)*v0)/(count0+1);
      }
   }
   
    v1=v[v.size()-1]; //init
   float count1=0;
   for (unsigned i=v.size();i>-1;i--) {
      
      if (vcl_abs(v[i]-v1)<threshold)
      {   count1++;
         v1=(v[i]+(count1)*v1)/(count1+1);
      }
   }

   if (count0>count1) {return v0;}
   else return(v1);
   


   
   
}
//// 4-25-04 Dongjin Han fit lines to edgel chain..
void rc3d_windows_frame::fit_line()
{
   bgui_vtol2D_tableau_sptr btab = this->get_selected_vtol2D_tableau();
   
   if (btab)
   {
      vgui_image_tableau_sptr itab = btab->get_image_tableau();
      img_ = itab->get_image();
   }
   else
   {
      vcl_cout << "In rc3d_manager::fit_line() - null tableau\n";
      return;
   }
   //vcl_vector<vdgl_edgel_chain_sptr> ec_in_BOX
//      =find_sovs_in_BOX(bit,s_x,s_y, R_+range);
   //vcl_vector<vdgl_edgel_chain_sptr> ec_inR;
   vdgl_edgel_chain_sptr  ec;
   
   vdgl_digital_curve_sptr dc;
   //vgui_soview* sov_selected;
   //vcl_vector<vgui_soview*> sovs_all= (bit)->get_all();
    //vgui_soview sov_selected=(btab)->get_selected_soviews();
    vcl_vector<vgui_soview*> sovs = (btab)->get_selected_soviews();
    vgui_soview* sov = sovs[0];
   int id=sov->get_id();
   vtol_edge_2d_sptr e = (btab)->get_mapped_edge(id);
   
   vsol_curve_2d_sptr c = e->curve();
   dc = c->cast_to_vdgl_digital_curve();
   
   vdgl_interpolator_sptr interp = dc->get_interpolator();
   ec = interp->get_edgel_chain();
   osl_edgel_chain oc =osl_edgel_chain(ec->size());
//   vcl_list<osl_edge *> *osedges=new osl_edge();


   for (unsigned j=0;j<ec->size();j++) {
      double x1=(*ec)[j].get_x();
      double y1=(*ec)[j].get_y();
      double th1=(*ec)[j].get_theta();
      double gr1=(*ec)[j].get_grad();
      oc.SetGrad(gr1,j);
      oc.SetTheta(th1,j);
      oc.SetX(x1,j);
      oc.SetY(y1,j);

   }
   vcl_list<osl_edge *> oslist;
   for (unsigned j=0;j<ec->size()-1;j++) {
      //double x1=(*ec)[j].get_x();
      //double y1=(*ec)[j].get_y();
      //double th1=(*ec)[j].get_theta();

      //double x2=(*ec)[j+1].get_x();
      //double y2=(*ec)[j+1].get_y();
      //double th2=(*ec)[j+1].get_theta();
      //osl_vertex *v1=new osl_vertex(x1,y1,0);
      //osl_vertex *v2=new osl_vertex(x2,y2,0);
      //osl_edge  *os=new osl_edge(oc,v1,v2);
      //oslist.push_back( os);
   }

   
double x11=(*ec)[0].get_x();
      double y11=(*ec)[0].get_y();
      double th11=(*ec)[0].get_theta();

      double x22=(*ec)[ec->size()-1].get_x();
      double y22=(*ec)[ec->size()-1].get_y();
      double th22=(*ec)[ec->size()-1].get_theta();
      osl_vertex *v1=new osl_vertex(x11,y11,0);
      osl_vertex *v2=new osl_vertex(x22,y22,0);
    osl_edge  *os=new osl_edge(oc,v1,v2);

   oslist.push_back(os);

   vcl_list<osl_edge *> out;
   osl_fit_lines_params para;
   osl_fit_lines *line=new osl_fit_lines(para);
   //osl_fit_lines::simple_fit_to_list(oslist,out);
   
   //line->simple_fit_to_list(&oslist,&out);
   line->incremental_fit_to_list(&oslist,&out);
   
   ////vcl_list<osl_edge *>::iterator itb = out.begin();
   ////osl_edge * tempb = (*itb);
   osl_edge * tempb = out.front();
   float x0= tempb->GetStartX();
   float y0= tempb->GetStartY();
//   

//   vcl_list<osl_edge *>::iterator ite = out.back();

   osl_edge * tempe = out.back();
   float x1= tempe->GetStartX();
   float y1= tempe->GetStartY();
   btab->set_line_width(3);
   btab->set_foreground(1,0,1);
   btab->add_line(x0,y0,x1,y1);
   //btab->post_redraw();

   btab->set_foreground(0,0,1);
   for (vcl_list<osl_edge *>::iterator it = out.begin(); it != out.end( ); it++ ) {
      osl_edge * temp = (*it);
      x0=temp->GetStartX();
      y0=temp->GetStartY();
      x1=temp->GetEndX();
      y1=temp->GetEndY();
      btab->add_line(x0,y0,x1,y1);
   }
   btab->post_redraw();
   //vcl_cout<<out<<vcl_endl;
   
   return;

   //osl_edge oe= new osl_edge(oc,1,1);

   /*for (unsigned int i = 0; i<sovs_all.size(); i++)
   {
      sov= sovs_all[i];
      int id=sovs_all[i]->get_id();
      
      // interested only in edge..not vertex..etc
      if (sov->type_name()!="bgui_vtol_soview2D_edge") continue;
      
      vtol_edge_2d_sptr e = (bit)->get_mapped_edge(id);
      
      vsol_curve_2d_sptr c = e->curve();
      dc = c->cast_to_vdgl_digital_curve();
      
      vdgl_interpolator_sptr interp = dc->get_interpolator();
      ec = interp->get_edgel_chain();
      
      int x0_index = bdgl_curve_algs:: closest_point(ec,x1 , y1);
      float a=(x1-(*ec)[x0_index].get_x())*(x1-(*ec)[x0_index].get_x())+
         (y1-(*ec)[x0_index].get_y())*(y1-(*ec)[x0_index].get_y());
      
      
      if (vcl_sqrt(a)<BOX) { 
         ec_inR.push_back(ec);
      }
   }
   return ec_inR;*/




}
/////////////////

bgui_vtol2D_tableau_sptr rc3d_windows_frame::get_selected_vtol2D_tableau()
{
   unsigned int row =0, col=0;
   grid_->get_last_selected_position(&col, &row);
   return this->get_vtol2D_tableau_at(col, row);
}


bgui_vtol2D_tableau_sptr rc3d_windows_frame::get_vtol2D_tableau_at(unsigned col, unsigned row)
{
   if (row!=0)
      return 0;
   bgui_vtol2D_tableau_sptr btab = 0;
   //if (col==0||col==1||col==2)
      btab = vtol_tabs_[col];
   return btab;
}

//////
void rc3d_windows_frame::select_curve_corres()
{
   vdgl_edgel_chain_sptr  ec;
   vdgl_digital_curve_sptr dc;
   bgui_picker_tableau_sptr ptab = this->get_selected_picker_tableau();
   unsigned int row=0, col=0;
   
    grid_->get_last_selected_position(&col, &row);
   vcl_cout<<col<<row<<vcl_endl;

   //for (vcl_vector<bgui_vtol2D_tableau_sptr>::iterator bit = vtol_tabs_.begin();
   //bit != vtol_tabs_.end(); bit++)
   //   if (*bit)
      bgui_vtol2D_tableau_sptr bit=vtol_tabs_[col];
      {
         vcl_vector<vgui_soview*> sovs = (bit)->get_selected_soviews();
         //vcl_ofstream w_out=vcl_ofstream("cv.txt");
         for (unsigned int i = 0; i<sovs.size(); i++)
         {
            vgui_soview* sov = sovs[i];
            int id = sov->get_id();
            vcl_cout << "id = " << id << "\n";
            
            vtol_edge_2d_sptr e = (bit)->get_mapped_edge(id);
            //vcl_cout<<(*bit)->get_mapped_edge(id)<<vcl_endl;
            vsol_curve_2d_sptr c = e->curve();
            
            
            dc = c->cast_to_vdgl_digital_curve();

            ///dcs[col]=c->cast_to_vdgl_digital_curve();
            //vcl_cout<<   dc->size()<<dc->get_x(i)<<vcl_endl;
            // pilou's code
            
            vdgl_interpolator_sptr interp = dc->get_interpolator();
            ec = interp->get_edgel_chain();
            //ecs[col]=interp->get_edgel_chain();
            static vcl_string id_num="a";
            
            ////vcl_ofstream w_out=vcl_ofstream("cv.txt");
            
            int nn=ec->size();
            ///w_out<< "id = " << id << "\n";
            ///for (unsigned int ii=0;ii<nn;ii++) {
            //   vdgl_edgel ed1=(*ec)[ii];
            ///   w_out<<"["<<ed1.get_x()<<","<<' '<<ed1.get_y()<<"]"<<' '<<"0  0"<<"\n";
            ///}
            ///w_out<<"\n\n";
            
            /////bdgl_curve_description descr(ec);
            /////descr.info();
            
            //if (dc) (bit)->add_digital_curve(dc);
         }
         
         
      }
      if (ec) {
         
         
         float x1=0, y1=0, x2=0, y2=0;
         
         vcl_cout<<"pick a start point\n";
         ptab->pick_point(&x1, &y1);
         vcl_cout<<x1<<" "<<y1<<vcl_endl;
         
         vcl_cout<<"pick a end point\n";
         ptab->pick_point(&x2, &y2);
         vcl_cout<<x2<<" "<<y2<<vcl_endl;
         
         int n=ec->size();
         int split_index1=0;
         double d = 1e10;
         for (int i=0;i+1<n;i++) {
            //vdgl_edgel ed1=(*ec)[i];
            float x_1=(*ec)[i].get_x(), y_1=(*ec)[i  ].get_y(),
               x_2=(*ec)[i+1].get_x(), y_2=(*ec)[i+1].get_y();
            float e = vgl_distance2_to_linesegment( x_1, y_1,x_2,y_2,x1,y1);
            if (e < d) { d=e; split_index1 = i+1;}
         }
         int split_index2=0;
         d = 1e10;
         for (int i=0;i+1<n;i++) {
            //vdgl_edgel ed1=(*ec)[i];
            float x_1=(*ec)[i].get_x(), y_1=(*ec)[i  ].get_y(),
               x_2=(*ec)[i+1].get_x(), y_2=(*ec)[i+1].get_y();
            float e = vgl_distance2_to_linesegment(x_1,y_1,x_2,y_2,x2,y2);
            if (e < d) { d=e; split_index2 = i+1;}
         }
         vcl_cout<<"col:" <<col<<" "<<vcl_endl;
         
         //copy needed..
         //esc[col].
         vdgl_digital_curve_sptr temp_dc1,temp_dc2,dummy;
         vdgl_edgel_chain_sptr  ec1;
         if (split_index1<split_index2) {
            ecs[col]=ec->extract_subchain(split_index1, split_index2);
            dcs[col]=new vdgl_digital_curve(new vdgl_interpolator_linear(ecs[col]));
            //vdgl_digital_curve_sptr apr=new vdgl_digital_curve(new vdgl_interpolator_linear(ecs[col]));
            //dcs[col]=apr;
            
            
            //dc->split(split_index1/n,temp_dc1,temp_dc2);
            //temp_dc2->split(split_index2/(n-split_index1),dcs[col],dummy);   
                        }
         else if (split_index1>split_index2){ 
            ecs[col]=ec->extract_subchain(split_index2, split_index1);
            //vdgl_digital_curve_sptr apr=new vdgl_digital_curve(new vdgl_interpolator_linear(ecs[col]));
            //dcs[col]=apr;
            dcs[col]=new vdgl_digital_curve(new vdgl_interpolator_linear(ecs[col]));

         }//
         else  { vcl_cerr<<"??\n";return;}
         
         vcl_cout<<ecs[col]->size()<<vcl_endl;
         for (unsigned t=0;t<ecs[col]->size();t++)
            vcl_cout<<(ecs[col])->edgel(t).x()<<vcl_endl;
      }
      // clean selected for next selection job..
      this->clear_selected();
      
      
}

void rc3d_windows_frame::clear_selected()
{
   for (vcl_vector<bgui_vtol2D_tableau_sptr>::iterator bit = vtol_tabs_.begin();
   bit != vtol_tabs_.end(); bit++)
      if (*bit)
         (*bit)->deselect_all();
}

bgui_picker_tableau_sptr rc3d_windows_frame::get_selected_picker_tableau()
{
   unsigned int row=0, col=0;
   grid_->get_last_selected_position(&col, &row);
   return this->get_picker_tableau_at(col, row);
}

bgui_picker_tableau_sptr
rc3d_windows_frame::get_picker_tableau_at(unsigned col, unsigned row)
{
   vgui_tableau_sptr top_tab = grid_->get_tableau_at(col, row);
   if (top_tab)
    {
      bgui_picker_tableau_sptr tt;
      tt.vertical_cast(vgui_find_below_by_type_name(top_tab,
         vcl_string("bgui_picker_tableau")));
      if (tt)
         return tt;
    }
   vgui_macro_warning << "Unable to get bgui_picker_tableau at (" << col << ", "
      << row << ")\n";
   return 0;
}

////////////////////////////////////////////////////////////
void rc3d_windows_frame::find_epipole()
{
   //bgui_picker_tableau_sptr ptab = this->get_selected_picker_tableau();
   float x1=0, y1=0, x2=0, y2=0;
   vgl_homg_point_2d<double> mp_left,mp_right;
   //l_;
   vcl_vector<vgl_homg_line_2d<double> > lines;
   for (int i=0;i<5;i++) {
      vcl_cout<<"# "<<i<<" :";
        vcl_cout<<"Left Point :";
      tabs_picker_[0]->pick_point(&x1, &y1);
      vcl_cout<<x1<<" "<<y1;
      vcl_cout<<"  -Right Point :";
      tabs_picker_[1]->pick_point(&x2, &y2);
      vcl_cout<<x2<<" "<<y2<<vcl_endl;
      
      
      mp_left.set(x1,y1,1.0);
      mp_right.set(x2,y2);
      vgl_homg_line_2d<double> l_(mp_left,mp_right);
      lines.push_back(l_);
   }
   
   vgl_homg_point_2d<double> epipole = vgl_homg_operators_2d<double>::lines_to_point(lines);
   vnl_double_3 e; e[0] = epipole.x(); e[1] = epipole.y(); e[2] = epipole.w();
   epi_=e;
   vcl_cout<<"your epipole is(homo): "<<e<<vcl_endl;
         vcl_cout<<"your epipole is: "<<e[0]/e[2]<<" "<<e[1]/e[2]<<vcl_endl;

   instance_->vtol_tabs_[0]->add_point(e[0]/e[2], e[1]/e[2]);
   epi_flag=true;
   // vnl_double_3 e((*e_)[0],(*e_)[1],1.0);
   
}
////
void rc3d_windows_frame::clear_display()
{


   bgui_vtol2D_tableau_sptr btab = this->get_selected_vtol2D_tableau();
   if (btab)
      btab->clear();
   else
      vcl_cout << "In rc3d_windows_frame::clear_display() - null tableau\n";
}

void rc3d_windows_frame::clear_display3()
{

    for (unsigned  i_view=0;i_view<3;i_view++) {
   
      
      vtol_tabs_[i_view]->clear();
   }
   grid_->post_redraw();
   
   
}
//////////////////////////////////////////////////////
void rc3d_windows_frame::manual_reconstruction_number() 
{
   float xm[3],ym[3];
   
   unsigned int col;
   vnl_double_2 l_c,r_c;
   
   bool flag=false;
   
   if (!init_3_cam_flag_) {
      vcl_cout<< "cam 3 initialization first\n"<<vcl_endl;
      return ;
   }





   static char ch;

   static double x,y,z;
   vcl_string ext="*.*";
   static vcl_string filename;
   vgui_dialog dlg("vgl point 3d");
   dlg.file("file name", ext, filename);
   dlg.field("x",x);
   dlg.field("y",y);
   dlg.field("z",z);

   //   dlg.field("choice 0->12 1->13 2->23",ch);
   if (!dlg.ask()){
     return;
   }

   int point_number;

   vcl_ifstream in(filename.c_str());
   vcl_cout<<"reading point file "<< filename.c_str()<<vcl_endl;

   in >>point_number;
   for (unsigned i1=0;i1<point_number;i1++) {
     in>> x>>y>>z;
     vcl_cout<<x<<" "<<y<<" "<<z<<vcl_endl;
     //vgl_point_3d<double> point_3d(x,z,y);
     vgl_point_3d<double> point_3d(x,y,z);

     // now we have 3d point
     
     vcl_cout<<point_3d<<vcl_endl;

     vgui_style_sptr style = vgui_style::new_style(1, 0, 0, 6, 1);
     for (unsigned i=0;i<3;i++) {
       vgl_point_2d<double> pd = brct_algos::projection_3d_point(point_3d, PL_[i]);

       vtol_vertex_2d_sptr v=new vtol_vertex_2d(pd.x(),pd.y());

       vtol_tabs_[i]->add_vertex(v,style);
     }
     grid_->post_redraw();
   }
   return;
}
//////////////////////////////////////////////////////
vgl_point_3d<double> rc3d_windows_frame::manual_reconstruction() 
{
   float xm[3],ym[3];
   
   unsigned int col;
   vnl_double_2 l_c,r_c;
   
   bool flag=false;
   vgl_point_3d<double> null;
   if (!init_3_cam_flag_) {
      vcl_cout<< "cam 3 initialization first\n"<<vcl_endl;
      return null;
   }
   char ch;
   for (col=0;col<3;col++) {
      vcl_cout<<" click on points"<<col<<vcl_endl;
      tabs_picker_[col]->pick_point(&xm[col], &ym[col]);
      vcl_cout<<xm[col]<<" "<<ym[col]<<vcl_endl;
    }
   vcl_cout<<"choice 12 13 23==> 0 1 2"<<vcl_endl;
   vcl_cin>>ch;
   vcl_cout<<" "<<ch<<vcl_endl;
   
    //initialize_camera_3();
   vnl_double_3x4 P_l,P_r;
   //vnl_double_3x4 P1 = M_in_*E1, P2 = M_in_*E2, P3 = M_in_*E3;
   vnl_double_3x4 P1 = PL_[0], P2 = PL_[1], P3 = PL_[2];
   switch(ch)
   {
    case '0' : l_c[0]=xm[0];l_c[1]=ym[0];r_c[0]=xm[1];r_c[1]=ym[1];P_l=P1;P_r=P2; break;
    case '1' : l_c[0]=xm[0];l_c[1]=ym[0];r_c[0]=xm[2];r_c[1]=ym[2];P_l=P1;P_r=P3; break;
    case '2' : l_c[0]=xm[1];l_c[1]=ym[1];r_c[0]=xm[2];r_c[1]=ym[2];P_l=P2;P_r=P3; break;
      
    default: l_c[0]=xm[0];l_c[1]=ym[0];r_c[0]=xm[1];r_c[1]=ym[1];P_l=P1;P_r=P2;break; // this will never happen
   }
   

   vgl_point_2d<double> x_l(l_c[0],l_c[1]);
   vgl_point_2d<double> x_r(r_c[0],r_c[1]);
   vcl_cout<<x_l<<x_r<<P_l<<P_r<<vcl_endl;
   vgl_point_3d<double> point_3d = brct_algos::triangulate_3d_point(x_l, P_l, x_r, P_r);
   // now we have 3d point
   double x_center_3d=point_3d.x();
   double y_center_3d=point_3d.y();
   double z_center_3d=point_3d.z();
   
   vcl_cout<<point_3d<<vcl_endl;
   d3_one_pt_[0]=point_3d.x();
   d3_one_pt_[1]=point_3d.y();
   d3_one_pt_[2]=point_3d.z();
   
   vgui_style_sptr style = vgui_style::new_style(1, 0, 0, 6, 1);
   for (unsigned i=0;i<3;i++) {
     vgl_point_2d<double> pd = brct_algos::projection_3d_point(point_3d, PL_[i]);
     
     vtol_vertex_2d_sptr v=new vtol_vertex_2d(pd.x(),pd.y());
                                 
     vtol_tabs_[i]->add_vertex(v,style);
   }
   grid_->post_redraw();
   return point_3d;
}

//////////7-10-2004 test recognizer/////////////////
void rc3d_windows_frame::manual_recon_rec_test() 
{
   float xm[3],ym[3];
   
   unsigned int row,col;
   vnl_double_2 l_c,r_c;
   
   bool flag=false;
   vgl_point_3d<double> null;
   if (!init_3_cam_flag_) {
      vcl_cout<< "cam 3 initialization first\n"<<vcl_endl;
      return ;
   }
   static int ch=0;
   static int probe_number=0;
   for (col=0;col<3;col++) {
      vcl_cout<<" click on points"<<col<<vcl_endl;
      tabs_picker_[col]->pick_point(&xm[col], &ym[col]);
      vcl_cout<<xm[col]<<" "<<ym[col]<<vcl_endl;
    }
   //vcl_cout<<"choice 12 13 23==> 0 1 2"<<vcl_endl;
   //vcl_cin>>ch;
   //vcl_cout<<" "<<ch<<vcl_endl;
   vgui_dialog dlg("recognizer test");
   
   dlg.field("choice ? 12 13 23",ch);
   dlg.field("probe number",probe_number);
   if (!dlg.ask()){
      return;
   }
    //initialize_camera_3();
   vnl_double_3x4 P_l,P_r;
   //vnl_double_3x4 P1 = M_in_*E1, P2 = M_in_*E2, P3 = M_in_*E3;
   vnl_double_3x4 P1 = PL_[0], P2 = PL_[1], P3 = PL_[2];
   switch(ch)
   {
    case 0 : l_c[0]=xm[0];l_c[1]=ym[0];r_c[0]=xm[1];r_c[1]=ym[1];P_l=P1;P_r=P2; break;
    case 1 : l_c[0]=xm[0];l_c[1]=ym[0];r_c[0]=xm[2];r_c[1]=ym[2];P_l=P1;P_r=P3; break;
    case 2 : l_c[0]=xm[1];l_c[1]=ym[1];r_c[0]=xm[2];r_c[1]=ym[2];P_l=P2;P_r=P3; break;
      
    default: l_c[0]=xm[0];l_c[1]=ym[0];r_c[0]=xm[1];r_c[1]=ym[1];P_l=P1;P_r=P2;break; // this will never happen
   }
   

   vgl_point_2d<double> x_l(l_c[0],l_c[1]);
   vgl_point_2d<double> x_r(r_c[0],r_c[1]);
   
   vgl_point_3d<double> point_3d = brct_algos::triangulate_3d_point(x_l, P_l, x_r, P_r);
   // now we have 3d point
   double x_center_3d=point_3d.x();
   double y_center_3d=point_3d.y();
   double z_center_3d=point_3d.z();
   
   vcl_cout<<point_3d<<vcl_endl;
   d3_one_pt_[0]=point_3d.x();
   d3_one_pt_[1]=point_3d.y();
   d3_one_pt_[2]=point_3d.z();
   //return point_3d;
   p_list_set p_set;
   vcl_vector <vnl_matrix <float>  >p_list;
   vnl_matrix<float> sel(10,1,0.0);
   //sel(1,0)=x_1;sel(2,0)=y_1;sel(3,0)=x_3;sel(4,0)=y_3;
   //            sel(5,0)=x_2;sel(6,0)=y_2;
   sel(7,0)=point_3d.x();
   sel(8,0)=point_3d.y();
   sel(9,0)=point_3d.z();
   p_list.push_back(sel);
   p_set.probe_number=probe_number;
   p_set.p_list=p_list;

   //7-10-2004 rignt? hmmm
   p_list_set_[probe_number].p_list.clear();
   p_list_set_[probe_number]=(p_set);
   //7-12-2004 right way..
   //p_list_set_.erase(pl_list_set_[probe_number)]);
   
}



// 10-28-2004
/////  manual recon for distribution optimization ////////
void rc3d_windows_frame::manual_recon_tripplet_optimization() 
{



   vnl_double_4x4 RT(BB_RT_matt_);
   vcl_cout<<RT<<vcl_endl;

   vnl_vector_fixed <double,4 > unit_y(0,1,0,1);
   vnl_vector_fixed <double,4> unit_yz(0,1,1,1);
   vnl_vector <double> center_3d(.5*RT*(unit_y+unit_yz));

   vcl_cout<<"center point: "<<center_3d<<vcl_endl;

   ///// trying to use... center of BB... 9-17-2004
   vnl_vector_fixed <double,4 > unit_xy(1,1,0,1);
   vnl_vector_fixed <double,4> unit_xyz(1,1,1,1);
 

   vcl_cout<<"center point: "<<center_3d<<vcl_endl;
    

         //1-13-2005 new world cam BB 
         vnl_vector_fixed <double,4 > x1(1,0,0,1);
         vnl_vector_fixed <double,4 > x2(1,1,0,1);
         vnl_vector <double> center_3d1(.5*RT*(x1+x2));
         center_3d=center_3d1;



   vcl_cout<<"center point: "<<center_3d<<vcl_endl;


   vnl_double_3x3 R(0.0);

   R[0][0]=RT[0][0];R[0][1]=RT[0][1];R[0][2]=RT[0][2];
   R[1][0]=RT[1][0];R[1][1]=RT[1][1];R[1][2]=RT[1][2];
   R[2][0]=RT[2][0];R[2][1]=RT[2][1];R[2][2]=RT[2][2];
   
        
         
          //1-11-2005
          double theta_x=90.0;
    double theta_y=0.0;
    double theta_z=180.0;


                float Rx=0,Ry=0,Rz=0;
         //World CAM FULL AUTO  //1-11-2005
   vnl_double_3x3 R_out(0.0);

   rotate_bb(R, R_out,theta_x*vnl_math::pi/180.0,
     theta_y*vnl_math::pi/180.0,theta_z*vnl_math::pi/180.0);
   vcl_cout<<R_out<<vcl_endl;
   R=R_out;
   float Rx_new=-Rx,Ry_new=-Rz,Rz_new=-Ry;
   Rx=Rx_new;Ry=Ry_new;Rz=Rz_new;
   // World CAM END

   // normalize R along columns..
   
    Rx=vcl_sqrt(R[0][0]*R[0][0]+R[1][0]*R[1][0]+R[2][0]*R[2][0]);
    Ry=vcl_sqrt(R[0][1]*R[0][1]+R[1][1]*R[1][1]+R[2][1]*R[2][1]);
    Rz=vcl_sqrt(R[0][2]*R[0][2]+R[1][2]*R[1][2]+R[2][2]*R[2][2]);

   
   //Scale=Rz/vcl_abs(-28.0527 -18.6547);
   vcl_cout<<"suggested scale"<<Rx/(vcl_abs( 198.7+130.053)/2.0)<<vcl_endl;
  // vcl_cout<<"Scale: "<<Scale<<" "<<Rz<<" "<<Rx<<" "<<Ry<<vcl_endl;

   //model dimension.. 9-17-04
   double Mx=vcl_abs( 198.7+130.053)/2.0;
   double My=vcl_abs(20.6846+37.5776);
   double Mz=vcl_abs(32.2621+24.6169);
   vcl_cout<<"mean(?) model_dimension(Mx,My,Mz) is "<<Mx<<" "<<My<<" "<<Mz<<vcl_endl;
   vcl_cout<<"and  "<<Rx/Mx<<" "<<Ry/My<<" "<<Rz/Mz<<vcl_endl;
    Mx=vcl_abs( 198.7);
    My=vcl_abs(20.6846)*2.0;
    Mz=vcl_abs(32.2621)*2.0;
   vcl_cout<<"model_dimension(Mx,My,Mz) is "<<Mx<<" "<<My<<" "<<Mz<<vcl_endl;
   vcl_cout<<"and  "<<Rx/Mx<<" "<<Ry/My<<" "<<Rz/Mz<<vcl_endl;
   //return;
   // model dimension ends..
   R.normalize_columns();

   vcl_cout<<R<<vcl_endl;
   //R.normalize_rows();
   //vcl_cout<<"after normalization\n"<<R<<vcl_endl;
   //shift_x=RT[0][3];shift_y=RT[1][3];shift_z=RT[2][3];
   float shift_x=center_3d(0);
   float shift_y=center_3d(1);
   float shift_z=center_3d(2);


   
   BB_shift_.set(shift_x,shift_y,shift_z);
   BB_Rot_=R;


   float xm[3],ym[3];
   
   unsigned int row,col;
   vnl_double_2 l_c,r_c;
   p_list_set_.clear();
   bool flag=false;
   vgl_point_3d<double> null;
   if (!init_3_cam_flag_) {
      vcl_cout<< "cam 3 initialization first\n"<<vcl_endl;
      return ;
   }
   static int ch=0;
   //static int probe_number=0;
   static double Scale=1.0; 
   
   vgui_dialog dlg("recognizer test");
   //vgui_dialog dlg_save_selected("probe save..");
   
   static vcl_string save_selected_filename = "c:/live/vorl/initfiles/junk.txt";
   vcl_string save_selected_ext="*.*";
   
   dlg.file("file name", save_selected_ext, save_selected_filename);
   dlg.field("choice ? 12 13 23",ch);
   //dlg.field("Scale ?",Scale);
   //dlg.field("probe number",probe_number);
   if (!dlg.ask()){
     return;
   }
   
   vcl_ofstream out(save_selected_filename.c_str());
   if (save_selected_filename != "")
   {
     
     if(!out.is_open()){
       std::cerr<<"Cannot open the write selected probes file.\n";
       //exit(2);
       return;
     }
     
   }
   
   //initialize_camera_3();
   vnl_double_3x4 P_l,P_r;
   //vnl_double_3x4 P1 = M_in_*E1, P2 = M_in_*E2, P3 = M_in_*E3;
   vnl_double_3x4 P1 = PL_[0], P2 = PL_[1], P3 = PL_[2];
   
   
   for (int probe_number=0;probe_number<10;probe_number++) {
     vcl_cout<<"## Probe number: "<<probe_number<<"\n"<<"----------------------"<<vcl_endl;

     for (col=0;col<3;col++) {
       vcl_cout<<" click on points"<<col<<vcl_endl;
       tabs_picker_[col]->pick_point(&xm[col], &ym[col]);
       vcl_cout<<xm[col]<<" "<<ym[col]<<vcl_endl;
     }
     
     switch(ch)
     {
     case 0 : l_c[0]=xm[0];l_c[1]=ym[0];r_c[0]=xm[1];r_c[1]=ym[1];P_l=P1;P_r=P2; break;
     case 1 : l_c[0]=xm[0];l_c[1]=ym[0];r_c[0]=xm[2];r_c[1]=ym[2];P_l=P1;P_r=P3; break;
     case 2 : l_c[0]=xm[1];l_c[1]=ym[1];r_c[0]=xm[2];r_c[1]=ym[2];P_l=P2;P_r=P3; break;
       
     default: l_c[0]=xm[0];l_c[1]=ym[0];r_c[0]=xm[1];r_c[1]=ym[1];P_l=P1;P_r=P2;break; // this will never happen
     }
     vgl_point_2d<double> x_l(l_c[0],l_c[1]);
     vgl_point_2d<double> x_r(r_c[0],r_c[1]);
     
     vgl_point_3d<double> point_3d = brct_algos::triangulate_3d_point(x_l, P_l, x_r, P_r);
     // now we have 3d point
     double x_center_3d=point_3d.x();
     double y_center_3d=point_3d.y();
     double z_center_3d=point_3d.z();
     
     vcl_cout<<point_3d<<vcl_endl;
     d3_one_pt_[0]=point_3d.x();
     d3_one_pt_[1]=point_3d.y();
     d3_one_pt_[2]=point_3d.z();
     //return point_3d;
     p_list_set p_set;
     vcl_vector <vnl_matrix <float>  >p_list;
     vnl_matrix<float> sel(10,1,0.0);
     //sel(1,0)=x_1;sel(2,0)=y_1;sel(3,0)=x_3;sel(4,0)=y_3;
     //            sel(5,0)=x_2;sel(6,0)=y_2;
     
    sel(7,0)=Scale*point_3d.x();
    sel(8,0)=Scale*point_3d.y();
    sel(9,0)=Scale*point_3d.z();
     
    // sel(7,0)=point_3d.x();
    // sel(8,0)=point_3d.y();
    // sel(9,0)=point_3d.z();
     //p_list.clear();
     p_list.push_back(sel);
     
     p_set.probe_number=probe_number;
     p_set.p_list=p_list;
     vcl_cout<<point_3d<<" "<<sel<<vcl_endl;
     
     //p_list_set_[probe_number].p_list.clear();
     p_list_set_.push_back(p_set);
     //7-12-2004 right way..
     //p_list_set_.erase(pl_list_set_[probe_number)]);
     //probe_number++;
   }


         // WC does not need WC...
   //rotate_back();
   float Box_Add=0.0;

   static double scale2=1;
   bool satisfied=true;
   vgui_dialog sat("scale2?");
     sat.field("scale2?",scale2);
     sat.checkbox("satsfied?",satisfied);
   while (!satisfied) {
   // for scale finding this whilw loop is needed..   
     
     if (p_list_set_[0].p_list.size()>0&&p_list_set_[7].p_list.size()>0)
       vcl_cout<<scale2*d3_point_distance(p_list_set_[0].p_list[0],
       p_list_set_[7].p_list[0])<<vcl_endl;
     if (p_list_set_[5].p_list.size()>0&&p_list_set_[8].p_list.size()>0)
       vcl_cout<<scale2*d3_point_distance(p_list_set_[5].p_list[0],
       p_list_set_[8].p_list[0])<<vcl_endl;
     
     
     if (!sat.ask()){
       return;
     }
     
   }


   out<<p_list_set_.size()<<"\n";
   for (unsigned i=0;i<p_list_set_.size();i++) {
     out<<i<<" "<<p_list_set_[i].p_list.size()<<vcl_endl;
     for( unsigned j=0;j<p_list_set_[i].p_list.size();j++){
       out<<(p_list_set_[i].p_list[j])*scale2 << vcl_endl ;
                         if (j>6) vcl_cout<<(p_list_set_[i].p_list[j])*scale2 << vcl_endl ;
     }
   }
   out<<"Scale1:"<<Scale<< "scale2"<<scale2<<" Box_Add: "<<Box_Add<<"\n "
     <<" cube_inc: "<<cube_inc_<<" d_tube_: "<<d_tube_<<"\n"
     <<" d3_rho_"<<d3_rho_<<" s_thresh_1"<<s_thresh_1<<vcl_endl;
   for (unsigned i=0;i<fore_name.size();i++) {
     out<<fore_name[i]<<vcl_endl;
   }
}



// 5-03-2004 Dongjin Han Map selected 2d point in image onto epi line
vcl_vector <vgl_point_3d<double> > rc3d_windows_frame::manual_reconstruction_on_epiline()

{
  float xm[3],ym[3];
   
   unsigned int row,col;
   vnl_double_2 l_c,r_c;
   
   bool flag=false;
   //vcl_vector <vgl_point_3d<double> >null;
   if (!init_3_cam_flag_) {
      vcl_cout<< "cam 3 initialization first\n"<<vcl_endl;
      //:return ;
   }
   char ch;
   /*for (col=0;col<3;col++) {
      vcl_cout<<" click on points"<<col<<vcl_endl;
      tabs_picker_[col]->pick_point(&xm[col], &ym[col]);
      vcl_cout<<xm[col]<<" "<<ym[col]<<vcl_endl;
    }*/
   mouse_point_selection_3_frame(xm,ym);

   vcl_cout<<"choice 12 13 23==> 0 1 2"<<vcl_endl;
   vcl_cin>>ch;
   vcl_cout<<" "<<ch<<vcl_endl;
   
    //initialize_camera_3();
   vnl_double_3x4 P_l,P_r;
   vnl_double_3x4 P1 = M_in_*E1, P2 = M_in_*E2, P3 = M_in_*E3;
   vcl_vector <vnl_double_3x4> PL;
   PL.push_back(P1);PL.push_back(P2);PL.push_back(P3);
   switch(ch)
   {
    case '0' : l_c[0]=xm[0];l_c[1]=ym[0];r_c[0]=xm[1];r_c[1]=ym[1];P_l=P1;P_r=P2; break;
    case '1' : l_c[0]=xm[0];l_c[1]=ym[0];r_c[0]=xm[2];r_c[1]=ym[2];P_l=P1;P_r=P3; break;
    case '2' : l_c[0]=xm[1];l_c[1]=ym[1];r_c[0]=xm[2];r_c[1]=ym[2];P_l=P2;P_r=P3; break;
      
    default: l_c[0]=xm[0];l_c[1]=ym[0];r_c[0]=xm[1];r_c[1]=ym[1];P_l=P1;P_r=P2;break; // this will never happen
   }
   

    //temporary test for FMatrix
  if (!epi_flag) {
     vcl_cout<<"epipole needed!\n"<<vcl_endl;;
    // return NULL;
  }
  vnl_double_3x3 F;
  F[0][0] = 0;     F[0][1] = -epi_[2];  F[0][2] = epi_[1];
  F[1][0] = epi_[2];  F[1][1] = 0;      F[1][2] = -epi_[0];
  F[2][0] = -epi_[1]; F[2][1] = epi_[0];   F[2][2] = 0;
  
  FMatrix FM(F);
  //FMatrix f = this->test_fmatrix();
  vgl_homg_point_2d<double> pl(l_c[0],l_c[1]);
  vgl_homg_line_2d<double> lr = FM.image2_epipolar_line(pl);
  //end test
  for (unsigned i=0;i<iview_-1;i++) {
     bgui_vtol2D_tableau_sptr v2D = this->get_vtol2D_tableau_at(i,row=0);
     if (v2D)
     {
        v2D->add_infinite_line(lr.a(), lr.b(), lr.c());
        v2D->post_redraw();
     }
  }

  
  //5-3-04  map the selected points on epi lines
  double ap=lr.a()/lr.b(), bp=(lr.c()+lr.b()*r_c[1])/lr.b();
  double xp=(-ap*bp+r_c[0])/(ap*ap+1);
  double yp=(-(lr.a())*xp-lr.c())/lr.b();



  vgl_point_2d<double> x_l(l_c[0],l_c[1]);
  //vgl_point_2d<double> x_r(r_c[0],r_c[1]);
  vgl_point_2d<double> x_r(xp,yp);
  vcl_cout<<xp<<" "<<yp<<vcl_endl;

  vgl_point_3d<double> point_3d_1 = brct_algos::triangulate_3d_point(x_l, P_l, x_r, P_r);
  // now we have 3d point
  //double x_center_3d=point_3d_1.x();
  //double y_center_3d=point_3d_1.y();
  //double z_center_3d=point_3d_1.z();
  
  vcl_cout<<point_3d_1<<vcl_endl;
  //d3_one_pt_[0]=point_3d_1.x();
  //d3_one_pt_[1]=point_3d_1.y();
  //d3_one_pt_[2]=point_3d_1.z();
  
  /// another bottom
  vcl_cout<<"other bootom point\n"<<vcl_endl;
  
  mouse_point_selection_3_frame(xm,ym);

  vcl_cout<<"thanks!\n"<<vcl_endl;

  
  switch(ch)
   {
    case '0' : l_c[0]=xm[0];l_c[1]=ym[0];r_c[0]=xm[1];r_c[1]=ym[1];P_l=P1;P_r=P2; break;
    case '1' : l_c[0]=xm[0];l_c[1]=ym[0];r_c[0]=xm[2];r_c[1]=ym[2];P_l=P1;P_r=P3; break;
    case '2' : l_c[0]=xm[1];l_c[1]=ym[1];r_c[0]=xm[2];r_c[1]=ym[2];P_l=P2;P_r=P3; break;
      
    default: l_c[0]=xm[0];l_c[1]=ym[0];r_c[0]=xm[1];r_c[1]=ym[1];P_l=P1;P_r=P2;break; // this will never happen
   }
  double ap1=lr.a()/lr.b(); bp=(lr.c()+lr.b()*l_c[1])/lr.b();
  double xp1=(-ap1*bp+l_c[0])/(ap1*ap1+1);
  double yp1=(-(lr.a())*xp1-lr.c())/lr.b();
   x_l.set(xp1,yp1);
  //vgl_point_2d<double> x_l(l_c[0],l_c[1]);
  //vgl_point_2d<double> x_r(r_c[0],r_c[1]);

  double ap2=lr.a()/lr.b(); bp=(lr.c()+lr.b()*r_c[1])/lr.b();
  double xp2=(-ap2*bp+r_c[0])/(ap2*ap2+1);
  double yp2=(-(lr.a())*xp2-lr.c())/lr.b();


   x_r.set(xp2,yp2);
 

  vgl_point_3d<double> point_3d_2 = brct_algos::triangulate_3d_point(x_l, P_l, x_r, P_r);
  
  
  vcl_cout<<point_3d_2<<vcl_endl;
  
  
  
  vcl_vector <vgl_point_3d <double> > t;
  t.push_back(point_3d_1); t.push_back(point_3d_2);
  BB_bottom_.erase(BB_bottom_.begin(), BB_bottom_.end());
  vcl_cout<< BB_bottom_.size()<<vcl_endl;
  BB_bottom_.push_back(point_3d_1);
  BB_bottom_.push_back(point_3d_2);
  vcl_cout<< BB_bottom_.size()<<vcl_endl;
  draw_point(PL,point_3d_1);
  draw_point(PL,point_3d_2);
  
  
  
  
  // 6-11-2004 front line selection ....TT was a big BUG!
  mouse_point_selection_3_frame(xm,ym);
  switch(ch)
  {
  case '0' : l_c[0]=xm[0];l_c[1]=ym[0];r_c[0]=xm[1];r_c[1]=ym[1];P_l=P1;P_r=P2; break;
  case '1' : l_c[0]=xm[0];l_c[1]=ym[0];r_c[0]=xm[2];r_c[1]=ym[2];P_l=P1;P_r=P3; break;
  case '2' : l_c[0]=xm[1];l_c[1]=ym[1];r_c[0]=xm[2];r_c[1]=ym[2];P_l=P2;P_r=P3; break;
     
  default: l_c[0]=xm[0];l_c[1]=ym[0];r_c[0]=xm[1];r_c[1]=ym[1];P_l=P1;P_r=P2;break; // this will never happen
  }

  vgl_homg_point_2d<double> pfl(l_c[0],l_c[1]);
  vgl_homg_line_2d<double> lfr = FM.image2_epipolar_line(pfl);
  
  for (unsigned i=0;i<iview_-1;i++) {
     bgui_vtol2D_tableau_sptr v2D = this->get_vtol2D_tableau_at(i,row=0);
     if (v2D)
     {
        v2D->add_infinite_line(lfr.a(), lfr.b(), lfr.c());
        v2D->post_redraw();
     }
  }

  ap1=lfr.a()/lfr.b(); bp=(lfr.c()+lfr.b()*l_c[1])/lfr.b();
  xp1=(-ap1*bp+l_c[0])/(ap1*ap1+1);
  yp1=(-(lfr.a())*xp1-lfr.c())/lfr.b();
  x_l.set(xp1,yp1);
  //vgl_point_2d<double> x_l(l_c[0],l_c[1]);
  //vgl_point_2d<double> x_r(r_c[0],r_c[1]);

  ap2=lfr.a()/lfr.b(); bp=(lfr.c()+lfr.b()*r_c[1])/lfr.b();
  xp2=(-ap2*bp+r_c[0])/(ap2*ap2+1);
  yp2=(-(lfr.a())*xp2-lfr.c())/lfr.b();


  x_r.set(xp2,yp2);
 

  vgl_point_3d<double> point_3d_front_1 = brct_algos::triangulate_3d_point(x_l, P_l, x_r, P_r);
  
  
  vcl_cout<<point_3d_front_1<<vcl_endl;

  // farther point


  mouse_point_selection_3_frame(xm,ym);
  switch(ch)
  {
  case '0' : l_c[0]=xm[0];l_c[1]=ym[0];r_c[0]=xm[1];r_c[1]=ym[1];P_l=P1;P_r=P2; break;
  case '1' : l_c[0]=xm[0];l_c[1]=ym[0];r_c[0]=xm[2];r_c[1]=ym[2];P_l=P1;P_r=P3; break;
  case '2' : l_c[0]=xm[1];l_c[1]=ym[1];r_c[0]=xm[2];r_c[1]=ym[2];P_l=P2;P_r=P3; break;
     
  default: l_c[0]=xm[0];l_c[1]=ym[0];r_c[0]=xm[1];r_c[1]=ym[1];P_l=P1;P_r=P2;break; // this will never happen
  }
  
  //vgl_homg_point_2d<double> pfl(l_c[0],l_c[1]);
  pfl.set(l_c[0],l_c[1]);
  //vgl_homg_line_2d<double> lfr = FM.image2_epipolar_line(pfl);
  lfr = FM.image2_epipolar_line(pfl);
  
  for (unsigned i=0;i<iview_-1;i++) {
    bgui_vtol2D_tableau_sptr v2D = this->get_vtol2D_tableau_at(i,row=0);
     if (v2D)
     {
          v2D->add_infinite_line(lfr.a(), lfr.b(), lfr.c());
          v2D->post_redraw();
       }
   }

  ap1=lfr.a()/lfr.b(); bp=(lfr.c()+lfr.b()*l_c[1])/lfr.b();
  xp1=(-ap1*bp+l_c[0])/(ap1*ap1+1);
  yp1=(-(lfr.a())*xp1-lfr.c())/lfr.b();
  x_l.set(xp1,yp1);
  //vgl_point_2d<double> x_l(l_c[0],l_c[1]);
  //vgl_point_2d<double> x_r(r_c[0],r_c[1]);

  ap2=lfr.a()/lfr.b(); bp=(lfr.c()+lfr.b()*r_c[1])/lfr.b();
  xp2=(-ap2*bp+r_c[0])/(ap2*ap2+1);
  yp2=(-(lfr.a())*xp2-lfr.c())/lfr.b();


  x_r.set(xp2,yp2);
 

  vgl_point_3d<double> point_3d_front_2 = brct_algos::triangulate_3d_point(x_l, P_l, x_r, P_r);
  
  
  vcl_cout<<point_3d_front_2<<vcl_endl;

  BB_bottom_.push_back(point_3d_front_1);
  BB_bottom_.push_back(point_3d_front_2);
  vcl_cout<< BB_bottom_.size()<<vcl_endl;
  draw_point(PL,point_3d_front_1);
  draw_point(PL,point_3d_front_2);

  return t;

}

//// temporary debugging  6-16-2004
void  rc3d_windows_frame::manual_reconstruction_on_epiline_debug_temp() {
 
   
   
//   vgl_point_3d<double> p0(58.3901,8.02418,-252.943);

 //vgl_point_3d<double> p1(46.5805,5.92898,-228.123);

 //vgl_point_3d<double> p2(36.5756,8.5468,-219.638);

 //vgl_point_3d<double> p3(27.7517,10.1052,-225.331);

 vgl_point_3d <double> p0(58.3312,7.91869,-253.05);

vgl_point_3d <double> p1(46.556,5.8195,-228.845);

vgl_point_3d <double> p2(32.5315,5.71011,-221.155);
 
vgl_point_3d <double> p3(29.4821,6.31324,-222.929);


 BB_bottom_.push_back(p0);
 BB_bottom_.push_back(p1);
 BB_bottom_.push_back(p2);
 BB_bottom_.push_back(p3);

 BB_front_.set(23.0191,9.60317,-230.37);
//vgl_point_3d 23.2079,9.19275,-226.051>
 BB_front_.set(23.2079,9.19275,-226.051);

}
////





//5-7 -04 front corner reconstruction
vgl_point_3d<double>  rc3d_windows_frame::manual_reconstruction_on_epiline_BB_Front()
 
{
   float xm[3],ym[3];
   
   unsigned int row,col;
   vnl_double_2 l_c,r_c;
   
   bool flag=false;
   vgl_point_3d<double> X(-999,-999,-999);
                
   if (!init_3_cam_flag_) {
      vcl_cout<< "cam 3 initialization first\n"<<vcl_endl;
        return X;
   }
   char ch;
   //for (col=0;col<3;col++) {
   //   vcl_cout<<" click on points"<<col<<vcl_endl;
   //   tabs_picker_[col]->pick_point(&xm[col], &ym[col]);
   //   vcl_cout<<xm[col]<<" "<<ym[col]<<vcl_endl;
   // }
   
         /*mouse_point_selection_3_frame(xm,ym);

   vcl_cout<<"choice 12 13 23==> 0 1 2"<<vcl_endl;
   vcl_cin>>ch;
   vcl_cout<<" "<<ch<<vcl_endl;
   
    //initialize_camera_3();
   vnl_double_3x4 P_l,P_r;
   vnl_double_3x4 P1 = M_in_*E1, P2 = M_in_*E2, P3 = M_in_*E3;
   vcl_vector <vnl_double_3x4> PL;
   PL.push_back(P1);PL.push_back(P2);PL.push_back(P3);
   switch(ch)
   {
    case '0' : l_c[0]=xm[0];l_c[1]=ym[0];r_c[0]=xm[1];r_c[1]=ym[1];P_l=P1;P_r=P2; break;
    case '1' : l_c[0]=xm[0];l_c[1]=ym[0];r_c[0]=xm[2];r_c[1]=ym[2];P_l=P1;P_r=P3; break;
    case '2' : l_c[0]=xm[1];l_c[1]=ym[1];r_c[0]=xm[2];r_c[1]=ym[2];P_l=P2;P_r=P3; break;
      
    default: l_c[0]=xm[0];l_c[1]=ym[0];r_c[0]=xm[1];r_c[1]=ym[1];P_l=P1;P_r=P2;break; // this will never happen
   }
   

    //temporary test for FMatrix
  if (!epi_flag) {
     vcl_cout<<"epipole needed!\n"<<vcl_endl;;
     return NULL;
  }*/


         static double im0=0,im2=0;
         vgui_dialog dlg("postion col?");
         dlg.field("imgae 0", im0);
         dlg.field("image 2", im2);


           if (!dlg.ask())
      return X;



         l_c[0]=im0;r_c[0]=im2;
   l_c[1]=600.0; //any point iis fine...

        vnl_double_3x4 P02_diff=PL_[2]-PL_[0];
  epi_[0]=P02_diff[0][3]/P02_diff[2][3];
        epi_[1]=P02_diff[1][3]/P02_diff[2][3];
        epi_[2]=1.0;
        vcl_cout<<"epipole:"<<epi_<<vcl_endl;
        
        vnl_double_3x3 F;
  F[0][0] = 0;     F[0][1] = -epi_[2];  F[0][2] = epi_[1];
  F[1][0] = epi_[2];  F[1][1] = 0;      F[1][2] = -epi_[0];
  F[2][0] = -epi_[1]; F[2][1] = epi_[0];   F[2][2] = 0;
  
  FMatrix FM(F);
  //FMatrix f = this->test_fmatrix();
  vgl_homg_point_2d<double> pl(l_c[0],l_c[1]);
  vgl_homg_line_2d<double> lr = FM.image2_epipolar_line(pl);
  //end test
  for (unsigned i=0;i<iview_-1;i++) {
     bgui_vtol2D_tableau_sptr v2D = this->get_vtol2D_tableau_at(i,row=0);
     if (v2D)
     {
        v2D->add_infinite_line(lr.a(), lr.b(), lr.c());
        v2D->post_redraw();
     }
  }

  
  //5-3-04  map the selected points on epi lines
  double ap=lr.a()/lr.b(), bp=(lr.c()+lr.b()*r_c[1])/lr.b();
  double xp=(-ap*bp+r_c[0])/(ap*ap+1);
  double yp=(-(lr.a())*xp-lr.c())/lr.b();


        //3-10-2005
        xp=r_c[0];
  yp=(-(lr.a())*xp-lr.c())/lr.b();

  vgl_point_2d<double> x_l(l_c[0],l_c[1]);
  //vgl_point_2d<double> x_r(r_c[0],r_c[1]);
  vgl_point_2d<double> x_r(xp,yp);
  vcl_cout<<xp<<" "<<yp<<vcl_endl;

  //vgl_point_3d<double> point_3d_1 = brct_algos::triangulate_3d_point(x_l, P_l, x_r, P_r);
        vgl_point_3d<double> point_3d_1 = brct_algos::triangulate_3d_point(x_l, PL_[0], x_r, PL_[2]);
  // now we have 3d point
  //double x_center_3d=point_3d_1.x();
  //double y_center_3d=point_3d_1.y();
  //double z_center_3d=point_3d_1.z();
  
  vcl_cout<<point_3d_1<<vcl_endl;
    
  draw_point(PL_,point_3d_1);
  
  BB_front_.set(point_3d_1.x(),point_3d_1.y(),point_3d_1.z());
  return point_3d_1;


}

// 5-4-04 mouse point selection
void rc3d_windows_frame::mouse_point_selection_3_frame(float *xm,float *ym)
{
   for (unsigned col=0;col<3;col++) {
      vcl_cout<<" click on points"<<col<<vcl_endl;
      tabs_picker_[col]->pick_point(&xm[col], &ym[col]);
      vcl_cout<<xm[col]<<" "<<ym[col]<<vcl_endl;
    }

}

void rc3d_windows_frame::mouse_point_selection_N_frame()
{
        
        float xm,ym;
        int N=Cam_List_.size();
        vcl_string filename;
        vcl_string ext="*.*";
        static int point_number=10;
        static bool manual_selection=true;
  vgui_dialog dlg("camera back projection test");
        dlg.file("file name", ext, filename);
        
        dlg.field("N of 3d points",point_number);
        
        dlg.checkbox("manual selection?",manual_selection);
        if (!dlg.ask()){
                return;
        }
        
        vcl_vector<vnl_double_2> pts;

         if (manual_selection) {
                 for (unsigned col=0;col<N;col++) {
                         vcl_cout<<" click on points"<<col<<vcl_endl;
                         tabs_picker_[col]->pick_point(&xm ,&ym);
                         vcl_cout<<xm<<" "<<ym<<vcl_endl;
                         vnl_double_2 pt(xm,ym);
                         pts.push_back(pt);
                         
                 }

                 vgl_point_3d<double> X=brct_algos::bundle_reconstruct_3d_point(pts, Cam_List_);
                 vgui_style_sptr style = vgui_style::new_style(1, 1, 0, 6, 1);
                 vcl_cout<<X<<vcl_endl;
                 for (unsigned col=0;col<N;col++) {
                         vgl_point_2d<double> pd = brct_algos::projection_3d_point(X, Cam_List_[col]);
                         vtol_vertex_2d_sptr v=new vtol_vertex_2d(pd.x(),pd.y());
                         vtol_tabs_[col]->add_vertex(v,style);
                 }
         }
         else {
                         vcl_ifstream in(filename.c_str());
                         vcl_cout<<"reading point file "<< filename.c_str()<<vcl_endl;
                         
                         //in>> total__number;
                         vgui_style_sptr style = vgui_style::new_style(1, 1, 0, 6, 1);
                         float x,y,z;
                         vcl_string dummy;
                         for (unsigned i=0;i<point_number;i++) {
                                 in>>dummy>> x>>y>>z;
                                 vcl_cout<<x<<" "<<y<<" "<<z<<vcl_endl;
                                 vgl_point_3d<double> point(x,z,y);
                                 
                                 for (unsigned col=0;col<N;col++) {
                                         
                                         vgl_point_2d<double> pd = brct_algos::projection_3d_point(point, Cam_List_[col]);
                                         
                                         vtol_vertex_2d_sptr v=new vtol_vertex_2d(pd.x(),pd.y());
                                         
                                         vtol_tabs_[col]->add_vertex(v,style);
                                 }
                                 
                         }
                         
                         
                         
                 
         }
   grid_->post_redraw();
                 
}

void rc3d_windows_frame::cad_back_N_frame()
{
            static double theta_x=90.0;
   static double theta_y=0.0;
   static double theta_z=180.0;
   static float shift_x_arg=0.0;
         static float shift_y_arg=0.0;
         static float shift_z_arg=0.0;


        float xm,ym;
        int N=Cam_List_.size();
        static vcl_string filename;
        vcl_string ext="*.*";
        static int point_number=10;
        static bool manual_selection=false;
  
        
        vgui_dialog dlg("camera back projection test");

        dlg.field("shift x",shift_x_arg);
   dlg.field("shift y",shift_y_arg);
   dlg.field("shift z",shift_z_arg);

   dlg.field("theta_x",theta_x);
   dlg.field("theta_y",theta_y);
   dlg.field("theta_z",theta_z);




        dlg.file("file name", ext, filename);
        
        dlg.field("N of 3d points",point_number);
        
//      dlg.checkbox("manual selection?",manual_selection);
        if (!dlg.ask()){
                return;
        }
        
        double shift_x=shift_x_arg;
   double shift_y=shift_y_arg;
   double shift_z=shift_z_arg;



   //6-16-2004 added x direction shift is .....
   
   // 6-30-2004 given stuff(BB and Camera)

   vnl_double_4x4 RT(BB_RT_matt_);
   vcl_cout<<RT<<vcl_endl;

   vnl_vector_fixed <double,4 > unit_y(0,1,0,1);
   vnl_vector_fixed <double,4> unit_yz(0,1,1,1);
   vnl_vector <double> center_3d(.5*RT*(unit_y+unit_yz));

         //1-13-2005 new world cam BB 
         vnl_vector_fixed <double,4 > x1(1,0,0,1);
         vnl_vector_fixed <double,4 > x2(1,1,0,1);
         vnl_vector <double> center_3d1(.5*RT*(x1+x2));
         center_3d=center_3d1;



   vcl_cout<<"center point: "<<center_3d<<vcl_endl;

   ///// trying to use... center of BB... 9-17-2004
   vnl_vector_fixed <double,4 > unit_xy(1,1,0,1);
   vnl_vector_fixed <double,4> unit_xyz(1,1,1,1);
  // vnl_vector <double> center_3d_back(.5*RT*(unit_xy+unit_xyz));

  // vcl_cout<<"center point_rear: "<<center_3d_back<<vcl_endl;
  // center_3d=.5*(center_3d+center_3d_back);
   //////////


   vcl_cout<<"center point: "<<center_3d<<vcl_endl;
   //vcl_cout<<center_3d/center_3d(3)<<vcl_endl;

   
   vnl_double_3x3 R(0.0);

   R[0][0]=RT[0][0];R[0][1]=RT[0][1];R[0][2]=RT[0][2];
   R[1][0]=RT[1][0];R[1][1]=RT[1][1];R[1][2]=RT[1][2];
   R[2][0]=RT[2][0];R[2][1]=RT[2][1];R[2][2]=RT[2][2];
    
         
         
         
         float Rx=0,Ry=0,Rz=0;
         
         //World CAM FULL AUTO  //1-11-2005
   vnl_double_3x3 R_out(0.0);

   rotate_bb(R, R_out,theta_x*vnl_math::pi/180.0,
     theta_y*vnl_math::pi/180.0,theta_z*vnl_math::pi/180.0);
   vcl_cout<<R_out<<vcl_endl;
   R=R_out;
   float Rx_new=-Rx,Ry_new=-Rz,Rz_new=-Ry;
   Rx=Rx_new;Ry=Ry_new;Rz=Rz_new;
   // World CAM END
   
         
         
         
         // normalize R along columns..
   
    Rx=vcl_sqrt(R[0][0]*R[0][0]+R[1][0]*R[1][0]+R[2][0]*R[2][0]);
    Ry=vcl_sqrt(R[0][1]*R[0][1]+R[1][1]*R[1][1]+R[2][1]*R[2][1]);
    Rz=vcl_sqrt(R[0][2]*R[0][2]+R[1][2]*R[1][2]+R[2][2]*R[2][2]);

   
   //model dimension.. 9-17-04
   double Mx=vcl_abs( 198.7+130.053)/2.0;
   double My=vcl_abs(20.6846+37.5776);
   double Mz=vcl_abs(32.2621+24.6169);
   vcl_cout<<"mean(?) model_dimension(Mx,My,Mz) is "<<Mx<<" "<<My<<" "<<Mz<<vcl_endl;
   vcl_cout<<"and  "<<Rx/Mx<<" "<<Ry/My<<" "<<Rz/Mz<<vcl_endl;
    Mx=vcl_abs( 198.7);
    My=vcl_abs(20.6846)*2.0;
    Mz=vcl_abs(32.2621)*2.0;
  
   R.normalize_columns();

   vcl_cout<<R<<vcl_endl;
         shift_x=center_3d(0)+shift_x_arg/12.0;shift_y=center_3d(1)+shift_y_arg/12.0;shift_z=center_3d(2)+shift_z_arg/12.0;


         



   BB_shift_.set(shift_x,shift_y,shift_z);
   BB_Rot_=R;
//   BB_Scale_=Scale;



        vcl_ifstream in(filename.c_str());
        vcl_cout<<"reading point file "<< filename.c_str()<<vcl_endl;
        
        //in>> total__number;
        vgui_style_sptr style = vgui_style::new_style(1, 1, 0, 2, 1);
        float x,y,z;
        vcl_string dummy;
        float x_max=-100000.0;
        float x_min=100000.0;
        float y_max=-100000.0;
        float y_min=100000.0;
        float z_max=-100000.0;
        float z_min=100000.0;
        vcl_vector <vgl_point_3d<double> > points;
        for (unsigned i=0;i<point_number;i++) {
                in>>dummy>> x>>y>>z;
                vcl_cout<<x<<" "<<y<<" "<<z<<vcl_endl;
                if (x_max<x) x_max=x;
                if (x_min>x) x_min=x;
                if (y_max<y) y_max=y;
                if (y_min>y) y_min=y;
                if (z_max<z) z_max=z;
                if (z_min>z) z_min=z;
                vgl_point_3d<double> point(x,y,z);
                
                points.push_back(point);
                
        }
        for (unsigned j=0;j<points.size(); j++)
        { 
                points[j].set(points[j].x()-x_max,      points[j].y(),points[j].z());
        }
                         
        
        
        for (unsigned i=0;i<point_number;i++) {
                for (unsigned col=0;col<N;col++) {
                        vgl_point_3d<double> X(-points[i].x()/12.0,-points[i].z()/12.0,-points[i].y()/12.0);
 vnl_matrix <double> t(3,1,0.0);
                         t[0][0]=-points[i].x()/12.0;
            t[1][0]=-points[i].z()/12.0;
            t[2][0]=-points[i].y()/12.0;
            vnl_matrix<double> tt4=(R*t);
            //vcl_cout<<t<<vcl_endl;
            X.set(tt4[0][0]+shift_x,tt4[1][0]+shift_y,tt4[2][0]+shift_z);
                        vgl_point_2d<double> pd = brct_algos::projection_3d_point(X, Cam_List_[col]);
                        
                        vtol_vertex_2d_sptr v=new vtol_vertex_2d(pd.x(),pd.y());
                        
                        vtol_tabs_[col]->add_vertex(v,style);
                }
        }
        grid_->post_redraw();
                 
}

void rc3d_windows_frame::draw_point(vcl_vector <vnl_double_3x4> PL,vgl_point_3d<double> X)
{

   double vpi=vnl_math::pi;
    
   vnl_double_3x4 P;
   
   vgui_style_sptr style = vgui_style::new_style(.8, .4, .2, 8, 1);
   //,.8,.4,.2,8
    //btab->add_vsol_point_2d(p, style); 

   for (unsigned  i_view=0;i_view<3;i_view++) {
   
      vgl_point_2d<double> p = brct_algos::projection_3d_point(X, PL[i_view]);
      vtol_vertex_2d_sptr v1=new vtol_vertex_2d(p.x(),p.y());
      
      vtol_tabs_[i_view]->add_vertex(v1,style);
   }
   grid_->post_redraw();
   
   
}
void rc3d_windows_frame::draw_point(vcl_vector <vnl_double_3x4> PL,vgl_point_3d<double> X,
                           float r, float g, float b, float size)
{

   vgui_style_sptr style = vgui_style::new_style(r, g, b, size, 1);
   double vpi=vnl_math::pi;
    
   vnl_double_3x4 P;
   
   for (unsigned  i_view=0;i_view<3;i_view++) {
   
      vgl_point_2d<double> p = brct_algos::projection_3d_point(X, PL_[i_view]);
      vtol_vertex_2d_sptr v1=new vtol_vertex_2d(p.x(),p.y());
      vtol_tabs_[i_view]->add_vertex(v1,style);

                        if (batch_images_save_) {
                                         
                                         add_vertex(i_view,p.x(),p.y(),7,1);
                                         //add_vertex(i_view,selected_x_2,selected_y_2,7,2);
                                         //add_vertex(i_view,selected_x_3,selected_y_3,7,2);
                                 }
   }
   grid_->post_redraw();

   
   
}

void rc3d_windows_frame::draw_point()
{

         float r=0,g=0,b=1;
         float size=7;

         vgui_style_sptr style;
   vgui_style_sptr style_best = vgui_style::new_style(r, g, b, size, 1);
         vgui_style_sptr style_2nd = vgui_style::new_style(1, 0, 0, size, 1);
   double vpi=vnl_math::pi;
    
   vcl_cout<<p_list_set_.size()<<vcl_endl;
         
   for (unsigned i=0;i<10;i++) {
                 for (unsigned j=0;j<p_list_set_[i].p_list.size();j++) {
                         
                           if (j==p_list_set_[i].p_list.size()-1) style=style_best;
                                 else style=style_2nd;

                                 vtol_vertex_2d_sptr v1=new vtol_vertex_2d(p_list_set_[i].p_list[j][1][0],
                                         p_list_set_[i].p_list[j][2][0]);
                                 vtol_tabs_[0]->add_vertex(v1,style);
                                 vtol_vertex_2d_sptr v2=new vtol_vertex_2d(p_list_set_[i].p_list[j][3][0],
                                         p_list_set_[i].p_list[j][4][0]);
                                 vtol_tabs_[2]->add_vertex(v2,style);
                                 vtol_vertex_2d_sptr v3=new vtol_vertex_2d(p_list_set_[i].p_list[j][5][0],
                                         p_list_set_[i].p_list[j][6][0]);
                                 vtol_tabs_[1]->add_vertex(v3,style);
                         //}
                         //vcl_cout<<j<<": "<<vcl_endl;
                         //vcl_cout<<p_list_set_[i].p_list[j][1][0]<<vcl_endl;
                 }
         }
   grid_->post_redraw();
   
   
}
////////////////////////////////////////////////////
void rc3d_windows_frame::reconstruct3d()
{
   
   initialize_camera();
   
   
   
   if (!(ecs[0]&&ecs[1])) return;
   vnl_double_3x4 P1 = M_in_*E1, P2 = M_in_*E2;
   int size0 = ecs[0]->size();
   int size1 = ecs[1]->size();

   vcl_cout<<"0 size:"<<size0<<" 1 size: "<<size1<<vcl_endl;
   int npts = 2* ((size0 < size1) ? size0 : size1); // interpolate 2 times more
   if (size0*size1==0) return;
   vcl_vector<vgl_point_3d<double> > pts_3d;
   vcl_vector<vgl_point_2d<double> > c0; // matched point for the first view
   vcl_vector<vgl_point_2d<double> > c1;
   
   // construct fundamental matrix between the first and second views.
   vnl_double_3x3 F;
   F[0][0] = 0;     F[0][1] = -epi_[2];  F[0][2] = epi_[1];
   F[1][0] = epi_[2];  F[1][1] = 0;      F[1][2] = -epi_[0];
   F[2][0] = -epi_[1]; F[2][1] = epi_[0];   F[2][2] = 0;
   
   FMatrix FM(F);
   
   for (int i=0; i<npts; i++)
   {
      double index = i/double(npts);
      //vgl_homg_point_2d<double> p1((*(ecs[0]))[index].get_x(),(*(ecs[0]))[index].get_y());
      //vgl_point_2d<double> x1(p1);
      //int x0_index = bdgl_curve_algs:: closest_point(ecs[0], x1.x(), x1.y());
      //double angle0 = (*(ecs[0]))[x0_index].get_theta();
      
      vgl_homg_point_2d<double> p1(dcs[0]->get_x(index),dcs[0]->get_y(index));
      vgl_point_2d<double> x1(p1);
      int x0_index = bdgl_curve_algs:: closest_point(ecs[0], x1.x(), x1.y());
      double angle0 = (*ecs[0])[x0_index].get_theta();
      vgl_line_2d<double> lr(FM.image2_epipolar_line(p1));
      
      // get rid of any point whose gradient is perpendicule to the epipole line
      double nx = lr.a(), ny = lr.b();
      nx = nx / vcl_sqrt(nx*nx + ny*ny);
      ny = ny / vcl_sqrt(nx*nx + ny*ny);
      if (vcl_fabs( nx*vcl_cos(angle0*vnl_math::pi/180) + ny*vcl_sin(angle0*vnl_math::pi/180) )< 0.95)
      {
         // getting the intersection point
         vgl_point_2d<double> p2;
         vcl_vector<vgl_point_2d<double> > pts;
         bdgl_curve_algs::intersect_line(dcs[1], lr, pts);
         
         // find the correspoinding point
         double dist = 1e10; // big number
         bool flag = false;
         for (unsigned int j=0; j<pts.size(); j++)
         {
            vgl_homg_point_2d<double> temp(pts[j].x(), pts[j].y());
            
            int x1_index = bdgl_curve_algs:: closest_point(ecs[1], pts[j].x(), pts[j].y());
            double angle1 = (*(ecs[1]))[x1_index].get_theta();
            
            double dist_p1p2 = vgl_homg_operators_2d<double>::distance_squared(p1, temp);
            if (vcl_fabs(angle1-angle0)<90 && dist > dist_p1p2 &&
               vcl_fabs(nx*vcl_cos(angle1*vnl_math::pi/180) + ny*vcl_sin(angle1*vnl_math::pi/180))<0.95 )
            { // make sure it filters out lines parallel to epipole lines.
               p2 = temp;
               flag = true;
               dist = dist_p1p2;
            }
            else
               continue;
         }
         
         if (flag) { // if have corresponding
            vgl_point_2d<double> x2(p2);
            vgl_point_3d<double> point_3d = brct_algos::triangulate_3d_point(x1, P1, x2, P2);
            pts_3d.push_back(point_3d);
            vcl_cout<<point_3d<<vcl_endl;
            c0.push_back(x1);
            c1.push_back(x2);
         }
      }
   }
   
   
   int  num_points_ = pts_3d.size();
   vcl_cout<<"num_points_: "<<num_points_<<vcl_endl;
   // save the motion
   //  motions_[0] = P1;
   // motions_[1] = P2;
   
   // get observes
   vnl_matrix<double> t0(2, num_points_);
   vnl_matrix<double> t1(2, num_points_);
   for(int i=0; i<num_points_; i++){
      t0[0][i] = c0[i].x();
      t0[1][i] = c0[i].y(); 
      t1[0][i] = c1[i].x();
      t1[1][i] = c1[i].y(); 
   }
   
   //  observes_[0] = t0;
   // observes_[1] = t1;
   
   //get local coordinates
   if (!centered_flag_) {
      xc_=0, yc_=0, zc_=0;
      for (int i=0; i<num_points_; i++) {
         xc_ += pts_3d[i].x();
         yc_ += pts_3d[i].y();
         zc_ += pts_3d[i].z();
      }
      
      xc_ /= num_points_;
      yc_ /= num_points_;
      zc_ /= num_points_;
      centered_flag_=true;
      
   }
   double temp_x,temp_y,temp_z;

      for (int i=0; i<num_points_; i++) {
         temp_x=pts_3d[i].x()-   xc_;
         temp_y=pts_3d[i].y()-   yc_;
         temp_z=pts_3d[i].z()-   zc_;
         pts_3d[i].set(temp_x,temp_y,temp_z);
         vcl_cout<<temp_x<<" "<<temp_y<<" "<<temp_z<<vcl_endl;
      }
   
   //  Xl_.resize(num_points_);
   // prob_.resize(num_points_);
   
   /*for (int i=0; i<num_points_; i++) {
    Xl_[i][0] = pts_3d[i].x() - xc;
    Xl_[i][1] = pts_3d[i].y() - yc;
    Xl_[i][2] = pts_3d[i].z() - zc;
    prob_[i] = 1.0/num_points_;
  }*/
   
   //for (int i=0; i<num_points_; i++) {
   //   Xl_[i][0] = pts_3d[i].x() ;
   //   Xl_[i][1] = pts_3d[i].y() ;
   //   Xl_[i][2] = pts_3d[i].z() ;
   //   prob_[i] = 1.0/num_points_;
   // }
   
   
   // X_[0] = xc;
   // X_[1] = yc;
   // X_[2] = zc;
   
   
   
   //add 3D resoult
   //vcl_vector<vgl_point_3d<double> > c3d = this->get_local_pts();
   add_curve3d(pts_3d);
   
   // show_epipole();
   this->post_redraw();
  }
  
  /*void brct_windows_frame::show_epipole()
  {
  instance_->easy_2d_->set_foreground(1, 0, 0);
  vgl_point_2d<double> e = kalman_->get_cur_epipole();
  vcl_cout<<"\n epipole ("<<e.x() <<'\t'<<e.y()<<")\n";
  instance_->easy_2d_->add_point(e.x(), e.y());
}*/

void rc3d_windows_frame::initialize_camera()
{
   //vcl_cout<<"camera initializiing...."<<vcl_endl;   
   M_in_[0][0] = 2000.0; M_in_[0][1] = 0;        M_in_[0][2] = 512.0;
   M_in_[1][0] = 0;        M_in_[1][1] = 2000.0; M_in_[1][2] = 384.0;
   M_in_[2][0] = 0;        M_in_[2][1] = 0;          M_in_[2][2] = 1;
   
   // get translation
    trans_dist = 125.0; // 105mm
   if (!epi_flag) {
      vcl_cout<<"epipole needed!\n"<<vcl_endl;;
      return;
   }
   T = vnl_inverse(M_in_) * epi_;
   T /= vcl_sqrt(T[0]*T[0] + T[1]*T[1] + T[2]*T[2]);
   T *= trans_dist;
   
   E1[0][0] = 1;       E1[0][1] = 0;        E1[0][2] = 0;          E1[0][3] = 0;
   E1[1][0] = 0;       E1[1][1] = 1;        E1[1][2] = 0;          E1[1][3] = 0;
   E1[2][0] = 0;       E1[2][1] = 0;        E1[2][2] = 1;          E1[2][3] = 0;
   
   E2[0][0] = 1;       E2[0][1] = 0;        E2[0][2] = 0;          E2[0][3] = T[0];
   E2[1][0] = 0;       E2[1][1] = 1;        E2[1][2] = 0;          E2[1][3] = T[1];
   E2[2][0] = 0;       E2[2][1] = 0;        E2[2][2] = 1;          E2[2][3] = T[2];

   vcl_cout<< "initializing camera.......\n"<<vcl_endl;
}

/////////////////////////////////////////////////////
void rc3d_windows_frame::initialize_camera_3()
{
   //vcl_cout<<"camera initializiing...."<<vcl_endl;   
   M_in_[0][0] = 2000.0; M_in_[0][1] = 0;        M_in_[0][2] = 512.0;
   M_in_[1][0] = 0;        M_in_[1][1] = 2000.0; M_in_[1][2] = 384.0;
   M_in_[2][0] = 0;        M_in_[2][1] = 0;      M_in_[2][2] = 1;
   
   // get translation
    trans_dist = 125.0; // 105mm
   if (!epi_flag) {
      vcl_cout<<"epipole needed!\n"<<vcl_endl;;
      return;
   }
   
   float xe=epi_[0]/epi_[2];
   float ye=epi_[1]/epi_[2];
   
   float x0,y0,x1,y1,x2,y2;
   bool flag=false;
   char ch;
   
   while (!flag) {
      tabs_picker_[0]->pick_point(&x0,&y0);
      tabs_picker_[1]->pick_point(&x1,&y1);
      tabs_picker_[2]->pick_point(&x2,&y2);
      //tabs_picker_[0]->pick_point(&x20,&y20);
      //tabs_picker_[1]->pick_point(&x21,&y21);
      //tabs_picker_[2]->pick_point(&x22,&y22);

      vcl_cout<<x0<<" "<<y0<<" "<<x1<<" "<<y1<<" "<<x2<<" "<<y2<<vcl_endl;
      //vcl_cout<<"satisfied?[y]";
      flag=true;
      //vcl_cin>>ch;
      //if (ch=='y') flag=true;
      //else flag=false;
      
   }
   T = vnl_inverse(M_in_) * epi_;
   double T_normal=vcl_sqrt(T[0]*T[0] + T[1]*T[1] + T[2]*T[2]);
   T /= T_normal;
   T *= trans_dist;
   
   float c=vcl_sqrt(vcl_pow(double(x2-xe),2)+vcl_pow(y2-ye,2));
   float b=vcl_sqrt(vcl_pow(double(x1-xe),2)+vcl_pow(y1-ye,2));
   float a=vcl_sqrt(vcl_pow(double(x0-xe),2)+vcl_pow(y0-ye,2));

   float cx=vcl_fabs(x2-xe),cy=vcl_fabs(y2-ye);
   float bx=vcl_fabs(x1-xe),by=vcl_fabs(y1-ye);
   float ax=vcl_fabs(x0-xe),ay=vcl_fabs(y0-ye);
   //float ratio=vcl_sqrt(vcl_pow(double(x1-xe),2.0)+vcl_pow(y1-ye,2.0))/vcl_sqrt(vcl_pow(double(x2-xe),2.0)+vcl_pow(y2-ye,2.0));
   float ratio=(b-a)/(c-a)*c/b;

   vcl_cout<<" camera ratio is: "<<ratio<<vcl_endl;
   //vcl_cout<<"ratio: "<<ratio<<vcl_endl;
   //vcl_cout<<ax<<" "<<ay<<" "<<bx<<" "<<by<<" "<<cx<<" "<<cy<<vcl_endl;
   //vcl_cout<<a/c<<" "<<b/c<<vcl_endl;
   
   //T_intpl = vnl_inverse(M_in_) * epi_;
   //T_intpl /= vcl_sqrt(T_intpl[0]*T_intpl[0] + T_intpl[1]*T_intpl[1] + T_intpl[2]*T_intpl[2]);
   //T_intpl *= trans_dist;

   
   
   //vnl_double_3 L(M_in_*T_intpl); 
   //L[0]=L[0] *ratio;//2d image projected
   //L[1]=L[1] *ratio;
   //L[2]=L[2] *ratio;
   //T_intpl =vnl_inverse(M_in_) *L;


   E1[0][0] = 1;   E1[0][1] = 0;    E1[0][2] = 0;      E1[0][3] = 0;
   E1[1][0] = 0;   E1[1][1] = 1;    E1[1][2] = 0;      E1[1][3] = 0;
   E1[2][0] = 0;   E1[2][1] = 0;    E1[2][2] = 1;      E1[2][3] = 0;
   
   E3[0][0] = 1;   E3[0][1] = 0;    E3[0][2] = 0;      E3[0][3] = T[0];
   E3[1][0] = 0;   E3[1][1] = 1;    E3[1][2] = 0;      E3[1][3] = T[1];
   E3[2][0] = 0;   E3[2][1] = 0;    E3[2][2] = 1;      E3[2][3] = T[2];
   

   


   //vnl_double_3 t(x1,y1,1.0);


   //vcl_cout<<t<<vcl_endl;

   /*T_intpl =vnl_inverse(M_in_) *t;

   
   vcl_cout<<T_intpl<<vcl_endl;

   T_intpl[0] -=x_3d;
   T_intpl[1] -=y_3d;
   T_intpl[2] -=z_3d;
   vcl_cout<<T_intpl<<vcl_endl;
   double scale=T_intpl[0]*T[0]+T_intpl[1]*T[1]+T_intpl[2]*T[2];
   vcl_cout<<"scale: "<<scale<<vcl_endl;
   scale /=(T_normal*T_normal);
   vcl_cout<<"scale: "<<scale<<vcl_endl;
   //E2[0][0] = 1;       E2[0][1] = 0;        E2[0][2] = 0;          E2[0][3] =T_intpl[0];
   //E2[1][0] = 0;       E2[1][1] = 1;        E2[1][2] = 0;          E2[1][3] =T_intpl[1];
   //E2[2][0] = 0;       E2[2][1] = 0;        E2[2][2] = 1;          E2[2][3] =T_intpl[2];
scale=2.0322;
scale=b/c;

*/


   E2[0][0] = 1;  E2[0][1] = 0;   E2[0][2] = 0;   E2[0][3] =ratio*T[0];
   E2[1][0] = 0;  E2[1][1] = 1;   E2[1][2] = 0;   E2[1][3] =ratio*T[1];
   E2[2][0] = 0;  E2[2][1] = 0;   E2[2][2] = 1;   E2[2][3] =ratio*T[2];

   vnl_double_3x4 P1 = M_in_*E1,P2 =M_in_*E2, P3=M_in_*E3;


   vgl_point_2d<double> x1_2d(x0,y0);
   vgl_point_2d<double> x3_2d(x2,y2);
   
   vgl_point_3d<double> point_3d = brct_algos::triangulate_3d_point(x1_2d, P1, x3_2d, P3);
   vcl_cout<<"1 vs 3: "<<point_3d<<vcl_endl;



   double x_3d=point_3d.x();
   double y_3d=point_3d.y();
   double z_3d=point_3d.z();

   if (0)
   for (unsigned i=0;i<3;i++) 
   {
      E1[i][3] /= T_normal;
      E1[i][3] *= trans_dist;
      E2[i][3] /= T_normal;
      E2[i][3] *= trans_dist;
      E3[i][3] /= T_normal;
      E3[i][3] *= trans_dist;
   }


   PL_.erase(PL_.begin(), PL_.end());
   PL_.push_back(P1);PL_.push_back(P2);PL_.push_back(P3);
   init_3_cam_flag_=true;
   vcl_cout<<"3 camera initialization finished.....!!\n"<<vcl_endl;
}


// 6-29-2004 Given Camera...
/////////////////////////////////////////////////////
void rc3d_windows_frame::initialize_camera_3_matt()
{
   //vcl_cout<<"camera initializiing...."<<vcl_endl;   
   M_in_[0][0] = 2000.0; M_in_[0][1] = 0;        M_in_[0][2] = 512.0;
   M_in_[1][0] = 0;        M_in_[1][1] = 2000.0; M_in_[1][2] = 384.0;
   M_in_[2][0] = 0;        M_in_[2][1] = 0;      M_in_[2][2] = 1;
   
   E1.fill(0.0); E1.fill_diagonal(1.0);
   E2.fill(0.0); E2.fill_diagonal(1.0);
   E3.fill(0.0); E3.fill_diagonal(1.0);

   vgui_dialog dlg("Read Matt's");
   //static vcl_string filename = "C:/CVPR2004/truck_bbox.txt";
   static vcl_string filename = "C:/CVPR2004/carbbox.txt";
   
   static vcl_string ext = "*.*";
   static int frame_1=3;
   static int frame_2=5;
   static int frame_3=7;

   dlg.field("Frame 1",frame_1);
   dlg.field("Frame 2",frame_2);
   dlg.field("Frame 3",frame_3);
   //static bool Ind_feature=false;
   //static bool k_auto=false;
   //static bool show_list=false;

//   double x1[3];
   vcl_string str1,str_filename;
//char str1[100];
    dlg.file("read file name", ext, filename);
   
   if (!dlg.ask()) { 
      vcl_cout<<"Camera FIle Read Fail!" <<vcl_endl;
      return;
   }
   
   vnl_double_4x4 RT(0.0);
   //double x,y,z,t;
   double temp=0;
   vnl_double_3x4 Camera(0.0);
   vcl_vector<vnl_double_3x4 > Cam_List;
   int total_camera_number=-1;
   int start_number=0,end_number=0;
   

   if (filename != "")
      {
         vcl_ifstream in(filename.c_str());
         vcl_cout<<"reading camera file "<< filename.c_str()<<vcl_endl;
         
         in>> total_camera_number;
         if (total_camera_number<0||total_camera_number>100) {
            vcl_cout<<"file read or format error"<<vcl_endl;
            return;
         }
         for (unsigned j=0;j<10;j++) {
            in>>str1;
            if (j==2) {
               str_filename=str1;
            }
            if (j==4) {
               start_number =vcl_atoi(str1.c_str());   
            }
            if (j==6) {
               end_number=vcl_atoi(str1.c_str());
            }
            vcl_cout<<str1<<vcl_endl;
         }
      
         //read R-T matrix...

         for (unsigned i=0;i<4;i++) {
            for (unsigned j=0;j<4;j++) {
               in >> temp;
               RT[i][j]=temp;
            }
         }
         vcl_cout<<RT<<vcl_endl;
         BB_RT_matt_=RT;


         for (unsigned i=0;i<total_camera_number;i++) {
            Camera.fill(0.0);
            in>>str1>>str1;
            vcl_cout<<str1<<vcl_endl;
            for (unsigned j=0;j<3; j++) {
               for (unsigned k=0;k<4;k++) {
                  in >> temp;
                  Camera[j][k]=temp;
               }
            }
            Cam_List.push_back(Camera);
            vcl_cout<<Camera<<vcl_endl;
         }
         
         
      }

   E1=Cam_List[frame_1];
   E2=Cam_List[frame_2];
   E3=Cam_List[frame_3];

   vcl_cout<<frame_1<<" "<<frame_2<<" "<<frame_3<<vcl_endl;
   

   //vnl_double_3x4 P1 = M_in_*E1,P2 =M_in_*E2, P3=M_in_*E3;
   vnl_double_3x4 P1 = E1,P2 =E2, P3=E3;

   vcl_cout<<E1<<E2<<E3<<vcl_endl;

   PL_.erase(PL_.begin(), PL_.end());
   PL_.push_back(E1);PL_.push_back(E2);PL_.push_back(E3);
   
   init_3_cam_flag_=true;
   vcl_cout<<"3 camera initialization finished.....!!\n"<<vcl_endl;
   epi_flag=true;

   // load image here..
   vcl_string f1,f2,f3;

   int fram1 = start_number + frame_1 ;
   int fram2 = start_number + frame_2 ;
   int fram3 = start_number + frame_3 ;


   char tf[10];char f11[10]; 
   fore_name.clear();
   //itoa(fram1,tf,10);
  // sprintf(f11,"%03s",tf);
   vcl_sprintf(f11,"%03d",fram1);
   f1=str_filename + "/00" + f11+".png";
   vcl_cout<<f1<<vcl_endl;

   fore_name.push_back(f1);
   //load_image_file("C:/E3D/REAL/Aq2_1_left_brown_U_white_van/00016.tiff", true, 2,0);
   load_image_file(f1, true, 0,0);
   
   //itoa(fram2,tf,10);
   vcl_sprintf(f11,"%03d",fram2);
   f1=str_filename + "/00" + f11+".png";
   vcl_cout<<f1<<vcl_endl;
   load_image_file(f1, true, 1,0);

   fore_name.push_back(f1);


   //itoa(fram3,tf,10);
   //sprintf(f11,"%03s",tf);
   vcl_sprintf(f11,"%03d",fram3);
   f1=str_filename + "/00" + f11+".png";
   vcl_cout<<f1<<vcl_endl;
   load_image_file(f1, true, 2,0);

   fore_name.push_back(f1);

   vcl_cout<< fore_name.size()<<vcl_endl;
   grid_->post_redraw();

   // hmmm 7-7-2004
   double Scale=0.045;
   d3_rho_=6*Scale;
   d_tube_=1*Scale;
   // also inc_ size too
   cube_inc_=1*Scale;

   d3_probe_load_all("c:/E3D/REAL/ICPR2004/Probe_Depot/MATT/BACK_GROUND_TEST/Back_Ground_test.txt");


   
}

// 6-29-2004 Given Camera...
/////////////////////////////////////////////////////
/*void rc3d_windows_frame::initialize_camera_3_string_scan()
{
   //vcl_cout<<"camera initializiing...."<<vcl_endl;   
   M_in_[0][0] = 2000.0; M_in_[0][1] = 0;        M_in_[0][2] = 512.0;
   M_in_[1][0] = 0;        M_in_[1][1] = 2000.0; M_in_[1][2] = 384.0;
   M_in_[2][0] = 0;        M_in_[2][1] = 0;      M_in_[2][2] = 1;
   
   E1.fill(0.0); E1.fill_diagonal(1.0);
   E2.fill(0.0); E2.fill_diagonal(1.0);
   E3.fill(0.0); E3.fill_diagonal(1.0);

   vgui_dialog dlg("Read Matt's");
   //static vcl_string filename = "C:/CVPR2004/truck_bbox.txt";
   static vcl_string filename = "C:/CVPR2004/carbbox.txt";
   
   static vcl_string ext = "*.*";
   static int frame_1=3;
   static int frame_2=5;
   static int frame_3=7;

   dlg.field("Frame 1",frame_1);
   dlg.field("Frame 2",frame_2);
   dlg.field("Frame 3",frame_3);
   //static bool Ind_feature=false;
   //static bool k_auto=false;
   //static bool show_list=false;

//   double x1[3];
   vcl_string str1,str_filename;
//char str1[100];
    dlg.file("read file name", ext, filename);
   
   if (!dlg.ask()) { 
      vcl_cout<<"Camera FIle Read Fail!" <<vcl_endl;
      return;
   }
   
   vnl_double_4x4 RT(0.0);
   //double x,y,z,t;
   double temp=0;
   vnl_double_3x4 Camera(0.0);
   vcl_vector<vnl_double_3x4 > Cam_List;
   int total_camera_number=-1;
   int start_number=0,end_number=0;
   
   
   if (filename != "")
   {
     vcl_ifstream in(filename.c_str());
     vcl_cout<<"reading camera file "<< filename.c_str()<<vcl_endl;
     vcl_string hhh="";
     while (hhh!="Transform") {
       in>> hhh;
      // vcl_cout<< hhh <<vcl_endl;
     }
     
     for (unsigned i=0;i<4;i++) {
       for (unsigned j=0;j<4;j++) {
         in >> temp;
         RT[i][j]=temp;
       }
     }
     vcl_cout<<RT<<vcl_endl;
     BB_RT_matt_=RT;
     in>>hhh;

     while (hhh=="Camera") {
       
       Camera.fill(0.0);
       in>>hhh;
       vcl_cout<<hhh<<vcl_endl;
       for (unsigned j=0;j<3; j++) {
         for (unsigned k=0;k<4;k++) {
           in >> temp;
           Camera[j][k]=temp;
         }
       }
       Cam_List.push_back(Camera);
       vcl_cout<<Camera<<vcl_endl;
       in>>hhh;
     }
     
     
     
     E1=Cam_List[frame_1];
     E2=Cam_List[frame_2];
     E3=Cam_List[frame_3];
     
     vcl_cout<<frame_1<<" "<<frame_2<<" "<<frame_3<<vcl_endl;
     
     
     //vnl_double_3x4 P1 = M_in_*E1,P2 =M_in_*E2, P3=M_in_*E3;
     vnl_double_3x4 P1 = E1,P2 =E2, P3=E3;
     
     vcl_cout<<E1<<E2<<E3<<vcl_endl;
     
     PL_.erase(PL_.begin(), PL_.end());
     PL_.push_back(E1);PL_.push_back(E2);PL_.push_back(E3);
     
     init_3_cam_flag_=true;
     vcl_cout<<"3 camera initialization finished.....!!\n"<<vcl_endl;
     epi_flag=true;
     
     // load image here..
     vcl_string f1,f2,f3;
     
     start_number=-(Cam_List[0])[2][3];
     
     int fram1 = start_number + frame_1 ;
     int fram2 = start_number + frame_2 ;
     int fram3 = start_number + frame_3 ;
     
     
     char tf[10];char f11[10]; 
     fore_name.clear();
     //itoa(fram1,tf,10);
     // sprintf(f11,"%03s",tf);
     vcl_sprintf(f11,"%03d",fram1);
     f1=str_filename + "/00" + f11+".png";
     vcl_cout<<f1<<vcl_endl;
     
     fore_name.push_back(f1);
     //load_image_file("C:/E3D/REAL/Aq2_1_left_brown_U_white_van/00016.tiff", true, 2,0);
     load_image_file(f1, true, 0,0);
     
     //itoa(fram2,tf,10);
     vcl_sprintf(f11,"%03d",fram2);
     f1=str_filename + "/00" + f11+".png";
     vcl_cout<<f1<<vcl_endl;
     load_image_file(f1, true, 1,0);
     
     fore_name.push_back(f1);
     
     
     //itoa(fram3,tf,10);
     //sprintf(f11,"%03s",tf);
     vcl_sprintf(f11,"%03d",fram3);
     f1=str_filename + "/00" + f11+".png";
     vcl_cout<<f1<<vcl_endl;
     load_image_file(f1, true, 2,0);
     
     fore_name.push_back(f1);
     
     vcl_cout<< fore_name.size()<<vcl_endl;
     grid_->post_redraw();
     
     // hmmm 7-7-2004
     double Scale=0.045;
     d3_rho_=6*Scale;
     d_tube_=1*Scale;
     // also inc_ size too
     cube_inc_=1*Scale;
}

}
*/
void rc3d_windows_frame::initialize_camera_3_string_scan()
{ 
  
  static int auto_cam=4,int first_cam_offset,int auto_offset;
  
  
  //  vcl_cout<<"in scan: "<<frame_1_<<" "<<frame_2_<<" "<<frame_3_<<vcl_endl;
  
  
  
  
  double Scale=1;
  
  
  //vcl_cout<<"camera initializiing...."<<vcl_endl;   
  M_in_[0][0] = 2000.0; M_in_[0][1] = 0;        M_in_[0][2] = 512.0;
  M_in_[1][0] = 0;        M_in_[1][1] = 2000.0; M_in_[1][2] = 384.0;
  M_in_[2][0] = 0;        M_in_[2][1] = 0;      M_in_[2][2] = 1;
  
  E1.fill(0.0); E1.fill_diagonal(1.0);
  E2.fill(0.0); E2.fill_diagonal(1.0);
  E3.fill(0.0); E3.fill_diagonal(1.0);
  
  vgui_dialog dlg("Read Matt's");
  //static vcl_string filename = "C:/CVPR2004/truck_bbox.txt";
  static vcl_string filename = "C:/CVPR2004/carbbox.txt";
  
  static vcl_string ext = "*.*";
  static int frame_1=3;
  static int frame_2=5;
  static int frame_3=7;

        static vcl_string con_filename = "";
   
        static bool con_flag=true;
        static bool flip=false;


  dlg.field("auto cam? (0: no 1: full other:offset)",auto_cam);
  dlg.field("first offset",first_cam_offset);
        dlg.field("auto offset",auto_offset);
  dlg.field("Frame 1",frame_1);
  dlg.field("Frame 2",frame_2);
  dlg.field("Frame 3",frame_3);


        dlg.checkbox("flip_image)",flip);
  
  //static bool Ind_feature=false;
  //static bool k_auto=false;
  //static bool show_list=false;
        //   double x1[3];
  //char str1[100];
  dlg.file("read file name", ext, filename);
        
        dlg.checkbox("CON file?",con_flag);
        dlg.file("CON Filename:", ext, con_filename);
        
  //Batch mode 7-12-2005
        if (BATCH_) {
                
                filename=batch_dir_+"/"+batch_cam_filename_;
                auto_cam=batch_auto_cam_;
    frame_1=batch_frame_1_;
                frame_2=batch_frame_2_;
                frame_3=batch_frame_3_;
                }
        else {
                if (!dlg.ask()) 
                { 
                        vcl_cout<<"Camera FIle Read Fail!" <<vcl_endl;
                        return;
                }
        }
        


  con_flag_=con_flag;


  vcl_string str1,str_filename;
  vnl_double_4x4 RT(0.0);
  //double x,y,z,t;
  double temp=0;
  vnl_double_3x4 Camera(0.0);
  vnl_double_3x4 Camera_Null(-1.0);
  vcl_vector<vnl_double_3x4 > Cam_List;
  int total_camera_number=-1;
  int start_number=0,end_number=0;
 

        vcl_string con_file;
  
  if (filename != "")
  {
    vcl_ifstream in(filename.c_str());
    vcl_cout<<"reading camera file "<< filename.c_str()<<vcl_endl;
    vcl_string hhh="";
    while (hhh!="Transform") {
      in>> hhh;
      // vcl_cout<< hhh <<vcl_endl;
    }
    
    for (unsigned i=0;i<4;i++) {
      for (unsigned j=0;j<4;j++) {
        in >> temp;
        RT[i][j]=temp;
      }
    }
    vcl_cout<<RT<<vcl_endl;
    BB_RT_matt_=RT;
    in>>hhh;
    
    
    int count=0; // for BRMF 2.7
    int empty_camera_number=0;       // for BRMF 2.7
    
    while (hhh=="Camera") {
      
      
      
      
      
      Camera.fill(0.0);
      in>>hhh;
      
      if (count==0) {                 // for BRMF 2.7
        char a[10];                   // for BRMF 2.7
        //vcl_sprintf(a,"%s",hhh);    // for BRMF 2.7
        
        empty_camera_number= atoi(hhh.c_str());// for BRMF 2.7
      }                               // for BRMF 2.7
      count++;                        // for BRMF 2.7
      
      
      vcl_cout<<hhh<<vcl_endl;
      
      
      for (unsigned j=0;j<3; j++) {
        for (unsigned k=0;k<4;k++) {
          in >> temp;
          Camera[j][k]=temp;
        }
      }

                        // flipping image 8-16-2005
      if (flip) {

                                vnl_matrix <double> dflip(3,3,0.0);
                                dflip(0,0)=-1.0;
                                dflip(0,2)=1023;

                                dflip(1,1)=1.0;
                                dflip(2,2)=1.0;
                                Camera=dflip*Camera;
                        }


      Cam_List.push_back(Camera);
      vcl_cout<<Camera<<vcl_endl;
      in>>hhh;
    }
    
    
    double p7x,p7y,p7z,p0x,p0y,p0z;

    /*vcl_cout<<hhh<<vcl_endl;
    if (hhh=="") {
      p7_[0]=0;p7_[1]=0;p7_[2]=0;
      p0_=p7_;
    }
    else {*/
      p7_[0]=atof(hhh.c_str());
      in>>p7_[1];in>>p7_[2];
      in>>p0_[0];in>>p0_[1];in>>p0_[2];

    //}
    vcl_cout<<p7_<<" "<<p0_<<vcl_endl;
 

    // int empty_camera_number=-(int)(Cam_List[0])[2][3];// for BRMF 2.7
    
    vcl_cout<<"empty cam:" <<empty_camera_number<<vcl_endl;
    
    
    for (unsigned i=0;i<empty_camera_number;i++) {
      //Cam_List.insert();
    }
    
    frame_1-=empty_camera_number;
    frame_2-=empty_camera_number;
    frame_3-=empty_camera_number;
    
    if (auto_cam>0) {
      int last_camera=Cam_List.size()-1;
      
      if (auto_cam==1) {
        frame_1=2; 
        frame_2=vcl_floor((double)last_camera/2);
        frame_3=last_camera-2;
      }
      else if (auto_cam==4) {



        vcl_cout<<vul_file::dirname(filename);
        // test if fname is a directory
        vcl_vector<vcl_string> flist;



        vul_file_iterator fn=vul_file::dirname(filename)+"/*.con";
        //vul_file_iterator fn=input_file_path+"/*.con";
        for ( ; fn; ++fn) 
        {
          vcl_string input_con_file = fn();//
          flist.push_back(input_con_file);
        }

        //vul_file::basename(flist[0]


        frame_1=vcl_atoi(vul_file::basename(flist[1]).c_str());
        int middle=vcl_floor( flist.size()/2.0 );
        frame_2=vcl_atoi(vul_file::basename( flist[middle]).c_str() );
        frame_3=vcl_atoi(vul_file::basename(flist[flist.size()-1]).c_str());

        frame_1-=empty_camera_number;
        frame_2-=empty_camera_number;
        frame_3-=empty_camera_number;

      }
      else {
        frame_1=first_cam_offset;
        frame_2=frame_1+auto_offset;
        frame_3=frame_2+auto_offset;
      }
      
      //frame_1_=empty_camera_number+frame_1;
      // frame_2_=empty_camera_number+frame_2;
      // frame_3_=empty_camera_number+frame_3;
      
      vcl_cout<<"auto_cam on! "<<frame_1<<" "<<frame_2<<" "<<frame_3<<vcl_endl;
      
    }    
    
    
    E1=Cam_List[frame_1];
    E2=Cam_List[frame_2];
    E3=Cam_List[frame_3];
    
    vcl_cout<<frame_1<<" "<<frame_2<<" "<<frame_3<<vcl_endl;
    //vnl_double_3x4 P1 = M_in_*E1,P2 =M_in_*E2, P3=M_in_*E3;
    vnl_double_3x4 P1 = E1,P2 =E2, P3=E3;
    
    vcl_cout<<E1<<E2<<E3<<vcl_endl;
    
    PL_.erase(PL_.begin(), PL_.end());
    PL_.push_back(E1);PL_.push_back(E2);PL_.push_back(E3);
    
    init_3_cam_flag_=true;
    vcl_cout<<"3 camera initialization finished.....!!\n"<<vcl_endl;
    epi_flag=true;
    
    // load image here..
    vcl_string f1,f2,f3,c1,c2,c3;
    
    start_number=-(Cam_List[0])[2][3];

    start_number=empty_camera_number;//1-12-2005
    
                int fram1 = start_number + frame_1 ;
    int fram2 = start_number + frame_2 ;
    int fram3 = start_number + frame_3 ;
    //  vcl_vector <bsol_intrinsic_curve_2d_sptr> curve_2dl;
    curve_2dl_.clear();
    char tf[10];char f11[10]; 
    fore_name.clear();
    //itoa(fram1,tf,10);
    // sprintf(f11,"%03s",tf);
    vcl_sprintf(f11,"%03d",fram1);
                
                if (BATCH_) f1=batch_dir_ + "./00" + f11+".png";
    else f1=str_filename + "./00" + f11+".png";
    vcl_cout<<f1<<vcl_endl;

                
    fore_name.push_back(f1);
    //load_image_file("C:/E3D/REAL/Aq2_1_left_brown_U_white_van/00016.tiff", true, 2,0);
                load_image_file(f1, true, 0,0);


    veh_cons_.clear();
    if (con_flag) {
                
        //c1=con_file + "./pgm/blobfinal_0" + f11+".con";
      c1=con_file + "00" + f11+".con";
        vcl_cout<<c1<<vcl_endl;   
                                bsol_intrinsic_curve_2d_sptr curve_2d = new bsol_intrinsic_curve_2d;
                    curve_2d->readCONFromFile(c1.c_str() );
                                curve_2dl_.push_back(curve_2d);
        
        double *x1,*y1;
        int N=curve_2d->size();
        x1 = (double *)malloc(sizeof(double)*N);
        y1 = (double *)malloc(sizeof(double)*N);
        for (unsigned j=0;j<curve_2d->size();j++)
        { 
          x1[j]=curve_2d->x(j);
          y1[j]=curve_2d->y(j);

        }
        vgl_polygon<double> veh_contour(x1,y1,N);
        veh_cons_.push_back(veh_contour);   
        free(x1);free(y1);
                }
    
    //itoa(fram2,tf,10);
    vcl_sprintf(f11,"%03d",fram2);

                if (BATCH_) f1=batch_dir_ + "./00" + f11+".png";
    else f1=str_filename + "./00" + f11+".png";
    vcl_cout<<f1<<vcl_endl;
                load_image_file(f1, true, 1,0);
    
    fore_name.push_back(f1);



    if (con_flag) {
        c1=con_file + "00" + f11+".con";
        vcl_cout<<c1<<vcl_endl;   
                                bsol_intrinsic_curve_2d_sptr curve_2d = new bsol_intrinsic_curve_2d;
                    curve_2d->readCONFromFile(c1.c_str() );
                                curve_2dl_.push_back(curve_2d);

         double *x1,*y1;
        int N=curve_2d->size();
        x1 = (double *)malloc(sizeof(double)*N);
        y1 = (double *)malloc(sizeof(double)*N);
        for (unsigned j=0;j<curve_2d->size();j++)
        { 
          x1[j]=curve_2d->x(j);
          y1[j]=curve_2d->y(j);

        }
        vgl_polygon<double> veh_contour(x1,y1,N);
        veh_cons_.push_back(veh_contour);   
        free(x1);free(y1);

                }
    
    //itoa(fram3,tf,10);
    //sprintf(f11,"%03s",tf);
    vcl_sprintf(f11,"%03d",fram3);

        if (BATCH_) f1=batch_dir_ + "./00" + f11+".png";
    else f1=str_filename + "./00" + f11+".png";
    
                vcl_cout<<f1<<vcl_endl;
                load_image_file(f1, true, 2,0);
    
    fore_name.push_back(f1);
    

                if (con_flag) {
        c1=con_file + "00" + f11+".con";
        vcl_cout<<c1<<vcl_endl;   
                                bsol_intrinsic_curve_2d_sptr curve_2d = new bsol_intrinsic_curve_2d;
                    curve_2d->readCONFromFile(c1.c_str() );
                                curve_2dl_.push_back(curve_2d);

         double *x1,*y1;
        int N=curve_2d->size();
        x1 = (double *)malloc(sizeof(double)*N);
        y1 = (double *)malloc(sizeof(double)*N);
        for (unsigned j=0;j<curve_2d->size();j++)
        { 
          x1[j]=curve_2d->x(j);
          y1[j]=curve_2d->y(j);

        }
        vgl_polygon<double> veh_contour(x1,y1,N);
        veh_cons_.push_back(veh_contour);   
        free(x1);free(y1);
                }



    vcl_cout<< fore_name.size()<<vcl_endl;
                grid_->post_redraw();
    
    // hmmm 7-7-2004
    double Scale=0.045;
    d3_rho_=6*Scale;
    d_tube_=1*Scale;
    // also inc_ size too
    cube_inc_=1*Scale;



    maps_[0].fill(0.0);
                maps_[1].fill(0.0);
                maps_[2].fill(0.0);
                //vil1_memory_image_of <float> maps_[3]; 
                maps_search_[0].fill(0.0);
                maps_search_[1].fill(0.0);
                        maps_search_[2].fill(0.0);
                        frame_1=fram1;
frame_2=fram2;
frame_3=fram3;




batch_frame_1_=frame_1;
batch_frame_2_=frame_2;
batch_frame_3_=frame_3;



}

}


void rc3d_windows_frame::initialize_camera_N_string_scan()
{ 
  
  int auto_cam=1,int first_cam_offset,int auto_offset;
  
  
  //  vcl_cout<<"in scan: "<<frame_1_<<" "<<frame_2_<<" "<<frame_3_<<vcl_endl;
  
  
  
  
  double Scale=1;
  
  
  //vcl_cout<<"camera initializiing...."<<vcl_endl;   
  M_in_[0][0] = 2000.0; M_in_[0][1] = 0;        M_in_[0][2] = 512.0;
  M_in_[1][0] = 0;        M_in_[1][1] = 2000.0; M_in_[1][2] = 384.0;
  M_in_[2][0] = 0;        M_in_[2][1] = 0;      M_in_[2][2] = 1;
  
  E1.fill(0.0); E1.fill_diagonal(1.0);
  E2.fill(0.0); E2.fill_diagonal(1.0);
  E3.fill(0.0); E3.fill_diagonal(1.0);
  
  vgui_dialog dlg("Read Matt's");
  //static vcl_string filename = "C:/CVPR2004/truck_bbox.txt";
  static vcl_string filename = "C:/CVPR2004/carbbox.txt";
  
  static vcl_string ext = "*.*";
  static int frame_1=3;
  static int frame_2=5;
  static int frame_3=7;
  
  dlg.field("Frame 1",frame_1);
  dlg.field("Frame 2",frame_2);
  dlg.field("Frame 3",frame_3);
  //static bool Ind_feature=false;
  //static bool k_auto=false;
  //static bool show_list=false;
  
  //   double x1[3];
  
  //char str1[100];
  dlg.file("read file name", ext, filename);
  
  if (!dlg.ask()) { 
    vcl_cout<<"Camera FIle Read Fail!" <<vcl_endl;
    return;
  }
  
  vcl_string str1,str_filename;
  vnl_double_4x4 RT(0.0);
  //double x,y,z,t;
  double temp=0;
  vnl_double_3x4 Camera(0.0);
  vnl_double_3x4 Camera_Null(-1.0);
  vcl_vector<vnl_double_3x4 > Cam_List;
  int total_camera_number=-1;
  int start_number=0,end_number=0;
  
  Cam_List_.clear();
  if (filename != "")
  {
    vcl_ifstream in(filename.c_str());
    vcl_cout<<"reading camera file "<< filename.c_str()<<vcl_endl;
    vcl_string hhh="";
    while (hhh!="Transform") {
      in>> hhh;
      // vcl_cout<< hhh <<vcl_endl;
    }
    
    for (unsigned i=0;i<4;i++) {
      for (unsigned j=0;j<4;j++) {
        in >> temp;
        RT[i][j]=temp;
      }
    }
    vcl_cout<<RT<<vcl_endl;
    BB_RT_matt_=RT;
    in>>hhh;
    
    
    int count=0; // for BRMF 2.7
    int empty_camera_number=0;       // for BRMF 2.7
    
    while (hhh=="Camera") {
      
      
      
      
      
      Camera.fill(0.0);
      in>>hhh;
      
      if (count==0) {                 // for BRMF 2.7
        char a[10];                   // for BRMF 2.7
        //vcl_sprintf(a,"%s",hhh);    // for BRMF 2.7
        
        empty_camera_number= atoi(hhh.c_str());// for BRMF 2.7
      }                               // for BRMF 2.7
      count++;                        // for BRMF 2.7
      
      
      vcl_cout<<hhh<<vcl_endl;
      
      
      for (unsigned j=0;j<3; j++) {
        for (unsigned k=0;k<4;k++) {
          in >> temp;
          Camera[j][k]=temp;
        }
      }
      
      Cam_List_.push_back(Camera);
      vcl_cout<<Camera<<vcl_endl;
      in>>hhh;
    }
    
    
    // int empty_camera_number=-(int)(Cam_List[0])[2][3];// for BRMF 2.7
    
    vcl_cout<<"empty cam:" <<empty_camera_number<<vcl_endl;
    
    
    for (unsigned i=0;i<Cam_List_.size();i++) {
      //Cam_List.insert();
                         //for (unsigned int col1=0, row1=0; col1<iview_; ++col1) {


        
                        if (i>3) {
      vgui_image_tableau_sptr tab_img = bgui_image_tableau_new();
      //img_2d_.push_back(tab_img);
         
      bgui_vtol2D_tableau_sptr btab = bgui_vtol2D_tableau_new(tab_img);
      vtol_tabs_.push_back(btab);

        bgui_picker_tableau_new picker(btab);

      //bgui_picker_tableau_sptr picker = bgui_picker_tableau_new(btab);
      tabs_picker_.push_back(picker);
      
      vgui_viewer2D_tableau_sptr v2d = vgui_viewer2D_tableau_new(picker);
      grid_->add_at(v2d, i, 0);
        
                        }
                        
                        
                        char tf[10];char f11[10]; 
                        fore_name.clear();
                        //itoa(fram1,tf,10);
                        // sprintf(f11,"%03s",tf);
                        
                        int fram1 = empty_camera_number + i ;
                        vcl_sprintf(f11,"%03d",fram1);
                        
                        vcl_string f1=str_filename+"./00" + f11+".png";
                        vcl_cout<<i<<" "<<f1<<vcl_endl;
                        load_image_file(f1, true, i,0);
   
    }
    
                return;
    frame_1-=empty_camera_number;
    frame_2-=empty_camera_number;
    frame_3-=empty_camera_number;
    
    if (auto_cam>0) {
      int last_camera=Cam_List.size()-1;
      
      if (auto_cam==1) {
        frame_1=2; 
        frame_2=vcl_floor((double)last_camera/2);
        frame_3=last_camera-2;
      }
      else {
        frame_1=first_cam_offset;
        frame_2=frame_1+auto_offset;
        frame_3=frame_2+auto_offset;
      }
      
      //frame_1_=empty_camera_number+frame_1;
      // frame_2_=empty_camera_number+frame_2;
      // frame_3_=empty_camera_number+frame_3;
      
      vcl_cout<<"auto_cam on! "<<frame_1<<" "<<frame_2<<" "<<frame_3<<vcl_endl;
      
    }    
    
    
    E1=Cam_List[frame_1];
    E2=Cam_List[frame_2];
    E3=Cam_List[frame_3];
    
    vcl_cout<<frame_1<<" "<<frame_2<<" "<<frame_3<<vcl_endl;
    //vnl_double_3x4 P1 = M_in_*E1,P2 =M_in_*E2, P3=M_in_*E3;
    vnl_double_3x4 P1 = E1,P2 =E2, P3=E3;
    
    vcl_cout<<E1<<E2<<E3<<vcl_endl;
    
    PL_.erase(PL_.begin(), PL_.end());
    PL_.push_back(E1);PL_.push_back(E2);PL_.push_back(E3);
    
    init_3_cam_flag_=true;
    vcl_cout<<"3 camera initialization finished.....!!\n"<<vcl_endl;
    epi_flag=true;
    
    // load image here..
    vcl_string f1,f2,f3;
    
    start_number=-(Cam_List[0])[2][3];

    start_number=empty_camera_number;//1-12-2005
    
                int fram1 = start_number + frame_1 ;
    int fram2 = start_number + frame_2 ;
    int fram3 = start_number + frame_3 ;
    
    
    char tf[10];char f11[10]; 
    fore_name.clear();
    //itoa(fram1,tf,10);
    // sprintf(f11,"%03s",tf);
    vcl_sprintf(f11,"%03d",fram1);
    f1=str_filename + "./00" + f11+".png";
    vcl_cout<<f1<<vcl_endl;
    
    fore_name.push_back(f1);
    //load_image_file("C:/E3D/REAL/Aq2_1_left_brown_U_white_van/00016.tiff", true, 2,0);
       load_image_file(f1, true, 0,0);
    
    //itoa(fram2,tf,10);
    vcl_sprintf(f11,"%03d",fram2);
    f1=str_filename + "./00" + f11+".png";
    vcl_cout<<f1<<vcl_endl;
        load_image_file(f1, true, 1,0);
    
    fore_name.push_back(f1);
    
    
    //itoa(fram3,tf,10);
    //sprintf(f11,"%03s",tf);
    vcl_sprintf(f11,"%03d",fram3);
    f1=str_filename + "./00" + f11+".png";
    vcl_cout<<f1<<vcl_endl;
       load_image_file(f1, true, 2,0);
    
    fore_name.push_back(f1);
    
    vcl_cout<< fore_name.size()<<vcl_endl;
       grid_->post_redraw();
    
    // hmmm 7-7-2004
    double Scale=0.045;
    d3_rho_=6*Scale;
    d_tube_=1*Scale;
    // also inc_ size too
    cube_inc_=1*Scale;
}
}


// 8-2-2004 (for Vorl) Given Camera...
/////////////////////////////////////////////////////
void rc3d_windows_frame::initialize_camera_3_matt_Vorl(int frame_1,
                                                       int frame_2,
                                                       int frame_3,
                                                       double Scale,
                                                       vcl_string filename)
{
   //vcl_cout<<"camera initializiing...."<<vcl_endl;   
   M_in_[0][0] = 2000.0; M_in_[0][1] = 0;        M_in_[0][2] = 512.0;
   M_in_[1][0] = 0;        M_in_[1][1] = 2000.0; M_in_[1][2] = 384.0;
   M_in_[2][0] = 0;        M_in_[2][1] = 0;      M_in_[2][2] = 1;
   
   E1.fill(0.0); E1.fill_diagonal(1.0);
   E2.fill(0.0); E2.fill_diagonal(1.0);
   E3.fill(0.0); E3.fill_diagonal(1.0);

   vgui_dialog dlg("Read Matt's");
   
   vcl_string str1,str_filename;

   vnl_double_4x4 RT(0.0);

   double temp=0;
   vnl_double_3x4 Camera(0.0);
   vcl_vector<vnl_double_3x4 > Cam_List;
   int total_camera_number=-1;
   int start_number=0,end_number=0;
   

   if (filename != "")
      {
         vcl_ifstream in(filename.c_str());
         vcl_cout<<"reading camera file "<< filename.c_str()<<vcl_endl;
         
         in>> total_camera_number;
         if (total_camera_number<0||total_camera_number>100) {
            vcl_cout<<"file read or format error"<<vcl_endl;
            return;
         }
         for (unsigned j=0;j<10;j++) {
            in>>str1;
            if (j==2) {
               str_filename=str1;
            }
            if (j==4) {
               start_number =vcl_atoi(str1.c_str());   
            }
            if (j==6) {
               end_number=vcl_atoi(str1.c_str());
            }
            vcl_cout<<str1<<vcl_endl;
         }
      
         //read R-T matrix...

         for (unsigned i=0;i<4;i++) {
            for (unsigned j=0;j<4;j++) {
               in >> temp;
               RT[i][j]=temp;
            }
         }
         vcl_cout<<RT<<vcl_endl;
         BB_RT_matt_=RT;


         for (unsigned i=0;i<total_camera_number;i++) {
            Camera.fill(0.0);
            in>>str1>>str1;
            vcl_cout<<str1<<vcl_endl;
            for (unsigned j=0;j<3; j++) {
               for (unsigned k=0;k<4;k++) {
                  in >> temp;
                  Camera[j][k]=temp;
               }
            }
            Cam_List.push_back(Camera);
            vcl_cout<<Camera<<vcl_endl;
         }
         
         
      }

   E1=Cam_List[frame_1];
   E2=Cam_List[frame_2];
   E3=Cam_List[frame_3];

   vcl_cout<<E3<<vcl_endl;
   

   //vnl_double_3x4 P1 = M_in_*E1,P2 =M_in_*E2, P3=M_in_*E3;
   vnl_double_3x4 P1 = E1,P2 =E2, P3=E3;

   vcl_cout<<E1<<E2<<E3<<vcl_endl;

   PL_.erase(PL_.begin(), PL_.end());
   PL_.push_back(E1);PL_.push_back(E2);PL_.push_back(E3);
   
   init_3_cam_flag_=true;
   vcl_cout<<"3 camera initialization finished.....!!\n"<<vcl_endl;
   epi_flag=true;

   // load image here..
   vcl_string f1,f2,f3;

   int fram1 = start_number + frame_1 ;
   int fram2 = start_number + frame_2 ;
   int fram3 = start_number + frame_3 ;


   char tf[10];char f11[10]; 
   
   //itoa(fram1,tf,10);
  // sprintf(f11,"%03s",tf);
   vcl_sprintf(f11,"%03d",fram1);
   f1=str_filename + "/00" + f11+".png";
   vcl_cout<<f1<<vcl_endl;
   //load_image_file("C:/E3D/REAL/Aq2_1_left_brown_U_white_van/00016.tiff", true, 2,0);
   load_image_file(f1, true, 0);
   
   //itoa(fram2,tf,10);
   vcl_sprintf(f11,"%03d",fram2);
   f1=str_filename + "/00" + f11+".png";
   vcl_cout<<f1<<vcl_endl;
   load_image_file(f1, true, 1);

   //itoa(fram3,tf,10);
   //sprintf(f11,"%03s",tf);
   vcl_sprintf(f11,"%03d",fram3);
   f1=str_filename + "/00" + f11+".png";
   vcl_cout<<f1<<vcl_endl;
   load_image_file(f1, true, 2);

  // grid_->post_redraw();

   // hmmm 7-7-2004
   //double Scale=0.045;
   d3_rho_=6*Scale;
   d_tube_=1*Scale;
   // also inc_ size too
   cube_inc_=1*Scale;

   d3_probe_load_all("c:/E3D/REAL/ICPR2004/Probe_Depot/MATT/BACK_GROUND_TEST/Back_Ground_test.txt");


   
}

///6-17-2004 debug temp 
void rc3d_windows_frame::initialize_camera_3_debug_temp()
{
   //vcl_cout<<"camera initializiing...."<<vcl_endl;   
   M_in_[0][0] = 2000.0; M_in_[0][1] = 0;        M_in_[0][2] = 512.0;
   M_in_[1][0] = 0;        M_in_[1][1] = 2000.0; M_in_[1][2] = 384.0;
   M_in_[2][0] = 0;        M_in_[2][1] = 0;      M_in_[2][2] = 1;
   
   // get translation
    trans_dist = 125.0; // 105mm
   if (!epi_flag) {
      vcl_cout<<"epipole needed!\n"<<vcl_endl;;
      return;
   }
   
   float xe=epi_[0]/epi_[2];
   float ye=epi_[1]/epi_[2];
   
   float x0,y0,x1,y1,x2,y2;
   bool flag=false;
   char ch;
   
   
   x0=   99.4444,y0= 193,x1= 341.781,y1=  181.136,x2=  616.778,y2= 164.556;
   //x0=99 192 341180616 163;
   T = vnl_inverse(M_in_) * epi_;
   double T_normal=vcl_sqrt(T[0]*T[0] + T[1]*T[1] + T[2]*T[2]);
   T /= T_normal;
   T *= trans_dist;
   
   float c=vcl_sqrt(vcl_pow((double)(x2-xe),2.0)+vcl_pow(double(y2-ye),2.0));
   float b=vcl_sqrt(pow(double(x1-xe),2.0)+pow(double(y1-ye),2.0));
   float a=vcl_sqrt(pow(double(x0-xe),2.0)+pow(double(y0-ye),2.0));

   float cx=vcl_fabs(x2-xe),cy=vcl_fabs(y2-ye);
   float bx=vcl_fabs(x1-xe),by=vcl_fabs(y1-ye);
   float ax=vcl_fabs(x0-xe),ay=vcl_fabs(y0-ye);
   //float ratio=vcl_sqrt(pow(double(x1-xe),2.0)+pow(y1-ye,2.0))/vcl_sqrt(pow(double(x2-xe),2.0)+pow(y2-ye,2.0));
   float ratio=(b-a)/(c-a)*c/b;

   vcl_cout<<" camera ratio is: "<<ratio<<vcl_endl;
   //vcl_cout<<"ratio: "<<ratio<<vcl_endl;
   //vcl_cout<<ax<<" "<<ay<<" "<<bx<<" "<<by<<" "<<cx<<" "<<cy<<vcl_endl;
   //vcl_cout<<a/c<<" "<<b/c<<vcl_endl;
   
   //T_intpl = vnl_inverse(M_in_) * epi_;
   //T_intpl /= vcl_sqrt(T_intpl[0]*T_intpl[0] + T_intpl[1]*T_intpl[1] + T_intpl[2]*T_intpl[2]);
   //T_intpl *= trans_dist;

   //vnl_double_3 L(M_in_*T_intpl); 
   //L[0]=L[0] *ratio;//2d image projected
   //L[1]=L[1] *ratio;
   //L[2]=L[2] *ratio;
   //T_intpl =vnl_inverse(M_in_) *L;


   E1[0][0] = 1;   E1[0][1] = 0;    E1[0][2] = 0;      E1[0][3] = 0;
   E1[1][0] = 0;   E1[1][1] = 1;    E1[1][2] = 0;      E1[1][3] = 0;
   E1[2][0] = 0;   E1[2][1] = 0;    E1[2][2] = 1;      E1[2][3] = 0;
   
   E3[0][0] = 1;   E3[0][1] = 0;    E3[0][2] = 0;      E3[0][3] = T[0];
   E3[1][0] = 0;   E3[1][1] = 1;    E3[1][2] = 0;      E3[1][3] = T[1];
   E3[2][0] = 0;   E3[2][1] = 0;    E3[2][2] = 1;      E3[2][3] = T[2];
   

   

   
   
   
   
   
   


   //vnl_double_3 t(x1,y1,1.0);


   //vcl_cout<<t<<vcl_endl;

   /*T_intpl =vnl_inverse(M_in_) *t;

   
   vcl_cout<<T_intpl<<vcl_endl;

   T_intpl[0] -=x_3d;
   T_intpl[1] -=y_3d;
   T_intpl[2] -=z_3d;
   vcl_cout<<T_intpl<<vcl_endl;
   double scale=T_intpl[0]*T[0]+T_intpl[1]*T[1]+T_intpl[2]*T[2];
   vcl_cout<<"scale: "<<scale<<vcl_endl;
   scale /=(T_normal*T_normal);
   vcl_cout<<"scale: "<<scale<<vcl_endl;
   //E2[0][0] = 1;       E2[0][1] = 0;        E2[0][2] = 0;          E2[0][3] =T_intpl[0];
   //E2[1][0] = 0;       E2[1][1] = 1;        E2[1][2] = 0;          E2[1][3] =T_intpl[1];
   //E2[2][0] = 0;       E2[2][1] = 0;        E2[2][2] = 1;          E2[2][3] =T_intpl[2];
scale=2.0322;
scale=b/c;

*/


   E2[0][0] = 1;  E2[0][1] = 0;   E2[0][2] = 0;   E2[0][3] =ratio*T[0];
   E2[1][0] = 0;  E2[1][1] = 1;   E2[1][2] = 0;   E2[1][3] =ratio*T[1];
   E2[2][0] = 0;  E2[2][1] = 0;   E2[2][2] = 1;   E2[2][3] =ratio*T[2];

   vnl_double_3x4 P1 = M_in_*E1,P2 =M_in_*E2, P3=M_in_*E3;


   vgl_point_2d<double> x1_2d(x0,y0);
   vgl_point_2d<double> x3_2d(x2,y2);
   
   vgl_point_3d<double> point_3d = brct_algos::triangulate_3d_point(x1_2d, P1, x3_2d, P3);
   vcl_cout<<"1 vs 3: "<<point_3d<<vcl_endl;



   double x_3d=point_3d.x();
   double y_3d=point_3d.y();
   double z_3d=point_3d.z();

   if (0)
   for (unsigned i=0;i<3;i++) 
   {
      E1[i][3] /= T_normal;
      E1[i][3] *= trans_dist;
      E2[i][3] /= T_normal;
      E2[i][3] *= trans_dist;
      E3[i][3] /= T_normal;
      E3[i][3] *= trans_dist;
   }


   vcl_cout<<E1<<E2<<E3<<vcl_endl;

   PL_.erase(PL_.begin(), PL_.end());
   PL_.push_back(P1);PL_.push_back(P2);PL_.push_back(P3);
   init_3_cam_flag_=true;
   vcl_cout<<"3 camera initialization finished.....!!\n"<<vcl_endl;
}










#include <vsl/vsl_quick_file.h>


void rc3d_windows_frame::binary_save()
{
//vsl_quick_file_save("my_object.bvl",this);

}



