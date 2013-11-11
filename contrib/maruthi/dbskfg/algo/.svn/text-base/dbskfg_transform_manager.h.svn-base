// This is brcv/shp/dbskfg/algo/dbskfg_transform_manager.h
#ifndef dbskfg_transform_manager_h_
#define dbskfg_transform_manager_h_
//:
// \file
// \brief Algorithm to detect transforms 
// \author Maruthi Narayanan
// \date 07/08/10
// 

// \verbatim
//  Modifications
//   Maruthi Narayanan 07/08/2010    Initial version.
//
// \endverbatim 
#include <dbskfg/algo/dbskfg_transform_descriptor_sptr.h>
#include <dbskfg/dbskfg_composite_graph_sptr.h>
#include <dbskfg/dbskfg_rag_graph_sptr.h>
#include <dbskfg/dbskfg_rag_node_sptr.h>
#include <vil/vil_image_resource_sptr.h>
#include <vcl_vector.h>
#include <vcl_map.h>
#include <vil3d/vil3d_image_view.h>

//: Form Composite Graph algorithm
class dbskfg_transform_manager 
{

public:

    // Set singleton
    static dbskfg_transform_manager& Instance()
    {
        static dbskfg_transform_manager manager;
        return manager;
    }
  
    // ************************ Setters/Getters *******************************

    // Set rag graph
    void set_rag_graph(dbskfg_rag_graph_sptr rag_graph)
    {rag_graph_ = rag_graph;}

    // Set cgraph
    void set_cgraph(dbskfg_composite_graph_sptr cgraph)
    {cgraph_ = cgraph;}

    // Set image
    void set_image(vil_image_resource_sptr image){image_=image;}

    // Set threshold cost
    void set_threshold(double threshold){threshold_ = threshold;}

    // Set ess completion
    void set_ess_completion(double ess){ess_ = ess;}

    // Set alpha completion
    void set_alpha(double alpha){alpha_ = alpha;}

    // Set output folder
    void set_output_frag_folder(vcl_string folder){out_folder_= folder;}

    // Set output prefix
    void set_output_prefix(vcl_string prefix){out_prefix_=prefix;}

    // Start binary file output
    void start_binary_file(vcl_string binary_file_output);

    // Start binary file output
    void start_region_file(vcl_string binary_file_output);

    // Start xml output
    void start_xml_file(vcl_string xml_file_output);

    // Get image
    vil_image_resource_sptr get_image()
    {return image_;}

    // Get rag graph
    dbskfg_rag_graph_sptr get_rag_graph()
    {return rag_graph_;}

    // Get cgraph
    dbskfg_composite_graph_sptr get_cgraph()
    {return cgraph_;}

    // Get threshold cost
    double get_threshold(){return threshold_;}

    // Get ess completion
    double get_ess(){return ess_;}

    // Get alpha completion
    double get_alpha(){return alpha_;}

    // Get output folder
    vcl_string get_output_frag_folder(){return out_folder_;}

    // Get output prefix
    vcl_string get_output_prefix(){return out_prefix_;}

    // Get bxml document
    bxml_document& get_xml_file(){return xml_file_;}

    // Get output stream
    void write_output_polygon(vgl_polygon<double>& poly);

    // Get output stream
    void write_output_region(dbskfg_rag_node_sptr rag_node);

    // Get output stream
    void write_xml_file();
 
    // Gets the original gap map
    vcl_map<vcl_string,unsigned int>& gap_map(){return gap_map_;}

    // Returns the map for storing existing gaps in root node
    vcl_map<vcl_string,unsigned int>& old_gap_map(){return old_gap_map_;}

    // Gets all the tranforms that this singleton holds
    vcl_vector<dbskfg_transform_descriptor_sptr>& objects()
    {return transforms_;}

    // Returns the transform for a specific id
    dbskfg_transform_descriptor_sptr& transform_at_id(unsigned int index)
    {return transforms_[index];}

