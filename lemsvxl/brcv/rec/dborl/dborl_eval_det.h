// This is dborl/dborl_eval_det.h
#ifndef dborl_eval_det_h_
#define dborl_eval_det_h_

//:
// \file
// \brief Functions to evaluate detection results
// \author Nhon Trinh (ntrinh@lems.brown.edu)
// \date Nov 4, 2008
//
// \verbatim
//  Modifications
// \endverbatim

#include <vector>
#include <string>
#include <dborl/dborl_det_desc_sptr.h>
#include <borld/borld_image_description_sptr.h>



// -----------------------------------------------------------------------------
//: Evaluation detection results, in a similar manner to that in the VOCdevkit 2008
// "image_desc": contains ground truth bounding boxes of objects for each category
// "model_category" : the category of the detection results
// "min_required_overlap" : minimum overlap ratio between detection bounding box
// and groundtruth bounding box (intersection area / union area) for detection result
// to be counted as true
// "list_det" : list of detection result returned by the algorithm
// "confidence", "TP", "FP": total number of true positives (TP) and false positive (FP)
// for each confidence threshold level of the detection result
bool dborl_VOC2008_eval_det(const borld_image_description_sptr& image_desc, 
                        const std::string& model_category,
                        double min_required_overlap,
                        const std::vector<dborl_det_desc_sptr >& list_det,
                        std::vector<double >& confidence,
                        std::vector<int >& FP,
                        std::vector<int >& TP,
                        int& num_positives);

//: Print evaluation results of object detections to a file
bool dborl_VOC2008_print_eval_results(const std::string& eval_file,
                                      int num_pos, // total number of positives
                                      const std::string& object_name, // name of input image
                                      const std::string& model_category, // detection category
                                      const std::vector<double >& confidence_vec, // confidence of each detection
                                      const std::vector<int >& TP_vec, // Is detection a true positive?
                                      const std::vector<int >& FP_vec // Is detection a false positive?
                                      );

#endif
