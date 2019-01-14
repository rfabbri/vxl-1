// This is dborl/dborl_eval_det.cxx

//:
// \file

#include "dborl_eval_det.h"

#include <dborl/dborl_det_desc.h>
#include <borld/borld_evaluation.h>

#include <borld/borld_image_description_utils.h>

#include <bsol/bsol_algs.h>
#include <vsol/vsol_box_2d.h>
#include <algorithm>



// -----------------------------------------------------------------------------
//: Detect an object, represented as a shock graph, in an image
bool dborl_VOC2008_eval_det(const borld_image_description_sptr& image_desc, 
                        const std::string& model_category,
                        double min_required_overlap,
                        const std::vector<dborl_det_desc_sptr >& list_det,
                        std::vector<double >& confidence,
                        std::vector<int >& FP,
                        std::vector<int >& TP,
                        int& num_positives)
{
  // get a set of ground truth bounding boxes from the image description
  std::vector<vsol_box_2d_sptr > gt_boxes;
  borld_get_boxes(gt_boxes, image_desc, model_category);
  num_positives = gt_boxes.size();

  // keep track of whether the ground truth objects have been counted (detected) by one of the detection solutions
  // = true if already counted
  std::vector<bool > gt_detected(gt_boxes.size(), false); 

  // sort the detection
  std::vector<dborl_det_desc_sptr > sorted_det = list_det;
  std::sort(sorted_det.begin(), sorted_det.end(), dborl_decreasing_confidence);


  // compare each of the detection bounding boxes with the ground truth
  unsigned num_det = sorted_det.size();
  FP.resize(num_det, 0);
  TP.resize(num_det, 0);
  confidence.resize(num_det, 0);
  
  for (unsigned i =0; i < num_det; ++i)
  {
    confidence[i] = sorted_det[i]->confidence();
    vsol_box_2d_sptr det_box = sorted_det[i]->bbox();
    
    
    // assigned each detection box to a ground truth object by finding the one it 
    // overlaps with most
    double max_overlap = -1;
    int max_overlap_idx = -1;
    for (unsigned k =0; k < gt_boxes.size(); ++k)
    {
      // compute intersection and union of the detection bounding box and the groundtruth box
      vsol_box_2d_sptr intersect_box;
      
      if (bsol_algs::intersection(det_box, gt_boxes[k], intersect_box))
      {
        // compute overlap as area of intersection / area of union
        double union_area = det_box->area() + gt_boxes[k]->area() - intersect_box->area();
        double overlap = intersect_box->area() / union_area;
        if (overlap > max_overlap)
        {
          max_overlap = overlap;
          max_overlap_idx = k;
        }
      }
    }

    // classify this detection as either true positive or false positive
    if (max_overlap < min_required_overlap)
    {
      FP[i] = 1;
    }
    else
    {
      if (max_overlap_idx < 0) // should never happens
      {
        std::cout << "ERROR: max_overlap_dex < 0!!!!!!!!" << std::endl;
        FP[i] = 1;
      }
      else if (gt_detected[max_overlap_idx]) // this object has been counted toward some other detection bbox
      {
        FP[i] = 1;
      }
      else
      {
        TP[i] = 1;
        gt_detected[max_overlap_idx] = true;
      }
    }
  }

  // Compute total number of true positive and false positive for each confidence level
  for (unsigned i =1; i < num_det; ++i)
  {
    FP[i] += FP[i-1];
    TP[i] += TP[i-1];
  }


  return true;
}




//: Print evaluation results of object detections to a file
bool dborl_VOC2008_print_eval_results(const std::string& eval_file,
                                      int num_pos, // total number of positives
                                      const std::string& object_name, // name of input image
                                      const std::string& model_category, // detection category
                                      const std::vector<double >& confidence_vec, // confidence of each detection
                                      const std::vector<int >& TP_vec, // Is detection a true positive?
                                      const std::vector<int >& FP_vec // Is detection a false positive?
                                      )
{
  // Open file for writing
  std::ofstream ofs(eval_file.c_str(), std::ios::out);
  if (!ofs)
  {
    return false;
  }

  // Write out data, following VOC2008 dev kit format
  ofs << "num_pos " << num_pos << "\n";
  for (unsigned i =0; i < confidence_vec.size(); ++i)
  {
    ofs << object_name << " "
      << model_category << " "
      << confidence_vec[i] << " "
      << TP_vec[i] << " "
      << FP_vec[i] << "\n";
  }
  ofs.close();

  return true;
}








