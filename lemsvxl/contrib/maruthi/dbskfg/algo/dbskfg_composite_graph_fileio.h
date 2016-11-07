// This is brcv/shp/dbskfg/algo/dbskfg_composite_graph_fileio.h
#ifndef dbskfg_composite_graph_fileio_h_
#define dbskfg_composite_graph_fileio_h_
//:
// \file
// \brief Load and save composite graph from an .xml file
// \author Maruthi Narayanan (mn@lems.brown.edu)
// \date 09/21/10
//
// 
// \verbatim
//  Modifications
//   Maruthi Narayanan Sep 21 2010 Initial Version
//
// \endverbatim

#include <dbskfg/dbskfg_composite_graph_sptr.h>
#include <dbskfg/dbskfg_composite_node_sptr.h>
#include <dbskfg/dbskfg_composite_link_sptr.h>
#include <dbskfg/dbskfg_rag_graph_sptr.h>
#include <dbskfg/dbskfg_rag_node_sptr.h>
#include <dbskfg/algo/dbskfg_transform_descriptor_sptr.h>
#include <vil/vil_image_resource_sptr.h>

#include <vcl_string.h>
#include <vcl_map.h>

#include <bxml/bxml_read.h>
#include <vgl/vgl_box_2d.h>

class dbskfg_shock_link;

//: class to load and save extrinsic shock graph files
class dbskfg_composite_graph_fileio
{

public:

  //: Constructor
  dbskfg_composite_graph_fileio();

  //:Destructor
  ~dbskfg_composite_graph_fileio();

  //: Load a composite graph xml file
  void load_composite_graph(
      dbskfg_composite_graph_sptr composite_graph,vcl_string filename);

  //: Write out a specific rag node
  void write_out(const dbskfg_rag_graph_sptr& rag_graph,
                 const vil_image_resource_sptr& image,
                 unsigned int id_to_extract,
                 double prune_threshold=1.0,
                 vcl_string output_prefix="",
                 vcl_string output_folder="");

  //: Write out all rag nodes
  void write_out(const dbskfg_rag_graph_sptr& rag_graph,
                 const vil_image_resource_sptr& image,
                 vcl_string output_prefix,
                 vcl_string output_folder,
                 double contour_threshold,
                 double prune_threshold);

  //: Write out all rag nodes
  void write_out(const dbskfg_rag_graph_sptr& rag_graph,
                 const vil_image_resource_sptr& image,
                 vcl_string output_prefix,
                 vcl_string output_folder,
                 double contour_threshold,
                 double prune_threshold,
                 vgl_box_2d<double> bbox);

  //: Write out rag node
  void write_out(const dbskfg_rag_node_sptr& rag_node,
                 const vil_image_resource_sptr& image,
                 double prune_threshold=1.0,
                  vcl_string output_prefix="",
                 vcl_string output_folder="");

  //: Write out transform
  void write_out_transform(const dbskfg_transform_descriptor_sptr& transform,
                           const vil_image_resource_sptr& image,
                           vcl_string filename);


  // Write out composite graph to two bnd files
  void write_contour_composite_graph(dbskfg_composite_graph_sptr 
                                     composite_graph,
                                     vcl_string filename);

private:

  //: Write an xml file of the graph
  void write_rag_node(const dbskfg_rag_node_sptr& rag_node,
                      vcl_map<unsigned int,dbskfg_shock_link*>& shock_links,
                      vcl_string file_name);

  //: Write all rag_nodes to one file
  void write_rag_node_one_file(const dbskfg_rag_node_sptr& rag_node,
                               vcl_map<unsigned int,dbskfg_shock_link*>& 
                               shock_links);

  //: Write an image of the graph
  void write_rag_node_image(const dbskfg_rag_node_sptr& rag_node,
                            vcl_map<unsigned int,dbskfg_shock_link*>& 
                            shock_links,
                            const vil_image_resource_sptr& image,
                            vcl_string file_name);

  //: Keep a method to prune ids
  void prune_rag_node(vcl_map<unsigned int,dbskfg_shock_link*>& prune_links,
                      dbskfg_rag_node_sptr rag_node,double prune_threshold);

  //: compile all nodes
  void compile_nodes(const bxml_data_sptr& root_xml);
 
  //: compile all links
  void compile_links(const bxml_data_sptr& root_xml);

  //: classify nodes of composite graph
  void classify_nodes();

  // Make copy constructor private
  dbskfg_composite_graph_fileio(const dbskfg_composite_graph_fileio&);

  // Make assignment operator private
  dbskfg_composite_graph_fileio& operator=(
      const dbskfg_composite_graph_fileio&);

  //: keep a composite graph
  dbskfg_composite_graph_sptr composite_graph_;

  //: Keep track of file name to write out
  vcl_string file_name_;

  //: Keep a set of all nodes
  vcl_map<unsigned int,dbskfg_composite_node_sptr> all_nodes_;
  
  //: Keep track of all contour links
  vcl_map<unsigned int,dbskfg_composite_link_sptr> contour_links_;


};

#endif //dbskfg_composite_graph_fileio_h_
