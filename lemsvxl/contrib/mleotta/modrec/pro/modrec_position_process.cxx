// This is contrib/mleotta/modrec/pro/modrec_position_process.cxx

//:
// \file

#include "modrec_position_process.h"
#include <bpro1/bpro1_parameters.h>
#include <vidpro1/storage/vidpro1_image_storage.h>
#include <vil/vil_image_resource.h>
#include <vil/vil_convert.h>
#include <vil/vil_new.h>
#include <vil/vil_math.h>
#include <vcl_limits.h>

#include <dbbgm/pro/dbbgm_image_storage.h>
#include <bbgm/bbgm_image_sptr.h>
#include <bbgm/bbgm_image_of.h>
#include <bbgm/bbgm_apply.h>
#include <bsta/bsta_attributes.h>
#include <bsta/bsta_mixture.h>
#include <bsta/bsta_gauss_if3.h>
#include <bsta/bsta_basic_functors.h>
#include <bsta/algo/bsta_bayes_functor.h>
#include <bsta/algo/bsta_mixture_functors.h>
#include <bsta/bsta_detector_gaussian.h>
#include <bsta/bsta_detector_mixture.h>

#include <dbul/dbul_solar_position.h>
#include <vpgl/vpgl_proj_camera.h>
#include <vpgl/vpgl_poly_radial_distortion.h>
#include <imesh/imesh_fileio.h>
#include <imesh/imesh_mesh.h>
#include <imesh/algo/imesh_project.h>
#include <modrec/modrec_proj_cost_func.h>

#include <vidpro1/storage/vidpro1_vsol2D_storage.h>
#include <vsol/vsol_point_2d.h>
#include <vsol/vsol_triangle_2d.h>

#include <vnl/algo/vnl_amoeba.h>

#include <vil/algo/vil_colour_space.h>

#include "modrec_pro_utils.h"


//: Constructor
modrec_position_process::modrec_position_process()
  : total_error_(0.0)
{
  if( !parameters()->add( "hour offset" ,      "-hour" ,    0.0f      ) ||
      !parameters()->add( "Scale" ,            "-scale" ,   1.0f   ) ||
      !parameters()->add( "Max Iterations" ,   "-max_eval", 200   ) ||
      !parameters()->add( "F Tolerance" ,      "-f_tol" ,   1.0f   ) ||
      !parameters()->add( "Init X" ,           "-initx" ,   -400.0f   ) ||
      !parameters()->add( "Init Y" ,           "-inity" ,   20.0f   ) ||
      !parameters()->add( "Init Vx" ,          "-initvx" ,  20.0f   ) ||
      !parameters()->add( "Calibration Params","-params" ,  bpro1_filepath("","*")  ) ||
      !parameters()->add( "Model      " ,      "-model" ,   bpro1_filepath("","*")  ) ||
      !parameters()->add( "YUV Color Space" ,  "-yuv",      false ) ||
      !parameters()->add( "debug" ,            "-debug",    false ) ){
    vcl_cerr << "ERROR: Adding parameters in " __FILE__<< vcl_endl;
  }
}


//: Destructor
modrec_position_process::~modrec_position_process()
{
}


//: Clone the process
bpro1_process* 
modrec_position_process::clone() const
{
  return new modrec_position_process(*this);
}


//: Return the name of this process
vcl_string
modrec_position_process::name()
{
  return "Position Model";
}


//: Return the number of input frame for this process
int
modrec_position_process::input_frames()
{
  return 1;
}


//: Return the number of output frames for this process
int
modrec_position_process::output_frames()
{
  return 1;
}


//: Provide a vector of required input types
vcl_vector< vcl_string > modrec_position_process::get_input_type()
{
  vcl_vector< vcl_string > to_return;
  to_return.push_back( "image" );
  to_return.push_back( "bbgm_image" );
  to_return.push_back( "bbgm_image" );

  return to_return;
}


//: Provide a vector of output types
vcl_vector< vcl_string > modrec_position_process::get_output_type()
{  
  vcl_vector<vcl_string > to_return;
  to_return.push_back( "image" );
  to_return.push_back( "image" );
  to_return.push_back( "vsol2D" );

  return to_return;
}


//: Returns a vector of strings with suggested names for output classes
vcl_vector< vcl_string > modrec_position_process::suggest_output_names()
{
  vcl_vector< vcl_string > names;
  names.push_back("overlay");
  names.push_back("state labels");
  names.push_back("projected mesh");

  return names;
}


