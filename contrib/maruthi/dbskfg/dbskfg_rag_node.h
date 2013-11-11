// This is the base class for all nodes in region adjancency graph
#ifndef dbskfg_rag_node_h_
#define dbskfg_rag_node_h_

//:
// \file
// \brief A base class to represent all nodes in region adjancency graph//
//  
// \author Maruthi Narayanan
// \date  07/12/2010
//
// \verbatim
//  Modifications
// \endverbatim

// dbskfg headers
#include <dbskfg/dbskfg_shock_link.h>
#include <dbskfg/dbskfg_rag_node_sptr.h>
// dbgrl headers
#include <dbgrl/dbgrl_vertex.h>
// vgl headers
#include <vgl/vgl_point_2d.h>
// vsol header
#include <vsol/vsol_polygon_2d_sptr.h>
#include <vsol/vsol_polygon_2d.h>
#include <vsol/vsol_spatial_object_2d.h>
#include <vsol/vsol_box_2d.h>
// vcl headers
#include <vcl_vector.h>
#include <vgl/vgl_box_2d.h>
class dbskfg_rag_link;
class dbskfg_shock_node;


class dbskfg_rag_node : public dbgrl_vertex<dbskfg_rag_link>
{

public:

    // Constructor
    dbskfg_rag_node(unsigned int id);

    // Destructor
    //: inline function
    virtual ~dbskfg_rag_node();

    //: Return centroid of this polygon
    //: inline function
    vgl_point_2d<double> pt(){ return pt_; }

    //: Return id of this node
    //: inline function
    unsigned int id(){ return id_; }
   
    //: Return the polygon that encompasses this rag node
    void fragment_boundary(vgl_polygon<double>& poly);

    //: Add shock links to this node
    //: inline function
    void add_shock_link(dbskfg_shock_link* shock);

    //: Returns whether shock link is part of this rag node
    bool contains_shock(dbskfg_composite_link_sptr test);

    //: Deletes a part of the graph encompassing this node
    void delete_shock(unsigned int shock_id);

    // Update outer shock nodes if necessary
    void update_outer_shock_nodes();

    //: prune incomplete wavefront ids
    void prune_wavefront_nodes();

    //: Get wavefront
    vcl_map<unsigned int, dbskfg_shock_node*> get_wavefront()
    {return wavefront_;}

    //: part of wavefront
    bool part_of_wavefront(unsigned int id){
        return (wavefront_.count(id) > 0); }

    //: Return all shock links of this node
    //: inline function
    vcl_map<unsigned int,dbskfg_shock_link*>
        get_shock_links(){return shock_links_;}
    
    //: Test if these two nodes are adjancent
    bool is_adjacent(dbskfg_shock_link* slink);

    //: Test if these two nodes are adjancent
    bool is_adjacent_exclude_endpoints(dbskfg_shock_link* slink);

    bool is_adjacent_overall(dbskfg_shock_link* slink,bool flag=true)
    {
        if ( flag )
        {
            return is_adjacent(slink);
        }
        else
        {
            return is_adjacent_exclude_endpoints(slink);
        }
    }

    //: Returns the ratio of real to non-real contour
    double contour_ratio();

    // Is in wavefront
    bool node_within_wavefront(unsigned int id){return wavefront_.count(id);}

    //: Is rag node within image
    bool is_rag_node_within_image(int ni, int nj);

    // Does rag node intersect bbox
    bool intersection_bbox(vgl_box_2d<double>& bbox);

    //: Does rag only come from endpoints
    bool endpoint_spawned_node();

    //: Does rag only come from endpoints
    bool endpoint_spawned_node_exclude_endpoints();

    //: Is within inner shock nodes
    bool node_within_rag(unsigned int id);

    //: Return information about this node
    void print(vcl_ostream& os);

    //: determine all shock rays
    vcl_vector< vcl_pair<vgl_point_2d<double>,vgl_point_2d<double> > >
        determine_shock_rays(vgl_polygon<double>& polygon);

    //: determine contour endpoints
    void determine_contour_points(vcl_vector<dbskfg_composite_node_sptr>&
                                  con_endpoints);

    // Determine all contour ids affecting this rag node
    void rag_contour_ids(vcl_set<unsigned int>& con_id);

    // See if wavefront exisits in this region
    bool wavefront_exists(
        const vcl_map<unsigned int, vgl_point_2d<double> >& wavefront);

    // Transform affects regions
    bool transform_local_context(dbskfg_transform_descriptor_sptr transform);

    // Return a string representing wavefront based on contour endpoints
    void wavefront_string(vcl_set<vcl_string>& wavefront);

protected:

    // Attributes
    // Point in this case is the centroid of the polygon
    vgl_point_2d<double> pt_;

private:

    // Attributes

    // Keep a map of all shock links for easy access
    vcl_map<unsigned int,dbskfg_shock_link*> shock_links_;

    // Keep an expanding wavefront of nodes to consider
    vcl_map<unsigned int,dbskfg_shock_node*> wavefront_;

    // Keep an ordered collection of nodes for counts
    vcl_map<unsigned int,unsigned int> nodes_visited_;

    // Keep a set of degree 1 nodes
    vcl_map<unsigned int,dbskfg_shock_node*> degree_1_nodes_;

    // Id of this node
    unsigned int id_;

    // Methods

    // Make copy constructor private
    dbskfg_rag_node(const dbskfg_rag_node&);

    // Make assignment operator private
    dbskfg_rag_node& operator=(const dbskfg_rag_node&);
};

#endif // dbskfg_rag_node_h_


