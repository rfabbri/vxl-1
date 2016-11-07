#include <cali/cali_vnl_least_squares_function.h>
#include <vnl/vnl_quaternion.h>
#include <vnl/vnl_vector.h>
#include <vnl/vnl_math.h>
#include <vcl_algorithm.h>
#include <vnl/vnl_double_3.h>
#include <vil/vil_new.h>
#include <cali/cali_simulated_scan_resource.h>
#include <xscan/xscan_orbit_base_sptr.h>
#include <xscan/xscan_uniform_orbit.h>


cali_vnl_least_squares_function::
cali_vnl_least_squares_function(cali_param par,
                                vcl_vector<vil_image_resource_sptr> scan_resources, 
                                xscan_scan scan, 
                                vcl_string file_base, 
                                int interval, 
                                int num_of_images)
                                :vnl_least_squares_function(par.NUM_OF_PARAMETERS,
                                ((num_of_images/interval)+1-1)*par.NUM_OF_CONICS*2,vnl_least_squares_function::no_gradient),  
                                //(num_of_images/interval)+1
                                par_(par),
                                scan_res_(scan_resources),
                                scan_(scan),
                                interval_(interval), 
                                num_of_images_(num_of_images)
{
  /*vcl_string txt_file = "C:\\test_images\\gen_calib\\diff.txt";
  fstream.open(txt_file.c_str());
  corresp.set_file(fstream);*/
    vcl_cout << "file_base " << file_base << "\n";
  for (unsigned i=0; i<scan.n_views(); i+=interval) {
    vcl_string file_name = corresp.gen_read_fname(file_base, i);
    vcl_cout << "reading conic file " << file_name << "\n";
    conic_vector_set saved_conics = corresp.read_conics_bin(file_name);
    vcl_cout << "read " << saved_conics.size() << " conics\n";
    real_img_conics.push_back(saved_conics);
  }
}

bool cali_vnl_least_squares_function::y_coord_compare(vsol_conic_2d_sptr x,vsol_conic_2d_sptr y) 
{ 
        double x1,y1,w1,h1;
        double garbage;
        double x2,y2,w2,h2;
        x->ellipse_parameters(x1,y1,garbage,w1,h1);
        y->ellipse_parameters(x2,y2,garbage,w2,h2);
        return fabs(y1) < fabs(y2); 
}


