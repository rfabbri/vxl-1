#include "bcri_calib.h"


//#include <vgl/vgl_homo
#include <vgl/algo/vgl_h_matrix_3d.h>
#include <vgl/vgl_homg_point_3d.h>
bcri_calib *bcri_calib::instance_ = 0;

//===============================================================
//: The singleton pattern - only one instance of the manager can occur
//==============================================================
bcri_calib *bcri_calib::instance()
{
   if (!instance_)
   {
      instance_ = new bcri_calib();
      instance_->init();
   }
   
   return bcri_calib::instance_;
}

//==================================================================
//: constructors/destructor
//==================================================================
bcri_calib::bcri_calib() : vgui_wrapper_tableau()
{
}

bcri_calib::~bcri_calib()
{
}

//======================================================================
//: set up the tableaux at each grid cell
//  the vtol2D_tableaux have been initialized in the constructor
//======================================================================
void bcri_calib::init()
{

  int iview_=2;

   grid_ = vgui_grid_tableau_new(2,1);
   grid_->set_grid_size_changeable(true);
   //unsigned int col=0, row = 0;
   // add 3D tableau
   road_only_=false;
   //for (unsigned int col1=0, row1=0; col1<iview_-1; ++col1) {
   for (unsigned int col1=0, row1=0; col1<static_cast<unsigned>(iview_); ++col1) {
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
bool bcri_calib::handle(const vgui_event &e)
{
   return this->child.handle(e);
}


void bcri_calib::quit()
{
   //clean_up();
   vcl_exit(1);
}

void bcri_calib::clean_up()
{
}

//////////////////////////////////////////////////
void bcri_calib::load_image()
{
   static bool greyscale = true;
   vgui_dialog load_image_dlg("Load Image");
   static vcl_string image_filename = "";
   static vcl_string ext = "*.*";
   load_image_dlg.file("Image Filename:", ext, image_filename);
   load_image_dlg.checkbox("greyscale ", greyscale);
   if (!load_image_dlg.ask())
      return;
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
   
   
}


void bcri_calib::clear_display()
{
   bgui_vtol2D_tableau_sptr btab = this->get_selected_vtol2D_tableau();
   if (btab)
      btab->clear();
   else
      vcl_cout << "In bcri_calib::clear_display() - null tableau\n";
}

void bcri_calib::select_correspond()
{



   clear_display();
   vgui_style_sptr yellow_dots = vgui_style::new_style(1,1,0.0,8, 1);
   vgui_style_sptr probe_candidate = vgui_style::new_style(.8,.4,.2,8, 1);
   vgui_style_sptr probe_best = vgui_style::new_style(.1,.1,.8,8, 1);
   vgui_style_sptr box_corner = vgui_style::new_style(1,1,0.7,4,1);


   //bgui_picker_tableau_sptr ptab = this->get_selected_picker_tableau();
   float x1=0, y1=0;// x2=0, y2=0;
   vgl_homg_point_2d<double> mp_left,mp_right;
   //l_;
   vcl_vector<vgl_homg_line_2d<double> > lines;
   for (int i=0;i<5;i++) {
      vcl_cout<<"# "<<i<<" :";
      vcl_cout<<"pick :";
      tabs_picker_[0]->pick_point(&x1, &y1);
      vcl_cout<<x1<<" "<<y1<<vcl_endl;
      if (i<4) {
        x_[i]=x1;
        y_[i]=y1;
        ones_[i]=1.0;
        vtol_vertex_2d_sptr v1=
          new vtol_vertex_2d(x_[i],y_[i]);
        
        vtol_tabs_[0]->add_vertex(v1,yellow_dots);}
      else {
        x_post_=x1;
        y_post_=y1;
        vtol_vertex_2d_sptr v1=
          new vtol_vertex_2d(x_post_,y_post_);
        
        vtol_tabs_[0]->add_vertex(v1,yellow_dots);
      }

   
   }
   
  
   this->post_redraw();
   
}

void bcri_calib::select_correspond_all()
{



   clear_display();
   vgui_style_sptr yellow_dots = vgui_style::new_style(1,1,0.0,8, 1);
   vgui_style_sptr probe_candidate = vgui_style::new_style(.8,.4,.2,8, 1);
   vgui_style_sptr probe_best = vgui_style::new_style(.1,.1,.8,8, 1);
   vgui_style_sptr box_corner = vgui_style::new_style(1,1,0.7,4,1);


   //bgui_picker_tableau_sptr ptab = this->get_selected_picker_tableau();
   float x1=0, y1=0; // x2=0, y2=0;
   vgl_homg_point_2d<double> mp_left,mp_right;
   //l_;
   vcl_vector<vgl_homg_line_2d<double> > lines;
   for (int i=0;i<26;i++) {
                 vcl_cout<<"# "<<i<<" :";
                 vcl_cout<<"pick :";
                 tabs_picker_[0]->pick_point(&x1, &y1);
                 vcl_cout<<x1<<" "<<y1<<vcl_endl;
                 
                 if (i<25) {
                 xall_[i]=x1;
                 yall_[i]=y1;
                 ones_[i]=1.0;
                 vtol_vertex_2d_sptr v1=
                         new vtol_vertex_2d(xall_[i],yall_[i]);
                 
                 vtol_tabs_[0]->add_vertex(v1,yellow_dots);
                 }
                 if (i==25) {
                         x_post_=x1;
                         y_post_=y1;
                         vtol_vertex_2d_sptr v1=
                         new vtol_vertex_2d(x_post_,y_post_);
                 
                 vtol_tabs_[0]->add_vertex(v1,probe_best);
                 }
                 
                 
                 
   }
   
  
   this->post_redraw();
   
}


////
void bcri_calib::solve() 
{

  
  /*static int image_xy1=4;
  static int image_xy2=2;
  static int image_xy3=9;
  static int image_xy4=10;*/
         static int image_xy1=0;
  static int image_xy2=1;
  static int image_xy3=20;
  static int image_xy4=24;
  static bool z1=true;
  static bool mundy=false;


  vgui_dialog dlg("Select index");
  dlg.field("point 1", image_xy1);
  dlg.field("point 2", image_xy2);
  dlg.field("point 3", image_xy3);
  dlg.field("point 4", image_xy4);
//  dlg.separator();

  dlg.checkbox("(right one H')",z1);

 // dlg.separator();
  
  dlg.checkbox("mundy's P?",mundy);
  //road_only_=false;
  dlg.checkbox("road_only?",road_only_);
    
   
  bool correct_choice=false;
  
  while (!correct_choice) {
    if (!dlg.ask())
      return;
    
    if (image_xy1>=0 && image_xy2>=0 && image_xy3>=0 && image_xy4>=0 && 
      image_xy1<25 && image_xy2<25 && image_xy3<25 && image_xy4<25 )
      correct_choice=true;
    else vcl_cout<<"choose again!"<<vcl_endl;
    
  }
  
   
//vnl_vector_fixed<double,4> p = svd()->solve(vnl_vector_fixed<double,3>(x.x(),x.y(),x.w()));

        post_=z1;
  i1_=image_xy1;
        i2_=image_xy2;
        i3_=image_xy3;
        i4_=image_xy4;
  //S_[image_xy1].x();
  vnl_double_4x3  A(0.0);
  A[0][0]=S_[i1_].x();A[0][1]=S_[i1_].y();A[0][2]=1.0;
  A[1][0]=S_[i2_].x();A[1][1]=S_[i2_].y();A[1][2]=1.0;
  A[2][0]=S_[i3_].x();A[2][1]=S_[i3_].y();A[2][2]=1.0;
  A[3][0]=S_[i4_].x();A[3][1]=S_[i4_].y();A[3][2]=1.0;

  //A[0][0]=S_[image_xy1].x();A[0][1]=S_[image_xy1].y();A[0][2]=1.0;
  //A[0][0]=S_[image_xy2].x();A[1][1]=S_[image_xy2].y();A[1][2]=1.0;
  //A[0][0]=S_[image_xy3].x();A[2][1]=S_[image_xy3].y();A[2][2]=1.0;
  //A[0][0]=S_[image_xy4].x();A[3][1]=S_[image_xy4].y();A[3][2]=1.0;



  vcl_cout <<"A matrix is: \n"<<A<<vcl_endl;
  
  //vnl_inverse(A);
  
//  vnl_vector <double> h0=vnl_inverse(A.transpose()*A)*A.transpose()*x_;
//  vnl_vector <double> h1=vnl_inverse(A.transpose()*A)*A.transpose()*y_;
//  vnl_vector <double> h2=vnl_inverse(A.transpose()*A)*A.transpose()*ones_;


  vcl_cout<<"x_:"<<"\n"<<x_<<vcl_endl;
  vcl_cout<<"y_:"<<"\n"<<y_<<vcl_endl;
  vcl_cout<<"ones_:"<<"\n"<<ones_<<vcl_endl;

  vnl_vector <double> h0=vnl_inverse(A.transpose()*A)*A.transpose()*x_;
  vnl_vector <double> h1=vnl_inverse(A.transpose()*A)*A.transpose()*y_;
  vnl_vector <double> h2=vnl_inverse(A.transpose()*A)*A.transpose()*ones_;

  vcl_cout<<h0<<"\n"<<h1<<"\n"<<h2<<vcl_endl;


  double Z;
  vnl_vector_fixed <double,4> Post;
  

        
  if (z1) { 
                Z=S_[14].z();
                Post[0]=S_[14].x();Post[1]=S_[14].y();Post[2]=S_[14].z();Post[3]=1.0;
  }
  else {
                Z=S_[16].z();
                Post[0]=S_[16].x();Post[1]=S_[16].y();Post[2]=S_[16].z();Post[3]=1.0;
  }


  
  //P_[1][2]=y_post_/Z;

  vcl_cout<<"post: "<<Post<<vcl_endl;

  P_[0][0]=h0[0];P_[0][1]=h0[1];P_[0][2]=0;P_[0][3]=h0[2]; 
  P_[1][0]=h1[0];P_[1][1]=h1[1];P_[1][2]=0;P_[1][3]=h1[2]; 
  P_[2][0]=h2[0];P_[2][1]=h2[1];P_[2][2]=0;P_[2][3]=h2[2]; 

  vcl_cout<<P_*Post<<"\n"<<(P_*Post)[1]<<"\n"<<vcl_endl;
  if (!road_only_) P_[1][2]=(y_post_-(P_*Post)[1])/Z;

  //P_[0][2]=(x_post_-(P_*Post)[0])/Z;

  
  vcl_cout<<"\n"<<P_<<vcl_endl;
  if (mundy) {
  P_[0][0]=2.2107;P_[0][1]=7.67681; P_[0][2]=0.0; P_[0][3]=23.3704;
  P_[1][0]=-0.872506; P_[1][1]=.394777; P_[1][2]=-8.20182; P_[1][3]=105.119; 
  P_[2][0]=-0.00355026; P_[2][1]=0.00194195; P_[2][2]=0.0; P_[2][3]=0.217277;

  }

        HomgPoint2D xh0(x_[0],y_[0],1.0);
        HomgPoint2D xh1(x_[0],y_[0],1.0);
        HomgPoint2D xh2(x_[0],y_[0],1.0);
        HomgPoint2D xh3(x_[0],y_[0],1.0);
        //vcl_vector <HomgPoint2D> HA;
        //
        vcl_vector<HomgPoint2D>  PA,PB;
        PA.push_back(xh0);PA.push_back(xh1);PA.push_back(xh2);PA.push_back(xh3);
        
        for (unsigned i=0;i<4;i++)
        {
                HomgPoint2D S1(A[i][0],A[i][1],1.0);            
                PB.push_back(S1);
                }
        
        
         vcl_vector<HomgPoint2D> four1_homg(4);
         vcl_vector<HomgPoint2D> four2_homg(4);
         for (int j = 0; j < 4; j++)
         {
                 
                 four1_homg[j] = HomgPoint2D(x_[j], y_[j], 1.0);
                 
                 
                 four2_homg[j] = HomgPoint2D(A[j][0],A[j][1], 1.0);
         }
         
         HMatrix2D H;
         
         
         //HMatrix2DCompute::compute_p(PA,PA,H);
         /*  HMatrix2DCompute4Point pp;
         if (!pp.compute(PA,PB,&H))
         vcl_cerr << "HMatrix2DCompute4Point - failure!\n";
         vcl_cout<<H<<vcl_endl;
         vnl_double_3x3 H1=H.get_matrix();
         vcl_cout<<H1<<vcl_endl;
         */     
         HMatrix2D Hs;
          HMatrix2DCompute4Point Computor;
                
    // Compute H with preconditioned points
                //  HMatrix2D* H_temp_homg = new HMatrix2D();
                //  HMatrix2D* H_temp = new HMatrix2D();
                
    if (!Computor.compute(four2_homg, four1_homg, &Hs))
      vcl_cerr << "HMatrix2DCompute4Point - failure!\n";
                
                vnl_double_3x3 H2=Hs.get_matrix();
                vcl_cout<<H2<<vcl_endl;
                // HMatrix2DComputeMLESAC Calc(std_);
                //      H_=Calc.compute(ptlist1_,ptlist2_);
                //      findplaneparams(H_,K1_,K2_,R_,t_);
                
                P_.fill(0.0);
                
                vcl_cout<<P_<<vcl_endl;
                
                for (unsigned i=0;i<3;i++)
                        for (unsigned j=0; j<2; j++) {
                                P_[i][j]=H2[i][j];
                        }
                        vcl_cout<<P_<<vcl_endl;
                        
                        for (unsigned i=0;i<3;i++)
                                P_[i][3]=H2[i][2];
                        
                        vcl_cout<<P_<<vcl_endl;
                        
                        if (!road_only_) {
                                //P_[1][2]=(y_post_-(P_*Post)[1])/Z;
                                P_[1][2]=(y_post_*(P_*Post)[2]-(P_*Post)[1])/Z;
                }
                

 vcl_cout<<P_<<vcl_endl;
 if (mundy) {
  P_[0][0]=2.2107;P_[0][1]=7.67681; P_[0][2]=0.0; P_[0][3]=23.3704;
  P_[1][0]=-0.872506; P_[1][1]=.394777; P_[1][2]=-8.20182; P_[1][3]=105.119; 
  P_[2][0]=-0.00355026; P_[2][1]=0.00194195; P_[2][2]=0.0; P_[2][3]=0.217277;

  }
 vcl_cout<<P_<<vcl_endl;
}

///////////////
// 9-21-2005

void bcri_calib::solve_reverse() 
{

  
  /*static int image_xy1=4;
  static int image_xy2=2;
  static int image_xy3=9;
  static int image_xy4=10;*/
        static int image_xy1=1;
  static int image_xy2=2;
  static int image_xy3=3;
  static int image_xy4=4;

        //static float image_xy1_x=0.0;
        //static float image_xy1_y=0.0;
        //static float image_xy1_z=0.0;
  
        //static float image_xy2_x=0.0;
        //static float image_xy2_y=0.0;
        //static float image_xy2_z=0.0;

        //static float image_xy3_x=0.0;
        //static float image_xy3_y=0.0;
        //static float image_xy3_z=0.0;

        //static float image_xy4_x=0.0;
        //static float image_xy4_y=0.0;
        //static float image_xy4_z=0.0;
        
        static bool z1=true;
  static bool mundy=false;


  vgui_dialog dlg("Select index");
  /*dlg.field("point 1x", image_xy1_x);
  dlg.field("point 1y", image_xy1_y);
  //dlg.field("point 1z", image_xy1_z);
  
  dlg.field("point 2x", image_xy2_x);
  dlg.field("point 2y", image_xy2_y);
  //dlg.field("point 2z", image_xy2_z);
    
  dlg.field("point 3x", image_xy3_x);
  dlg.field("point 3y", image_xy3_y);
  //dlg.field("point 3z", image_xy3_z);
      
  dlg.field("point 4x", image_xy4_x);
  dlg.field("point 4y", image_xy4_y);
  //dlg.field("point 4z", image_xy4_z);
   //  dlg.separator();
  */

  dlg.field("point 1", image_xy1);
  dlg.field("point 2", image_xy2);
  dlg.field("point 3", image_xy3);
  dlg.field("point 4", image_xy4);

  dlg.checkbox("(right one H')",z1);

 // dlg.separator();
  
 // dlg.checkbox("mundy's P?",mundy);
  //road_only_=false;
  dlg.checkbox("road_only?",road_only_);
    
   
  bool correct_choice=false;
  
  while (!correct_choice) {
    if (!dlg.ask())
      return;
    
    if (image_xy1>=0 && image_xy2>=0 && image_xy3>=0 && image_xy4>=0 && 
      image_xy1<25 && image_xy2<25 && image_xy3<25 && image_xy4<25 )
      correct_choice=true;
    else vcl_cout<<"choose again!"<<vcl_endl;
    
  }
  
   
//vnl_vector_fixed<double,4> p = svd()->solve(vnl_vector_fixed<double,3>(x.x(),x.y(),x.w()));

        post_=z1;
  i1_=image_xy1;
        i2_=image_xy2;
        i3_=image_xy3;
        i4_=image_xy4;
  //S_[image_xy1].x();
  vnl_double_4x3  A(0.0);
 // A[0][0]=image_xy1_x;A[0][1]=image_xy1_y;A[0][2]=1.0;
 // A[1][0]=image_xy2_x;A[1][1]=image_xy2_y;A[1][2]=1.0;
 // A[2][0]=image_xy3_x;A[2][1]=image_xy3_y;A[2][2]=1.0;
 // A[3][0]=image_xy4_x;A[3][1]=image_xy4_y;A[3][2]=1.0;

  A[0][0]=S_[i1_].x();A[0][1]=S_[i1_].y();A[0][2]=1.0;
  A[1][0]=S_[i2_].x();A[1][1]=S_[i2_].y();A[1][2]=1.0;
  A[2][0]=S_[i3_].x();A[2][1]=S_[i3_].y();A[2][2]=1.0;
  A[3][0]=S_[i4_].x();A[3][1]=S_[i4_].y();A[3][2]=1.0;

  //A[0][0]=S_[image_xy1].x();A[0][1]=S_[image_xy1].y();A[0][2]=1.0;
  //A[0][0]=S_[image_xy2].x();A[1][1]=S_[image_xy2].y();A[1][2]=1.0;
  //A[0][0]=S_[image_xy3].x();A[2][1]=S_[image_xy3].y();A[2][2]=1.0;
  //A[0][0]=S_[image_xy4].x();A[3][1]=S_[image_xy4].y();A[3][2]=1.0;



  vcl_cout <<"A matrix is: \n"<<A<<vcl_endl;
  
  //vnl_inverse(A);
  
//  vnl_vector <double> h0=vnl_inverse(A.transpose()*A)*A.transpose()*x_;
//  vnl_vector <double> h1=vnl_inverse(A.transpose()*A)*A.transpose()*y_;
//  vnl_vector <double> h2=vnl_inverse(A.transpose()*A)*A.transpose()*ones_;


  vcl_cout<<"x_:"<<"\n"<<x_<<vcl_endl;
  vcl_cout<<"y_:"<<"\n"<<y_<<vcl_endl;
  vcl_cout<<"ones_:"<<"\n"<<ones_<<vcl_endl;

  vnl_vector <double> h0=vnl_inverse(A.transpose()*A)*A.transpose()*x_;
  vnl_vector <double> h1=vnl_inverse(A.transpose()*A)*A.transpose()*y_;
  vnl_vector <double> h2=vnl_inverse(A.transpose()*A)*A.transpose()*ones_;

  vcl_cout<<h0<<"\n"<<h1<<"\n"<<h2<<vcl_endl;


  double Z;
  vnl_vector_fixed <double,4> Post;
  

        
  if (z1) { 
                Z=S_[14].z();
                Z=10.0;
                Post[0]=S_[14].x();Post[1]=S_[14].y();Post[2]=S_[14].z();Post[3]=1.0;
  }
  else {
                Z=S_[16].z();
                Post[0]=S_[16].x();Post[1]=S_[16].y();Post[2]=S_[16].z();Post[3]=1.0;
  }


  
  //P_[1][2]=y_post_/Z;

  vcl_cout<<"post: "<<Post<<vcl_endl;

  P_[0][0]=h0[0];P_[0][1]=h0[1];P_[0][2]=0;P_[0][3]=h0[2]; 
  P_[1][0]=h1[0];P_[1][1]=h1[1];P_[1][2]=0;P_[1][3]=h1[2]; 
  P_[2][0]=h2[0];P_[2][1]=h2[1];P_[2][2]=0;P_[2][3]=h2[2]; 

  vcl_cout<<P_*Post<<"\n"<<(P_*Post)[1]<<"\n"<<vcl_endl;
  if (!road_only_) P_[1][2]=(y_post_-(P_*Post)[1])/Z;

  //P_[0][2]=(x_post_-(P_*Post)[0])/Z;

  
  vcl_cout<<"\n"<<P_<<vcl_endl;
  if (mundy) {
  P_[0][0]=2.2107;P_[0][1]=7.67681; P_[0][2]=0.0; P_[0][3]=23.3704;
  P_[1][0]=-0.872506; P_[1][1]=.394777; P_[1][2]=-8.20182; P_[1][3]=105.119; 
  P_[2][0]=-0.00355026; P_[2][1]=0.00194195; P_[2][2]=0.0; P_[2][3]=0.217277;

  }

        HomgPoint2D xh0(x_[0],y_[0],1.0);
        HomgPoint2D xh1(x_[0],y_[0],1.0);
        HomgPoint2D xh2(x_[0],y_[0],1.0);
        HomgPoint2D xh3(x_[0],y_[0],1.0);
        //vcl_vector <HomgPoint2D> HA;
        //
        vcl_vector<HomgPoint2D>  PA,PB;
        PA.push_back(xh0);PA.push_back(xh1);PA.push_back(xh2);PA.push_back(xh3);
        
        for (unsigned i=0;i<4;i++)
        {
                HomgPoint2D S1(A[i][0],A[i][1],1.0);            
                PB.push_back(S1);
                }
        
        
         vcl_vector<HomgPoint2D> four1_homg(4);
         vcl_vector<HomgPoint2D> four2_homg(4);
         for (int j = 0; j < 4; j++)
         {
                 
                 four1_homg[j] = HomgPoint2D(x_[j], y_[j], 1.0);
                 
                 
                 four2_homg[j] = HomgPoint2D(A[j][0],A[j][1], 1.0);
         }
         
         HMatrix2D H;
         
         
         //HMatrix2DCompute::compute_p(PA,PA,H);
         /*  HMatrix2DCompute4Point pp;
         if (!pp.compute(PA,PB,&H))
         vcl_cerr << "HMatrix2DCompute4Point - failure!\n";
         vcl_cout<<H<<vcl_endl;
         vnl_double_3x3 H1=H.get_matrix();
         vcl_cout<<H1<<vcl_endl;
         */     
         HMatrix2D Hs;
          HMatrix2DCompute4Point Computor;
                
    // Compute H with preconditioned points
                //  HMatrix2D* H_temp_homg = new HMatrix2D();
                //  HMatrix2D* H_temp = new HMatrix2D();
                
    if (!Computor.compute(four2_homg, four1_homg, &Hs))
      vcl_cerr << "HMatrix2DCompute4Point - failure!\n";
                
                vnl_double_3x3 H2=Hs.get_matrix();
                vcl_cout<<H2<<vcl_endl;
                // HMatrix2DComputeMLESAC Calc(std_);
                //      H_=Calc.compute(ptlist1_,ptlist2_);
                //      findplaneparams(H_,K1_,K2_,R_,t_);
                
                P_.fill(0.0);
                
                vcl_cout<<P_<<vcl_endl;
                
                for (unsigned i=0;i<3;i++)
                        for (unsigned j=0; j<2; j++) {
                                P_[i][j]=H2[i][j];
                        }
                        vcl_cout<<P_<<vcl_endl;
                        
                        for (unsigned i=0;i<3;i++)
                                P_[i][3]=H2[i][2];
                        
                        vcl_cout<<P_<<vcl_endl;
                        
                        if (!road_only_) {
                                //P_[1][2]=(y_post_-(P_*Post)[1])/Z;
                                P_[1][2]=(y_post_*(P_*Post)[2]-(P_*Post)[1])/Z;
                }
                

 vcl_cout<<P_<<vcl_endl;
 if (mundy) {
  P_[0][0]=2.2107;P_[0][1]=7.67681; P_[0][2]=0.0; P_[0][3]=23.3704;
  P_[1][0]=-0.872506; P_[1][1]=.394777; P_[1][2]=-8.20182; P_[1][3]=105.119; 
  P_[2][0]=-0.00355026; P_[2][1]=0.00194195; P_[2][2]=0.0; P_[2][3]=0.217277;

  }
 vcl_cout<<P_<<vcl_endl;
}



//////////////// 5-19-2005

void bcri_calib::solve_robust() 
{
        
        vcl_vector<vgl_homg_point_2d<double> > points1;
        
        vcl_vector<vgl_homg_point_2d<double> > points2;
        
        
        
        for (unsigned i=0;i<S_.size();i++) {    

                vgl_homg_point_2d<double> point1(xall_[i],yall_[i],0.0);
    points1.push_back(point1);

                vgl_homg_point_2d<double> point2(S_[i].x(),S_[i].y(),0.0);
                points2.push_back(point2);
                
                }
        

}


/////////////// 5-19-2005

void bcri_calib::select_pts1_pts2()
{
        
         //bool point_flag[26];
          bool image_xy[27];
         for (unsigned i=0;i<27;i++) image_xy[i]=true;
         /*static bool image_xy0 =true;
         static bool image_xy1 =true;
         static bool image_xy2 =true;
         static bool image_xy3 =true;
         static bool image_xy4 =true;
         static bool image_xy5 =true;
         static bool image_xy6 =true;
         static bool image_xy7 =true;
         static bool image_xy8 =true;

         static bool image_xy9 =true;
         static bool image_xy10 =true;
         static bool image_xy11 =true;
         static bool image_xy12 =true;
         static bool image_xy13 =true;
         static bool image_xy14 =true;
         static bool image_xy15 =true;
         static bool image_xy16 =true;
         static bool image_xy17 =true;
         
         static bool image_xy18 =true;
         static bool image_xy19 =true;
         static bool image_xy20 =true;
         static bool image_xy21 =true;
         static bool image_xy22 =true;
         static bool image_xy23=true;
         static bool image_xy24 =true;
         static bool image_xy25 =true;
         static bool image_xy26 =true;*/
         
         vgui_dialog dlg("Select index");
         dlg.checkbox("point 0", image_xy[0]);
         dlg.checkbox("point 1", image_xy[1]);
         dlg.checkbox("point 2", image_xy[2]);
         dlg.checkbox("point 3", image_xy[3]);
         dlg.checkbox("point 4", image_xy[4]);

         dlg.checkbox("point 5", image_xy[5]);
         dlg.checkbox("point 6", image_xy[6]);
         dlg.checkbox("point 7", image_xy[7]);
         dlg.checkbox("point 8", image_xy[8]);
         dlg.checkbox("point 9", image_xy[9]);

         dlg.checkbox("point10", image_xy[10]);
         dlg.checkbox("point11", image_xy[11]);
         dlg.checkbox("point12", image_xy[12]);
         dlg.checkbox("point13", image_xy[13]);
         dlg.checkbox("point14", image_xy[14]);

         dlg.checkbox("point15", image_xy[15]);
         dlg.checkbox("point16", image_xy[16]);
         dlg.checkbox("point17", image_xy[17]);
         dlg.checkbox("point18", image_xy[18]);
         dlg.checkbox("point19", image_xy[19]);
         
         dlg.checkbox("point20", image_xy[20]);
         dlg.checkbox("point21", image_xy[21]);
         dlg.checkbox("point22", image_xy[22]);
         dlg.checkbox("point23", image_xy[23]);
         dlg.checkbox("point24", image_xy[24]);

         dlg.checkbox("point25", image_xy[25]);
         dlg.checkbox("point26", image_xy[26]);

   clear_display();
   vgui_style_sptr yellow_dots = vgui_style::new_style(1,1,0.0,8, 1);
   vgui_style_sptr probe_candidate = vgui_style::new_style(.8,.4,.2,8, 1);
   vgui_style_sptr probe_best = vgui_style::new_style(.1,.1,.8,8, 1);
   vgui_style_sptr box_corner = vgui_style::new_style(1,1,0.7,4,1);

         
         
         if (!dlg.ask())
                 return;
            float x1=0, y1=0; // x2=0, y2=0;
   vgl_homg_point_2d<double> mp_left,mp_right;

         
         for (unsigned i=0;i<27;i++) {
                 if (image_xy[i]) {
        
                         S_selected_.push_back(S_[i]);
                         
                         for (int i=0;i<26;i++) {
                                 vcl_cout<<"# "<<i<<" :";
                                 vcl_cout<<"pick :";
                                 tabs_picker_[0]->pick_point(&x1, &y1);
                                 vcl_cout<<x1<<" "<<y1<<vcl_endl;
                                 
                                 
                                 if (i<25) {
                                         xall_.push_back(x1);
                                         yall_.push_back(y1);
                                         //yall_[i]=y1;
                                         oneall_.push_back(1.0);
                                         vtol_vertex_2d_sptr v1=
                                                 new vtol_vertex_2d(xall_[i],yall_[i]);
                                         
                                         vtol_tabs_[0]->add_vertex(v1,yellow_dots);
                                 }
                                 if (i==25) {//post
                                         x_post_=x1;
                                         y_post_=y1;
                                         vtol_vertex_2d_sptr v1=
                                                 new vtol_vertex_2d(x_post_,y_post_);
                                         
                                         vtol_tabs_[0]->add_vertex(v1,probe_best);
                                 }
                         }
         }//for (unsigned i=0;i<27;i++) { ==> }
         }//if (image_xy[i]) { ==> }
                 
         
                 
         
   this->post_redraw();
   
}


void bcri_calib::solve_quiet() 
{
        
        
  vnl_double_4x3  A(0.0);
  A[0][0]=S_[i1_].x();A[0][1]=S_[i1_].y();A[0][2]=1.0;
  A[1][0]=S_[i2_].x();A[1][1]=S_[i2_].y();A[1][2]=1.0;
  A[2][0]=S_[i3_].x();A[2][1]=S_[i3_].y();A[2][2]=1.0;
  A[3][0]=S_[i4_].x();A[3][1]=S_[i4_].y();A[3][2]=1.0;
        
        

  vcl_cout <<"A matrix is: \n"<<A<<vcl_endl;
  


  vcl_cout<<"x_:"<<"\n"<<x_<<vcl_endl;
  vcl_cout<<"y_:"<<"\n"<<y_<<vcl_endl;
  vcl_cout<<"ones_:"<<"\n"<<ones_<<vcl_endl;

  vnl_vector <double> h0=vnl_inverse(A.transpose()*A)*A.transpose()*x_;
  vnl_vector <double> h1=vnl_inverse(A.transpose()*A)*A.transpose()*y_;
  vnl_vector <double> h2=vnl_inverse(A.transpose()*A)*A.transpose()*ones_;

  vcl_cout<<h0<<"\n"<<h1<<"\n"<<h2<<vcl_endl;


  double Z;
  vnl_vector_fixed <double,4> Post;

  bool z1=post_;
  if (z1) Z=S_[14].z();
  else Z=S_[16].z();

  if (z1) {
    Post[0]=S_[14].x();Post[1]=S_[14].y();Post[2]=S_[14].z();Post[3]=1.0;i5_=14;
  }
  else {
    Post[0]=S_[16].x();Post[1]=S_[16].y();Post[2]=S_[16].z();Post[3]=1.0;i5_=16;
  }


  
  //P_[1][2]=y_post_/Z;

  vcl_cout<<"post: "<<Post<<vcl_endl;

  P_[0][0]=h0[0];P_[0][1]=h0[1];P_[0][2]=0;P_[0][3]=h0[2]; 
  P_[1][0]=h1[0];P_[1][1]=h1[1];P_[1][2]=0;P_[1][3]=h1[2]; 
  //P_[2][0]=h2[0];P_[2][1]=h2[1];P_[2][2]=0;P_[2][3]=h2[2]; 
        P_[2][0]=0;P_[2][1]=0;P_[2][2]=0;P_[2][3]=1 ;

  vcl_cout<<P_*Post<<"\n"<<(P_*Post)[1]<<"\n"<<vcl_endl;
  if (!road_only_) P_[1][2]=(y_post_-(P_*Post)[1])/Z;

  //P_[0][2]=(x_post_-(P_*Post)[0])/Z;

  
  vcl_cout<<"\n"<<P_<<vcl_endl;
 //if (mundy) {
 // P_[0][0]=2.2107;P_[0][1]=7.67681; P_[0][2]=0.0; P_[0][3]=23.3704;
 // P_[1][0]=-0.872506; P_[1][1]=.394777; P_[1][2]=-8.20182; P_[1][3]=105.119; 
 // P_[2][0]=-0.00355026; P_[2][1]=0.00194195; P_[2][2]=0.0; P_[2][3]=0.217277;

 // }

         vcl_vector<HomgPoint2D> four1_homg(4);
    vcl_vector<HomgPoint2D> four2_homg(4);
    for (int j = 0; j < 4; j++)
    {
      
      four1_homg[j] = HomgPoint2D(x_[j], y_[j], 1.0);
      
      
      four2_homg[j] = HomgPoint2D(A[j][0],A[j][1], 1.0);
    }
        
                HMatrix2D Hs;
          HMatrix2DCompute4Point Computor;

  
    if (!Computor.compute(four2_homg, four1_homg, &Hs))
      vcl_cerr << "HMatrix2DCompute4Point - failure!\n";

                 vnl_double_3x3 H2=Hs.get_matrix();
         vcl_cout<<H2<<vcl_endl;
        
         P_.fill(0.0);
         for (unsigned i=0;i<3;i++)
                 for (unsigned j=0; j<2; j++) {
                         P_[i][j]=H2[i][j];
                 }
        for (unsigned i=0;i<3;i++)
                P_[i][3]=H2[i][2];
        if (!road_only_) 
        { 
                P_[1][2]=(y_post_*(P_*Post)[2]-(P_*Post)[1])/Z;
                //P_[1][2]/=(y_post_-(P_*Post)[2]);
        }
                
                

 vcl_cout<<P_<<vcl_endl;
}


//void bcri_calib::find_closest() {}
void bcri_calib::change() 
{
clear_display();
   vgui_style_sptr yellow_dots = vgui_style::new_style(1,.5,1,6, 1);
   vgui_style_sptr probe_candidate = vgui_style::new_style(.8,.4,.2,8, 1);
   vgui_style_sptr probe_best = vgui_style::new_style(.1,.1,.8,8, 1);
   vgui_style_sptr box_corner = vgui_style::new_style(1,1,0.7,4,1);
         
         
   //bgui_picker_tableau_sptr ptab = this->get_selected_picker_tableau();
   float x1=0, y1=0; // x2=0, y2=0;
   vgl_homg_point_2d<double> mp_left,mp_right;
   //l_;
   vcl_vector<vgl_homg_line_2d<double> > lines;
         
         
         vcl_cout<<"pick"<<vcl_endl;
         tabs_picker_[0]->pick_point(&x1, &y1);
   vcl_cout<<x1<<" "<<y1<<vcl_endl;
         int i_sel=0;
         float min=10000.0;
   for (int i=0;i<4;i++) {
                 
                 //vcl_cout<<"# "<<i<<" :";
                 //vcl_cout<<"pick :";
                 //tabs_picker_[0]->pick_point(&x1, &y1);
                 //vcl_cout<<x1<<" "<<y1<<vcl_endl;
                
                 
                 float d;
                 d=(x_[i]-x1)*(x_[i]-x1)+(y_[i]-y1)*(y_[i]-y1);
                 vcl_cout<<"i&d: "<<i<<" "<<d<<vcl_endl;
                 if (d<min) {
                         min=d; i_sel=i;
                 }
                 
                 //ones_[i]=1.0;
                 
                 
         }
         x_[i_sel]=x1;
         y_[i_sel]=y1;

         vtol_vertex_2d_sptr v1=
                 new vtol_vertex_2d(x1,y1);
                 //clear_display();
                 vtol_tabs_[0]->add_vertex(v1,yellow_dots);
                 this->post_redraw();
                 
}

void bcri_calib::solve_all() 
{

  /*
  static int image_xy0=0;
  static int image_xy1=1;
  static int image_xy2=2;
  static int image_xy3=3;
        static int image_xy4=4;
        static int image_xy5=5;
        static int image_xy6=6;
        static int image_xy7=7;
        static int image_xy8=8;
        static int image_xy9=9;
        static int image_xy10=10;
        static int image_xy11=11;
        static int image_xy12=12;
        static int image_xy13=13;
        static int image_xy14=14;
        static int image_xy15=15;
        static int image_xy16=16;
        static int image_xy17=17;
        static int image_xy18=18;
        static int image_xy19=19;
        static int image_xy20=20;
        static int image_xy21=21;
        static int image_xy22=22;
        static int image_xy23=23;
        static int image_xy24=24;
*/
        static bool image_xy0=true;
        static bool image_xy1=true;
        static bool image_xy2=true;
        static bool image_xy3=true;
        static bool image_xy4=true;
        static bool image_xy5=true;
        static bool image_xy6=true;
        static bool image_xy7=true;
        static bool image_xy8=true;
        static bool image_xy9=true;
        static bool image_xy10=true;
        static bool image_xy11=true;
        static bool image_xy12=false;
        static bool image_xy13=true;
        static bool image_xy14=false;
        static bool image_xy15=true;
        static bool image_xy16=false;
        static bool image_xy17=false;
        static bool image_xy18=true;
        static bool image_xy19=true;
        static bool image_xy20=true;
        static bool image_xy21=true;
        static bool image_xy22=true;
        static bool image_xy23=true;
        static bool image_xy24=true;

  static bool z1=true;
  static bool mundy=false;


  vgui_dialog dlg("Select index");
        dlg.checkbox("p0", image_xy0);
  dlg.checkbox("p1", image_xy1);
  dlg.checkbox("p2", image_xy2);
  dlg.checkbox("p3", image_xy3);
  dlg.checkbox("p4", image_xy4);
        dlg.checkbox("p5", image_xy5);
        dlg.checkbox("p6", image_xy6);
        dlg.checkbox("p7", image_xy7);
        dlg.checkbox("p8", image_xy8);
        dlg.checkbox("p9", image_xy9);
        dlg.checkbox("p10", image_xy10);
        dlg.checkbox("p11", image_xy11);
        dlg.checkbox("p12", image_xy12);
        dlg.checkbox("p13", image_xy13);
        dlg.checkbox("p14", image_xy14);
        dlg.checkbox("p15", image_xy15);
        dlg.checkbox("p16", image_xy16);
        dlg.checkbox("p17", image_xy17);
        dlg.checkbox("p18", image_xy18);
        dlg.checkbox("p19", image_xy19);
        dlg.checkbox("p20", image_xy20);
        dlg.checkbox("p21", image_xy21);
        dlg.checkbox("p22", image_xy22);
        dlg.checkbox("p23", image_xy23);
        dlg.checkbox("p24", image_xy24);
        


  dlg.checkbox("(right one H')",z1);

 // dlg.separator();
  
  dlg.checkbox("mundy's P?",mundy);
  //dlg.field
    //dlg.checkbox("use front estimation? ",use_front_estimated);
    
   
  //bool correct_choice=false;
  if (!dlg.ask())
      return;
  
  
   
//vnl_vector_fixed<double,4> p = svd()->solve(vnl_vector_fixed<double,3>(x.x(),x.y(),x.w()));
  

  S_[image_xy1].x();
  vnl_matrix <double> A(20,3,0.0);
  /*A[0][0]=S_[image_xy0].x();A[0][1]=S_[image_xy0].y();A[0][2]=1.0;
  A[1][0]=S_[image_xy1].x();A[1][1]=S_[image_xy1].y();A[1][2]=1.0;
  A[2][0]=S_[image_xy2].x();A[2][1]=S_[image_xy2].y();A[2][2]=1.0;
  A[3][0]=S_[image_xy3].x();A[3][1]=S_[image_xy3].y();A[3][2]=1.0;
        A[4][0]=S_[image_xy4].x();A[4][1]=S_[image_xy4].y();A[4][2]=1.0;
        A[5][0]=S_[image_xy5].x();A[5][1]=S_[image_xy5].y();A[5][2]=1.0;
        A[6][0]=S_[image_xy6].x();A[6][1]=S_[image_xy6].y();A[6][2]=1.0;
        A[7][0]=S_[image_xy7].x();A[7][1]=S_[image_xy7].y();A[7][2]=1.0;
        A[8][0]=S_[image_xy8].x();A[8][1]=S_[image_xy8].y();A[8][2]=1.0;
        A[9][0]=S_[image_xy9].x();A[9][1]=S_[image_xy9].y();A[9][2]=1.0;
        A[10][0]=S_[image_xy10].x();A[10][1]=S_[image_xy10].y();A[10][2]=1.0;
        A[11][0]=S_[image_xy11].x();A[11][1]=S_[image_xy11].y();A[11][2]=1.0;
        A[12][0]=S_[image_xy12].x();A[12][1]=S_[image_xy12].y();A[12][2]=1.0;
        A[13][0]=S_[image_xy13].x();A[13][1]=S_[image_xy13].y();A[13][2]=1.0;
        A[14][0]=S_[image_xy14].x();A[14][1]=S_[image_xy14].y();A[14][2]=1.0;
        A[15][0]=S_[image_xy15].x();A[15][1]=S_[image_xy15].y();A[15][2]=1.0;
        A[16][0]=S_[image_xy16].x();A[16][1]=S_[image_xy16].y();A[16][2]=1.0;
        A[17][0]=S_[image_xy17].x();A[17][1]=S_[image_xy17].y();A[17][2]=1.0;
        A[18][0]=S_[image_xy18].x();A[18][1]=S_[image_xy18].y();A[18][2]=1.0;
        A[19][0]=S_[image_xy19].x();A[19][1]=S_[image_xy19].y();A[19][2]=1.0;
        A[20][0]=S_[image_xy20].x();A[20][1]=S_[image_xy20].y();A[20][2]=1.0;
        A[21][0]=S_[image_xy21].x();A[21][1]=S_[image_xy21].y();A[21][2]=1.0;
        A[22][0]=S_[image_xy22].x();A[22][1]=S_[image_xy22].y();A[22][2]=1.0;
        A[23][0]=S_[image_xy23].x();A[23][1]=S_[image_xy23].y();A[23][2]=1.0;
        A[24][0]=S_[image_xy24].x();A[24][1]=S_[image_xy24].y();A[24][2]=1.0;*/

        if (image_xy0 ) {A[0][0]= S_[0].x(); A[0][1]= S_[0].y();A[0][2]=1.0;}    else {xall_[0]=0.0;yall_[0]=0.0;}
  if (image_xy1 ) {A[1][0]= S_[1].x(); A[1][1]= S_[1].y();A[1][2]=1.0;}  else {xall_[1]=0.0;yall_[1]=0.0;}
        if (image_xy2 ) {A[2][0]= S_[2].x(); A[2][1]= S_[2].y();A[2][2]=1.0;}    else {xall_[2]=0.0;yall_[2]=0.0;}
  if (image_xy3 ) {A[3][0]= S_[3].x(); A[3][1]= S_[3].y();A[3][2]=1.0;}  else {xall_[3]=0.0;yall_[3]=0.0;}
        if (image_xy4 ) {A[4][0]= S_[4].x(); A[4][1]= S_[4].y();A[4][2]=1.0;}    else {xall_[4]=0.0;yall_[4]=0.0;}
        if (image_xy5 ) {A[5][0]= S_[5].x(); A[5][1]= S_[5].y();A[5][2]=1.0;}    else {xall_[5]=0.0;yall_[5]=0.0;}
        if (image_xy6 ) {A[6][0]= S_[6].x(); A[6][1]= S_[6].y();A[6][2]=1.0;}  else {xall_[6]=0.0;yall_[6]=0.0;}
        if (image_xy7 ) {A[7][0]= S_[7].x(); A[7][1]= S_[7].y();A[7][2]=1.0;}  else {xall_[7]=0.0;yall_[7]=0.0;}
        if (image_xy8 ) {A[8][0]= S_[8].x(); A[8][1]= S_[8].y();A[8][2]=1.0;}  else {xall_[8]=0.0;yall_[8]=0.0;}
        if (image_xy9 ) {A[9][0]= S_[9].x(); A[9][1]= S_[9].y();A[9][2]=1.0;}  else {xall_[9]=0.0;yall_[9]=0.0;}
        if (image_xy10) {A[10][0]=S_[10].x();A[10][1]=S_[10].y();A[10][2]=1.0;}else {xall_[10]=0.0;yall_[10]=0.0;}
        if (image_xy11) {A[11][0]=S_[11].x();A[11][1]=S_[11].y();A[11][2]=1.0;}else {xall_[11]=0.0;yall_[11]=0.0;}
        if (image_xy12) {A[12][0]=S_[12].x();A[12][1]=S_[12].y();A[12][2]=1.0;}else {xall_[12]=0.0;yall_[12]=0.0;}
        if (image_xy13) {A[13][0]=S_[13].x();A[13][1]=S_[13].y();A[13][2]=1.0;}else {xall_[13]=0.0;yall_[13]=0.0;}
        if (image_xy14) {A[14][0]=S_[14].x();A[14][1]=S_[14].y();A[14][2]=1.0;}else {xall_[14]=0.0;yall_[14]=0.0;}
        if (image_xy15) {A[15][0]=S_[15].x();A[15][1]=S_[15].y();A[15][2]=1.0;}else {xall_[15]=0.0;yall_[15]=0.0;}
        if (image_xy16) {A[16][0]=S_[16].x();A[16][1]=S_[16].y();A[16][2]=1.0;}else {xall_[16]=0.0;yall_[16]=0.0;}
        if (image_xy17) {A[17][0]=S_[17].x();A[17][1]=S_[17].y();A[17][2]=1.0;}else {xall_[17]=0.0;yall_[17]=0.0;}
        if (image_xy18) {A[18][0]=S_[18].x();A[18][1]=S_[18].y();A[18][2]=1.0;}else {xall_[18]=0.0;yall_[18]=0.0;}
        if (image_xy19) {A[19][0]=S_[19].x();A[19][1]=S_[19].y();A[19][2]=1.0;}else {xall_[19]=0.0;yall_[19]=0.0;}
        if (image_xy20) {A[20][0]=S_[20].x();A[20][1]=S_[20].y();A[20][2]=1.0;}else {xall_[20]=0.0;yall_[20]=0.0;}
        if (image_xy21) {A[21][0]=S_[21].x();A[21][1]=S_[21].y();A[21][2]=1.0;}else {xall_[21]=0.0;yall_[21]=0.0;}
        if (image_xy22) {A[22][0]=S_[22].x();A[22][1]=S_[22].y();A[22][2]=1.0;}else {xall_[22]=0.0;yall_[22]=0.0;}
        if (image_xy23) {A[23][0]=S_[23].x();A[23][1]=S_[23].y();A[23][2]=1.0;}else {xall_[23]=0.0;yall_[23]=0.0;}
        if (image_xy24) {A[24][0]=S_[24].x();A[24][1]=S_[24].y();A[24][2]=1.0;}else {xall_[24]=0.0;yall_[24]=0.0;}

        

  //A[0][0]=S_[image_xy1].x();A[0][1]=S_[image_xy1].y();A[0][2]=1.0;
  //A[0][0]=S_[image_xy2].x();A[1][1]=S_[image_xy2].y();A[1][2]=1.0;
  //A[0][0]=S_[image_xy3].x();A[2][1]=S_[image_xy3].y();A[2][2]=1.0;
  //A[0][0]=S_[image_xy4].x();A[3][1]=S_[image_xy4].y();A[3][2]=1.0;



  vcl_cout <<"A matrix is: \n"<<A<<vcl_endl;
  
  //vnl_inverse(A);
  
//  vnl_vector <double> h0=vnl_inverse(A.transpose()*A)*A.transpose()*x_;
//  vnl_vector <double> h1=vnl_inverse(A.transpose()*A)*A.transpose()*y_;
//  vnl_vector <double> h2=vnl_inverse(A.transpose()*A)*A.transpose()*ones_;


  vcl_cout<<"x_:"<<"\n"<<x_<<vcl_endl;
  vcl_cout<<"y_:"<<"\n"<<y_<<vcl_endl;
  vcl_cout<<"ones_:"<<"\n"<<ones_<<vcl_endl;

  vnl_vector <double> h0=vnl_inverse(A.transpose()*A)*A.transpose()*x_;
  vnl_vector <double> h1=vnl_inverse(A.transpose()*A)*A.transpose()*y_;
  vnl_vector <double> h2=vnl_inverse(A.transpose()*A)*A.transpose()*ones_;

  vcl_cout<<h0<<"\n"<<h1<<"\n"<<h2<<vcl_endl;


  double Z;
  vnl_vector_fixed <double,4> Post;
  
  if (z1) Z=S_[14].z();
  else Z=S_[16].z();

  if (z1) {
    Post[0]=S_[14].x();Post[1]=S_[14].y();Post[2]=S_[14].z();Post[3]=1.0;
  }
  else {
    Post[0]=S_[16].x();Post[1]=S_[16].y();Post[2]=S_[16].z();Post[3]=1.0;
  }


  
  //P_[1][2]=y_post_/Z;

  //vcl_cout<<"post: "<<Post<<vcl_endl;

  P_[0][0]=h0[0];P_[0][1]=h0[1];P_[0][2]=0;P_[0][3]=h0[2]; 
  P_[1][0]=h1[0];P_[1][1]=h1[1];P_[1][2]=0;P_[1][3]=h1[2]; 
  P_[2][0]=h2[0];P_[2][1]=h2[1];P_[2][2]=0;P_[2][3]=h2[2]; 

  //vcl_cout<<P_*Post<<"\n"<<(P_*Post)[1]<<"\n"<<vcl_endl;
  //P_[1][2]=(y_post_-(P_*Post)[1])/Z;

  //P_[0][2]=(x_post_-(P_*Post)[0])/Z;

  
  vcl_cout<<"\n"<<P_<<vcl_endl;
  if (mundy) {
  P_[0][0]=2.2107;P_[0][1]=7.67681; P_[0][2]=0.0; P_[0][3]=23.3704;
  P_[1][0]=-0.872506; P_[1][1]=.394777; P_[1][2]=-8.20182; P_[1][3]=105.119; 
  P_[2][0]=-0.00355026; P_[2][1]=0.00194195; P_[2][2]=0.0; P_[2][3]=0.217277;



  }



}

////////////back projection of calibration point.....

void bcri_calib::back_project() {
        //anat's camera test
        if (0) {
                P_[0][0]= -19.2353 ;P_[0][1]=-31.7814;P_[0][2]= 0;P_[0][3]= 352.65;
    P_[1][0]=-10.3124;  P_[1][1]= -10.196;P_[1][2]= 11.2076;P_[1][3]= 68.395;
    P_[2][0]=-0.00644916;P_[2][1]= -0.0194706;P_[2][2]= 0;P_[2][3]= -0.196178;


                }

        if (0) {
                P_[0][0]=-6.00122;P_[0][1]= -14.6822;P_[0][2]= 0;P_[0][3]= 49.1228;
    P_[1][0]=-1.2737; P_[1][1]=-3.41346; P_[1][2]=15.8224 ;P_[1][3]=-103.207;
    P_[2][0]=0.0024448;P_[2][1]= -0.0076778;P_[2][2]= 0;P_[2][3]= -0.300161;

                }

        if (0) {
P_[0][0]=6.55584; P_[0][1]=-15.1614; P_[0][2]=0 ;P_[0][3]=40.929;
P_[1][0]= 0.223694;P_[1][1]= -1.13482;P_[1][2]= 14.3894;P_[1][3]= -180.04;
P_[2][0]= 0.00431944;P_[2][1]= -0.00534274; P_[2][2]=0; P_[2][3]=-0.4032;


                }
  vgui_style_sptr red_dots = vgui_style::new_style(1,0.2,0.0,7, 1);
        vgui_style_sptr y_dots = vgui_style::new_style(1,1.0,0.0,8, 1);
  for (unsigned i=0;i<25;i++) {
    //vcl_cout<<S_[i]<<vcl_endl;
    //vgl_point_2d<double> p = brct_algos::projection_3d_point( S_[i],P_);
    vnl_double_4 X;
    X[0]=S_[i].x();
    X[1]=S_[i].y();
    X[2]=S_[i].z();
    X[3]=1.0;
    //vnl_double_2 p = brct_algos::projection_3d_point( P_,X);
    //t0[0][i] = p.x();
    //t0[1][i] = p.y();  
    vnl_double_3 p = P_*X;

    //vcl_cout<<i<<": "<<p<<" "<<p[0]/p[2] <<" "<<p[1]/p[2] <<vcl_endl;
    vtol_vertex_2d_sptr v1=
      new vtol_vertex_2d(p[0]/p[2],p[1]/p[2]);
    
    vtol_tabs_[0]->add_vertex(v1,red_dots);
  }

        for (unsigned i=0;i<4;i++)  {
                vtol_vertex_2d_sptr v1=
      new vtol_vertex_2d(x_[i],y_[i]);
    
    vtol_tabs_[0]->add_vertex(v1,y_dots);
                
        }

                vtol_vertex_2d_sptr v1=
      new vtol_vertex_2d(x_post_,y_post_);
    
    vtol_tabs_[0]->add_vertex(v1,y_dots);

        vtol_tabs_[0]->add_line(x_[0],y_[0],x_[1],y_[1]);
        vtol_tabs_[0]->add_line(x_[1],y_[1],x_[2],y_[2]);
        vtol_tabs_[0]->add_line(x_[2],y_[2],x_[3],y_[3]);
        vtol_tabs_[0]->add_line(x_[3],y_[3],x_[0],y_[0]);
  this->post_redraw();

}

///////
void bcri_calib::back_project_reverse() {
        //anat's camera test
        if (0) {
                P_[0][0]= -19.2353 ;P_[0][1]=-31.7814;P_[0][2]= 0;P_[0][3]= 352.65;
    P_[1][0]=-10.3124;  P_[1][1]= -10.196;P_[1][2]= 11.2076;P_[1][3]= 68.395;
    P_[2][0]=-0.00644916;P_[2][1]= -0.0194706;P_[2][2]= 0;P_[2][3]= -0.196178;


                }

        if (0) {
                P_[0][0]=-6.00122;P_[0][1]= -14.6822;P_[0][2]= 0;P_[0][3]= 49.1228;
    P_[1][0]=-1.2737; P_[1][1]=-3.41346; P_[1][2]=15.8224 ;P_[1][3]=-103.207;
    P_[2][0]=0.0024448;P_[2][1]= -0.0076778;P_[2][2]= 0;P_[2][3]= -0.300161;

                }

        if (0) {
P_[0][0]=6.55584; P_[0][1]=-15.1614; P_[0][2]=0 ;P_[0][3]=40.929;
P_[1][0]= 0.223694;P_[1][1]= -1.13482;P_[1][2]= 14.3894;P_[1][3]= -180.04;
P_[2][0]= 0.00431944;P_[2][1]= -0.00534274; P_[2][2]=0; P_[2][3]=-0.4032;


                }
  vgui_style_sptr red_dots = vgui_style::new_style(1,0.2,0.0,7, 1);
        vgui_style_sptr y_dots = vgui_style::new_style(1,1.0,0.0,8, 1);
  for (unsigned i=0;i<6;i++) {
    //vcl_cout<<S_[i]<<vcl_endl;
    //vgl_point_2d<double> p = brct_algos::projection_3d_point( S_[i],P_);
    //if (i!=0&i!=1||i!=2||i!=3||i!=4||i!=14) continue;
    vnl_double_4 X;
    X[0]=S_[i].x();
    X[1]=S_[i].y();
    X[2]=S_[i].z();
    X[3]=1.0;
    //vnl_double_2 p = brct_algos::projection_3d_point( P_,X);
    //t0[0][i] = p.x();
    //t0[1][i] = p.y();  
    vnl_double_3 p = P_*X;

    vcl_cout<<i<<": "<<p<<" "<<p[0]/p[2] <<" "<<p[1]/p[2] <<vcl_endl;
    vtol_vertex_2d_sptr v1=
      new vtol_vertex_2d(p[0]/p[2],p[1]/p[2]);
    
    vtol_tabs_[0]->add_vertex(v1,red_dots);
  }

        for (unsigned i=0;i<4;i++)  {
                vtol_vertex_2d_sptr v1=
      new vtol_vertex_2d(x_[i],y_[i]);
    
    vtol_tabs_[0]->add_vertex(v1,y_dots);
                
        }

                vtol_vertex_2d_sptr v1=
      new vtol_vertex_2d(x_post_,y_post_);
    
    vtol_tabs_[0]->add_vertex(v1,y_dots);

        vtol_tabs_[0]->add_line(x_[0],y_[0],x_[1],y_[1]);
        vtol_tabs_[0]->add_line(x_[1],y_[1],x_[2],y_[2]);
        vtol_tabs_[0]->add_line(x_[2],y_[2],x_[3],y_[3]);
        vtol_tabs_[0]->add_line(x_[3],y_[3],x_[0],y_[0]);
  this->post_redraw();

}
///////

bgui_vtol2D_tableau_sptr bcri_calib::get_selected_vtol2D_tableau()
{
   unsigned int row =0, col=0;
   grid_->get_last_selected_position(&col, &row);
   return this->get_vtol2D_tableau_at(col, row);
}


bgui_vtol2D_tableau_sptr bcri_calib::get_vtol2D_tableau_at(unsigned col, unsigned row)
{
   if (row!=0)
      return 0;
   bgui_vtol2D_tableau_sptr btab = 0;
   //if (col==0||col==1||col==2)
      btab = vtol_tabs_[col];
   return btab;
}

bgui_picker_tableau_sptr bcri_calib::get_selected_picker_tableau()
{
   unsigned int row=0, col=0;
   grid_->get_last_selected_position(&col, &row);
   return this->get_picker_tableau_at(col, row);
}

bgui_picker_tableau_sptr
bcri_calib::get_picker_tableau_at(unsigned col, unsigned row)
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


//#define VERTEX vgl_homg_point_3d<double>
void bcri_calib::read_3d_real_points() {

VERTEX      S0(0,0,0); // origin   
VERTEX      S1(28.7, 0.0 ,0.0 );
VERTEX      S2(34.7, 0.0, 0.0); 
VERTEX      S3(38.7, 0.0, 0.0); 
VERTEX      S4(34.7, -4.7, 0.0 );
VERTEX      S5(38.7, -4.7, 0.0 );
VERTEX      S6(38.7, 0.6, 0.0 );
VERTEX      S7(20.13, 10.78, 0.0); 
VERTEX      S8(20.13, 6.23, 0.0 );
VERTEX      S9(25, 6.51, 0.0 );
VERTEX      S10(25, 11.0, 0.0 );
VERTEX      S11(-21.1, 34.4, 0.0); 
VERTEX      S12(-21.1, 34.4, 7.5 );
VERTEX      S13(-82.9, 34.4, 0.0 );
VERTEX      S14(-82.9, 34.4, 8.6 );
VERTEX      S15(-111.4, 34.4, 0.0 );
VERTEX      S16(-111.4, 34.4, 8.7 );
VERTEX      S17(20.13, 6.23, 0.0 );
VERTEX      S18(9.03, 6.23, 0.0 );
VERTEX      S19(14.13, 6.23, 0.0 );
VERTEX      S20(15.4, 10.78, 0.0 );
VERTEX      S21(-3.0, 10.78,  0.0 );
VERTEX      S22(-3.0, 6.26,  0.0 );
VERTEX      S23(2.24, 6.26,  0.0 );
VERTEX      S24(2.24, 10.78,  0.0  );   
S_.clear();
S_.push_back(S0);
S_.push_back(S1);
S_.push_back(S2);
S_.push_back(S3);
S_.push_back(S4);
S_.push_back(S5);
S_.push_back(S6);
S_.push_back(S7);
S_.push_back(S8);
S_.push_back(S9);
S_.push_back(S10);
S_.push_back(S11);
S_.push_back(S12);
S_.push_back(S13);
S_.push_back(S14);
S_.push_back(S15);
S_.push_back(S16);
S_.push_back(S17);
S_.push_back(S18);
S_.push_back(S19);
S_.push_back(S20);
S_.push_back(S21);
S_.push_back(S22);
S_.push_back(S23);
S_.push_back(S24);

 /* VERTEX      1 0 3 28.7 0.0 0.0 
VERTEX      2 0 3 34.7 0.0 0.0 
VERTEX      3 0 3 38.7 0.0 0.0 
VERTEX      4 0 3 34.7 -4.7 0.0 
VERTEX      5 0 3 38.7 -4.7 0.0 
VERTEX      6 0 3 38.7 0.6 0.0 
VERTEX      7 0 3 20.13 10.78 0.0 
VERTEX      8 0 3 20.13 6.23 0.0 
VERTEX      9 0 3 25 6.51 0.0 
VERTEX      10 0 3 25 11.0 0.0 
VERTEX      11 0 3 -21.1 34.4 0.0 
VERTEX      12 0 3 -21.1 34.4 7.5 
VERTEX      13 0 3 -82.9 34.4 0.0 
VERTEX      14 0 3 -82.9 34.4 8.6 
VERTEX      15 0 3 -111.4 34.4 0.0 
VERTEX      16 0 3 -111.4 34.4 8.7 
VERTEX      17 0 3 20.13 6.23 0.0 
VERTEX      18 0 3 9.03 6.23 0.0 
VERTEX      19 0 3 14.13 6.23 0.0 
VERTEX      20 0 3 15.4 10.78 0.0 
VERTEX      21 0 3 -3.0 10.78  0.0 
VERTEX      22 0 3 -3.0 6.26  0.0 
VERTEX      23 0 3 2.24 6.26  0.0 
VERTEX      23 0 3 2.24 10.78  0.0     */
}


void bcri_calib::read_3d_real_points_reverse() {

VERTEX      S0(0,0,0); // origin   

VERTEX      S1(-8.4, 0.0 ,0.0 );
VERTEX      S2(-30.0, 0.0, 0.0); 
VERTEX      S3(-17.9, 6.7, 0.0); 
VERTEX      S4(-17.9, 11.0, 0.0 );

//VERTEX      S1(28.7, 0.0 ,0.0 );
//VERTEX      S2(34.7, 0.0, 0.0); 
//VERTEX      S3(38.7, 0.0, 0.0); 
//VERTEX      S4(34.7, -4.7, 0.0 );
VERTEX      S5(38.7, -4.7, 0.0 );
VERTEX      S6(38.7, 0.6, 0.0 );
VERTEX      S7(20.13, 10.78, 0.0); 
VERTEX      S8(20.13, 6.23, 0.0 );
VERTEX      S9(25, 6.51, 0.0 );
VERTEX      S10(25, 11.0, 0.0 );
VERTEX      S11(-21.1, 34.4, 0.0); 
VERTEX      S12(-21.1, 34.4, 7.5 );
VERTEX      S13(-82.9, 34.4, 0.0 );
//VERTEX      S14(-82.9, 34.4, 8.6 );
VERTEX      S14(0.0, 0.0, 10.5 );
VERTEX      S15(-111.4, 34.4, 0.0 );
VERTEX      S16(-111.4, 34.4, 8.7 );
VERTEX      S17(20.13, 6.23, 0.0 );
VERTEX      S18(9.03, 6.23, 0.0 );
VERTEX      S19(14.13, 6.23, 0.0 );
VERTEX      S20(15.4, 10.78, 0.0 );
VERTEX      S21(-3.0, 10.78,  0.0 );
VERTEX      S22(-3.0, 6.26,  0.0 );
VERTEX      S23(2.24, 6.26,  0.0 );
VERTEX      S24(2.24, 10.78,  0.0  );   
S_.clear();
S_.push_back(S0);
S_.push_back(S1);
S_.push_back(S2);
S_.push_back(S3);
S_.push_back(S4);
S_.push_back(S5);
S_.push_back(S6);
S_.push_back(S7);
S_.push_back(S8);
S_.push_back(S9);
S_.push_back(S10);
S_.push_back(S11);
S_.push_back(S12);
S_.push_back(S13);
S_.push_back(S14);
S_.push_back(S15);
S_.push_back(S16);
S_.push_back(S17);
S_.push_back(S18);
S_.push_back(S19);
S_.push_back(S20);
S_.push_back(S21);
S_.push_back(S22);
S_.push_back(S23);
S_.push_back(S24);

 /* VERTEX      1 0 3 28.7 0.0 0.0 
VERTEX      2 0 3 34.7 0.0 0.0 
VERTEX      3 0 3 38.7 0.0 0.0 
VERTEX      4 0 3 34.7 -4.7 0.0 
VERTEX      5 0 3 38.7 -4.7 0.0 
VERTEX      6 0 3 38.7 0.6 0.0 
VERTEX      7 0 3 20.13 10.78 0.0 
VERTEX      8 0 3 20.13 6.23 0.0 
VERTEX      9 0 3 25 6.51 0.0 
VERTEX      10 0 3 25 11.0 0.0 
VERTEX      11 0 3 -21.1 34.4 0.0 
VERTEX      12 0 3 -21.1 34.4 7.5 
VERTEX      13 0 3 -82.9 34.4 0.0 
VERTEX      14 0 3 -82.9 34.4 8.6 
VERTEX      15 0 3 -111.4 34.4 0.0 
VERTEX      16 0 3 -111.4 34.4 8.7 
VERTEX      17 0 3 20.13 6.23 0.0 
VERTEX      18 0 3 9.03 6.23 0.0 
VERTEX      19 0 3 14.13 6.23 0.0 
VERTEX      20 0 3 15.4 10.78 0.0 
VERTEX      21 0 3 -3.0 10.78  0.0 
VERTEX      22 0 3 -3.0 6.26  0.0 
VERTEX      23 0 3 2.24 6.26  0.0 
VERTEX      23 0 3 2.24 10.78  0.0     */
}


