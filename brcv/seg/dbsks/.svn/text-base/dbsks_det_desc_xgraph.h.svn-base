// This is seg/dbsks/dbsks_det_desc_xgraph.h
#ifndef dbsks_det_desc_xgraph_h_
#define dbsks_det_desc_xgraph_h_

//:
// \file
// \brief A class to describe the a detection solution
//        
// \author Nhon Trinh (ntrinh@lems.brown.edu)
// \date Nov 4, 2008
//
// \verbatim
//  Modifications
// \endverbatim




#include <dbsksp/dbsksp_xshock_graph_sptr.h>
#include <dborl/dborl_det_desc.h>
#include <dbsks/dbsks_det_desc_xgraph_sptr.h>
#include <vsol/vsol_box_2d_sptr.h>



// ============================================================================
// dbsks_det_desc_xgraph
// ============================================================================


//: A class to describe the solution of xgraph detection
class dbsks_det_desc_xgraph: public dborl_det_desc
{
public:
  // Constructor / destructor -------------------------------------------------
  
  //: constructor
  dbsks_det_desc_xgraph(){};

  //: Constructor
  dbsks_det_desc_xgraph(const dbsksp_xshock_graph_sptr& xgraph, double confidence) :
    xgraph_(xgraph), confidence_(confidence){};

  //: destructor
  virtual ~dbsks_det_desc_xgraph(){};

  virtual vcl_string is_a() const { return "dbsks_det_desc_xgraph"; }

  //: Return bounding box of the detection
  virtual vsol_box_2d_sptr bbox();

  //: Return the confidence level of the detection
  virtual double confidence() const {return this->confidence_; }
  void set_confidence(double new_confidence)
  {
    this->confidence_ = new_confidence;
  }

  //: Return the detected xshock graph
  dbsksp_xshock_graph_sptr xgraph() const {return this->xgraph_;}
  void set_xgraph(const dbsksp_xshock_graph_sptr& xgraph)
  {
    this->xgraph_ = xgraph;
  }

  //: Compute bounding box from xgraph_
  bool compute_bbox();

  // Member variables
protected:
  vsol_box_2d_sptr bbox_;
  double confidence_;
  dbsksp_xshock_graph_sptr xgraph_; 
};


//: Return true if two detection are in decreasing confidence level
bool dbsks_decreasing_confidence(const dbsks_det_desc_xgraph_sptr& a, const dbsks_det_desc_xgraph_sptr& b);

//: Return true if two detection are in increasing confidence level
bool dbsks_increasing_confidence(const dbsks_det_desc_xgraph_sptr& a, const dbsks_det_desc_xgraph_sptr& b);



#endif // seg/dbsks/dbsks_det_desc_xgraph.h