//:takes 13 parameters of camera, artifact, turn table, and sensor array information 
// and constructs the residuals of ball center differences
void
cali_vnl_least_squares_function::f(vnl_vector<double> const& ax, vnl_vector<double>& fx)
{
    for (unsigned int i = 0;i<par_.NUM_OF_PARAMETERS;i++)
    {
        vcl_cout<<" ax values "<<ax[i]<<vcl_endl;
    }
   vcl_cout<<" size of ax " <<ax.size()<<vcl_endl;
   assert(ax.size() >= par_.NUM_OF_PARAMETERS);
   vcl_vector<double> dum;
   int count = 0;
   for (int j = 0;j < 18 ;j++)
   {
     if (par_.SETPARAM[j])
       {
         vcl_cout << ax[count] << "\n";
         count++;
       }
     else
       vcl_cout << par_.PARAM[j] << "\n"; 

   }

   vcl_vector<conic_vector_set> sim_artf_conics;

   vnl_quaternion<double> artf_rot;
   vgl_point_3d<double> artf_tr;
   vnl_quaternion<double> turn_table_rot;
   vnl_double_3 turn_table_tr;
   double x_scale;
   double y_scale;
   vgl_point_2d<double> princp_point;
   vcl_vector<double> x_coord_tol;
   vcl_vector<double> y_coord_tol;
   vcl_vector<double> z_coord_tol;
   vcl_vector<double> rad_tol;


   // ball_distance = cali_cylinder_artifact::ball_spacing;
   // lsqr_fn_params(x, ball_indent, ball_indent_big, ball_distance, artf_rot, artf_tr, turn_table_rot, turn_table_tr, x_scale, y_scale, xy_scale, princp_point);
   lsqr_fn_params_with_tolerance(ax, artf_rot, artf_tr, turn_table_rot, turn_table_tr, x_scale, y_scale,princp_point,
                   x_coord_tol,y_coord_tol,z_coord_tol,rad_tol);

   //artf_rot.normalize();

   vcl_cout << vcl_endl << vcl_endl << "Artifact Rotation "
           << artf_rot.axis() << "," 
           << artf_rot.angle() 
          << "(" << (artf_rot.angle()/vnl_math::pi)*180  << " deg)" << " " ;
  vcl_cout << "Translation " << artf_tr << vcl_endl;

  vcl_cout << vcl_endl << vcl_endl << "Turntable Rotation "  
          << turn_table_rot.axis() << "," 
          << turn_table_rot.angle() 
          << "(" << (turn_table_rot.angle()/vnl_math::pi)*180  << " deg)" << " " ;
  vcl_cout << "Translation " << turn_table_tr << vcl_endl;

  vcl_cout<<"scale ("<<x_scale <<","<<y_scale<< ")" << " ";
  vcl_cout<<" principal point ("<<princp_point.x()<<","<<princp_point.y()<< ")" << vcl_endl;

/*
  for (int i = 0;i< static_cast<int>(x_coord_tol.size());i++)
  {
      vcl_cout<<" x_coord_tol: "<< "for ball " <<i+1<<" : "<<x_coord_tol[i]<<vcl_endl;
       vcl_cout<<" y_coord_tol: "<< "for ball " <<i+1<<" : "<<y_coord_tol[i]<<vcl_endl;
        vcl_cout<<" z_coord_tol: "<< "for ball " <<i+1<<" : "<<z_coord_tol[i]<<vcl_endl;
         vcl_cout<<" rad_tol: "<< "for ball " <<i+1<<" : "<<rad_tol[i]<<vcl_endl;
  }
  */

  vpgl_calibration_matrix<double> kk(scan_.kk());
  //kk.set_focal_length(focal_length);
  kk.set_x_scale(x_scale);
  kk.set_y_scale(y_scale);
  
  /*vcl_cout << xy_scale << vcl_endl;
  if (xy_scale < 0) 
    xy_scale *= -1.0;
  kk.set_x_scale(xy_scale * 100 * scan_.kk().x_scale());
  kk.set_y_scale(xy_scale * 100 * scan_.kk().y_scale());*/

  kk.set_principal_point(princp_point);

  scan_.set_calibration_matrix(kk);



  xscan_orbit_base_sptr orbit_base = (scan_.orbit());
  xscan_uniform_orbit orbit = static_cast<const xscan_uniform_orbit&>(*orbit_base);
  xscan_orbit_base_sptr new_orbit = new xscan_uniform_orbit(orbit.rot(),
                        orbit.t(),
                        turn_table_rot,
                        turn_table_tr);
  scan_.set_orbit(new_orbit);

  cali_simulated_scan_resource scan_res(scan_, scan_res_, par_, 
                  x_coord_tol, y_coord_tol, z_coord_tol, rad_tol);

   vcl_vector<vcl_vector<vsol_conic_2d> > centers = scan_res.gen_ball_projections(artf_rot, 
                   artf_tr,
                   par_.INTERVAL);
                                                     
 
   cali_artf_corresponder corr(par_);
 
 //  corr.diff_corresp_set(centers, real_img_conics, fx);
 //  corr.manual_corresp_set(centers,fx);
   
 /*
    vcl_cerr << "centers is of size: " << centers.size() << "\n"; 
    int sum = 0;
    for(int i = 0; i < centers.size(); i++)
            sum+=centers[i].size(); 
    vcl_cerr << "sum of center sizes: " << sum << "\n"; 
    */

               
   /*
    vcl_cout << "real_img_conics is of size: " << real_img_conics.size() << "\n"; 
    int sum = 0;
    for(int i = 0; i < real_img_conics.size(); i++){
            vcl_cout << "\treal_img_conics[" << i << "] has " << real_img_conics[i].size() << "\n";
            for(int j = 0; j < real_img_conics[i].size(); j++){
                    vcl_cout << "\t\treal_img_conics[" << i << "][" << j << "] has" << real_img_conics[i][j].size() << "\n";
                    sum+=real_img_conics[i][j].size();
            }
    }
    vcl_cout << "sum of real_img_conic sizes: " << sum << "\n"; 
    */

   

   vcl_vector<vcl_vector<vsol_conic_2d_sptr> > modified_real_conic_vector;
   modified_real_conic_vector.resize(real_img_conics.size());

   for(int i = 0; i < real_img_conics.size(); i++) {
           for(int j = 0; j < real_img_conics[i].size(); j++){
                   modified_real_conic_vector[i].push_back(real_img_conics[i][j][0]);
           }
           vcl_sort(modified_real_conic_vector[i].begin(), 
                    modified_real_conic_vector[i].end(), 
                    cali_vnl_least_squares_function::y_coord_compare );
   }




   /*
    vcl_cout << "modified_real_conic_vector is of size: " << modified_real_conic_vector.size() << "\n"; 
    for(int i = 0; i < modified_real_conic_vector.size(); i++){
            vcl_cout << "\tmodified_real_conic_vector[" << i << "] has " << modified_real_conic_vector[i].size() << "\n";
    }
    */

   //fx is the vector of square-root of sum of squared differences between
   //center locations & width/heights.  quoted end error is the RMS of this
   //delta vector.  typically  of size near (num imgs)*(13 balls)*(2 measurements)
//   corr.masked_corresp_set(centers,real_img_conics,fx);

   corr.verbose_error_summary(centers,modified_real_conic_vector,fx);
   //corr.new_correspondence(centers,modified_real_conic_vector,fx);
}

