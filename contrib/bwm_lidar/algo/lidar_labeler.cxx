// This is lemsvxlsrc/contrib/bwm_lidar/algo/lidar_labeler.cxx

//:
// \file
// \brief Main Class for the classification LIDAR images
//
// \author Ibrahim Eden, (ieden@lems.brown.edu)
// \date 12/16/2007
//      
// \endverbatim

#include "lidar_labeler.h"

//: This is the main function where pixels are classified and 
//    labeled into buildings and vegetations.
//    vil_image_view<int>& labeled : is the pixels labeled
//      0     ground
//      >0    buildings
//      <0    vegetation
void lidar_labeler::label_lidar(const vnl_matrix<double>& first_ret, 
                                 const vnl_matrix<double>& last_ret, 
                                 const lidar_labeling_params params,
                                 const double min_elev,
                                 const double max_elev,
                                 const vcl_string input_path,
                                 vil_image_view<int>& labeled)
{
  int GND_PLANE = 2; //blue
  int VEG_PLANE = 1; // green
  int BLD_PLANE = 0; // red

  unsigned ni = first_ret.rows();
  unsigned nj = first_ret.cols();

  vil_structuring_element se;

  // Make sure images are same size
  if ( (first_ret.rows() != last_ret.rows()) || (first_ret.cols() != last_ret.cols()) ) {
    vcl_cerr << "error, first return and last return images should be same size!\n";
    return;
  }

  // Assume ground is horizontal and most common height value
  double bin_size = 1.0;
  int nbins = (int)((max_elev - min_elev)/bin_size);
  vnl_vector<int> histo(nbins+1,0);

  for(unsigned i=0; i<ni; i++){
    for(unsigned j=0; j<nj; j++){
      if(last_ret(i,j) > max_elev){
        continue;
      }
      int curr_bin = (int)((last_ret(i,j) - min_elev)/bin_size);
      histo(curr_bin)++;
    }
  }

  int max_count = 0;
  int max_index = -1;
  for(unsigned i=0; i<histo.size(); i++){
    if(histo[i] > max_count){
      max_count = histo[i];
      max_index = i;
    }
  }

  // Find out the ground elevation
  double gnd_val = min_elev + (bin_size*(double)max_index);

  vnl_matrix<int> classification(ni,nj,-1);

  // Initial sweep for the segmentation of pixels
  for (unsigned i=0; i < ni; i++) {
    for (unsigned j=0; j < nj; j++) {
      // If the pixel is illegal (never happens!)
      if(last_ret(i,j) > max_elev || first_ret(i,j) < min_elev){  continue; }

      double diff = first_ret(i,j) - last_ret(i,j);
      
      // if the difference between the first and last return, classify as vegetation
      if(diff > params.veg_min_variation_){
        classification(i,j) = VEG_PLANE;
      }
      // if close to the ground elevation and the difference between the first and last return is small
      //    label as ground pixel
      else if(
        (vnl_math_abs(first_ret(i,j)- gnd_val) < params.gnd_max_diff_) && 
        (vnl_math_abs(last_ret(i,j)- gnd_val) < params.gnd_max_diff_) && 
        ((first_ret(i,j) - last_ret(i,j)) < params.gnd_max_variation_)){
          classification(i,j) = GND_PLANE;
      }
      // if over the minimum building height and the difference between the first and last return is small
      //    label as building pixel
      else if(
        ((last_ret(i,j)-gnd_val) > params.bld_min_height_) && 
        ((first_ret(i,j)-gnd_val) > params.bld_min_height_) && 
        ((first_ret(i,j) - last_ret(i,j)) < params.bld_max_variation_) ){
          classification(i,j) = BLD_PLANE;
      }
    }
  }

  //save_classification_image(input_path + "\\classification1.tif",classification);

  vil_image_view<bool> bld_image;
  vil_image_view<bool> bld_image_temp;
  vil_image_view<int> bld_lbl_image;
  vil_image_view<int> bld_lbl_image_sep;
  bld_image.set_size(ni,nj);
  bld_image.fill(false);

  vil_image_view<bool> veg_image;
  vil_image_view<bool> veg_image_temp;
  vil_image_view<int> veg_lbl_image;
  veg_image.set_size(ni,nj);
  veg_image.fill(false);

  // Create two boolean images. One for buildings, one for vegetation
  for (unsigned i=0; i<bld_image.ni(); i++) {
    for (unsigned j=0; j<bld_image.nj(); j++) {
      if(classification(i,j)==BLD_PLANE){
        bld_image(i,j) = true;
      }
      if(classification(i,j)==VEG_PLANE){
        veg_image(i,j) = true;
      }
    }
  }

  vil_image_view<int> veg_image_fh(veg_image.ni(),veg_image.nj());
  veg_image_fh.fill(0);
  for(unsigned i=0; i<veg_image.ni(); i++){
    for(unsigned j=0; j<veg_image.nj(); j++){
      if(veg_image(i,j)){
        veg_image_fh(i,j) = 1;
      }
    }
  }
  // Fill holes inside the boolean vegetation image
  get_connected_components::fill_holes(veg_image_fh);
  veg_image.fill(false);
  for(unsigned i=0; i<veg_image.ni(); i++){
    for(unsigned j=0; j<veg_image.nj(); j++){
      if(veg_image_fh(i,j)>0){
        veg_image(i,j) = true;
      }
    }
  }
  // Do morphological closing
  se.set_to_disk(1.5);
  vil_binary_erode(veg_image,veg_image_temp,se);
  vil_binary_dilate(veg_image_temp,veg_image,se);
  for(unsigned i=0; i<veg_image.ni(); i++){
    for(unsigned j=0; j<veg_image.nj(); j++){
      if(classification(i,j)==VEG_PLANE){
        classification(i,j)=-2;
      }
    }
  }
  for(unsigned i=0; i<veg_image.ni(); i++){
    for(unsigned j=0; j<veg_image.nj(); j++){
      if(veg_image(i,j)){
        classification(i,j)=VEG_PLANE;
      }
    }
  }
  // Label each vegetation cluster with a different index
  get_connected_components::label_image(veg_image,veg_lbl_image,params.veg_min_area_);
  //save_classification_image(input_path + "\\classification2.tif",classification);

  vil_image_view<int> bld_image_fh(ni,nj);
  bld_image_fh.fill(0);
  for(unsigned i=0; i<ni; i++){
    for(unsigned j=0; j<nj; j++){
      if(bld_image(i,j)){
        bld_image_fh(i,j) = 1;
      }
    }
  }
  // Fill holes inside the boolean buiilding image
  get_connected_components::fill_holes(bld_image_fh);
  bld_image.fill(false);
  for(unsigned i=0; i<bld_image.ni(); i++){
    for(unsigned j=0; j<bld_image.nj(); j++){
      if(bld_image_fh(i,j)>0){
        bld_image(i,j) = true;
      }
    }
  }
  for(unsigned i=0; i<bld_image.ni(); i++){
    for(unsigned j=0; j<bld_image.nj(); j++){
      if(classification(i,j)==BLD_PLANE){
        classification(i,j)=-2;
      }
    }
  }
  for(unsigned i=0; i<bld_image.ni(); i++){
    for(unsigned j=0; j<bld_image.nj(); j++){
      if(bld_image(i,j)){
        classification(i,j)=BLD_PLANE;
      }
    }
  }
  // Label each building cluster with a different index
  get_connected_components::label_image(bld_image,bld_lbl_image,params.bld_min_area_);

  // Seperate buildings that share a common boundary
  get_connected_components::seperate_buildings(bld_lbl_image,first_ret,last_ret,bld_lbl_image_sep);
  //save_classification_image(input_path + "\\classification3.tif",classification);

  // Join labeled building and vegetation images to create the desired output image
  join_buildings_and_vegetation(bld_lbl_image_sep,veg_lbl_image,labeled);

  return;
}

