// This is brcv/seg/dbdet/algo/dbdet_eval_edge_det.h
#ifndef dbdet_eval_edge_det_h
#define dbdet_eval_edge_det_h
//:
//\file
//\brief Various functions to compute the performance of edge detection and linking results
//
//\author Amir Tamrakar
//\date 08/01/08
//
//\verbatim
//Modifications
//
// \endverbatim

#include <vcl_fstream.h>
#include <vcl_vector.h>
#include <vcl_string.h>
#include <dbdet/edge/dbdet_edgemap_sptr.h>
#include <dbdet/sel/dbdet_curve_fragment_graph.h>
#include <vil/vil_image_resource_sptr.h>
#include <vil/vil_image_view.h>

//: a class to specify the evaluation parameters
class dbdet_eval_edge_det_params
{
public:
  int edge_label;      //the value of the pixel marked as "edge" in the GT image
  int no_edge_label;   //the value of the pixel marked as "no edge" in the GT image
  int dont_care_label; //the value of the pixel marked as "don't care" in the GT image

  int T_match;      //tolerance for corresponding edgels

  vcl_vector<double> thresh; //vector of threshold values

  //: constructor
  dbdet_eval_edge_det_params(int tol=2): edge_label(0), no_edge_label(5), dont_care_label(255), T_match(tol){}

  //: constructor 2
  dbdet_eval_edge_det_params(int e_label, int ne_label, int dc_label, int tol): 
    edge_label(e_label), no_edge_label(ne_label), dont_care_label(dc_label), T_match(tol){}

  ~dbdet_eval_edge_det_params(){}

};

//: A class to hold the results of an evaluation process
class dbdet_eval_result
{
public:
  vcl_vector<double> params; //vector of parameter values
  vcl_vector<int> TP;        //vector of TP count at each param value
  vcl_vector<int> FP;
  vcl_vector<int> TN;
  vcl_vector<int> FN;

  dbdet_eval_result(unsigned size=0): params(size), TP(size), FP(size), TN(size), FN(size) {}
  ~dbdet_eval_result(){}

  bool save_ROC(vcl_string filename)
  {
    //1)If file open fails, return.
    vcl_ofstream outfp(filename.c_str(), vcl_ios::out);

    if (!outfp){
      vcl_cout << " Error opening file  " << filename.c_str() << vcl_endl;
      return false;
    }

    // output ROC table
    for (unsigned i=0; i<params.size(); i++)
      outfp << TP[i] << " " << FP[i] << " " << TN[i] << " " << FN[i] << " " << params[i] << vcl_endl;

    //close file
    outfp.close();
    
    return true;
  }

};

//: evaluate the accuracy of an edgemap given a ground truth edgemap (as an image)
dbdet_eval_result dbdet_eval_edge_det(dbdet_eval_edge_det_params params, dbdet_edgemap_sptr edge_map, vil_image_view<vxl_byte>& GT_image);

//: evaluate the performace of contour extraction given a ground truth edgemap (as an image)
dbdet_eval_result dbdet_eval_edge_linking(dbdet_eval_edge_det_params params, dbdet_curve_fragment_graph& CFG, vil_image_view<vxl_byte>& GT_image);



#endif // dbdet_eval_edge_det_h