cali_vnl_least_squares_function::~cali_vnl_least_squares_function(void)
{
}


void 
cali_vnl_least_squares_function::lsqr_fn_params(vnl_vector<double> const& x, 
                                                vnl_quaternion<double> &artf_rot, 
                                                vgl_point_3d<double> &artf_trans,
                                                vnl_quaternion<double> &turn_table_rot,
                                                vnl_double_3 &turn_table_trans,
                                                double &x_scale,
                                                double &y_scale,
                                                vgl_point_2d<double> &princp_point)
{
  int i=0;
  
 
 // vnl_vector<double>dum;
  //dum.set_size(SIZE_OF_X);
 /*vnl_vector<double>dum(x.size());

for (int j = 0;j<x.size();j++)
   {
       
           dum[j] = x[j];
     
   }*/
  int count = 0;
  vcl_vector<double>dum;
  for (int j = 0;j < par_.SIZE_OF_X ;j++)
   {
      
       if (par_.SETPARAM[j])
       {
        dum.push_back(x[count]);
       count++;
       }
       else
           dum.push_back(par_.PARAM[j]); 
     
   }

  // vnl_vector<double>dum;


  // construct the artifact rotation quaternion
  vnl_vector<double> v(4);
  v[0] = dum[i++];
  v[1] = dum[i++];
  v[2] = dum[i++];
  v[3] = dum[i++];
  //artf_rot = vnl_quaternion<double>(v, theta);
  artf_rot = vnl_quaternion<double>(v[0], v[1], v[2], v[3]);

  // construct the artifact translation
  double tr_x = dum[i++];
  double tr_y = dum[i++];
  double tr_z = dum[i++];
  artf_trans = vgl_point_3d<double> (tr_x, tr_y, tr_z);

  x_scale = dum[i++];
  y_scale = dum[i++];
  //xy_scale = x[i++];
  double p_x = dum[i++];
  double p_y = dum[i++];
  princp_point = vgl_point_2d<double> (p_x, p_y);
  
  // construct the turn table parameters
  v[0] = dum[i++];
  v[1] = dum[i++];
  v[2] = dum[i++];
  v[3] = dum[i++];
  
  turn_table_rot = vnl_quaternion<double> (v[0], v[1], v[2], v[3]);
  turn_table_rot.normalize();
 
  double tt_tx = dum[i++];
  double tt_ty = dum[i++];
  double tt_tz = dum[i++];
  turn_table_trans = vnl_double_3 (tt_tx, tt_ty, tt_tz);

   vcl_cout << vcl_endl << vcl_endl << "Artifact Rotation-->" << vcl_endl;
  vcl_cout << artf_rot.axis() << vcl_endl;
  vcl_cout << artf_rot.angle() << " deg=" << (artf_rot.angle()/vnl_math::pi)*180 << vcl_endl;
  vcl_cout << "Artifact Translation-->" << vcl_endl;
  vcl_cout << artf_trans << vcl_endl;

  vcl_cout << "Turn Table Rotation-->" << vcl_endl;
  vcl_cout << turn_table_rot.axis() << vcl_endl;
  vcl_cout << turn_table_rot.angle() << " deg=" << (turn_table_rot.angle()/vnl_math::pi)*180 << vcl_endl;
  vcl_cout << "Turn Table Translation-->" << vcl_endl;
  vcl_cout << turn_table_trans << vcl_endl;

  
  }


