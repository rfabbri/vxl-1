// This is contrib/mleotta/modrec/pro/modrec_classify_process.cxx

//:
// \file

#include "modrec_classify_process.h"
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
#include <imesh/algo/imesh_project.h>
#include <modrec/modrec_proj_cost_func.h>

#include <vidpro1/storage/vidpro1_vsol2D_storage.h>
#include <vsol/vsol_point_2d.h>
#include <vsol/vsol_triangle_2d.h>

#include <vnl/algo/vnl_amoeba.h>

#include <vil/algo/vil_colour_space.h>
#include <vul/vul_file.h>
#include <bxml/bxml_read.h>
#include <bxml/bxml_find.h>

#include "modrec_pro_utils.h"




//: Constructor
modrec_classify_process::modrec_classify_process()
{
  if( !parameters()->add( "hour offset" ,      "-hour" ,    0.0f      ) ||
      !parameters()->add( "Scale" ,            "-scale" ,   1.0f   ) ||
      !parameters()->add( "Max Iterations" ,   "-max_eval", 200   ) ||
      !parameters()->add( "F Tolerance" ,      "-f_tol" ,   1.0f   ) ||
      !parameters()->add( "Init X" ,           "-initx" ,   -400.0f   ) ||
      !parameters()->add( "Init Y" ,           "-inity" ,   20.0f   ) ||
      !parameters()->add( "Init Vx" ,          "-initvx" ,  20.0f   ) ||
      !parameters()->add( "Calibration Params","-params" ,  bpro1_filepath("","*")  ) ||
      !parameters()->add( "Models File" ,      "-models" ,  bpro1_filepath("","*")  ) ||
      !parameters()->add( "YUV Color Space" ,  "-yuv",      false ) ||
      !parameters()->add( "mask foreground" ,  "-mask_fg",  false )  ||
      !parameters()->add( "debug" ,            "-debug",    false )   ){
    vcl_cerr << "ERROR: Adding parameters in " __FILE__<< vcl_endl;
  }
}


//: Destructor
modrec_classify_process::~modrec_classify_process()
{
}


//: Clone the process
bpro1_process* 
modrec_classify_process::clone() const
{
  return new modrec_classify_process(*this);
}


//: Return the name of this process
vcl_string
modrec_classify_process::name()
{
  return "Model Classify";
}


//: Return the number of input frame for this process
int
modrec_classify_process::input_frames()
{
  return 1;
}


//: Return the number of output frames for this process
int
modrec_classify_process::output_frames()
{
  return 1;
}


//: Provide a vector of required input types
vcl_vector< vcl_string > modrec_classify_process::get_input_type()
{
  vcl_vector< vcl_string > to_return;
  to_return.push_back( "image" );
  to_return.push_back( "bbgm_image" );
  to_return.push_back( "bbgm_image" );

  return to_return;
}


//: Provide a vector of output types
vcl_vector< vcl_string > modrec_classify_process::get_output_type()
{
  vcl_vector<vcl_string > to_return;
  to_return.push_back( "image" );
  bool debug=false;
  bpro1_filepath models_file;
  parameters()->get_value( "-debug" ,  debug );
  parameters()->get_value( "-models", models_file);

  if(debug){
    modrec_pro_read_models(models_file.path, mesh_, mesh_names_);
    for(unsigned int i=0; i<mesh_names_.size(); ++i)
      to_return.push_back( "image" );
  }
  return to_return;
}

//: Returns a vector of strings with suggested names for output classes
vcl_vector< vcl_string > modrec_classify_process::suggest_output_names()
{
  vcl_vector< vcl_string > names;
  names.push_back("state labels");

  bool debug=false;
  bpro1_filepath models_file;
  parameters()->get_value( "-debug" ,  debug );
  parameters()->get_value( "-models", models_file);
  if(debug){
    if(mesh_.empty())
      modrec_pro_read_models(models_file.path, mesh_, mesh_names_);
    for(unsigned int i=0; i<mesh_names_.size(); ++i)
      names.push_back( mesh_names_[i] );
  }
  return names;
}


