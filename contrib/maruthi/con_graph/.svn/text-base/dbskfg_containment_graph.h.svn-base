// This is brcv/shp/dbskfg/dbskfg_containment_graph.h
#ifndef dbskfg_containment_graph_h_
#define dbskfg_containment_graph_h_

//:
// \file
// \brief A class that holds all possible transform paths 
//        
// \author Maruthi Narayanan ( mn@lems.brown.edu)
// \date July 07, 2010
//
// \verbatim
//  Modifications
// \endverbatim

#include <dbgrl/dbgrl_graph.h>
#include <dbskfg/dbskfg_rag_node_sptr.h>
#include <con_graph/dbskfg_containment_node.h>
#include <con_graph/dbskfg_containment_link.h>
#include <vidpro1/storage/vidpro1_vsol2D_storage_sptr.h>
#include <vidpro1/storage/vidpro1_image_storage_sptr.h>
#include <vil/vil_image_resource_sptr.h>
#include <vsol/vsol_spatial_object_2d_sptr.h>
#include <vsol/vsol_spatial_object_2d.h>
#include <vcl_string.h>

//: 
class dbskfg_containment_graph : 
public dbgrl_graph<dbskfg_containment_node, dbskfg_containment_link>
{
public:

    // -------------------------------------------------------------------------
    // Constructors / Destructors
    // -------------------------------------------------------------------------
    //: Constructor
    dbskfg_containment_graph(vidpro1_vsol2D_storage_sptr contour_map,
                             dbskfg_containment_node::ExpandType expansion,
                             double path_threshold);
  
    //: Destructor
    /* virtual */ ~dbskfg_containment_graph();
  
    // Get next available id
    //: increment the id counter and return new id
    unsigned int next_available_id();

    // Get current id
    unsigned int current_id(){ return next_available_id_;}

    // -------------------------------------------------------------------------
    // DATA ACCESS
    // -------------------------------------------------------------------------

    //: Return the type identifier string
    vcl_string is_a() const {return "dbskfg_containment_graph"; }

    //: Compute region root
    void compute_region_root(dbskfg_rag_node_sptr rag_node);
  
    //: Expand region rood
    bool expand_region_root(dbskfg_rag_node_sptr rag_node);

    //: Print out graph
    void print(vcl_ostream& os);

    //: Returns depth map
    vcl_map<unsigned int,unsigned int>& depth_map(){return depth_map_;}

    //: Set outputfilename
    void set_output_filename(vcl_string file){output_filename_ = file;}
    
    void print_node_cache();

    //: Return node if
    dbskfg_containment_node_sptr node_merge(vcl_map<unsigned int,bool>&
                                            attr ,
                                            vcl_set<vcl_string>& wavefront);

    //: insert node
    void insert_node(dbskfg_containment_node_sptr node);

    //: Expand tree
    void expand_tree(vcl_ofstream& stream);

    
protected:
  

private:
  
    // Hold next available id
    unsigned int next_available_id_;
  
    // Holds expansion type
    dbskfg_containment_node::ExpandType expand_type_;

    // Holds all contours with their ids
    vcl_map<unsigned int, vcl_vector<vsol_spatial_object_2d_sptr> > 
        original_list_;

    // Keep initial attributes
    vcl_map<unsigned int,bool> initial_attributes_;

    // Keep a map of all nodes at depth
    vcl_map<unsigned int,unsigned int> depth_map_;

    // Keep nodes by depth
    vcl_map<unsigned int, vcl_vector<dbskfg_containment_node_sptr> >
        depth_nodes_;

    // Keep a map of all nodes for easy cache
    vcl_map< vcl_vector<bool>, vcl_vector< vcl_pair<vcl_set<vcl_string>
        ,dbskfg_containment_node_sptr > > > node_cache_;

    // Keep a threshold for total path probability
    double path_threshold_;

    // Keep outfile name
    vcl_string output_filename_;

    //: Print node out
    void print_node(vcl_map<unsigned int,bool> attributes,
                    vcl_string filename,
                    vgl_polygon<double>& vgl_poly);

    //: Expand tree level
    void expand_tree_level(
        vcl_vector<dbskfg_containment_node_sptr>& queue,
        vcl_ofstream& ofstream);

    // Make copy constructor private
    dbskfg_containment_graph(const dbskfg_containment_graph&);

    // Make assignment operator private
    dbskfg_containment_graph& operator=(const dbskfg_containment_graph&);

    
};

#endif // brcv/shp/dbskfg/dbskfg_containment_graph.h