void 
cali_vnl_least_squares_function::lsqr_fn_params_with_tolerance(vnl_vector<double> const& x, 
                                                vnl_quaternion<double> &artf_rot, 
                                                vgl_point_3d<double> &artf_trans,
                                                vnl_quaternion<double> &turn_table_rot,
                                                vnl_double_3 &turn_table_trans,
                                                double &x_scale,
                                                double &y_scale,
                                                vgl_point_2d<double> &princp_point,
                                                vcl_vector<double>& x_coord_tol,vcl_vector<double>& y_coord_tol,
                                                vcl_vector<double>& z_coord_tol,vcl_vector<double>& rad_tol)
{
  int i=0;
  
 
 // vnl_vector<double>dum;
  //dum.set_size(SIZE_OF_X);
 /*vnl_vector<double>dum(x.size());

for (int j = 0;j<x.size();j++)
   {
       
           dum[j] = x[j];
     
   }*/
  int count = 0;
  vcl_vector<double>dum;
  for (int j = 0;j < par_.SIZE_OF_X ;j++)
   {
      
       if (par_.SETPARAM[j])
       {
        dum.push_back(x[count]);
       count++;
       }
       else
           dum.push_back(par_.PARAM[j]); 
     
   }

  // vnl_vector<double>dum;


  // construct the artifact rotation quaternion
  vnl_vector<double> v(4);
  v[0] = dum[i++];
  v[1] = dum[i++];
  v[2] = dum[i++];
  v[3] = dum[i++];
  //artf_rot = vnl_quaternion<double>(v, theta);
  artf_rot = vnl_quaternion<double>(v[0], v[1], v[2], v[3]);

  // construct the artifact translation
  double tr_x = dum[i++];
  double tr_y = dum[i++];
  double tr_z = dum[i++];
  artf_trans = vgl_point_3d<double> (tr_x, tr_y, tr_z);

  x_scale = dum[i++];
  y_scale = dum[i++];
  //xy_scale = x[i++];
  double p_x = dum[i++];
  double p_y = dum[i++];
  princp_point = vgl_point_2d<double> (p_x, p_y);
  
  // construct the turn table parameters
  v[0] = dum[i++];
  v[1] = dum[i++];
  v[2] = dum[i++];
  v[3] = dum[i++];
  
  turn_table_rot = vnl_quaternion<double> (v[0], v[1], v[2], v[3]);
  turn_table_rot.normalize();
 
  double tt_tx = dum[i++];
  double tt_ty = dum[i++];
  double tt_tz = dum[i++];
  turn_table_trans = vnl_double_3 (tt_tx, tt_ty, tt_tz);

  for (int k = 0;k<par_.BALL_NUMBER;k++)
  {
  x_coord_tol.push_back(dum[i++]);
  y_coord_tol.push_back(dum[i++]);
  z_coord_tol.push_back(dum[i++]);
  rad_tol.push_back(dum[i++]);
  }

  /*
   vcl_cout << vcl_endl << vcl_endl << "Artifact Rotation-->" << vcl_endl;
  vcl_cout << artf_rot.axis() << vcl_endl;
  vcl_cout << artf_rot.angle() << " deg=" << (artf_rot.angle()/vnl_math::pi)*180 << vcl_endl;
  vcl_cout << "Artifact Translation-->" << vcl_endl;
  vcl_cout << artf_trans << vcl_endl;

  vcl_cout << "Turn Table Rotation-->" << vcl_endl;
  vcl_cout << turn_table_rot.axis() << vcl_endl;
  vcl_cout << turn_table_rot.angle() << " deg=" << (turn_table_rot.angle()/vnl_math::pi)*180 << vcl_endl;
  vcl_cout << "Turn Table Translation-->" << vcl_endl;
  vcl_cout << turn_table_trans << vcl_endl;

  for (int k = 0;k<par_.BALL_NUMBER;k++)
  {
  vcl_cout << "x_coord_tol for ball "<<k+1<<" -->"<<vcl_endl;
  vcl_cout << x_coord_tol[k] <<vcl_endl;
 vcl_cout << "y_coord_tol for ball "<<k+1<<" -->"<<vcl_endl;
  vcl_cout << y_coord_tol[k] <<vcl_endl;
 vcl_cout << "z_coord_tol for ball "<<k+1<<" -->"<<vcl_endl;
  vcl_cout << z_coord_tol[k] <<vcl_endl;
   vcl_cout << "rad_tol for ball "<<k+1<<" -->"<<vcl_endl;
  vcl_cout << rad_tol[k] <<vcl_endl;
  }
  */
  
  }



