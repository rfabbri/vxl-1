#include <cali/cali_simulated_scan_resource.h>
#include <imgr/imgr_bounded_image_view_3d.h>
#include <vil/vil_save.h>
#include <vil/vil_new.h>
#include <vil/vil_image_view_base.h>
#include <vnl/vnl_quaternion.h>
#include <vnl/vnl_double_3.h>
#include <vcl_iostream.h>
#include <vnl/vnl_math.h>



  cali_simulated_scan_resource::cali_simulated_scan_resource(xscan_scan const& scan, 
                                                           vcl_vector<vil_image_resource_sptr> const& resources,cali_param par,
                                                           vcl_vector<double> x_coord_tol,vcl_vector<double> y_coord_tol,
                                                           vcl_vector<double> z_coord_tol,vcl_vector<double> rad_tol)
                                                           : imgr_scan_resource(scan, resources),par_(par) {
                                                          
    // create a cylinder artifact
    vgl_point_3d<double> artf_center(0.0,0.0,0.0);
    artifact_ = new cali_cylinder_artifact(artf_center,par,x_coord_tol,y_coord_tol,z_coord_tol,rad_tol);
   // artifact_ = new cali_cylinder_artifact(artf_center, ball_indent_perc, ball_indent_big, ball_distance);
    projector_ = new cali_artf_projector(artifact_,par);
    // set the image format to btye, since we are going to create grey-scale images
    format_ = VIL_PIXEL_FORMAT_BYTE;
}


cali_simulated_scan_resource::cali_simulated_scan_resource(xscan_scan const& scan, 
                                                           vcl_vector<vil_image_resource_sptr> const& resources)
                                                           : imgr_scan_resource(scan, resources) {
                                                           
    // create a cylinder artifact
    vgl_point_3d<double> artf_center(0.0,0.0,0.0);
    artifact_ = new cali_cylinder_artifact(artf_center);
   // artifact_ = new cali_cylinder_artifact(artf_center, ball_indent_perc, ball_indent_big, ball_distance);
    projector_ = new cali_artf_projector(artifact_);
    // set the image format to btye, since we are going to create grey-scale images
    format_ = VIL_PIXEL_FORMAT_BYTE;
}

cali_simulated_scan_resource::cali_simulated_scan_resource(xscan_scan const& scan, 
                                                           vcl_vector<vil_image_resource_sptr> const& resources,cali_param par)
                                                           : imgr_scan_resource(scan, resources),par_(par) {
                                                          
    // create a cylinder artifact
    vgl_point_3d<double> artf_center(0.0,0.0,0.0);
    artifact_ = new cali_cylinder_artifact(artf_center,par);
   // artifact_ = new cali_cylinder_artifact(artf_center, ball_indent_perc, ball_indent_big, ball_distance);
    projector_ = new cali_artf_projector(artifact_,par);
    // set the image format to btye, since we are going to create grey-scale images
    format_ = VIL_PIXEL_FORMAT_BYTE;
}
cali_simulated_scan_resource::cali_simulated_scan_resource(xscan_scan const& scan, 
                                                           vcl_vector<vil_image_resource_sptr> const& resources,vnl_quaternion<double>const& rot,vgl_point_3d<double> trans)
                                                           : imgr_scan_resource(scan, resources) {
                                                           
    // create a cylinder artifact
    vgl_point_3d<double> artf_center(0.0,0.0,0.0);
    artifact_ = new cali_cylinder_artifact(artf_center);
    artifact_->change_position(rot,trans);
   // artifact_ = new cali_cylinder_artifact(artf_center, ball_indent_perc, ball_indent_big, ball_distance);
    projector_ = new cali_artf_projector(artifact_);
    // set the image format to btye, since we are going to create grey-scale images
    format_ = VIL_PIXEL_FORMAT_BYTE;
}

// simulate the scan by giving some transformation parameters for the artifact,
// interval is the number of scan position skipped at every ith position to 
//  generate a projection
vcl_vector<vil_image_resource_sptr> 
cali_simulated_scan_resource::simulate_scan(char* folder_name, char* file_name,
                                                 vnl_quaternion<double> artf_rot,
                                                 vgl_point_3d<double> artf_trans = vgl_point_3d<double>(0.0,0.0,0.0), 
                                                 int interval=1) {
    // position the artifact with the given transformation parameters
    artifact_->change_position(artf_rot, artf_trans);
  
    // get the bounding box of the artifact
    vgl_box_3d<double> bbox_3d = artifact_->bounding_box();
    // get the views created based on the bounded box of the artifact
    imgr_image_view_3d_base_sptr views_ptr = get_bounded_view(bbox_3d);
    imgr_bounded_image_view_3d<unsigned char> views= views_ptr;
    // generate the artifact images for the views
    return gen_artifact_views(views, bbox_3d, folder_name, file_name, interval);
}


