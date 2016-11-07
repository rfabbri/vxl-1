// This is contrib/mleotta/vidreg/vidreg_feature_edgel.h
#ifndef vidreg_feature_edgel_h_
#define vidreg_feature_edgel_h_

//:
// \file
// \brief An rgrl feature face point with edge magnitude
// \author Matt Leotta (mleotta@lems.brown.edu)
// \date 8/4/06
//
// \verbatim
//  Modifications
// \endverbatim

#include <rgrl/rgrl_feature_face_pt.h>

//: An rgrl feature face point with edge magnitude
class vidreg_feature_edgel : public rgrl_feature_face_pt
{
  public:
    //: Default Constructor
    vidreg_feature_edgel();

    //: Constructor
    vidreg_feature_edgel(const vnl_vector<double>& loc,
                         const vnl_vector<double>& norm,
                         double mag);

    //: Destructor
    virtual ~vidreg_feature_edgel();

    //: Computes a weight based on orientation and magnitude
    double absolute_signature_weight(rgrl_feature_sptr) const;

    //: Transform the feature
    rgrl_feature_sptr transform( rgrl_transformation const& xform ) const;

    // Defines type-related functions
    rgrl_type_macro( vidreg_feature_edgel, rgrl_feature_face_pt );

    //: Return the magnitude of the gradient
    double magnitude() const { return magnitude_; }

    //: Pointer to the next edgel
    vidreg_feature_edgel* next() const { return next_; }
    //: Pointer to the previous edgel
    vidreg_feature_edgel* prev() const { return prev_; }

    //: Set the next edgel in the chain (also sets next->prev = this)
    void set_next(vidreg_feature_edgel* next);
    //: Set the previous edgel in the chain (also sets prev->next = this)
    void set_prev(vidreg_feature_edgel* prev);


    //: Used for sorting by decreasing magnitude
    static bool dec_mag_order(const rgrl_feature_sptr& f1,
                              const rgrl_feature_sptr& f2);

    //: Used for sorting by increasing magnitude
    static bool inc_mag_order(const rgrl_feature_sptr& f1,
                              const rgrl_feature_sptr& f2);

  private:
    //: Copy Constructor
    vidreg_feature_edgel(const vidreg_feature_edgel& other);

    double magnitude_;
    //: dumb pointers to next and previous in the edgel chain
    vidreg_feature_edgel* next_;
    vidreg_feature_edgel* prev_;
};


class vidreg_edge
{
  public:
    //: Constructor
    vidreg_edge(vidreg_feature_edgel* head);

    //: Access the number of edgels
    unsigned size() const { return size_; }

    //: Access the average manitude along the edge
    double avg_magnitude() const { return avg_magnitude_; }

    vidreg_feature_edgel* head() const { return head_; }

    //: Used for sorting by decreasing magnitude
    static bool dec_mag_order(const vidreg_edge& e1,
                              const vidreg_edge& e2);

    //: Used for sorting by decreasing size
    static bool dec_size_order(const vidreg_edge& e1,
                               const vidreg_edge& e2);

  private:
    vidreg_feature_edgel* head_;
    unsigned size_;
    double avg_magnitude_;
};


#endif // vidreg_feature_edgel_h_