void 
cali_vnl_least_squares_function::gen_scan_lsqr_fn_params(vnl_vector<double> const& x, 
                                                vnl_quaternion<double> &artf_rot, 
                                                vgl_point_3d<double> &artf_trans,
                                                vnl_quaternion<double> &turn_table_rot,
                                                vnl_double_3 &turn_table_trans,
                                                double &x_scale,
                                                double &y_scale,
                                                vgl_point_2d<double> &princp_point)
{
  int i=0;
  
 
 // vnl_vector<double>dum;
  //dum.set_size(SIZE_OF_X);
 vnl_vector<double>dum(x.size());

for (unsigned int j = 0;j<x.size();j++)
   {
       
           dum[j] = x[j];
     
   }
  

  // vnl_vector<double>dum;


  // construct the artifact rotation quaternion
  vnl_vector<double> v(4);
  v[0] = dum[i++];
  v[1] = dum[i++];
  v[2] = dum[i++];
  v[3] = dum[i++];
  //artf_rot = vnl_quaternion<double>(v, theta);
  artf_rot = vnl_quaternion<double>(v[0], v[1], v[2], v[3]);

  // construct the artifact translation
  double tr_x = dum[i++];
  double tr_y = dum[i++];
  double tr_z = dum[i++];
  artf_trans = vgl_point_3d<double> (tr_x, tr_y, tr_z);

  x_scale = dum[i++];
  y_scale = dum[i++];
  //xy_scale = x[i++];
  double p_x = dum[i++];
  double p_y = dum[i++];
  princp_point = vgl_point_2d<double> (p_x, p_y);
  
  // construct the turn table parameters
  v[0] = dum[i++];
  v[1] = dum[i++];
  v[2] = dum[i++];
  v[3] = dum[i++];
  
  turn_table_rot = vnl_quaternion<double> (v[0], v[1], v[2], v[3]);
  turn_table_rot.normalize();
 
  double tt_tx = dum[i++];
  double tt_ty = dum[i++];
  double tt_tz = dum[i++];
  turn_table_trans = vnl_double_3 (tt_tx, tt_ty, tt_tz);

   vcl_cout << vcl_endl << vcl_endl << "Artifact Rotation-->" << vcl_endl;
  vcl_cout << artf_rot.axis() << vcl_endl;
  vcl_cout << artf_rot.angle() << " deg=" << (artf_rot.angle()/vnl_math::pi)*180 << vcl_endl;
  vcl_cout << "Artifact Translation-->" << vcl_endl;
  vcl_cout << artf_trans << vcl_endl;

  vcl_cout << "Turn Table Rotation-->" << vcl_endl;
  vcl_cout << turn_table_rot.axis() << vcl_endl;
  vcl_cout << turn_table_rot.angle() << " deg=" << (turn_table_rot.angle()/vnl_math::pi)*180 << vcl_endl;
  vcl_cout << "Turn Table Translation-->" << vcl_endl;
  vcl_cout << turn_table_trans << vcl_endl;

  
  }


