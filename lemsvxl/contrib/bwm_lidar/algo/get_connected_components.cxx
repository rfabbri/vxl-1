// This is lemsvxlsrc/contrib/bwm_lidar/algo/get_connected_components.cxx

//:
// \file
// \brief Some morphological operations related to Connected Component Analysis
//
// \author Ibrahim Eden, (ieden@lems.brown.edu)
// \date 12/16/2007
//      
// \endverbatim

#include "get_connected_components.h"

//: Connected component analysis. The input is a boolean image, "false" is the background and 
//    "true" is foreground pixels. The function returns an integer image where each intensity 
//    value corresponds to a set of connected component.
void get_connected_components::label_image(const vil_image_view<bool> &src_image, vil_image_view<int> &dest_image, int min_label_area){
  // Initialize the putput image
  dest_image.set_size(src_image.ni(),src_image.nj());
  dest_image.fill(0);

  int num_labels = 0;

  set_operations sets;

  for(unsigned i=0; i<src_image.ni(); i++){
    for(unsigned j=0; j<src_image.nj(); j++){
      if(src_image(i,j)){
        // Look at the all the pixles that was processed by this loop.
        int lbls[4] = {-1,-1,-1,-1};
        if(i>0 && j>0){
          lbls[0] = dest_image(i-1,j-1);
        }
        if(i>0){
          lbls[1] = dest_image(i-1,j);
        }
        if(i>0 && j<src_image.nj()-1){
          lbls[2] = dest_image(i-1,j+1);
        }
        if(j>0){
          lbls[3] = dest_image(i,j-1);
        }

        // Maximum label 
        int lbl_max = vnl_math_max(vnl_math_max(lbls[0],lbls[1]),vnl_math_max(lbls[2],lbls[3]));
        if(lbl_max<1){
          // All pre-neighbors are not labeled, so this pixel starts a new label
          num_labels++;
          dest_image(i,j) = num_labels;
          sets.add(num_labels);
        }
        else{
          // Some of the pre-neighbors labeled before, this pixel should adopt it's label
          dest_image(i,j) = lbl_max;
          for(unsigned k=0; k<4; k++){
            if(lbls[k]==-1 || lbls[k]==lbl_max){
              continue;
            }
            else{
              // If current pixel joins two connected sets, join them
              sets.join(lbls[k],lbl_max);
            }
          }
        }
      }      
    }
  }

  // Find the corresponding set id for each pixel
  vnl_vector<int> label_map(num_labels+1,0);
  for(unsigned i=1; i<label_map.size(); i++){
    label_map[i] = sets.get_set_id(i)+1;
  }

  // Count number of pixels for each label ID
  vnl_vector<int> label_count(num_labels+1,0);
  for(unsigned i=0; i<dest_image.ni(); i++){
    for(unsigned j=0; j<dest_image.nj(); j++){
      if(dest_image(i,j)>0){
        dest_image(i,j) = label_map[dest_image(i,j)];
        label_count[dest_image(i,j)]++;
      }
    }
  }

  // If the area of a component is smaller than a certain threshold, delete that label
  for(unsigned i=0; i<dest_image.ni(); i++){
    for(unsigned j=0; j<dest_image.nj(); j++){
      if(label_count[dest_image(i,j)] < min_label_area){
        dest_image(i,j) = 0;
      }
    }
  }
  remove_empty_labels(dest_image);
}