//: This function creates a labeled image for buildings and vegetation where building indices are positive and vegetation indices are negative
void lidar_labeler::join_buildings_and_vegetation(vil_image_view<int>& bld, vil_image_view<int>& veg, vil_image_view<int>& joined){
  joined.set_size(bld.ni(), bld.nj());
  joined.fill(0);

  for(unsigned i=0; i<joined.ni(); i++){
    for(unsigned j=0; j<joined.nj(); j++){
      if(bld(i,j)!=0 && veg(i,j)!=0){
        vcl_cout << "Classification Error: Pixel (" << i << "," << j << ") is assigned to more than one classes\n";
        return;
      }
      joined(i,j) = bld(i,j) - veg(i,j);
    }
  }
}

//: This function creates an RGB image for classification where ground (blue), building (red) and vegetation (green)
//    pixels are shown
void lidar_labeler::save_classification_image(const vcl_string out_path, const vnl_matrix<int>& classification){
  vil_image_view<unsigned char> classification_image(classification.rows(),classification.cols(),3);
  classification_image.fill(0);
  for(unsigned i=0; i<classification_image.ni(); i++){
    for(unsigned j=0; j<classification_image.nj(); j++){
      if(classification(i,j)>-1 && classification(i,j)<3){
        classification_image(i,j,classification(i,j)) = 255;
      }
    }
  }
  vil_save(classification_image,out_path.c_str());
}