void 
 cali_vnl_least_squares_function::set_lsqr_fn_params(vnl_vector<double> &x, 
                                                 vnl_quaternion<double> const& artf_rot, 
                                                 vgl_point_3d<double> const& artf_tr,
                                                xscan_scan& scan)
{
  int i=0;
  
 
   vnl_vector<double>dummy;
   dummy.set_size(par_.SIZE_OF_X);

   dummy[i++] = artf_rot.x();
  dummy[i++] = artf_rot.y();
   dummy[i++] = artf_rot.z();
   dummy[i++] = artf_rot.r();

   dummy[i++] = artf_tr.x();
   dummy[i++] = artf_tr.y();
   dummy[i++] = artf_tr.z();
   // set principal point
  dummy[i++] = scan.kk().x_scale();
   dummy[i++] = scan.kk().y_scale();

   vcl_cout<<" x-scale " <<  scan.kk().x_scale()<<vcl_endl;
   vcl_cout<<" y-scale " << scan.kk().y_scale()<<vcl_endl;
 
  
   dummy[i++] = scan.kk().principal_point().x();
   dummy[i++] = scan.kk().principal_point().y();

   // set the turn table rot, tr
   xscan_orbit_base_sptr orbit_base = (scan.orbit());
  xscan_uniform_orbit orbit = static_cast<const xscan_uniform_orbit&>(*orbit_base);
   vnl_quaternion<double> tt_rot = orbit.r0();
   
   dummy[i++] = tt_rot.x();
   dummy[i++] = tt_rot.y();
  dummy[i++] = tt_rot.z();
   dummy[i++] = tt_rot.r();
 
  dummy[i++] = orbit.t0()[0];
   dummy[i++] = orbit.t0()[1];
  dummy[i++] = orbit.t0()[2];

  int count = 0;
   for (int j = 0;j<par_.SIZE_OF_X;j++)
   {
       if (par_.SETPARAM[j])
       {
           x[count++] = dummy[j];
       }
   }
 
   vcl_cout << vcl_endl << vcl_endl << "Artifact Rotation-->" << vcl_endl;
   vcl_cout << artf_rot.axis() << vcl_endl;
   vcl_cout << artf_rot.angle()  << vcl_endl;
   vcl_cout << "Artifact Translation-->" << vcl_endl;
  vcl_cout << artf_tr << vcl_endl;
 
   vcl_cout << "Turn Table Rotation-->" << vcl_endl;
   vcl_cout << tt_rot.axis() << vcl_endl;
  vcl_cout << tt_rot.angle() << vcl_endl;


   }