// generates artifact views, by given intervals, e.g. if interval is 10 only every 10th 
// view is generated on view set. like 0th, 9th, 19th etc.
vcl_vector<vil_image_resource_sptr>
cali_simulated_scan_resource::gen_artifact_views(imgr_bounded_image_view_3d<unsigned char> views, 
                                                 vgl_box_3d<double> const& bounds,
                                                 char* folder_name, char* file_name,
                                                 int interval=1)
{

    vcl_vector<vil_image_resource_sptr> result_set;
    
    for (int k = par_.START; k < par_.END ; k += interval) {  
        dbil_bounded_image_view<unsigned char> view = views.view_2d(k);
        // after projection, the image is now filled with values
        vil_image_resource_sptr img_resource = projector_->build_projection_img(scan_(k), view);
        // save the images
        vcl_string fname = gen_file_name(folder_name, file_name, k);
        vil_save_image_resource(img_resource, fname.c_str());
        vcl_cout << "Generated:" << fname.c_str() << vcl_endl;
        //vil_image_resource_sptr img_resource = vil_new_image_resource_of_view(img);
        result_set.push_back(img_resource); 
    }
    //return new imgr_bounded_image_view_3d<unsigned char>(result_set, bounds);
    return result_set;
}

// generates artifact ball center views, by given intervals, e.g. if interval is 10 only every 10th 
// view is generated on view set. like 0th, 9th, 19th etc.
vcl_vector<vil_image_resource_sptr>
cali_simulated_scan_resource::gen_artifact_center_views(imgr_bounded_image_view_3d<unsigned char> views, 
                                                 vgl_box_3d<double> const& bounds,
                                                 //vnl_quaternion<double> artf_rot, 
                                                 //vgl_point_3d<double> artf_trans,
                                                 char* folder_name, char* file_name,
                                                 int interval=1)
{

    vcl_vector<vil_image_resource_sptr> result_set;
    
    for (int k = par_.START; k < par_.END ; k += interval) {  
        dbil_bounded_image_view<unsigned char> view = views.view_2d(k);
        // after projection, the image is now filled with values
        vil_image_resource_sptr img_resource = projector_->build_centers_projection_img(scan_(k), view);
        // save the images
        vcl_string fname = gen_file_name(folder_name, file_name, k);
        vil_save_image_resource(img_resource, fname.c_str());
        vcl_cout << "Generated:" << fname.c_str() << vcl_endl;
        //vil_image_resource_sptr img_resource = vil_new_image_resource_of_view(img);
        result_set.push_back(img_resource);
       
        
    }
    //return new imgr_bounded_image_view_3d<unsigned char>(result_set, bounds);
    return result_set;
}

//: returns the projected ball center points for each scan position 
vcl_vector<vcl_vector<vsol_conic_2d > >
cali_simulated_scan_resource::gen_ball_projections(vnl_quaternion<double> artf_rot, 
                                                    vgl_point_3d<double> artf_trans,
                                                    int interval)
{
   
  vcl_vector<vcl_vector<vsol_conic_2d > > ball_centers_list;
  // position the artifact with the given transformation parameters
  artifact_->change_position(artf_rot, artf_trans);
  for (int k = par_.START; k < par_.END ; k += interval) {
        vcl_vector<vsol_conic_2d > ball_centers = projector_->build_ball_projections(scan_(k));

        
        /*vcl_cout << k << " BALL CENTERS--> " <<vcl_endl;
        for (int g=0; g<ball_centers.size(); g++)
          vcl_cout << ball_centers[g] << vcl_endl;*/
        
        ball_centers_list.push_back(ball_centers);
        

  }
    //return new imgr_bounded_image_view_3d<unsigned char>(result_set, bounds);
    return ball_centers_list;  
}

// generates file names with a sequence number i, adds the extension jpg
vcl_string
cali_simulated_scan_resource::gen_file_name(char* folder, char *file, int i)
{
  char* num = new char[4];
  num[0] = num[1] = num[2] = num[3] = '0';
  char imgfileroot[512];
  vcl_strcpy(imgfileroot, folder);
  vcl_strcat(imgfileroot, "/");
  vcl_strcat(imgfileroot, file);
  vcl_string imgfile;
  
  sprintf (num, "%d", i);
  //itoa(i,num,10);
  imgfile.assign(imgfileroot);
  imgfile.append("0000", 4);

  vcl_string num_str = num;
  imgfile.replace(imgfile.size()-num_str.size(), num_str.size(), num);
  //vcl_strncat(imgfile, num, 4);
  imgfile.append(".tif");
  return imgfile;
}