//: Execute the process
bool
modrec_position_process::execute()
{
  if ( input_data_.size() != 1 ){
    vcl_cout << "In modrec_position_process::execute() - "
             << "not exactly one input image \n";
    return false;
  }
  clear_output();

   // get the process parameters 
  bool yuv=false, debug=false;
  parameters()->get_value( "-yuv" ,   yuv);
  parameters()->get_value( "-debug" ,  debug );


  // get image from the storage class
  vidpro1_image_storage_sptr frame_image;
  frame_image.vertical_cast(input_data_[0][0]);

  // convert the image to float and scale the intensities
  vil_image_resource_sptr image_rsc = frame_image->get_image();
  vil_image_view<float> fimg = *vil_convert_cast(float(), image_rsc->get_view());
  vil_image_view<float> img;
  img.deep_copy(fimg);
  if(image_rsc->pixel_format() == VIL_PIXEL_FORMAT_BYTE)
    vil_math_scale_values(img,1.0/255.0);

  const unsigned int ni = img.ni();
  const unsigned int nj = img.nj();


  typedef bsta_num_obs<bsta_gauss_if3> gauss_type;
  typedef bsta_num_obs<bsta_mixture<gauss_type> > mix_gauss_type;

  bbgm_image_of<mix_gauss_type> *model=NULL, *model_shadow=NULL;

  // get the background model
  dbbgm_image_storage_sptr frame_model;
  if(input_data_[0][1]){
    frame_model.vertical_cast(input_data_[0][1]);
  }
  else{ 
    vcl_cerr << "background model not provided" << vcl_endl;
    return false;
  }
  model = dynamic_cast<bbgm_image_of<mix_gauss_type>*>(frame_model->dist_image().ptr());
  if(!model){
    vcl_cerr << "background model not correct type" << vcl_endl;
    return false;
  }

  frame_model = NULL;
  if(input_data_[0][2]){
    frame_model.vertical_cast(input_data_[0][2]);
    model_shadow = dynamic_cast<bbgm_image_of<mix_gauss_type>*>(frame_model->dist_image().ptr());
  }


  vil_image_view<float> new_img; new_img.deep_copy(img);
  vidpro1_image_storage_sptr output_storage = vidpro1_image_storage_new();
  output_storage->set_image(vil_new_image_resource_of_view( new_img ));
  output_data_[0].push_back(output_storage);

  //convert to YUV
  if(yuv){
    for(unsigned int i=0; i<ni; ++i){
      for(unsigned int j=0; j<nj; ++j){
        float pixel[3] = {img(i,j,0), img(i,j,1), img(i,j,2)};
        float yuv[3];
        vil_colour_space_RGB_to_YUV(pixel, yuv);
        img(i,j,0) = yuv[0];
        img(i,j,1) = yuv[1];
        img(i,j,2) = yuv[2];
      }
    }
  }


  // Detect the probabilities
  vil_image_view<float> probs(ni,nj,2);
  float uniform_val = 1.0f;
  // since the pixel values are in [0.0, 1.0] the uniform distribution has
  // probability density 1.0 which contributes -log(1.0) = 0 to the error term
  // hence only two images of non-zero error terms need to be computed

  vil_image_view<float> p0(vil_plane(probs,0));
  vil_image_view<float> p1(vil_plane(probs,1));

  typedef bsta_prob_density_functor<gauss_type> prob_functor;
  typedef bsta_top_weight_functor<mix_gauss_type,prob_functor> top_prob_functor;
  prob_functor pf;
  top_prob_functor probs_func(pf, 0.7);
  bbgm_apply(*model,probs_func,img,p0,&uniform_val);
  if(!model_shadow)
    p1.fill(1.0);
  else
    bbgm_apply(*model_shadow,probs_func,img,p1,&uniform_val);


  vil_transform(probs,vil_math_log_functor());
  vil_math_scale_values(probs,-1.0);

  error_images_.push_back(probs);

  return true;
}



