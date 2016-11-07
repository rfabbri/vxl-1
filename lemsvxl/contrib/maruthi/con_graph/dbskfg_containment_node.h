// This is a class to represent containment nodes in a composite graph
#ifndef dbskfg_containment_node_h_
#define dbskfg_containment_node_h_

//:
// \file
// \brief A class to represent all the geometry/classification needed for
//        a containment node in a composite graph
//  
// \author Maruthi Narayanan
// \date  06/27/2010
//
// \verbatim
//  Modifications
// \endverbatim

#include <dbgrl/dbgrl_vertex.h>
#include <con_graph/dbskfg_containment_link_sptr.h>
#include <vsol/vsol_spatial_object_2d_sptr.h>

#include <dbskfg/algo/dbskfg_transform_descriptor.h>
#include <dbskfg/algo/dbskfg_transform_descriptor_sptr.h>
#include <dbskfg/algo/dbskfg_detect_transforms_sptr.h>
#include <dbskfg/dbskfg_composite_graph_sptr.h>
#include <dbskfg/dbskfg_rag_graph_sptr.h>
#include <dbskfg/dbskfg_rag_node_sptr.h>
#include <con_graph/dbskfg_containment_node_sptr.h>
#include <vidpro1/storage/vidpro1_image_storage.h>
#include <vidpro1/storage/vidpro1_image_storage_sptr.h>
#include <vcl_ostream.h>
#include <vcl_vector.h>
#include <vcl_map.h>
#include <vcl_set.h>


class dbskfg_containment_graph;
class dbskfg_containment_link;

class dbskfg_containment_node : public dbgrl_vertex<dbskfg_containment_link>
{

public:

    enum ExpandType
    {
        IMPLICIT,
        EXPLICIT
    };

    // Constructor needed for implicit method
    dbskfg_containment_node(unsigned int id,
                            unsigned int rag_node_id,
                            unsigned int depth);

    // Construt needed for explicit method
    dbskfg_containment_node(unsigned int id,
                            dbskfg_composite_graph& prev_cgraph,
                            dbskfg_rag_graph& prev_rag_graph,
                            unsigned int depth);

    // Destructor
    ~dbskfg_containment_node();

    // ******************** Info Methods ************************************
    // Prints out information about this node
    void print(vcl_ostream& os);

    // Print map
    void print_cem(  
        vcl_map<unsigned int,vcl_vector<vsol_spatial_object_2d_sptr> >& 
        contours);
     
    // Returns id
    unsigned int id(){return id_;}

    // ******************** Setters/Getters ************************************

    // Holds the attribute vector which tells which contours on/off
    void set_attributes(vcl_map<unsigned int,bool> attr){attr_ = attr;}

    // Returns attribute map
    vcl_map<unsigned int,bool> get_attributes(){return attr_;}

    // Holds the original set of contour ids for this node
    void set_rag_con_ids(vcl_set<unsigned int> ids){
        rag_con_ids_=ids;}
   
    // Holds the initial wavefront of the node
    void set_rag_wavefront(vcl_set<vcl_string> wavefront){
        rag_node_wavefront_=wavefront;}

    // Holds the initial polygon of the node
    void set_polygon(vgl_polygon<double> polygon){
        poly_=polygon;}

    // Holds the initial polygon of the node
    vgl_polygon<double>& get_polygon(){return poly_;}

    // Holds the initial wavefront of the node
    vcl_set<vcl_string> get_rag_wavefront(){return rag_node_wavefront_;}

    // Update attributes
    void update_attributes();

    // Combine node
    bool nodes_merge(dbskfg_containment_node_sptr compare_node,
                     vcl_vector<unsigned int> contours_affected,
                     dbskfg_transform_descriptor::TransformType transform);

    // Node has zero contours
    bool node_empty();

    // Return depth of node
    unsigned int depth(){return depth_;}

    // *************** Methods for expanding nodes ****************************x
    // Expand node using explicit method
    void expand_node_explicit(
        vcl_map<unsigned int,dbskfg_containment_node_sptr>& nodes,
        vcl_map<unsigned int,vcl_vector<vsol_spatial_object_2d_sptr> >& 
        contours,
        dbskfg_containment_graph& con_graph);

    // Expand node using implicit method
    void expand_node_implicit(
        vcl_map<unsigned int,dbskfg_containment_node_sptr>& nodes,
        vcl_map<unsigned int,vcl_vector<vsol_spatial_object_2d_sptr> >& 
        contours,
        dbskfg_containment_graph& con_graph);

    // Expand node using implicit method
    void expand_node_implicit_multi(
        vcl_map<unsigned int,vcl_vector<vsol_spatial_object_2d_sptr> >& 
        contours,
        dbskfg_containment_graph& con_graph);

    // Expand root node
    void expand_root_node(
        vcl_map<unsigned int,vcl_vector<vsol_spatial_object_2d_sptr> >& 
        contours,
        dbskfg_containment_graph& con_graph,
        dbskfg_rag_node_sptr root_node);

    // Expand the node
    void expand_node_helper(
        const dbskfg_transform_descriptor_sptr& transform,
        vcl_map<unsigned int,vcl_vector<vsol_spatial_object_2d_sptr> >& 
        contours,
        vcl_vector<unsigned int>& contours_affected);

    // Insert new node
    void insert_new_node(
         dbskfg_containment_graph& cgraph,
         const vcl_vector<unsigned int>& contours_affected,
         const dbskfg_transform_descriptor_sptr& transform,
         dbskfg_rag_node_sptr rag_node);


    // Insert new node
    void insert_new_node(
         vcl_map<unsigned int,dbskfg_containment_node_sptr>& nodes,
         dbskfg_containment_graph& cgraph,
         const vcl_vector<unsigned int>& contours_affected,
         const dbskfg_transform_descriptor_sptr& transform);

    // Set probability of fragment
    double get_prob_of_fragment(){return prob_fragment_;}

    // Set probability of fragment
    void set_prob_of_fragment(double prob){prob_fragment_=prob;}

    // Print node
    void print_rag_node(ExpandType expansion);

private:

    // Hold id for node
    unsigned int id_;

    // Holds vector of all attributes
    vcl_map<unsigned int,bool> attr_;

    // Holds contours of original rag node
    vcl_set<unsigned int> rag_con_ids_;

    // Holds rag id for this node
    unsigned int rag_node_id_;

    // Holds this composite graph
    dbskfg_composite_graph_sptr cgraph_;

    // Holds rag graph
    dbskfg_rag_graph_sptr rag_graph_;

    // Holds depth of this node
    unsigned int depth_;

    // Holds probability of fragment
    double prob_fragment_;

    // holds original polygon
    vgl_polygon<double> poly_;

    // Keep a set of old rag node wavefront
    vcl_set<vcl_string> rag_node_wavefront_;

    // Make copy constructor private
    dbskfg_containment_node(const dbskfg_containment_node&);

    // Make assignment operator private
    dbskfg_containment_node& operator=(const dbskfg_containment_node&);
};

#endif // dbskfg_containment_node_h_


