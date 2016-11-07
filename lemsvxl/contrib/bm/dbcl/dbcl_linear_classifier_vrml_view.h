//this is /contrib/bm/dbcl/dbcl_linear_classifier_vrml_view.h
#ifndef dbcl_linear_classifier_vrml_view_h_
#define dbcl_linear_classifier_vrml_view_h_
//:
// \file
// \date August 23, 2010
// \author Brandon A. Mayer
//
// Class with static method to produce a wrl file for visualization of test/training data and
// the hyperplane decision boundary of the classifier.
//
// \verbatim
//  Modifications
// \endverbatim

#include"dbcl_classifier_sptr.h"
#include"dbcl_linear_classifier_sptr.h"

#include"dvrml.h"

#include<vnl/vnl_math.h>
#include<vcl_ostream.h>

#include<vgl/vgl_box_2d.h>
#include<vgl/vgl_line_segment_3d.h>
#include<vgl/vgl_intersection.h>
#include<vgl/vgl_distance.h>

class dbcl_decision_boundary
{
public:
    dbcl_decision_boundary(){}
    dbcl_decision_boundary( vgl_line_segment_3d<double>& l, unsigned& tl, unsigned& bl ):line(l),top_label(tl),bottom_label(bl){}
    dbcl_decision_boundary( vgl_point_3d<double>& endpt1, vgl_point_3d<double>& endpt2, unsigned& tl, unsigned& bl):
    top_label(tl),bottom_label(bl){ line.set(endpt1,endpt2); }

    ~dbcl_decision_boundary(){}

    vgl_line_segment_3d<double> line;
    unsigned top_label;
    unsigned bottom_label;
};

class dbcl_linear_classifier_vrml_view
{
public:
    static void vrml_view(vcl_ofstream& os, dbcl_classifier_sptr classifier_sptr);

    //this is a helper function, call vrml_view regardless
    //static void vrml_view_3d( vcl_ofstream& os, dbcl_classifier_sptr classifier_sptr);

    ~dbcl_linear_classifier_vrml_view(){}
private:
    dbcl_linear_classifier_vrml_view(){}

    
};

#endif //dbcl_linear_classifier_vrml_view_h_