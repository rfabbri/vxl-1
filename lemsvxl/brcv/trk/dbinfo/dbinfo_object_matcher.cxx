#include <vul/vul_timer.h> 
#include <vil/vil_new.h>
#include <vil/vil_save.h>
#include <brip/brip_vil_float_ops.h>

#include <vsol/vsol_point_2d.h>

#include <dbinfo/dbinfo_observation.h>
#include <dbinfo/dbinfo_observation_matcher.h>
#include <dbinfo/dbinfo_observation_generator.h>
#include <dbinfo/dbinfo_match_optimizer.h>
#include <dbinfo/dbinfo_intensity_feature.h>
#include <dbinfo/dbinfo_object_matcher.h>


//: make an image from observation data
vil_image_resource_sptr 
dbinfo_object_matcher::make_image(dbinfo_observation_sptr obs, int w, int h, double translation_x, double translation_y, bool background_noise) 
{
  float i0 = static_cast<float>(translation_x),
    j0 = static_cast<float>(translation_y);
  unsigned ni = static_cast<unsigned>(w), nj = static_cast<unsigned>(h);
  dbinfo_region_geometry_sptr geo = obs->geometry();
  dbinfo_feature_base_sptr feat = (obs->features())[0];
  if(feat->format()!=DBINFO_INTENSITY_FEATURE)
    return 0;
  vcl_vector<vgl_point_2d<float> > points = geo->points();
  vcl_vector<bool> valid = geo->masks();
  return feat->image(points, valid, ni, nj, i0, j0, background_noise);
}

//: make a color image that shows the alignment of the query with
//  the database image.  final_obs is the aligned query. xform is the
//  transform that aligned the query with the database observation.
vil_image_resource_sptr 
dbinfo_object_matcher::make_alignment_image(vil_image_resource_sptr const& image_db, dbinfo_observation_sptr const&  final_obs)
{
  if(!image_db||!final_obs)
    return 0;
  dbinfo_region_geometry_sptr fgeo = final_obs->geometry();  
  unsigned fw = fgeo->cols(), fh = fgeo->rows();
  vil_image_resource_sptr fimage = make_image(final_obs, fw, fh, 0, 0, false); 
  vil_image_view<vil_rgb<vxl_byte> > match = 
    brip_vil_float_ops::combine_color_planes(image_db, fimage, fimage);
  return vil_new_image_resource_of_view(match);
}

