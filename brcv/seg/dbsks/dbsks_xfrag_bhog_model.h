// This is shp/dbsksp/dbsks_xfrag_bhog_model.h
#ifndef dbsks_xfrag_bhog_model_h_
#define dbsks_xfrag_bhog_model_h_

//:
// \file
// \brief A model to classify the boundary histogram of orientation gradient magnitude (BHOG)
//        
// \author Nhon Trinh ( ntrinh@lems.brown.edu )
// \date Oct 6, 2008
//
// \verbatim
//  Modifications
// \endverbatim


//#include <vbl/vbl_ref_count.h>
//#include <vgl/vgl_box_2d.h>
//#include <dbsksp/dbsksp_xshock_node_descriptor.h>
//#include <dbgl/algo/dbgl_biarc.h>

#include <vcl_string.h>
#include <vnl/vnl_vector.h>
#include <vcl_vector.h>

// =============================================================================
// dbsks_xfrag_bhog_model
// =============================================================================

struct svm_model;

//: A model to classify the boundary histogram of orientation gradient magnitude 
// (BHOG). These models are learned using SVM.
// The current implementation is a wrapper for the SVM model of "LIBSVM"
class dbsks_xfrag_bhog_model
{
public:
  // CONSTRUCTORS/DESTRUCTORS/INITIALIZATION ===================================
  
  //: constructor
  dbsks_xfrag_bhog_model(): libsvm_model_(0) {};

  //: destructor
  virtual ~dbsks_xfrag_bhog_model();
  
  // DATA ACCESS ===============================================================


  // UTILITY ===================================================================

  //: Compute the score (probablity) of a BHOG feature vector
  bool compute_score(const vnl_vector<double >& x, double& score);

  //: Compute the score (probablity) of a list of BHOG feature vector
  bool compute_score(const vcl_vector<vnl_vector<double > >& feat_list, 
    vcl_vector<double >& score_list);

  
  // I/O =======================================================================

  //: Load SVM model from file
  bool load_from_file(const vcl_string& libsvm_model_file);

  //: write info of the dbskbranch to an output stream
  virtual void print(vcl_ostream & os){};


protected:
  svm_model* libsvm_model_;

};

#endif // shp/dbsksp/dbsks_xfrag_bhog_model.h