bool
modrec_position_process::finish()
{
  // get the process parameters
  float hour, lat, lon, heading;
  bool yuv, debug=false;
  bpro1_filepath param_file, model_file;
  int day=0;
  parameters()->get_value( "-hour" ,   hour);
  parameters()->get_value( "-params",  param_file);
  parameters()->get_value( "-model",   model_file);
  parameters()->get_value( "-yuv" ,    yuv);
  parameters()->get_value( "-debug" ,  debug );

  bxml_document xdoc = bxml_read(param_file.path);
  bxml_element query("param");
  query.set_attribute("name","camera");
  bxml_data_sptr result = bxml_find(xdoc.root_element(), query);

  vnl_double_3x4 camera;
  if(result){
    bxml_element* elm = static_cast<bxml_element*>(result.ptr());
    vcl_stringstream s(static_cast<bxml_text*>(elm->data_begin()->ptr())->data());
    s >> camera;
  }
  else{
    vcl_cout << "error reading \"camera\" from XML" <<vcl_endl;
    return false;
  }

  query.set_attribute("name","lens");
  result = bxml_find(xdoc.root_element(), query);

  double k[4];
  if(result){
    bxml_element* elm = static_cast<bxml_element*>(result.ptr());
    vcl_stringstream s(static_cast<bxml_text*>(elm->data_begin()->ptr())->data());
    s >> k[0] >> k[1] >> k[2] >> k[3];
  }
  else{
    vcl_cout << "error reading \"lens\" from XML" <<vcl_endl;
    return false;
  }

  float starttime;
  if(!modrec_pro_read_param(xdoc,"starttime",starttime)){
    vcl_cout << "error reading \"starttime\" from XML" <<vcl_endl;
    return false;
  }
  hour += starttime;

  if(!modrec_pro_read_param(xdoc,"date",day)){
    vcl_cout << "error reading \"date\" from XML" <<vcl_endl;
    return false;
  }

  if(!modrec_pro_read_param(xdoc,"lat",lat)){
    vcl_cout << "error reading \"lat\" from XML" <<vcl_endl;
    return false;
  }

  if(!modrec_pro_read_param(xdoc,"lon",lon)){
    vcl_cout << "error reading \"lon\" from XML" <<vcl_endl;
    return false;
  }

  if(!modrec_pro_read_param(xdoc,"heading",heading)){
    vcl_cout << "error reading \"heading\" from XML" <<vcl_endl;
    return false;
  }

  imesh_mesh mesh;
  if(!imesh_read_ply2(model_file.path,mesh))
    return false;


  // compute the normals
  if(!mesh.faces().has_normals())
    mesh.compute_face_normals();

  const unsigned int ni = error_images_[0].ni();
  const unsigned int nj = error_images_[0].nj();

  // determine BAE camera scale matrix from image dimensions
  vnl_double_3x3 A(0.0);
  A[0][2] = ni/2.0;
  A[1][2] = nj/2.0;
  A[0][0] = A[1][1] = A[0][2];
  A[2][2] = 1.0;

  // scale the camera
  camera = A * camera;

  // create the lens
  double rad_scale = A[0][0];
  k[0] /= rad_scale; rad_scale *= A[0][0];
  k[1] /= rad_scale; rad_scale *= A[0][0];
  k[2] /= rad_scale; rad_scale *= A[0][0];
  k[3] /= rad_scale;
  vgl_point_2d<double> center(A[0][2],A[1][2]);
  vpgl_poly_radial_distortion<double,4> lens(center,k);

  // compute the solar direction vector [xa, ya]
  double alt, az;
  dbul_solar_position(day, hour , lat, lon, alt, az);
  az = heading*3.1415926/180.0 - az;
  double xa = -vcl_cos(az)/vcl_tan(alt), ya = -vcl_sin(az)/vcl_tan(alt);


  float scale, f_tol, initx, inity, initvx;
  int max_eval;
  parameters()->get_value( "-scale" ,    scale);
  parameters()->get_value( "-max_eval" , max_eval);
  parameters()->get_value( "-f_tol" ,    f_tol);
  parameters()->get_value( "-initx" ,    initx);
  parameters()->get_value( "-inity" ,    inity);
  parameters()->get_value( "-initvx" ,   initvx);

  // the simplex minimizer
  vnl_vector<double> x(3); x(0)=initx; x(1)=inity; x(2)=initvx;
  vnl_vector<double> ds(3); ds(0)=10*scale; ds(1)=scale; ds(2)=scale;

  modrec_motion_cost_func func(error_images_, camera, lens, mesh, xa, ya);
  vnl_amoeba a(func);
  a.verbose = debug;
  a.set_max_iterations(max_eval);
  a.set_f_tolerance(f_tol);
  a.minimize(x,ds);
  double min_error = func.f(x);

  vcl_cout << "x="<<x(0)<< " y="<<x(1)<< " v="<<x(2)<<"  error="<<min_error<<vcl_endl;

  unsigned int t0 = 0;
  for(; t0<input_data_.size(); ++t0){
    if(!input_data_[t0].empty())
      break;
  }
  for(unsigned int t=t0; t<input_data_.size(); ++t){
    vidpro1_image_storage_sptr frame_image;
    frame_image.vertical_cast(input_data_[t][3]);
    output_data_[t].push_back(frame_image);
    vil_image_resource_sptr image_rsc = frame_image->get_image();
    vil_image_view<float> img = image_rsc->get_view();

    // compute the x and y position
    double xp = x(0) + (t-t0)*x(2);
    double yp = x(1);
    // shift the camera to account for vehicle motion
    vnl_double_3x4 cam(camera);
    cam.set_column(3, xp*camera.get_column(0) +
                      yp*camera.get_column(1) + camera.get_column(3));

    // create a degenerate camera to project the vehicle shadow
    vnl_double_3x4 shadow_cam(cam);
    shadow_cam.set_column(2,xa*cam.get_column(0)+ya*cam.get_column(1));

    vil_image_view<bool> mask(ni,nj), shadow_mask(ni,nj);
    mask.fill(false);
    shadow_mask.fill(false);

    imesh_project(mesh,mesh.faces().normals(),vpgl_proj_camera<double>(cam),lens,mask);
    imesh_project(mesh,mesh.faces().normals(),vpgl_proj_camera<double>(shadow_cam),
                  lens,shadow_mask);

    vil_image_view<vxl_byte> label_image(ni,nj);
    vidpro1_image_storage_sptr label_stg = vidpro1_image_storage_new();
    label_stg->set_image(vil_new_image_resource_of_view( label_image ));
    output_data_[t].push_back(label_stg);

    for(unsigned int i=0; i<ni; ++i){
      for(unsigned int j=0; j<nj; ++j){
        if(mask(i,j) ) label_image(i,j) = 2;
        else if(shadow_mask(i,j)) label_image(i,j) = 1;
        else label_image(i,j) = 0;
      }
    }

    float min,max;
    vil_math_value_range(img,min,max);
    for(unsigned int i=0; i<ni; ++i){
      for(unsigned int j=0; j<nj; ++j){
        img(i,j,0) = img(i,j,0)*0.75f;
        img(i,j,1) = img(i,j,1)*0.75f;
        img(i,j,2) = img(i,j,2)*0.75f;
        if(mask(i,j)){
          img(i,j,2) += 0.25f;
          shadow_mask(i,j) = false;
        }
        else if(shadow_mask(i,j)) img(i,j,1) += 0.25f;
        else img(i,j,0) += 0.25f;
      }
    }

    vcl_vector<vgl_point_2d<double> > verts2d;
    vpgl_proj_camera<double> pcam(cam);
    imesh_project_verts(mesh.vertices<3>(), pcam, verts2d);
    imesh_distort_verts(verts2d, lens, verts2d);
    vcl_vector<vsol_point_2d_sptr> vsol_pts;
    for(unsigned int k=0; k<verts2d.size(); ++k)
      vsol_pts.push_back(new vsol_point_2d(verts2d[k]));

    vidpro1_vsol2D_storage_sptr output_storage = vidpro1_vsol2D_storage_new();
    output_data_[t].push_back(output_storage);

    vgl_homg_point_3d<double> c(pcam.camera_center());
    if(c.w() < 0.0)
      c.rescale_w(-c.w());
    typedef imesh_regular_face_array<3>::const_iterator itr_t;
    typedef vcl_vector<vgl_vector_3d<double> >::const_iterator itr_n;
    itr_n n = mesh.faces().normals().begin();
    const imesh_regular_face_array<3>& tris =
        static_cast<const imesh_regular_face_array<3>&>(mesh.faces());

    const imesh_vertex_array<3>& verts3d = mesh.vertices<3>();
    for(itr_t itr = tris.begin(); itr != tris.end(); ++itr, ++n){
      const vgl_point_3d<double>& v1 = verts3d[(*itr)[0]];
      vgl_vector_3d<double> d(c.x()-v1.x()*c.w(),
                              c.y()-v1.y()*c.w(),
                              c.z()-v1.z()*c.w());
      if(dot_product(*n,d) < 0.0)
        continue;
      output_storage->add_object(new vsol_triangle_2d(vsol_pts[(*itr)[0]],
                                                      vsol_pts[(*itr)[1]],
                                                      vsol_pts[(*itr)[2]]));

    }

    vidpro1_image_storage_sptr output_image = vidpro1_image_storage_new();
    output_image->set_image(vil_new_image_resource_of_view( shadow_mask ));
    output_data_[t].push_back(output_image);
  }

  error_images_.clear();

  return true;
}