//: find the rigid transformation that aligns a query observation, obsq, to 
//  a database observation, obdb, such that mutual information is maximized
//  and return this max mutual info.
//  This function is a debug version to visualize the final optimized 
//  observations. 
//  ratio defines a circle with radius ratio/2 times the query diameter 
//  to be randomly searched in aligning the query observation with the
//  database observation prior to optimization.
float 
dbinfo_object_matcher::minfo_rigid_alignment_rand(dbinfo_observation_sptr obsq, 
                           dbinfo_observation_sptr obsdb, 
                           float dx, float dr, float ds, float ratio, unsigned Nob,
                           vil_image_resource_sptr& image_q,
                           vil_image_resource_sptr& image_rdb,
                           vil_image_resource_sptr& image_match,
                           bool verbose,
                           float forced_scale,
                           bool use_intensity,
                           bool use_gradient) 
{
  // make an image at the size of database observations's image, blank initially,
  // but dump pixel values from query observation such that its center of gravity 
  // coincides with database observation's center of gravity on the image.
  // obsq: query, obsdb: database
  vul_timer t;

  dbinfo_region_geometry_sptr geo_db = obsdb->geometry();
  dbinfo_region_geometry_sptr geo_q = obsq->geometry();

  //double radius_db = geo_db->diameter()/2.0f;
  //double radius_q = geo_q->diameter()/2.0f;
  vsol_point_2d_sptr cent_db = geo_db->centroid();
  vsol_point_2d_sptr cent_q = geo_q->centroid();
  vcl_cout << "center of db obs: " << *cent_db << " center of query: " << *cent_q << "\n";

  int w = geo_db->cols();
  int h = geo_db->rows();
  
  image_q = make_image(obsq, w, h, (-cent_q->x()+cent_db->x()), (-cent_q->y()+cent_db->y()));
  vcl_cout << "making the image of database observation also\n";
  image_rdb = make_image(obsdb, w, h, 0, 0);

  vgl_h_matrix_2d<float> H;  H.set_identity();
  dbinfo_observation_sptr initial_obs = dbinfo_observation_generator::generate(obsdb, H);
  initial_obs->scan(0, image_q);
  //image_rdb = make_image(initial_obs, w, h, 0, 0, true);
  float initial_info = dbinfo_observation_matcher::minfo(obsdb, initial_obs,
                                                         use_intensity,
                                                         use_gradient,
                                                         false);
  vcl_cout << "before random iterations info is: " << initial_info << vcl_endl;
  
  vcl_cout << " size of query obs: " << geo_q->size() << vcl_endl;
  // before optimizing randomly search for a good starting point
  // we know database cog should align with some point "on" the query,
  // so pick points randomly inside the query Nob times
  float max_info = initial_info;
  float radius_ok = ratio*(float(geo_q->diameter())/2.0f);

  //randomly search for the best match
  vcl_vector<dbinfo_observation_sptr> obss;
  vcl_vector<vgl_h_matrix_2d<float> > xforms;
  bool success = 
    dbinfo_observation_generator::uniform_about_seed(Nob, obsdb, obss, 
                                                     xforms,
                                                     radius_ok,
                                                     radius_ok,
                                                     dr, ds);
  unsigned no_obs=obss.size();
  vcl_cout << "Constructed " << no_obs << " valid observations\n";
  vgl_h_matrix_2d<float> best_trans;
  for(unsigned i = 0; i<no_obs; ++i)
  {
  dbinfo_observation_sptr ob = obss[i];
  if(!ob)
   continue;
  ob->scan(0, image_q);
        float mi = dbinfo_observation_matcher::minfo(obsdb, ob,
                                                     use_intensity,
                                                     use_gradient,
                                                     false);
        if(mi>max_info)
          {
            initial_obs = ob;
            max_info = mi;
            best_trans = xforms[i];
          }
      }
  
  vcl_cout << "after " << no_obs << " random samples the best info is "  
           << max_info << " time: " << t.real()/1000.0f << " seconds.\n";
  vcl_cout << "The transform is\n";
  vcl_cout << best_trans << '\n';

  image_match = make_alignment_image(image_rdb, initial_obs);
  return initial_info>max_info?initial_info:max_info;
}