void 
 cali_vnl_least_squares_function::set_lsqr_fn_params_with_tolerance(vnl_vector<double> &x, 
                                                vnl_quaternion<double> const& artf_rot, 
                                                vgl_point_3d<double> const& artf_tr,
                                                xscan_scan& scan,
                                                vcl_vector<double>& x_coord_tol,
                                                vcl_vector<double>& y_coord_tol,
                                                vcl_vector<double>& z_coord_tol,
                                                vcl_vector<double>& rad_tol)
{

        vcl_cout << "cali_vnl_least_squares_function::set_lsqr_fn_params_with_tolerance\n";
   vnl_vector<double>dummy;
   dummy.set_size(par_.SIZE_OF_X);
   int i=0;

   dummy[i++] = artf_rot.x();
   dummy[i++] = artf_rot.y();
   dummy[i++] = artf_rot.z();
   dummy[i++] = artf_rot.r();

   dummy[i++] = artf_tr.x();
   dummy[i++] = artf_tr.y();
   dummy[i++] = artf_tr.z();
   // set principal point
   dummy[i++] = scan.kk().x_scale();
   dummy[i++] = scan.kk().y_scale();

   vcl_cout<<" x-scale " <<  scan.kk().x_scale()<<vcl_endl;
   vcl_cout<<" y-scale " << scan.kk().y_scale()<<vcl_endl;
  
   dummy[i++] = scan.kk().principal_point().x();
   dummy[i++] = scan.kk().principal_point().y();

   // set the turn table rot, tr
   xscan_orbit_base_sptr orbit_base = (scan.orbit());
   xscan_uniform_orbit orbit = static_cast<const xscan_uniform_orbit&>(*orbit_base);
   vnl_quaternion<double> tt_rot = orbit.r0();
   
   dummy[i++] = tt_rot.x();
   dummy[i++] = tt_rot.y();
  dummy[i++] = tt_rot.z();
   dummy[i++] = tt_rot.r();
 
  dummy[i++] = orbit.t0()[0];
   dummy[i++] = orbit.t0()[1];
  dummy[i++] = orbit.t0()[2];

  for (int k = 0;k<par_.BALL_NUMBER;k++)
  {
  dummy[i++] = x_coord_tol[k];
  dummy[i++] = y_coord_tol[k];
  dummy[i++] = z_coord_tol[k];
  dummy[i++] = rad_tol[k];
  }


  int count = 0;
   for (int j = 0;j<par_.SIZE_OF_X;j++)
   {
       if (par_.SETPARAM[j])
       {
           x[count++] = dummy[j];
       }
   }

   /*
   vcl_cout << vcl_endl << vcl_endl << "Artifact Rotation-->" << vcl_endl;
   vcl_cout << artf_rot.axis() << vcl_endl;
   vcl_cout << artf_rot.angle()  << vcl_endl;
   vcl_cout << "Artifact Translation-->" << vcl_endl;
  vcl_cout << artf_tr << vcl_endl;
 
   vcl_cout << "Turn Table Rotation-->" << vcl_endl;
   vcl_cout << tt_rot.axis() << vcl_endl;
  vcl_cout << tt_rot.angle() << vcl_endl;


   for (int k = 0;k<par_.BALL_NUMBER;k++)
  {
  vcl_cout << "x_coord_tol for ball "<<k+1<<" -->"<<vcl_endl;
  vcl_cout << x_coord_tol[k] <<vcl_endl;
 vcl_cout << "y_coord_tol for ball "<<k+1<<" -->"<<vcl_endl;
  vcl_cout << y_coord_tol[k] <<vcl_endl;
 vcl_cout << "z_coord_tol for ball "<<k+1<<" -->"<<vcl_endl;
  vcl_cout << z_coord_tol[k] <<vcl_endl;
   vcl_cout << "rad_tol for ball "<<k+1<<" -->"<<vcl_endl;
  vcl_cout << rad_tol[k] <<vcl_endl;
  }
  */

}

cali_vnl_least_squares_function::
cali_vnl_least_squares_function(vcl_vector<vil_image_resource_sptr> scan_resources, 
                                xscan_scan scan, 
                                vcl_string file_base, 
                                int interval, 
                                int num_of_images)
                                : vnl_least_squares_function(18,((num_of_images/interval))*4*2,vnl_least_squares_function::no_gradient), 
    scan_res_(scan_resources), scan_(scan), interval_(interval), 
    num_of_images_(num_of_images)
{
  /*vcl_string txt_file = "C:\\test_images\\gen_calib\\diff.txt";
  fstream.open(txt_file.c_str());
  corresp.set_file(fstream);*/

  vcl_cout << "constructing cali_vnl_least_squares_function without cali_param.\n";
  vcl_cout << "press a key, then press enter.\n";
  char x;
  vcl_cin >> x;

    vcl_cout << "file_base " << file_base << "\n";
  for (unsigned i=0; i<scan.n_views(); i+=interval) {
    vcl_string file_name = corresp.gen_read_fname(file_base, i);
    vcl_cout << "adding conic " << file_name << "\n";
    conic_vector_set saved_conics = corresp.read_conics_bin(file_name);
    vcl_cout << "read " << saved_conics.size() << " conics\n";
    real_img_conics.push_back(saved_conics);
  }
}




