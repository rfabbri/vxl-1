// This is contrib/mleotta/vidreg/vidreg_feature_group.h
#ifndef vidreg_feature_group_h_
#define vidreg_feature_group_h_

//:
// \file
// \brief A structure to contain a group of detected features
// \author Matt Leotta (mleotta@lems.brown.edu)
// \date 7/26/06
//
// \verbatim
//  Modifications
// \endverbatim


#include <vcl_vector.h>
#include <rgrl/rgrl_feature_sptr.h>

#include <vbl/vbl_ref_count.h>
#include <vbl/vbl_smart_ptr.h>
#include <vidreg/vidreg_feature_edgel.h>

// forward declare the edgel
class vidreg_feature_edgel;


class vidreg_feature_group : public vbl_ref_count
{
  public:
    vidreg_feature_group() {}

    vidreg_feature_group(const vcl_vector<rgrl_feature_sptr>& e,
                         const vcl_vector<rgrl_feature_sptr>& c)
  : edgels(e), corners(c) {}

    vcl_vector<rgrl_feature_sptr>     edgels;
    vcl_vector<vidreg_edge>           edges;
    vcl_vector<double>                edgel_weights;

    vcl_vector<rgrl_feature_sptr> corners;
    vcl_vector<double>            corner_weights;
};

typedef vbl_smart_ptr<vidreg_feature_group> vidreg_feature_group_sptr;



#endif // vidreg_feature_group_h_