//: find the rigid transformation that aligns observation0 to 
// observationi such that mutual information is maximised and 
// return this max mutual info
// this function is to visualize the final optimized observations
// ratio is the ratio of pixels to be randomly searched in query observation
// prior to optimization to find a better initial placement 
float 
dbinfo_object_matcher::minfo_rigid_alignment_rand(dbinfo_observation_sptr obsq, 
                           dbinfo_observation_sptr obsdb, 
                           vgl_h_matrix_2d<float> & best_trans,
                           float dx, float dr, float ds, float da,
                           float ratio, float valid_thresh, unsigned Nob,
                           vil_image_resource_sptr& image_match,
                           bool verbose,
                           float forced_scale,
                           bool use_intensity,
                           bool use_gradient)
{
  vul_timer t;
  dbinfo_region_geometry_sptr geo_db = obsdb->geometry();
  dbinfo_region_geometry_sptr geo_q = obsq->geometry();
  vsol_point_2d_sptr cent_db = geo_db->centroid();
  vsol_point_2d_sptr cent_q = geo_q->centroid();
  if(verbose)
    vcl_cout << "center of db obs: " << *cent_db << " center of query: " << *cent_q << "\n";
  int w = geo_db->cols();
  int h = geo_db->rows();
  
  vil_image_resource_sptr image_q = 
    make_image(obsq, w, h, (-cent_q->x()+cent_db->x()), (-cent_q->y()+cent_db->y()));
  vil_image_resource_sptr image_rdb = make_image(obsdb, w, h, 0, 0);

  vgl_h_matrix_2d<float> H;  H.set_identity();
  dbinfo_observation_sptr initial_obs =
      dbinfo_observation_generator::generate(obsdb, H, valid_thresh);
  initial_obs->scan(0, image_q);
  float initial_info = dbinfo_observation_matcher::minfo(obsdb, initial_obs,
                                                         use_intensity,
                                                         use_gradient,
                                                         false);
  if(verbose)
    vcl_cout << "before random iterations info is: " << initial_info 
             << vcl_endl << " size of query obs: " << geo_q->size() 
             << vcl_endl;
  // before optimizing randomly search for a good starting point
  // we know database cog should align with some point "on" the query,
  // so pick points randomly inside the query Nob times
  float max_info = initial_info;
  float radius_ok = ratio*(float(geo_q->diameter())/2.0f);

  //randomly search for the best match
  vcl_vector<dbinfo_observation_sptr> obss;
  vcl_vector<vgl_h_matrix_2d<float> > xforms;
  bool success = dbinfo_observation_generator::uniform_about_seed_affine_no_skew(Nob, obsdb, obss, 
                                                                    xforms,
                                                                    radius_ok,
                                                                    radius_ok,
                                                                    dr, ds, da,
                                                                    valid_thresh);

  unsigned no_obs=obss.size();
  
  if(verbose)
      vcl_cout << "Constructed " << no_obs << " valid observations\n";
  best_trans.set_identity();
  for(unsigned i = 0; i<no_obs; ++i)
  {
      dbinfo_observation_sptr ob = obss[i];
      if(!ob)
          continue;
      ob->scan(0, image_q);
      float mi = dbinfo_observation_matcher::minfo(obsdb, ob,use_intensity,use_gradient,false);
      //vcl_cout<<"MI: "<<mi<<"\t"<<xforms[i]<<"\n";
      if(mi>max_info)
      {
          initial_obs = ob;
          max_info = mi;
          best_trans = xforms[i];
      }
  }
  
  vcl_cout << "after " << no_obs << " random samples the best info is "  
           << max_info << " time: " << t.real()/1000.0f << " seconds.\n"
           << "The transform is\n"
           << best_trans << '\n';
  image_match = make_alignment_image(image_rdb, initial_obs);
  return initial_info>max_info?initial_info:max_info;
}

