// This is seg/dbsks/dbsks_detect_symmetry.h
#ifndef dbsks_detect_symmetry_h_
#define dbsks_detect_symmetry_h_

//:
// \file
// \brief A class to detect symmetry points from image evidence
//        
// \author Nhon Trinh ( ntrinh@lems.brown.edu)
// \date Oct 3, 2007
//
// \verbatim
//  Modifications
// \endverbatim


#include <vbl/vbl_ref_count.h>
#include <dbdet/edge/dbdet_edgemap_sptr.h>
#include <vnl/vnl_vector.h>
#include <vgl/vgl_point_2d.h>
#include <vgl/vgl_vector_3d.h>
#include <vcl_string.h>
#include <dbsksp/dbsksp_xshock_node.h>
#include <vcl_vector.h>
class dbsks_detect_symmetry : public vbl_ref_count
{
public:
  //: constructor
  dbsks_detect_symmetry(){};

  //: destructor
  virtual ~dbsks_detect_symmetry(){};

  // Utility functions --------------------------------------------------------
  //: Compute symmetry points from every pair of edge points
  // Pick those that are below the threshold and save to sym_points;
  void analyze();

  //: write the distribution to a file
  void write_sym_score_distribution_to_file(const vcl_string& filename);

  // Support functions --------------------------------------------------------

  //: Compute the medial axis point given two point-tangent.
  // This is only an estimate of the symmetry point because the two point-tangents may not 
  // form a medial point.
  dbsksp_xshock_node_descriptor compute_medial_point(
    const vgl_point_2d<double >& pt1, const vgl_vector_2d<double >& t1,
    const vgl_point_2d<double >& pt2, const vgl_vector_2d<double >& t2);


  // Member variables --------------------------------------------

  // input edge map
  dbdet_edgemap_sptr edgemap_;

  // number of bins for the distribution
  int num_bins_;
  double sym_score_threshold_;


  // output
  vnl_vector<double > sym_score_distr_;

  // list of symmetry points computed from the edges
  vcl_vector<dbsksp_xshock_node_descriptor > medial_pts_;

};


#endif // shp/dbsks/dbsks_detect_symmetry.h