#if 0
//:takes 13 parameters of camera, artifact, turn table, and sensor array information 
// and constructs the residuals of ball center differences
void
cali_vnl_least_squares_function::f(vnl_vector<double> const& ax, vnl_vector<double>& fx)
{
    for (unsigned int i = 0;i<par_.NUM_OF_PARAMETERS;i++)
    {
        vcl_cout<<" ax values "<<ax[i]<<vcl_endl;
    }
  vcl_vector<conic_vector_set> sim_artf_conics;
  assert(ax.size() >= par_.NUM_OF_PARAMETERS);

   vnl_quaternion<double> artf_rot;
  vgl_point_3d<double> artf_tr;
  vnl_quaternion<double> turn_table_rot;
  vnl_double_3 turn_table_tr;
  double x_scale;
  double y_scale;
vgl_point_2d<double> princp_point;

vcl_cout<<" size of ax " <<ax.size()<<vcl_endl;

 // ball_distance = cali_cylinder_artifact::ball_spacing;
  // lsqr_fn_params(x, ball_indent, ball_indent_big, ball_distance, artf_rot, artf_tr, turn_table_rot, turn_table_tr, x_scale, y_scale, xy_scale, princp_point);
  lsqr_fn_params(ax, artf_rot, artf_tr, turn_table_rot, turn_table_tr, x_scale, y_scale,princp_point);

  //artf_rot.normalize();
  
  vcl_cout << vcl_endl << vcl_endl << "Artifact Rotation-->" << vcl_endl;
  vcl_cout << artf_rot.axis() << vcl_endl;
  vcl_cout << artf_rot.angle() << " deg=" << (artf_rot.angle()/vnl_math::pi)*180 << vcl_endl;
  vcl_cout << "Artifact Translation-->" << vcl_endl;
  vcl_cout << artf_tr << vcl_endl;

  vcl_cout << "Turn Table Rotation-->" << vcl_endl;
  vcl_cout << turn_table_rot.axis() << vcl_endl;
  vcl_cout << turn_table_rot.angle() << " deg=" << (turn_table_rot.angle()/vnl_math::pi)*180 << vcl_endl;
  vcl_cout << "Turn Table Translation-->" << vcl_endl;
  vcl_cout << turn_table_tr << vcl_endl;

  vcl_cout<<" x & y scale "<<x_scale <<"  "<<y_scale<<vcl_endl;
  vcl_cout<<" principal point "<<princp_point.x()<<"  "<<princp_point.y()<<vcl_endl;

  vpgl_calibration_matrix<double> kk(scan_.kk());
  //kk.set_focal_length(focal_length);
  kk.set_x_scale(x_scale);
  kk.set_y_scale(y_scale);
  
  /*vcl_cout << xy_scale << vcl_endl;
  if (xy_scale < 0) 
    xy_scale *= -1.0;
  kk.set_x_scale(xy_scale * 100 * scan_.kk().x_scale());
  kk.set_y_scale(xy_scale * 100 * scan_.kk().y_scale());*/

  kk.set_principal_point(princp_point);

  scan_.set_calibration_matrix(kk);



  xscan_orbit_base_sptr orbit_base = (scan_.orbit());
  xscan_uniform_orbit orbit = static_cast<const xscan_uniform_orbit&>(*orbit_base);
  xscan_orbit_base_sptr new_orbit = new xscan_uniform_orbit(orbit.rot(),
                        orbit.t(),
                        turn_table_rot,
                        turn_table_tr);
  scan_.set_orbit(new_orbit);
 // vcl_cout<<" scan values in vnl_least_squares_function " <<scan_<<vcl_endl;
  //ball_distance = cali_cylinder_artifact::ball_spacing;
 // cali_simulated_scan_resource scan_res(scan_, scan_res_, ball_indent, ball_indent_big, ball_distance);
//
//vcl_string path = "C:\\amherst_artifact\\parameters.txt";
//cali_param par(path);

cali_simulated_scan_resource scan_res(scan_, scan_res_,par_);
//cali_simulated_scan_resource scan_res(scan_, scan_res_);
//  vcl_vector<vcl_vector<vsol_conic_2d> > centers = scan_res.gen_ball_projections(artf_rot, artf_tr, par.INTERVAL);
   vcl_vector<vcl_vector<vsol_conic_2d> > centers = scan_res.gen_ball_projections(artf_rot, artf_tr,par_.INTERVAL);
 
   cali_artf_corresponder corr(par_);
 
 //  corr.diff_corresp_set(centers, real_img_conics, fx);
 //  corr.manual_corresp_set(centers,fx);
   corr.masked_corresp_set(centers,real_img_conics,fx);
}
#endif





