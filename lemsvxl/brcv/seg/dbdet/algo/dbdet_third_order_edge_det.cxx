//This is dbdet/edge/dbdet_third_order_edge_det.cxx

#include <dbdet/algo/dbdet_third_order_edge_det.h>
#include <vul/vul_timer.h>
#include <dbdet/algo/dbdet_edge_appearance_util.h>
#include <dbdet/algo/dbdet_edge_uncertainty_measure.h>

//: function to compute generic edges
dbdet_edgemap_sptr dbdet_detect_third_order_edges(vil_image_view<vxl_byte> image, 
                                                  double sigma, double threshold, 
                                                  int N, unsigned parabola_fit, 
                                                  unsigned grad_op, unsigned conv_algo,
                                                  bool use_adaptive_thresh, 
                                                  bool interp_grid, //output edges on the interpolated (finer) grid
                                                  bool reduce_tokens)
{
  //convert to grayscale
  vil_image_view<vxl_byte> greyscale_view;
  if(image.nplanes() == 3){
    vil_convert_planes_to_grey(image, greyscale_view );
  }
  else if (image.nplanes() == 1){
    greyscale_view = image;
  }

  //start the timer
  vul_timer t;

  //compute image gradients before performing nonmax suppression
  vil_image_view<double> grad_x, grad_y, grad_mag; 
  int scale = (int) vcl_pow(2.0, N);

  //compute gradients
  switch (grad_op)
  {
    case 0: //Interpolated Gaussian
    {
      if (conv_algo==0){ //2-d convolutions
        dbdet_subpix_convolve_2d(greyscale_view, grad_x, dbdet_Gx_kernel(sigma), double(), N);
        dbdet_subpix_convolve_2d(greyscale_view, grad_y, dbdet_Gy_kernel(sigma), double(), N);
      }
      else {
        dbdet_subpix_convolve_2d_sep(greyscale_view, grad_x, dbdet_Gx_kernel(sigma), double(), N);
        dbdet_subpix_convolve_2d_sep(greyscale_view, grad_y, dbdet_Gy_kernel(sigma), double(), N);
      }

      break;
    }
    case 1: //h0-operator
    {
      if (conv_algo==0){ //2-d convolutions
        dbdet_subpix_convolve_2d(greyscale_view, grad_x, dbdet_h0_Gx_kernel(sigma), double(), N);
        dbdet_subpix_convolve_2d(greyscale_view, grad_y, dbdet_h0_Gy_kernel(sigma), double(), N);
      }
      else {
        dbdet_subpix_convolve_2d_sep(greyscale_view, grad_x, dbdet_h0_Gx_kernel(sigma), double(), N);
        dbdet_subpix_convolve_2d_sep(greyscale_view, grad_y, dbdet_h0_Gy_kernel(sigma), double(), N);
      }
      break;
    }
    case 2:  //h1-operator
    {
      if (conv_algo==0){ //2-d convolutions
        dbdet_subpix_convolve_2d(greyscale_view, grad_x, dbdet_h1_Gx_kernel(sigma), double(), N);
        dbdet_subpix_convolve_2d(greyscale_view, grad_y, dbdet_h1_Gy_kernel(sigma), double(), N);
      }
      else {
        dbdet_subpix_convolve_2d_sep(greyscale_view, grad_x, dbdet_h1_Gx_kernel(sigma), double(), N);
        dbdet_subpix_convolve_2d_sep(greyscale_view, grad_y, dbdet_h1_Gy_kernel(sigma), double(), N);
      }
      break;
    }
  }

  //compute gradient magnitude
  grad_mag.set_size(grad_x.ni(), grad_x.nj());

  //get the pointers to the memory chunks
  double *gx  =  grad_x.top_left_ptr();
  double *gy  =  grad_y.top_left_ptr();
  double *g_mag  =  grad_mag.top_left_ptr();

  //compute the gradient magnitude
  for(unsigned long i=0; i<grad_mag.size(); i++)
    g_mag[i] = vcl_sqrt(gx[i]*gx[i] + gy[i]*gy[i]);

  double conv_time = t.real();  
  t.mark(); //reset timer

  //Now call the nms code to determine the set of edgels and their subpixel positions
  vcl_vector<vgl_point_2d<double> > edge_locs, edge_locations;
  vcl_vector<vgl_point_2d<int> > pix_locs;
  vcl_vector<double> orientation, mag, d2f;

  //parameters for reliable NMS
  double noise_sigma = 1.5;
  double rel_thresh = 1.3*noise_sigma/(sigma*sigma*sigma);
  dbdet_nms NMS(dbdet_nms_params(threshold, (dbdet_nms_params::PFIT_TYPE)parabola_fit,
                                 (unsigned)vcl_pow(2.0,N)*(4*sigma+1), 
                                 rel_thresh, use_adaptive_thresh), 
                grad_x, grad_y, grad_mag);

  NMS.apply(true, edge_locs, orientation, mag, d2f, pix_locs);

  double nms_time = t.real(); 
  t.mark(); //reset timer
  
  //convert to the original image scale coordinates
  for (unsigned i=0; i<edge_locs.size(); i++)
    edge_locations.push_back(vgl_point_2d<double>(edge_locs[i].x()/scale, edge_locs[i].y()/scale));

  //for each edge, compute all the gradients to compute the new orientation
  vcl_vector<double> Ix, Iy, Ixx, Ixy, Iyy, Ixxy, Ixyy, Ixxx, Iyyy;

  switch (grad_op)
  {
    case 0: //Interpolated Gaussian
    { 
      dbdet_subpix_convolve_2d(greyscale_view, edge_locations, Ix,   dbdet_Gx_kernel(sigma),   double(), N);
      dbdet_subpix_convolve_2d(greyscale_view, edge_locations, Iy,   dbdet_Gy_kernel(sigma),   double(), N);
      dbdet_subpix_convolve_2d(greyscale_view, edge_locations, Ixx,  dbdet_Gxx_kernel(sigma),  double(), N);
      dbdet_subpix_convolve_2d(greyscale_view, edge_locations, Ixy,  dbdet_Gxy_kernel(sigma),  double(), N);
      dbdet_subpix_convolve_2d(greyscale_view, edge_locations, Iyy,  dbdet_Gyy_kernel(sigma),  double(), N);
      dbdet_subpix_convolve_2d(greyscale_view, edge_locations, Ixxx, dbdet_Gxxx_kernel(sigma), double(), N);
      dbdet_subpix_convolve_2d(greyscale_view, edge_locations, Ixxy, dbdet_Gxxy_kernel(sigma), double(), N);
      dbdet_subpix_convolve_2d(greyscale_view, edge_locations, Ixyy, dbdet_Gxyy_kernel(sigma), double(), N);
      dbdet_subpix_convolve_2d(greyscale_view, edge_locations, Iyyy, dbdet_Gyyy_kernel(sigma), double(), N);
      break;
    }
    case 1: //h0-operator
    {
      dbdet_subpix_convolve_2d(greyscale_view, edge_locations, Ix,   dbdet_h0_Gx_kernel(sigma),   double(), N);
      dbdet_subpix_convolve_2d(greyscale_view, edge_locations, Iy,   dbdet_h0_Gy_kernel(sigma),   double(), N);
      dbdet_subpix_convolve_2d(greyscale_view, edge_locations, Ixx,  dbdet_h0_Gxx_kernel(sigma),  double(), N);
      dbdet_subpix_convolve_2d(greyscale_view, edge_locations, Ixy,  dbdet_h0_Gxy_kernel(sigma),  double(), N);
      dbdet_subpix_convolve_2d(greyscale_view, edge_locations, Iyy,  dbdet_h0_Gyy_kernel(sigma),  double(), N);
      dbdet_subpix_convolve_2d(greyscale_view, edge_locations, Ixxx, dbdet_h0_Gxxx_kernel(sigma), double(), N);
      dbdet_subpix_convolve_2d(greyscale_view, edge_locations, Ixxy, dbdet_h0_Gxxy_kernel(sigma), double(), N);
      dbdet_subpix_convolve_2d(greyscale_view, edge_locations, Ixyy, dbdet_h0_Gxyy_kernel(sigma), double(), N);
      dbdet_subpix_convolve_2d(greyscale_view, edge_locations, Iyyy, dbdet_h0_Gyyy_kernel(sigma), double(), N);
      break;
    }
    case 2:  //h1-operator
    {
      dbdet_subpix_convolve_2d(greyscale_view, edge_locations, Ix,   dbdet_h1_Gx_kernel(sigma),   double(), N);
      dbdet_subpix_convolve_2d(greyscale_view, edge_locations, Iy,   dbdet_h1_Gy_kernel(sigma),   double(), N);
      dbdet_subpix_convolve_2d(greyscale_view, edge_locations, Ixx,  dbdet_h1_Gxx_kernel(sigma),  double(), N);
      dbdet_subpix_convolve_2d(greyscale_view, edge_locations, Ixy,  dbdet_h1_Gxy_kernel(sigma),  double(), N);
      dbdet_subpix_convolve_2d(greyscale_view, edge_locations, Iyy,  dbdet_h1_Gyy_kernel(sigma),  double(), N);
      dbdet_subpix_convolve_2d(greyscale_view, edge_locations, Ixxx, dbdet_h1_Gxxx_kernel(sigma), double(), N);
      dbdet_subpix_convolve_2d(greyscale_view, edge_locations, Ixxy, dbdet_h1_Gxxy_kernel(sigma), double(), N);
      dbdet_subpix_convolve_2d(greyscale_view, edge_locations, Ixyy, dbdet_h1_Gxyy_kernel(sigma), double(), N);
      dbdet_subpix_convolve_2d(greyscale_view, edge_locations, Iyyy, dbdet_h1_Gyyy_kernel(sigma), double(), N);
      break;
    }
  }
      
  //Now, compute and update each edge with its new orientation
  vcl_vector<double> edge_orientations(edge_locations.size());
  for (unsigned i=0; i<edge_locations.size();i++)
  {
    //compute F
    //F[i]   = Ix[i]*Ix[i]*Ixx[i] + 2*Ix[i]*Iy[i]*Ixy[i] + Iy[i]*Iy[i]*Iyy[i];

    //compute Fx and Fy
    double Fx = 2*Ix[i]*Ixx[i]*Ixx[i] + 2*Ix[i]*Ixy[i]*Ixy[i] + 2*Ixx[i]*Iy[i]*Ixy[i] + 
                2*Ixy[i]*Iyy[i]*Iy[i] + 2*Ix[i]*Iy[i]*Ixxy[i] + Ixyy[i]*Iy[i]*Iy[i] + Ix[i]*Ix[i]*Ixxx[i];
    double Fy = 2*Iy[i]*Iyy[i]*Iyy[i] + 2*Iy[i]*Ixy[i]*Ixy[i] + 2*Ixy[i]*Ix[i]*Ixx[i] + 
                2*Ix[i]*Iyy[i]*Ixy[i]  + 2*Ix[i]*Iy[i]*Ixyy[i] + Ixxy[i]*Ix[i]*Ix[i] + Iyyy[i]*Iy[i]*Iy[i];

    //save new orientation
    edge_orientations[i] = dbdet_angle0To2Pi(vcl_atan2(Fx, -Fy));

    //experiment: does the orientation from the detH condition give good orientations? Does'nt look like it
    //edge_orientations[i] = dbdet_angle0To2Pi(vcl_atan(Ixy[i]*(Ixx[i]+Iyy[i])/(Ixx[i]*Ixx[i]+Ixy[i]*Ixy[i])));
  }

  double third_order_time = t.real();

  //report timings
  vcl_cout << vcl_endl;
  vcl_cout << "time taken for conv: " << conv_time << " msec" << vcl_endl;
  vcl_cout << "time taken for nms: " << nms_time << " msec" << vcl_endl;
  vcl_cout << "time taken for third-order: " << third_order_time << " msec" << vcl_endl;

  //------------------------------------------------------------------------------------------
  // Compute the edge uncertainty measures

  vcl_vector<double> edge_uncertainties(edge_locations.size());
  dbdet_edge_uncertainty_measure edge_uncer(grad_x, grad_y, sigma);
  edge_uncer.get_edgel_uncertainties(edge_locs, edge_uncertainties);

  //------------------------------------------------------------------------------------------
  //create a new edgemap from the tokens identified by NMS
  dbdet_edgemap_sptr edge_map;
  if (interp_grid)
    edge_map = new dbdet_edgemap(grad_mag.ni(), grad_mag.nj());
  else
    edge_map = new dbdet_edgemap(greyscale_view.ni(), greyscale_view.nj());
  
  for (unsigned i=0; i<edge_locations.size(); i++)
  {
    //Now insert them into the edge map appropriately
    if (interp_grid){
      dbdet_edgel* new_edgel = new dbdet_edgel(edge_locs[i], edge_orientations[i], mag[i], d2f[i], edge_uncertainties[i]); 
      new_edgel->gpt = pix_locs[i];
      edge_map->insert(new_edgel, pix_locs[i].x(), pix_locs[i].y());
    }
    else {
      if (reduce_tokens){
        //only insert one edgel per grid position
        int xx = dbdet_round(edge_locations[i].x());
        int yy = dbdet_round(edge_locations[i].y());

        if (edge_map->edge_cells(yy, xx).size()>0)
          continue;

        dbdet_edgel* new_edgel = new dbdet_edgel(edge_locations[i], edge_orientations[i], mag[i], d2f[i], edge_uncertainties[i]); 
        new_edgel->gpt = vgl_point_2d<int>(xx, yy);
        edge_map->insert(new_edgel, xx, yy);
      }
      else { //insert all of them
        dbdet_edgel* new_edgel = new dbdet_edgel(edge_locations[i], edge_orientations[i], mag[i], d2f[i], edge_uncertainties[i]); 
        new_edgel->gpt = pix_locs[i];
        edge_map->insert(new_edgel);
      }
    }
  }

  //add intensity appearance to the edges
  //dbdet_add_intensity_app(greyscale_view, edge_map, sigma, 1); //opt: 0: original , 1: smoothed, 2: Half gaussian

  return edge_map;
}