float 
dbinfo_object_matcher::minfo_alignment_in_interval(dbinfo_observation_sptr obsq, 
                            dbinfo_observation_sptr obsdb,
                            const float xmin, const float xmax,
                            const float ymin, const float ymax, 
                            const float theta_min, const float theta_max,
                            const float scale_min, const float scale_max,
                            const float aspect_min, const float aspect_max,
                            const unsigned n_intervals,
                            const float valid_thresh,
                            vil_image_resource_sptr& alignment_image)
{
    vul_timer t;
    bool verbose = true;
    dbinfo_region_geometry_sptr geo_db = obsdb->geometry();
    dbinfo_region_geometry_sptr geo_q = obsq->geometry();
    vsol_point_2d_sptr cent_db = geo_db->centroid();
    vsol_point_2d_sptr cent_q = geo_q->centroid();
    if(verbose)
        vcl_cout << "center of db obs: " << *cent_db << " center of query: " << *cent_q << "\n";
    int w = geo_db->cols();
    int h = geo_db->rows();

    vil_image_resource_sptr image_q = 
        make_image(obsq, w, h, (-cent_q->x()+cent_db->x()), (-cent_q->y()+cent_db->y()));
    //vil_image_resource_sptr image_rdb = make_image(obsdb, w, h, 0, 0);
    vil_image_resource_sptr image_rdb = make_image(obsdb, w, h, 0, 0, false);
    vgl_h_matrix_2d<float> H;  H.set_identity();
    dbinfo_observation_sptr initial_obs =
        dbinfo_observation_generator::generate(obsdb, H, valid_thresh);
    initial_obs->scan(0, image_q);

    float initial_info = dbinfo_observation_matcher::minfo(obsdb, initial_obs,
        true,
        true,
        false);
  //randomly search for the best match
  vcl_vector<dbinfo_observation_sptr> obss;
  vcl_vector<vgl_h_matrix_2d<float> > xforms;
  bool success = dbinfo_observation_generator::
    uniform_in_interval_affine_no_skew(n_intervals, obsdb, obss, xforms,
                                      xmin, xmax, ymin, ymax,
                                      theta_min, theta_max,
                                      scale_min, scale_max,
                                      aspect_min, aspect_max,
                                      valid_thresh);
  unsigned no_obs=obss.size();
  float max_info = initial_info;
  if(verbose)
    vcl_cout << "Constructed " << no_obs << " valid observations\n";
  vgl_h_matrix_2d<float> best_trans;
  for(unsigned i = 0; i<no_obs; ++i)
  {
  dbinfo_observation_sptr ob = obss[i];
  if(!ob)
   continue;
  ob->scan(0, image_q);
        float mi = dbinfo_observation_matcher::minfo(obsdb, ob,
                                                     true,
                                                     true,
                                                     false);
        vil_image_resource_sptr out_img = ob->image_cropped(false);
/*        char buffer[1000];
        sprintf(buffer, "%01f", xmin);
        vcl_string xmin_str = buffer;
        sprintf(buffer, "%01f", xmax);
        vcl_string xmax_str = buffer;
        sprintf(buffer, "%01f", theta_min);
        vcl_string themin = buffer;
        sprintf(buffer, "%01f", theta_max);
        vcl_string themax = buffer;
        sprintf(buffer, "%01f", scale_min);
        vcl_string scamin = buffer;
        sprintf(buffer, "%01f", scale_max);
        vcl_string scamax = buffer;
        sprintf(buffer, "%01f", aspect_min);
        vcl_string aspmin = buffer;
        sprintf(buffer, "%01f", aspect_max);
        vcl_string aspmax = buffer;
        vcl_string out_name = "d://projects//temp//img_"+xmin_str+"_"+xmax_str+"_"+themin+"_"+themax+"_"+scamin+"_"+scamax+"_"+aspmin+"_"+aspmax+".png";

        vil_save_image_resource(out_img, out_name.c_str());
        */
        if(mi>max_info)
          {
            initial_obs = ob;
            max_info = mi;
            best_trans = xforms[i];
          }

      }
  
  vcl_cout << "after " << no_obs << " random samples the best info is "  
           << max_info << " time: " << t.real()/1000.0f << " seconds.\n"
           << "The transform is\n"
           << best_trans << '\n';
  alignment_image = make_alignment_image(image_rdb, initial_obs);
  return initial_info>max_info?initial_info:max_info;
}


