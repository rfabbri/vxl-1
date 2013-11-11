// This is brcv/shp/dbskfg/pro/dbskfg_match_composite_graph_process.h
#ifndef dbskfg_match_composite_graph_process_h_
#define dbskfg_match_composite_graph_process_h_

//:
// \file
// \brief This process matches two composite graphs using edit distance
//
// \author Maruthi Narayanan
// \date 09/25/10
//
// \verbatim
//  Modifications
//
// \endverbatim

#include <bpro1/bpro1_process.h>
#include <bpro1/bpro1_parameters.h>
#include <dbskr/dbskr_scurve.h>
#include <vcl_utility.h>
#include <dbskr/dbskr_edit_distance_base.h>
#include <vil/vil_image_resource_sptr.h>
#include <vl/sift.h>


class dbskfg_match_composite_graph_process : public bpro1_process 
{

public:
  //: Constructor
  dbskfg_match_composite_graph_process();
  
  //: Destructor
  virtual ~dbskfg_match_composite_graph_process();

  //: Clone the process
  virtual bpro1_process* clone() const;

  vcl_string name();

  vcl_vector< vcl_string > get_input_type();
  vcl_vector< vcl_string > get_output_type();

  int input_frames();
  int output_frames();

  bool execute();
  bool finish();

  double final_cost(){return final_cost_;}

private:

  //: Final cost
  double final_cost_;

  //: Curve list 1
  vcl_vector<dbskr_scurve_sptr> curve_list1_;
  
  //: Curve list 2
  vcl_vector<dbskr_scurve_sptr> curve_list2_;

  //: Map points from curve list 1 to curve list 2
  vcl_vector< vcl_vector < vcl_pair <int,int> > > map_list_;

  //: Get path key
  vcl_vector< pathtable_key > path_map_;

  // compute app cost
  double compute_app_cost(vl_sift_pix* grad_data1,
                          vl_sift_pix* grad_data2,
                          VlSiftFilt* filter1,
                          VlSiftFilt* filter2);
  
  // Compute gradient maps of image
  void compute_grad_maps(vil_image_resource_sptr& image,
                         vl_sift_pix** grad_data,
                         VlSiftFilt** filter);

};

#endif  //dbskfg_match_composite_graph_process_h_
