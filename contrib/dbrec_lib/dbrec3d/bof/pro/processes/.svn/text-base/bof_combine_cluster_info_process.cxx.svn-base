//:
// \brief
// \file
// \author Isabel Restrepo
// \date 5-Apr-2011

#include <bprb/bprb_func_process.h>
#include <bprb/bprb_parameters.h>

#include <brdb/brdb_value.h>

#include <bxml/bxml_read.h>
#include <bxml/bxml_find.h>

#include <bof/bof_codebook.h>
#include <dbcll/dbcll_euclidean_cluster.h>

#include <vcl_fstream.h>

//:global variables
namespace bof_combine_cluster_info_process_globals 
{
  const unsigned n_inputs_ = 4;
  const unsigned n_outputs_ = 0;
}


//:sets input and output types
bool bof_combine_cluster_info_process_cons(bprb_func_process& pro)
{
  using namespace bof_combine_cluster_info_process_globals ;
  
  vcl_vector<vcl_string> input_types_(n_inputs_);
  input_types_[0] = "vcl_string"; //suffix of info files to combine
  input_types_[1] = "unsigned"; //number of info files to combine
  input_types_[2] = "unsigned"; //number of means 
  input_types_[3] = "vcl_string"; //info file out

  
  vcl_vector<vcl_string> output_types_(n_outputs_);
 
  
  return pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
}


//:the process
bool bof_combine_cluster_info_process(bprb_func_process& pro)
{
  using namespace bof_combine_cluster_info_process_globals;
  
  //get inputs
  vcl_string sfx = pro.get_input<vcl_string>(0);
  unsigned n_files = pro.get_input<unsigned>(1);  
  unsigned k = pro.get_input<unsigned>(2);  
  vcl_string info_file_out = pro.get_input<vcl_string>(3);

  vcl_vector<unsigned> sizes(k,0);
  vcl_vector<double> sse_v(k,0.0);
  
  for (unsigned fi = 0; fi < n_files; fi++) {
    
    //read info file
    vcl_stringstream ss;
    ss << sfx << fi << "_info.xml";
    
    vcl_ifstream ifs(ss.str().c_str());
    if(!ifs.is_open()){
      vcl_cerr << "Error: Could not open file: " << ss.str() <<  "\n";
      return false;
    }
    
    bxml_document doc = bxml_read(ifs);
    bxml_element query("dbcll_clusters");
    bxml_data_sptr root = bxml_find_by_name(doc.root_element(), query);
    if (!root) {
      vcl_cerr << "Error: bof_info - could not parse xml root\n";
      return false;
    }

    bxml_element cluster_query("cluster");
    vcl_vector<bxml_data_sptr> clusters_data = bxml_find_all_with_name(root, cluster_query);
    
    if(clusters_data.size()!=k){
      vcl_cerr << "Wrong number of clusters in xml file \n";
      return false;
    }
    
    for (unsigned ci = 0; ci < k; ci++) {
      bxml_element* cluster_elm = dynamic_cast<bxml_element*>(clusters_data[ci].ptr());
      unsigned id;
      unsigned size = 0;
      double sse = 0.0;
      cluster_elm->get_attribute("id", id);
      cluster_elm->get_attribute("size", size);
      cluster_elm->get_attribute("sse", sse);
      
      sizes[id]+=size;
      sse_v[id]+=sse;
     }
    
  }
  
  //create a vector o clusters with dummy mean to be written to xml
  dbcll_xml_write(sizes, sse_v, info_file_out);
 
  return true;
}