//  ozge: added to return best transformation
//: find the rigid transformation that aligns a query observation, obsq, to 
//  a database observation, obdb, such that mutual information is maximized
//  and return this max mutual info.
//  This function is a debug version to visualize the final optimized 
//  observations. 
//  ratio defines a circle with radius ratio/2 times the query diameter 
//  to be randomly searched in aligning the query observation with the
//  database observation prior to optimization.
float 
dbinfo_object_matcher::minfo_rigid_alignment_rand(dbinfo_observation_sptr obsq, 
                           dbinfo_observation_sptr obsdb, 
                           vgl_h_matrix_2d<float> & best_trans,
                           float dx, float dr, float ds, float ratio, unsigned Nob,
                           vil_image_resource_sptr& image_q,
                           vil_image_resource_sptr& image_rdb,
                           vil_image_resource_sptr& image_match,
                           bool verbose,
                           float forced_scale,
                           bool use_intensity,
                           bool use_gradient) 
{
  // make an image at the size of database observations's image, blank initially,
  // but dump pixel values from query observation such that its center of gravity 
  // coincides with database observation's center of gravity on the image.
  // obsq: query, obsdb: database
  vul_timer t;

  dbinfo_region_geometry_sptr geo_db = obsdb->geometry();
  dbinfo_region_geometry_sptr geo_q = obsq->geometry();

  //double radius_db = geo_db->diameter()/2.0f;
  //double radius_q = geo_q->diameter()/2.0f;
  vsol_point_2d_sptr cent_db = geo_db->centroid();
  vsol_point_2d_sptr cent_q = geo_q->centroid();
  vcl_cout << "center of db obs: " << *cent_db << " center of query: " << *cent_q << "\n";

  int w = geo_db->cols();
  int h = geo_db->rows();
  
  image_q = make_image(obsq, w, h, (-cent_q->x()+cent_db->x()), (-cent_q->y()+cent_db->y()));
  vcl_cout << "making the image of database observation also\n";
  //image_rdb = make_image(obsdb, w, h, 0, 0);

  vgl_h_matrix_2d<float> H;  H.set_identity(); H.set_scale(forced_scale);
  vcl_cout << "database diameter before transformation: " << obsdb->geometry()->diameter() << vcl_endl;
  dbinfo_observation_sptr initial_obs = dbinfo_observation_generator::generate(obsdb, H);
  vcl_cout << "database diameter after transformation: " << initial_obs->geometry()->diameter() << vcl_endl;
  initial_obs->scan(0, image_q);
  image_rdb = make_image(initial_obs, w, h, 0, 0);
  best_trans = H;
  float initial_info = dbinfo_observation_matcher::minfo(obsdb, initial_obs,
                                                         use_intensity,
                                                         use_gradient,
                                                         false);
  vcl_cout << "before random iterations info is: " << initial_info << vcl_endl;
  
  vcl_cout << " size of query obs: " << geo_q->size() << vcl_endl;
  // before optimizing randomly search for a good starting point
  // we know database cog should align with some point "on" the query,
  // so pick points randomly inside the query Nob times
  float max_info = initial_info;
  float radius_ok = ratio*(float(geo_q->diameter())/2.0f);

  //randomly search for the best match
  vcl_vector<dbinfo_observation_sptr> obss;
  vcl_vector<vgl_h_matrix_2d<float> > xforms;
  bool success = 
    dbinfo_observation_generator::uniform_about_seed(Nob, obsdb, obss, 
                                                     xforms,
                                                     radius_ok,
                                                     radius_ok,
                                                     dr, ds);
  unsigned no_obs=obss.size();
  vcl_cout << "Constructed " << no_obs << " valid observations\n";
  
  for(unsigned i = 0; i<no_obs; ++i)
  {
  dbinfo_observation_sptr ob = obss[i];
  if(!ob)
   continue;
  ob->scan(0, image_q);
        float mi = dbinfo_observation_matcher::minfo(obsdb, ob,
                                                     use_intensity,
                                                     use_gradient,
                                                     false);
        if(mi>max_info)
          {
            initial_obs = ob;
            max_info = mi;
            best_trans = xforms[i];
          }
      }
  
  vcl_cout << "after " << no_obs << " random samples the best info is "  
           << max_info << " time: " << t.real()/1000.0f << " seconds.\n";
  vcl_cout << "The transform is\n";
  vcl_cout << best_trans << '\n';

  image_match = make_alignment_image(image_rdb, initial_obs);
  return initial_info>max_info?initial_info:max_info;
}
