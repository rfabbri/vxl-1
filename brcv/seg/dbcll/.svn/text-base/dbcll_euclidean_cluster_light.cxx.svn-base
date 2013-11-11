//:
// \file
// \author Isabel Restrepo
// \date 8-Apr-2011

#include "dbcll_euclidean_cluster_light.h"

void dbcll_xml_write(const vcl_vector<unsigned> &sizes, const vcl_vector<double> sse_vector, vcl_string xml_file)
{
  bxml_document doc;
  bxml_element *root = new bxml_element("dbcll_clusters");
  doc.set_root_element(root);
  root->append_text("\n");
  
  double total_sse = 0.0;
  unsigned long total_size = 0;
  for(unsigned ci = 0; ci<sse_vector.size(); ci++)
  {
    bxml_element* cluster_elm = new bxml_element("cluster");
    cluster_elm->append_text("\n");
    cluster_elm->set_attribute("size", sizes[ci]);
    cluster_elm->set_attribute("id", ci );
    if(sizes[ci]>0)
      cluster_elm->set_attribute("var", sse_vector[ci]/(double)sizes[ci]);
    else
      cluster_elm->set_attribute("var", 0.0);

    cluster_elm->set_attribute("sse", sse_vector[ci] );
    
    root->append_data(cluster_elm);
    root->append_text("\n");
    
    total_sse+=sse_vector[ci];
    total_size+= sizes[ci];
    
  }
  
  bxml_element* general_elm = new bxml_element("general");
  general_elm->append_text("\n");
  general_elm->append_text("\n");
  general_elm->set_attribute("total_size", total_size);
  general_elm->set_attribute("total_sse", total_sse, 15);
  root->append_data(general_elm);
  root->append_text("\n");
  
  //write to disk  
  vcl_ofstream os(xml_file.c_str());
  os.precision(15);
  bxml_write(os, doc);
  os.close();  
  
}