//: This function assigns a random RGB color to each different label for visualization purposes
void get_connected_components::colorize_image(const vil_image_view<int> &src_image, vil_image_view<unsigned char> &dest_image){
  dest_image.set_size(src_image.ni(),src_image.nj(),3);
  dest_image.fill(0);

  int max_color = 0;
  for(unsigned i=0; i<src_image.ni(); i++){
    for(unsigned j=0; j<src_image.nj(); j++){
      int curr_label = src_image(i,j);
      if((int)vnl_math_abs(curr_label) > max_color){
        max_color = vnl_math_abs(curr_label);
      }
    }
  }

  vnl_matrix<int> random_colors(max_color+1,3);
  vnl_random rndm;
  for(unsigned i=0; i<random_colors.rows(); i++){
    for(unsigned j=0; j<random_colors.cols(); j++){
      random_colors(i,j) = 100 + rndm.lrand32(0,155);
    }
  }

  for(unsigned i=0; i<src_image.ni(); i++){
    for(unsigned j=0; j<src_image.nj(); j++){
      int curr_label = src_image(i,j);
      if(curr_label > 0){
        dest_image(i,j,0) = random_colors(curr_label,0);
        dest_image(i,j,1) = 0;
        dest_image(i,j,2) = random_colors(curr_label,2)/2;
      }
      if(curr_label < 0){
        dest_image(i,j,0) = 0;
        dest_image(i,j,1) = random_colors(-curr_label,1);
        dest_image(i,j,2) = 128 + random_colors(-curr_label,2)/2;
      }
    }
  }
}

//: This function handles a special case that might occur in LIDAR images. Sometimes 
//    a building consists of many components that has different heights. Those components 
//    should be labeled as different components.
void get_connected_components::seperate_buildings(const vil_image_view<int> &labeled_image, const vnl_matrix<double>& first_ret, const vnl_matrix<double>& last_ret, vil_image_view<int> &dest_image){
  dest_image.set_size(labeled_image.ni(),labeled_image.nj());
  dest_image.fill(0);

  double variance_threshold = 25.0;
  double INF = 9999999.9;

  // Count the total number of labels
  int num_labels = 0;
  for(unsigned i=0; i<labeled_image.ni(); i++){
    for(unsigned j=0; j<labeled_image.nj(); j++){
      if(labeled_image(i,j) > num_labels){
        num_labels = labeled_image(i,j);
      }
    }
  }

  // Calculate mean intensity of each label
  vnl_vector<double> total_intensity(num_labels+1, 0.0);
  vnl_vector<int> num_pixels(num_labels+1, 0);
  for(unsigned i=0; i<labeled_image.ni(); i++){
    for(unsigned j=0; j<labeled_image.nj(); j++){
      if(labeled_image(i,j) > 0 && first_ret(i,j) < INF && first_ret(i,j) > -INF){
        total_intensity[labeled_image(i,j)] = total_intensity[labeled_image(i,j)] + first_ret(i,j);
        num_pixels[labeled_image(i,j)] = num_pixels[labeled_image(i,j)] + 1;
      }
    }
  }
  vnl_vector<double> mean_intensity(num_labels+1, 0.0);
  for(int i=1; i<num_labels+1; i++){
    mean_intensity[i] = total_intensity[i]/(double)num_pixels[i];
  }

  // Estimate variance of each label
  vnl_vector<double> variance(num_labels+1, 0.0);
  for(unsigned i=0; i<labeled_image.ni(); i++){
    for(unsigned j=0; j<labeled_image.nj(); j++){
      if(labeled_image(i,j) > 0 && first_ret(i,j) < INF && first_ret(i,j) > -INF){
        double curr_diff = first_ret(i,j) - mean_intensity[labeled_image(i,j)];
        variance[labeled_image(i,j)] = variance[labeled_image(i,j)] + (curr_diff*curr_diff);
      }
    }
  }
  for(int i=1; i<num_labels+1; i++){
    variance[i] = variance[i]/(double)num_pixels[i];
  }

  // The following a simple procedure that works with our current data, but needs improvement to 
  //    to work with general data
  for(unsigned i=0; i<labeled_image.ni(); i++){
    for(unsigned j=0; j<labeled_image.nj(); j++){
      dest_image(i,j) = labeled_image(i,j);
      if(labeled_image(i,j) > 0 && first_ret(i,j) < INF && first_ret(i,j) > -INF){
        if(variance[labeled_image(i,j)] > variance_threshold){
          if(first_ret(i,j) < mean_intensity[labeled_image(i,j)]){
            // If the variance is above a threshold and the value of the pixel is 
            //   smaller than the mean of the label, re-label the pixel
            dest_image(i,j) = labeled_image(i,j) + num_labels;
          }
        }
      }
    }
  }
  remove_empty_labels(dest_image);
}

