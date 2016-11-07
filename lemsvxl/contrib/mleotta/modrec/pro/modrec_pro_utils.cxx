// This is contrib/mleotta/modrec/pro/modrec_pro_utils.cxx

//:
// \file



#include "modrec_pro_utils.h"
#include <imesh/imesh_fileio.h>
#include <vul/vul_file.h>



void modrec_pro_read_models(const vcl_string& path,
                            vcl_vector<imesh_mesh>& models,
                            vcl_vector<vcl_string>& names)
{
  models.clear();
  names.clear();
  bxml_document models_doc = bxml_read(path);
  vcl_string base_path = vul_file::dirname(path);
  bxml_element* root_elm = static_cast<bxml_element*>(models_doc.root_element().ptr());
  for(bxml_element::const_data_iterator i = root_elm->data_begin();
      i != root_elm->data_end();  ++i)
  {
    if((*i)->type() != bxml_data::ELEMENT)
      continue;
    bxml_element* elm = static_cast<bxml_element*>(i->ptr());
    if(elm->name() != "model")
      continue;
    vcl_string file = elm->attribute("ply2_file");
    imesh_mesh tmp_mesh;
    if(imesh_read_ply2(base_path+"/"+file,tmp_mesh)){
      models.push_back(tmp_mesh);
      names.push_back(elm->attribute("name"));
    }
  }
}

