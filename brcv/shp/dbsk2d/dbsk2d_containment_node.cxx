// This is brcv/shp/dbsk2d/algo/dbsk2d_containment_node.cxx

//:
// \file

#include <dbsk2d/dbsk2d_containment_node.h>
#include <dbsk2d/algo/dbsk2d_ishock_transform.h>
#include <dbsk2d/algo/dbsk2d_ishock_gap_detector.h>
#include <dbsk2d/algo/dbsk2d_ishock_gap_transform.h>
#include <dbsk2d/algo/dbsk2d_ishock_grouping_transform.h>

//: constructor
dbsk2d_containment_node::dbsk2d_containment_node(
    dbsk2d_ishock_transform_sptr parent_transform,
    unsigned int depth,
    unsigned int id)
    :parent_transform_(parent_transform),
     visited_(false),
     depth_(depth),
     id_(id),
     prob_(1.0),
     gap_prob_(1.0)
{

}

