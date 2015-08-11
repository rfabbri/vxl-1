// This is brcv/shp/dbskfg/algo/dbskr_align_shapes.h
#ifndef dbskr_align_shapes_h_
#define dbskr_align_shapes_h_
//:
// \file
// \brief Algorithm to compute shape based alignment of shapes and save
// to binary file
// \author Maruthi Narayanan
// \date 08/07/2015
// 

// \verbatim
//  Modifications
//   Maruthi Narayanan 08/07/2015    Initial version.
//
// \endverbatim 

#include <vcl_string.h>
#include <vcl_vector.h>
#include <vcl_utility.h>


#include <dbskr/dbskr_tree.h>

//: Form Composite Graph algorithm
class dbskr_align_shapes
{

public:

    //: Constructor
    dbskr_align_shapes(vcl_string model_filename,
                       vcl_string query_filename,
                       bool elastic_splice_cost    = false, 
                       float scurve_sample_ds      = 5.0f, 
                       float scurve_interpolate_ds = 1.0f, 
                       bool localized_edit         = false,
                       double scurve_matching_R    = 6.0f, 
                       bool circular_ends          = true, 
                       bool combined_edit          = false, 
                       bool use_approx             = true,
                       double lambda_area          = 10000);

    //: Destructor
    ~dbskr_align_shapes();

    //: Match
    void match();

private:

    bool elastic_splice_cost_;
    float scurve_sample_ds_;
    float scurve_interpolate_ds_; 
    bool localized_edit_;
    double scurve_matching_R_; 
    bool circular_ends_; 
    bool combined_edit_;
    bool use_approx_;
    double lambda_area_;

    // Loop all model shock files and keeps a mirrored version also
    vcl_vector< vcl_pair<dbskr_tree_sptr,dbskr_tree_sptr> > model_trees_;

    // Load all query shock files and keeps a mirrored version also
    vcl_vector< vcl_pair<dbskr_tree_sptr,dbskr_tree_sptr> > query_trees_;

    // Keep track of correspondence

    //: Curve list 1
    vcl_vector<dbskr_scurve_sptr> curve_list1_;
    
    //: Curve list 2
    vcl_vector<dbskr_scurve_sptr> curve_list2_;

    //: Map points from curve list 1 to curve list 2
    vcl_vector< vcl_vector < vcl_pair <int,int> > > map_list_;

    //: keep track of query polygons
    vcl_vector<vgl_polygon<double> > query_polygons_;

    // Keep track of whethe query is tree1 or tree 2
    // switch = true, means query is tree2
    bool switched_;

    // Keep track if tree 1 has been mirrored
    bool tree1_mirror_;

    // Keep track if tree 2 has been mirrored
    bool tree2_mirror_;
    
    // Load model
    void load_esf(vcl_string& filename,bool flag);

    // Compute area/boundary
    double compute_boundary(dbsk2d_shock_graph_sptr& sg,
                            vgl_polygon<double>& final_poly);

    // Private matching
    double edit_distance(dbskr_tree_sptr& tree1,dbskr_tree_sptr& tree2,
                         float test_curve_matching_R,
                         bool switched=false,
                         double prev_distance=1.0e6);

    // Get dense correspondence between two shapes
    void shape_alignment(
        dbskr_tree_sptr query_tree,
        vcl_vector<dbskr_scurve_sptr>& curve_list1,
        vcl_vector<dbskr_scurve_sptr>& curve_list2,
        vcl_vector< vcl_vector < vcl_pair <int,int> > >& map_list,
        bool query_mirror=false,
        bool switched=false);

    // Make copy ctor private
    dbskr_align_shapes(const dbskr_align_shapes&);

    // Make assign operator private
    dbskr_align_shapes& operator
        =(const dbskr_align_shapes& );

};

#endif //dbsk2d_ishock_prune_h_