//: This function removes empty labels, basically re-labels the image so that the maximum label index is minimal
void get_connected_components::remove_empty_labels(vil_image_view<int> &labeled_image){
  int num_labels = 0;
  for(unsigned i=0; i<labeled_image.ni(); i++){
    for(unsigned j=0; j<labeled_image.nj(); j++){
      if(labeled_image(i,j) > num_labels){
        num_labels = labeled_image(i,j);
      }
    }
  }

  vnl_vector<int> num_pixels(num_labels+1, 0);
  for(unsigned i=0; i<labeled_image.ni(); i++){
    for(unsigned j=0; j<labeled_image.nj(); j++){
      if(labeled_image(i,j) > 0){
        num_pixels[labeled_image(i,j)] = num_pixels[labeled_image(i,j)] + 1;
      }
    }
  }

  int label_count = 0;
  vnl_vector<int> pixel_map(num_labels+1, 0);
  for(int i=1; i<num_labels+1; i++){
    if(num_pixels[i]>0){
      label_count++;
      pixel_map[i] = label_count;
    }
  }

  for(unsigned i=0; i<labeled_image.ni(); i++){
    for(unsigned j=0; j<labeled_image.nj(); j++){
      labeled_image(i,j) = pixel_map[labeled_image(i,j)];
    }
  }
}

//: This is a morphological operation that fills inside the holes in each connected component
void get_connected_components::fill_holes(vil_image_view<int> &labeled_image){
  int num_labels = 0;
  for(unsigned i=0; i<labeled_image.ni(); i++){
    for(unsigned j=0; j<labeled_image.nj(); j++){
      if(labeled_image(i,j) > num_labels){
        num_labels = labeled_image(i,j);
      }
    }
  }

  // Process each label seperately
  for(int l=1; l<num_labels+1; l++){
    vil_image_view<bool> curr_bool_image;
    vil_image_view<int> curr_label_image;
    curr_bool_image.set_size(labeled_image.ni(),labeled_image.nj());
    curr_bool_image.fill(true);
    curr_label_image.set_size(labeled_image.ni(),labeled_image.nj());
    curr_label_image.fill(0);

    // Create a boolean image for each label where the label is treated as background and all the other pixels are treated as foregorung
    for(unsigned i=0; i<labeled_image.ni(); i++){
      for(unsigned j=0; j<labeled_image.nj(); j++){
        if(labeled_image(i,j)==l){
          curr_bool_image(i,j) = false;
        }
      }
    }

    label_image(curr_bool_image,curr_label_image,0);
    // Count the number of labels in this image
    int curr_num_labels = 0;
    for(unsigned i=0; i<curr_label_image.ni(); i++){
      for(unsigned j=0; j<curr_label_image.nj(); j++){
        if(curr_label_image(i,j)>curr_num_labels){
          curr_num_labels = curr_label_image(i,j);
        }
      }
    }

    // Count the number of pixels in each label
    vnl_vector<int> curr_count_labels(curr_num_labels+1,0);
    for(unsigned i=0; i<curr_label_image.ni(); i++){
      for(unsigned j=0; j<curr_label_image.nj(); j++){
        if(curr_label_image(i,j)>0){
          curr_count_labels[curr_label_image(i,j)]++;
        }
      }
    }

    // Find the label that has the most number of pixels
    int curr_max_label_index = 0;
    int curr_max_label_count = 0;
    for(unsigned i=1; i<curr_count_labels.size(); i++){
      if(curr_count_labels[i]>curr_max_label_count){
        curr_max_label_count = curr_count_labels[i];
        curr_max_label_index = i;
      }
    }

    // Select all pixels other that is not in the largest label 
    //    for the current label
    for(unsigned i=0; i<labeled_image.ni(); i++){
      for(unsigned j=0; j<labeled_image.nj(); j++){
        if(curr_label_image(i,j)!=curr_max_label_index){
          labeled_image(i,j) = l;
        }
      }
    }
  }
}
