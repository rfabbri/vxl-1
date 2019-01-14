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
#include <string>

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
    std::string is_a() const {return "dbskfg_containment_graph"; }

    //: Compute region root
    void compute_region_root(dbskfg_rag_node_sptr rag_node);
  
    //: Expand region rood
    bool expand_region_root(dbskfg_rag_node_sptr rag_node);

    //: Print out graph
    void print(std::ostream& os);

    //: Returns depth map
    std::map<unsigned int,unsigned int>& depth_map(){return depth_map_;}

    //: Set outputfilename
    void set_output_filename(std::string file){output_filename_ = file;}
    
    void print_node_cache();

    //: Return node if
    dbskfg_containment_node_sptr node_merge(std::map<unsigned int,bool>&
                                            attr ,
                                            std::set<std::string>& wavefront);

    //: insert node
    void insert_node(dbskfg_containment_node_sptr node);

    //: Expand tree
    void expand_tree(std::ofstream& stream);

    
protected:
  

private:
  
    // Hold next available id
    unsigned int next_available_id_;
  
    // Holds expansion type
    dbskfg_containment_node::ExpandType expand_type_;

    // Holds all contours with their ids
    std::map<unsigned int, std::vector<vsol_spatial_object_2d_sptr> > 
        original_list_;

    // Keep initial attributes
    std::map<unsigned int,bool> initial_attributes_;

    // Keep a map of all nodes at depth
    std::map<unsigned int,unsigned int> depth_map_;

    // Keep nodes by depth
    std::map<unsigned int, std::vector<dbskfg_containment_node_sptr> >
        depth_nodes_;

    // Keep a map of all nodes for easy cache
    std::map< std::vector<bool>, std::vector< std::pair<std::set<std::string>
        ,dbskfg_containment_node_sptr > > > node_cache_;

    // Keep a threshold for total path probability
    double path_threshold_;

    // Keep outfile name
    std::string output_filename_;

    //: Print node out
    void print_node(std::map<unsigned int,bool> attributes,
                    std::string filename,
                    vgl_polygon<double>& vgl_poly);

    //: Expand tree level
    void expand_tree_level(
        std::vector<dbskfg_containment_node_sptr>& queue,
        std::ofstream& ofstream);

    // Make copy constructor private
    dbskfg_containment_graph(const dbskfg_containment_graph&);

    // Make assignment operator private
    dbskfg_containment_graph& operator=(const dbskfg_containment_graph&);

    
};

#endif // brcv/shp/dbskfg/dbskfg_containment_graph.h


