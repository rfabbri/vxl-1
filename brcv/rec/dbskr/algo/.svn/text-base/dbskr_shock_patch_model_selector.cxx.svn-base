// This is brcv/rec/dbskr/algo/dbskr_shock_patch_model_selector.cxx

#include <dbskr/algo/dbskr_shock_patch_model_selector.h>

#include <bil/algo/bil_color_conversions.h>
#include <dbskr/pro/dbskr_shock_patch_storage.h>
#include <dbskr/dbskr_shock_patch.h>
#include <dbskr/algo/dbskr_shock_patch_match.h>
#include <dbskr/algo/dbskr_rec_algs.h>

#include <dbsk2d/dbsk2d_shock_graph.h>
#include <dbsk2d/algo/dbsk2d_extract_subgraph.h>

#include <vil/vil_image_resource.h>

#include <vcl_algorithm.h>

#include <dbdet/tracer/dbdet_contour_tracer.h>

#include <vsol/vsol_polygon_2d.h>
#include <dbskr/dbskr_utilities.h>

void dbskr_shock_patch_model_selector::clear() 
{
  for (unsigned d = 0; d < patch_sets_.size(); d++)
    delete patch_sets_[d];
  patch_sets_.clear();
}

//: extract all the patches at a given depth
//  area_threshold default is 0.1 and no need to pass anything
//  since anything larger than zero works since this is a shock graph of a simple closed
//  curve, there are no inner loops which causes outer traces with nearly zero areas
//  as in the extraction of patches from natural image shock graphs  
bool dbskr_shock_patch_model_selector::extract(int depth, bool circular_ends, double area_threshold)
{
  if (!sg_)
    return false;

  vcl_vector<dbskr_shock_patch_sptr> *pv = new vcl_vector<dbskr_shock_patch_sptr>();
  for (dbsk2d_shock_graph::vertex_iterator v_itr = sg_->vertices_begin(); v_itr != sg_->vertices_end(); v_itr++)
  { 
    if ((*v_itr)->degree() < 3)
      continue;
    
    //: create a patch from the tree rooted at *v_itr and keep the id of this shock node as start_node_id in this patch
    dbskr_shock_patch_sptr sp = extract_patch_from_subgraph(sg_, *v_itr, depth, area_threshold, circular_ends);
   
    dbsk2d_shock_graph_sptr sub_sg = new dbsk2d_shock_graph();    
    dbsk2d_extract_subgraph(sub_sg, 0, *v_itr, depth);
    sp->set_shock_graph(sub_sg);

    if (sp->get_outer_boundary()) {
      pv->push_back(sp);
    }
  }
   
  patch_sets_.push_back(pv);
  return true;
}

//: prune all the patches at this depth for which all the nodes in v_graph's are overlapping
bool dbskr_shock_patch_model_selector::prune_same_patches(int depth)
{

  for (unsigned index = 0; index < patch_sets_.size(); index++) {
    
    vcl_vector<dbskr_shock_patch_sptr> *pv = patch_sets_[index];

    for (unsigned i = 0; i < pv->size(); i++) {
      dbskr_shock_patch_sptr sp = (*pv)[i];
      if (!sp)
        continue;

      if (sp->depth() != depth)
        break;
 
      for (unsigned j = i+1; j <pv->size(); j++) {
        dbskr_shock_patch_sptr sp2 = (*pv)[j];
        if (!sp2)
          continue;

        if (sp->v_graph_same(sp2))  // delete sp2
          (*pv)[j] = 0;
      }
    }

    //: now actually delete all the deleted ones from the patch_sets_
    vcl_vector<dbskr_shock_patch_sptr> temp;
    for (unsigned i = 0; i < pv->size(); i++) {
      if ((*pv)[i])
        temp.push_back((*pv)[i]);
    }

    if (temp.size() > 0) {
      pv->clear();
      pv->insert(pv->begin(), temp.begin(), temp.end());  // add back the kept ones
    }

  }

  return true;
}

//: assumes that the same patches at the same depths are already pruned
bool dbskr_shock_patch_model_selector::prune_same_patches_at_all_depths()
{
  for (unsigned index = 0; index < patch_sets_.size(); index++) {
    
    vcl_vector<dbskr_shock_patch_sptr> *pv = patch_sets_[index];

    for (unsigned i = 0; i < pv->size(); i++) {
      dbskr_shock_patch_sptr sp = (*pv)[i];
      if (!sp)
        continue;

      for (unsigned index2 = 0; index2 < patch_sets_.size(); index2++) {
        if (index == index2)  // assumes that the same patches at the same depths are already pruned
          continue;
    
        vcl_vector<dbskr_shock_patch_sptr> *pv2 = patch_sets_[index2];

        for (unsigned j = 0; j < pv2->size(); j++) {
          dbskr_shock_patch_sptr sp2 = (*pv2)[j];
          if (!sp2)
            continue;

          if (sp->v_graph_same(sp2))  // delete sp2
            (*pv2)[j] = 0;
        }
      }
    }

    //: now actually delete all the deleted ones from the patch_sets_
    vcl_vector<dbskr_shock_patch_sptr> temp;
    for (unsigned i = 0; i < pv->size(); i++) {
      if ((*pv)[i])
        temp.push_back((*pv)[i]);
    }

    if (temp.size() > 0) {
      pv->clear();
      pv->insert(pv->begin(), temp.begin(), temp.end());  // add back the kept ones
    } else {  // no kept ones at this depth
      pv->clear();
    }
  }
  //: clear the empty depths
  vcl_vector<vcl_vector<dbskr_shock_patch_sptr>* > temp_patch_sets;
  for (unsigned index = 0; index < patch_sets_.size(); index++) {
    vcl_vector<dbskr_shock_patch_sptr> *pv = patch_sets_[index];
    if (pv->size())
      temp_patch_sets.push_back(pv);
    else
      delete pv;
  }
  patch_sets_.clear();
  patch_sets_.insert(patch_sets_.begin(), temp_patch_sets.begin(), temp_patch_sets.end());
  return true;
}

//: add the patches that are kept at this depth to the storage
bool dbskr_shock_patch_model_selector::add_to_storage(int depth, dbskr_shock_patch_storage_sptr str)
{
  for (unsigned j = 0; j < patch_sets_.size(); j++) {
    vcl_vector<dbskr_shock_patch_sptr>* pv = patch_sets_[j];
    for (unsigned i = 0; i < pv->size(); i++) {
      if ((*pv)[i]->depth() != depth)
        break;

      if ((*pv)[i]->shock_graph())
        str->add_patch((*pv)[i]);
    }
  }

  return true;
}

  


