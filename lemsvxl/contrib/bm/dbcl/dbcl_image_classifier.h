//this is /contrib/bm/dbcl/dbcl_image_classifier.h
#ifndef dbcl_image_classifier_h_
#define dbcl_image_classifier_h_
//:
// \file
// \date August 30, 2010
// \author Brandon A. Mayer
//
// Class to classify non compact neighborhoods from images
//
// \verbatim
//  Modifications
// \endverbatim

#include"dbcl_classifier_factory.h"
#include"dbcl_linear_classifier.h"
#include"dbcl_linear_classifier_vrml_view.h"


#include"dncn_factory.h"
#include"dncn_less_than.h"


class dbcl_image_classifier
{
public:

    typedef vcl_map<vgl_point_2d<unsigned>, dbcl_classifier_sptr, dncn_less_than > target_classifier_map_type;

    dbcl_image_classifier(){}

    ~dbcl_image_classifier(){}

    dbcl_image_classifier( registered_classifiers const& classifier_type, dncn_target_list_2d_sptr target_list_sptr );

    //process
    void classify();

    //getter
    target_classifier_map_type target_classifier_map(){ return this->target_classifier_map_; }

    unsigned rows(){ return rows_; }

    unsigned cols(){ return cols_; }

    unsigned nframes(){ return nframes_; }

	unsigned target(){ return this->target_; }

    //visualize
    void draw_classifiers_vrml( vcl_string result_glob );

private:
    target_classifier_map_type target_classifier_map_;
    unsigned rows_;
    unsigned cols_;
    unsigned nframes_;
	unsigned target_;
};

#endif //dbcl_image_classifier_h_