//: Execute the process
bool
modrec_classify_process::execute()
{
  if ( input_data_.size() != 1 ){
    vcl_cout << "In modrec_classify_process::execute() - "
             << "not exactly one input image \n";
    return false;
  }
  clear_output();

  // get the process parameters 
  bool yuv=false, mask_fg=false, debug=false;
  parameters()->get_value( "-yuv" ,   yuv);
  parameters()->get_value( "-mask_fg", mask_fg );
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


  vil_image_view<vxl_byte> label_image(ni,nj);
  label_image.fill(1);
  vidpro1_image_storage_sptr label_stg = vidpro1_image_storage_new();
  label_stg->set_image(vil_new_image_resource_of_view( label_image ));
  output_data_[0].push_back(label_stg);


  if(debug){
    for(unsigned int i=0; i<mesh_names_.size(); ++i){
      vil_image_view<float> new_img; new_img.deep_copy(img);
      vidpro1_image_storage_sptr output_storage = vidpro1_image_storage_new();
      output_storage->set_image(vil_new_image_resource_of_view( new_img ));
      output_data_[0].push_back(output_storage);
    }
  }

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

  if(mask_fg){
    bsta_top_weight_detector<mix_gauss_type, bsta_g_mdist_detector<bsta_gauss_if3> >
        bg_detector(bsta_g_mdist_detector<bsta_gauss_if3>(2.5f), 0.7f);

    vil_image_view<bool> mask(ni,nj);
    mask.fill(false);
    bbgm_apply(*model,bg_detector,img,mask);


    for(unsigned int i=0; i<ni; ++i){
      for(unsigned int j=0; j<nj; ++j){
        if(mask(i,j))
          label_image(i,j)=0;
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
modrec_classify_process::finish()
{

  // get the process parameters
  float hour, lat, lon, heading;
  bool yuv, debug=false;
  bpro1_filepath param_file, models_file;
  int day=0;
  parameters()->get_value( "-hour" ,   hour);
  parameters()->get_value( "-params",  param_file);
  parameters()->get_value( "-models",  models_file);
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

  // read the models
  modrec_pro_read_models(models_file.path, mesh_, mesh_names_);
  if(mesh_.empty()){
    vcl_cerr << "Error: no mesh files loaded" <<vcl_endl;
    return false;
  }


  // compute the normals
  for(unsigned int i=0; i<mesh_.size(); ++i){
    if(!mesh_[i].faces().has_normals())
      mesh_[i].compute_face_normals();
  }

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
  vnl_vector<double> x1(3); x1(0)=initx; x1(1)=inity; x1(2)=initvx;
  vnl_vector<double> ds(3); ds(0)=10*scale; ds(1)=scale; ds(2)=scale;
  vcl_vector<double> min_error(mesh_.size());
  unsigned int best_ind = 0;
  double best_error = 0;
  {
    modrec_motion_cost_func func(error_images_, camera, lens, mesh_[0], xa, ya);
    vnl_amoeba a(func);
    a.verbose = debug;
    a.set_max_iterations(max_eval);
    a.set_f_tolerance(f_tol);
    a.minimize(x1,ds);
    min_error[0] = func.f(x1);
    best_error = min_error[0];
  }

  vcl_vector<vnl_vector<double> > x(mesh_.size(),x1);
  for(unsigned int i=1; i<mesh_.size(); ++i){
    modrec_motion_cost_func func(error_images_, camera, lens, mesh_[i], xa, ya);
    vnl_amoeba a(func);
    a.verbose = debug;
    a.set_max_iterations(max_eval);
    a.set_f_tolerance(f_tol);
    a.minimize(x[i],ds);
    min_error[i] = func.f(x[i]);
    if(min_error[i] < best_error){
      best_ind = i;
      best_error = min_error[i];
    }
  }

  vcl_cout << "best model: "<< mesh_names_[best_ind] << vcl_endl;

  for(unsigned int i=0; i<mesh_.size(); ++i){
    vcl_cout << mesh_names_[i]<<": x="<<x[i](0)<< " y="<<x[i](1)<< " v="<<x[i](2)
             <<"  error="<<min_error[i]<<vcl_endl;
  }


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
    vil_image_view<vxl_byte> label_image = image_rsc->get_view();

    // compute the x and y position
    double xp = x[best_ind](0) + (t-t0)*x[best_ind](2);
    double yp = x[best_ind](1);
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

    imesh_project(mesh_[best_ind],mesh_[best_ind].faces().normals(),vpgl_proj_camera<double>(cam),lens,mask);
    imesh_project(mesh_[best_ind],mesh_[best_ind].faces().normals(),vpgl_proj_camera<double>(shadow_cam),
                        lens,shadow_mask);

    for(unsigned int i=0; i<ni; ++i){
      for(unsigned int j=0; j<nj; ++j){
        if(mask(i,j) && label_image(i,j)) label_image(i,j) = 2;
        else if(shadow_mask(i,j) && label_image(i,j)) label_image(i,j) = 1;
        else label_image(i,j) = 0;
      }
    }

    if(debug){
      for(unsigned int m=0; m<mesh_.size(); ++m){
        frame_image.vertical_cast(input_data_[t][4+m]);
        output_data_[t].push_back(frame_image);

        // cast the image to float
        vil_image_resource_sptr image_rsc = frame_image->get_image();
        vil_image_view<float> img = image_rsc->get_view();

        double xp = x[m](0) + (t-t0)*x[m](2);
        double yp = x[m](1);
        vnl_double_3x4 cam(camera);
        cam.set_column(3, xp*camera.get_column(0) +
                          yp*camera.get_column(1) + camera.get_column(3));

        vnl_double_3x4 shadow_cam(cam);
        shadow_cam.set_column(2,xa*cam.get_column(0)+ya*cam.get_column(1));

        vil_image_view<bool> mask(img.ni(),img.nj()), shadow_mask(mask.ni(),mask.nj());
        mask.fill(false);
        shadow_mask.fill(false);

        imesh_project(mesh_[m],mesh_[m].faces().normals(),vpgl_proj_camera<double>(cam),lens,mask);
        imesh_project(mesh_[m],mesh_[m].faces().normals(),vpgl_proj_camera<double>(shadow_cam),
                            lens,shadow_mask);

        float min,max;
        vil_math_value_range(img,min,max);
        for(unsigned int i=0; i<ni; ++i){
          for(unsigned int j=0; j<nj; ++j){
            img(i,j,0) = img(i,j,0)*0.75f;
            img(i,j,1) = img(i,j,1)*0.75f;
            img(i,j,2) = img(i,j,2)*0.75f;
            if(mask(i,j)) img(i,j,2) += 0.25f;
            else if(shadow_mask(i,j)) img(i,j,1) += 0.25f;
            else img(i,j,0) += 0.25f;
          }
        }
      }
    }
  }

  error_images_.clear();

  return true;
}