    // Returns whether point is endpoint
    bool endpoint(vcl_string endpoint)
    {return loop_map_.count(endpoint) > 0;}

    // Returns whether endpoint pair exists
    bool loop_pair(vcl_pair<vcl_string,vcl_string> pair1)
    {return loop_end_map_.count(pair1) > 0;}

    // Returns transform held by contour id
    unsigned int loop_by_contour_id(unsigned int idb)
    {
        if ( loop_contour_map_.count(idb ) )
        {
            return loop_contour_map_[idb];
        }

        return 0;
    }

    // Returns transform held by endpoints
    dbskfg_transform_descriptor_sptr& loop_by_endpoints(
        vcl_pair<vcl_string,vcl_string> pair1)
    {
        return transforms_[loop_end_map_[pair1]];
    }

    // Returns transforms helds at this loop
    dbskfg_transform_descriptor_sptr& loop_at_endpoint(vcl_string temp)
    {return transforms_[loop_map_[temp]];}

    // Destroy singleton 
    void destroy_singleton();

    // Populates data structure
    void update_transforms_conflicts();

    // Read in training data
    void read_in_training_data(vcl_string filename);

    // Set beta0 for logitic function
    void set_beta0_logit(double beta0){logistic_beta0_ = beta0;}

    // Set beta1 for logitic function
    void set_beta1_logit(double beta1){logistic_beta1_ = beta1;}

    // Determine gap cost
    double transform_probability(double gamma_norm,double k0_norm, 
                                 double length);

    void find_transform(vcl_vector<dbskfg_composite_link_sptr>&
                        contour_links_to_remove,
                        dbskfg_transform_descriptor_sptr& grouped_transform);


    void write_binary_transforms(vcl_string binary_file_output);

private:

    // Keep track of all transform objects
    vcl_vector<dbskfg_transform_descriptor_sptr> transforms_;

    // Store a loop map
    vcl_map<vcl_string,unsigned int> loop_map_;

    // Store all loops by contour id
    vcl_map<unsigned int,unsigned int> loop_contour_map_;

    // Store all loops by id of contour remove
    vcl_map<unsigned int,unsigned int> loop_segment_map_;

    // Stores all loops by endpoints
    vcl_map< vcl_pair<vcl_string,vcl_string> , unsigned int> loop_end_map_;

    // Store a gap map that is populated with contour ids 
    vcl_map<vcl_string,unsigned int> gap_map_;

    // Stores a map of all exisiting gaps
    vcl_map<vcl_string,unsigned int> old_gap_map_;

    //: store image
    vil_image_resource_sptr image_; 
 
    // Keep track of composite graph
    dbskfg_composite_graph_sptr cgraph_;

    // Keep track of rag graph
    dbskfg_rag_graph_sptr rag_graph_;

    // Keep threshold for transforms
    double threshold_;

    // Keep ess for completion
    double ess_;

    // Keep alpha for app/con
    double alpha_;

    // Keep a 3d volume of distances from separation surface
    vil3d_image_view<double> dist_volume_;

    // Keep output folder for fragments
    vcl_string out_folder_;

    // Keep output prefix for fragments
    vcl_string out_prefix_;

    // Keep binary file
    vcl_string output_binary_file_;

    // Keep binary file
    vcl_string output_region_file_;

    // Keep binary file
    vcl_string xml_file_output_;

    //: First Coefficient of logistic function 
    double logistic_beta0_;

    //: Second Coefficient of logistic function 
    double logistic_beta1_;

    //: Keep xml file
    bxml_document xml_file_;

    // Make default constructor private
    dbskfg_transform_manager();

    // Make default dtor private
    ~dbskfg_transform_manager();

    // Make copy ctor private
    dbskfg_transform_manager(const dbskfg_transform_manager&);

    // Make assign operator private
    dbskfg_transform_manager& operator
        =(const dbskfg_transform_manager& );
   
};

#endif //dbsk2d_ishock_prune_h_
