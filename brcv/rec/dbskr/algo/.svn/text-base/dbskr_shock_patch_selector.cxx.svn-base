// This is brcv/rec/dbskr/algo/dbskr_shock_patch_selector.cxx

#include <dbskr/algo/dbskr_shock_patch_selector.h>

#include <bil/algo/bil_color_conversions.h>
#include <dbskr/pro/dbskr_shock_patch_storage.h>
#include <dbskr/dbskr_shock_patch.h>
#include <dbskr/algo/dbskr_shock_patch_match.h>
#include <dbskr/algo/dbskr_rec_algs.h>

#include <dbsk2d/dbsk2d_shock_graph.h>

#include <vil/vil_image_resource.h>

#include <vcl_algorithm.h>

#include <dbsk2d/algo/dbsk2d_xshock_graph_fileio.h>
#include <vil/vil_load.h>
#include <vil/vil_save.h>
#include <vil/vil_new.h>
#include <vil/vil_plane.h>
#include <bbas/bsol/bsol_algs.h>
#include <vsol/vsol_polygon_2d.h>
#include <vsol/vsol_curve_2d.h>
#include <vsol/vsol_line_2d.h>
#include <vgl/vgl_polygon_scan_iterator.h>
#include <vul/vul_file.h>
#include <dbsk2d/dbsk2d_file_io.h>
#include <dbsk2d/algo/dbsk2d_compute_bounding_box.h>

void dbskr_shock_patch_selector::set_image(vil_image_resource_sptr img_sptr)
{
  I_ = img_sptr->get_view(0, img_sptr->ni(), 0, img_sptr->nj());

  //make sure these images are one plane images
  if (I_.nplanes() != 3){
    color_image_ = false;
  } else {
    convert_RGB_to_Lab(I_, L_, A_, B_);
    color_image_ = true;
  }

  image_set_ = true;
}

void dbskr_shock_patch_selector::clear() 
{
  for (unsigned d = 0; d < patch_sets_.size(); d++) {
    vcl_vector<vcl_pair<float, dbskr_shock_patch_sptr> > *pv = patch_sets_[d];
    for (unsigned i = 0; i < pv->size(); i++) {
      (*pv)[i].second = 0;
    }
  }
    
  for (unsigned d = 0; d < patch_sets_.size(); d++)
    delete patch_sets_[d];
  patch_sets_.clear();

  for (unsigned d = 0; d < patch_set_id_maps_.size(); d++) {
    vcl_map<int, vcl_pair<dbskr_shock_patch_sptr, dbsk2d_shock_node_sptr> > *pv = patch_set_id_maps_[d];
    for (vcl_map<int, vcl_pair<dbskr_shock_patch_sptr, dbsk2d_shock_node_sptr> >::iterator it = pv->begin();
      it != pv->end(); it++) {
        (it->second).first = 0;
        (it->second).second = 0;
    }  
  }
  
  for (unsigned d = 0; d < patch_set_id_maps_.size(); d++)
    delete patch_set_id_maps_[d];
  patch_set_id_maps_.clear();

  for (unsigned d = 0; d < disc_patch_sets_.size(); d++) {
    vcl_vector<vcl_pair<float, dbskr_shock_patch_sptr> > *pv = disc_patch_sets_[d];
    for (unsigned i = 0; i < pv->size(); i++) {
      (*pv)[i].second = 0;
    }
  }

  for (unsigned d = 0; d < disc_patch_sets_.size(); d++)
    delete disc_patch_sets_[d];
  disc_patch_sets_.clear();

  for (vcl_map<dbskr_shock_patch_sptr, vcl_vector<dbskr_shock_patch_sptr>*>::iterator iter = disc_patch_map_.begin();
    iter != disc_patch_map_.end(); iter++) {
      for (unsigned i = 0; i < iter->second->size(); i++)
        (*iter->second)[i] = 0;
  }

  for (vcl_map<dbskr_shock_patch_sptr, vcl_vector<dbskr_shock_patch_sptr>*>::iterator iter = disc_patch_map_.begin();
    iter != disc_patch_map_.end(); iter++) {
      iter->first->clear();
      delete iter->second;
  }

  disc_patch_map_.clear();

  sg_ = 0;
}


//: extract all the patches at a given depth
bool dbskr_shock_patch_selector::extract(int depth, bool circular_ends)
{
  if (!sg_)
    return false;

  vcl_vector<vcl_pair<float, dbskr_shock_patch_sptr> > *pv = new vcl_vector<vcl_pair<float, dbskr_shock_patch_sptr> >();
  vcl_map<int, vcl_pair<dbskr_shock_patch_sptr, dbsk2d_shock_node_sptr> >* pvm = 
    new vcl_map<int, vcl_pair<dbskr_shock_patch_sptr, dbsk2d_shock_node_sptr> >();
  //int cnt = 0;
  for (dbsk2d_shock_graph::vertex_iterator v_itr = sg_->vertices_begin(); v_itr != sg_->vertices_end(); v_itr++)
  { 
    if ((*v_itr)->degree() < 3)
      continue;

    //vcl_cout << cnt << vcl_endl;
    //: create a patch from the tree rooted at *v_itr and keep the id of this shock node as start_node_id in this patch
    dbskr_shock_patch_sptr sp = extract_patch_from_subgraph(sg_, *v_itr, depth, area_threshold_, circular_ends);
    if (!sp) 
      return false;

    if (sp->get_outer_boundary()) {
      vcl_pair<float, dbskr_shock_patch_sptr> p;
      p.first = -1; p.second = sp;
      pv->push_back(p);
      vcl_pair<dbskr_shock_patch_sptr, dbsk2d_shock_node_sptr> pp;
      pp.first = sp;
      pp.second = *v_itr;
      (*pvm)[sp->start_node_id()] = pp;
      //cnt++;
    }
  }
   
  patch_sets_.push_back(pv);
  patch_set_id_maps_.push_back(pvm);

  depth_index_map_[depth] = patch_sets_.size()-1;

  return true;
}

//: eliminate all the patches with an edge on the bounding box of the contour set
//  if the starting point of any real boundary of the patch is "on" the bounding box, then eliminate the patch
bool dbskr_shock_patch_selector::prune_bounding_box_patches(int depth, vsol_box_2d_sptr bbox)
{
  vcl_map<int, int>::iterator iter = depth_index_map_.find(depth);
  if (iter == depth_index_map_.end())
    return false;

  int index = iter->second;
  if (index >= int(patch_sets_.size()))
    return false;

  vcl_vector<vcl_pair<float, dbskr_shock_patch_sptr> > *pv = patch_sets_[index];
  vcl_vector<vcl_pair<float, dbskr_shock_patch_sptr> > patches_to_keep;
  vcl_vector<vcl_pair<float, dbskr_shock_patch_sptr> > *disc_vec = new vcl_vector<vcl_pair<float, dbskr_shock_patch_sptr> >();
  for (unsigned i = 0; i < pv->size(); i++) {
    dbskr_shock_patch_sptr sp = (*pv)[i].second;
    if (sp->boundary_on_the_box(bbox))  // delete this one
      disc_vec->push_back((*pv)[i]);
    else
     patches_to_keep.push_back((*pv)[i]);
  }
  for (unsigned i = 0; i < pv->size(); i++) 
      (*pv)[i].second = 0; 
  pv->clear();
  pv->insert(pv->begin(), patches_to_keep.begin(), patches_to_keep.end());  // add back the kept ones
  disc_patch_sets_.push_back(disc_vec); 
  return true;
}

//: clear all the bbox contours from the real boundary sets of the patches
bool dbskr_shock_patch_selector::clear_bounding_box_contours(int depth, vsol_box_2d_sptr bbox)
{
  if (!bbox || !(bbox->area() > 0))
    return false;

  vcl_map<int, int>::iterator iter = depth_index_map_.find(depth);
  if (iter == depth_index_map_.end())
    return false;

  int index = iter->second;
  if (index >= int(patch_sets_.size()))
    return false;

  vcl_vector<vcl_pair<float, dbskr_shock_patch_sptr> > *pv = patch_sets_[index];
  for (unsigned i = 0; i < pv->size(); i++) {
    dbskr_shock_patch_sptr sp = (*pv)[i].second;
    sp->clean_real_boundaries(bbox);
  }

  return true;
}


//: For sorting pairs by their first element
inline
bool first_less( const vcl_pair<float,dbskr_shock_patch_sptr>& left,
                 const vcl_pair<float,dbskr_shock_patch_sptr>& right )
{
  return left.first < right.first;
}

//: sort all the patches at a given depth
bool dbskr_shock_patch_selector::find_and_sort_wrt_color_contrast(int depth, float color_threshold)
{
  if (!image_set_ || !color_image_)
    return false;

  vcl_map<int, int>::iterator iter = depth_index_map_.find(depth);
  if (iter == depth_index_map_.end())
    return false;

  int index = iter->second;
  if (index >= int(patch_sets_.size()))
    return false;

  //vcl_cout << " finding color contrasts..";
  vcl_vector<vcl_pair<float, dbskr_shock_patch_sptr> > *pv = patch_sets_[index];
  for (unsigned i = 0; i < pv->size(); i++) {
    dbskr_shock_patch_sptr sp = (*pv)[i].second;
    sp->find_color_contrast(L_, A_, B_);
    (*pv)[i].first = sp->color_contrast();
  }

  //vcl_cout << " deleting based on threshold..";
  if (color_threshold > 0) {
    vcl_vector<vcl_pair<float, dbskr_shock_patch_sptr> > temp;
    vcl_vector<vcl_pair<float, dbskr_shock_patch_sptr> > *disc_vec = new vcl_vector<vcl_pair<float, dbskr_shock_patch_sptr> >();
    for (unsigned i = 0; i < pv->size(); i++) {
      dbskr_shock_patch_sptr sp = (*pv)[i].second;
      if ((*pv)[i].first < color_threshold)
        temp.push_back((*pv)[i]);
      else
        disc_vec->push_back((*pv)[i]);
    }
    for (unsigned i = 0; i < pv->size(); i++) 
      (*pv)[i].second = 0; 
    pv->clear();
    pv->insert(pv->begin(), temp.begin(), temp.end());

    disc_patch_sets_.push_back(disc_vec);
  }

  //vcl_cout << " sorting..";
  vcl_sort(pv->begin(), pv->end(), first_less );
  //vcl_cout << " all done..";

  return true;
}

//: sort all the patches at a given depth
bool dbskr_shock_patch_selector::find_and_sort_wrt_app_contrast(int depth, float app_threshold)
{
  if (!image_set_ || color_image_)
    return false;
 
  vcl_map<int, int>::iterator iter = depth_index_map_.find(depth);
  if (iter == depth_index_map_.end())
    return false;

  int index = iter->second;
  if (index >= int(patch_sets_.size()))
    return false;

  vcl_vector<vcl_pair<float, dbskr_shock_patch_sptr> > *pv = patch_sets_[index];
  for (unsigned i = 0; i < pv->size(); i++) {
    dbskr_shock_patch_sptr sp = (*pv)[i].second;
    vsol_polygon_2d_sptr poly = sp->get_traced_boundary();
    if (!poly) {
      sp->trace_outer_boundary();
      poly = sp->get_traced_boundary();
      if (!poly) {
        (*pv)[i].first = 10000.0f;
        continue;
      }
    }
     
    sp->find_grey_contrast(I_);
    (*pv)[i].first = sp->app_contrast();
  }

  if (app_threshold > 0) {
    vcl_vector<vcl_pair<float, dbskr_shock_patch_sptr> > temp;
    vcl_vector<vcl_pair<float, dbskr_shock_patch_sptr> > *disc_vec = new vcl_vector<vcl_pair<float, dbskr_shock_patch_sptr> >();
    for (unsigned i = 0; i < pv->size(); i++) {
      dbskr_shock_patch_sptr sp = (*pv)[i].second;
      if ((*pv)[i].first < app_threshold)
        temp.push_back((*pv)[i]);
      else
        disc_vec->push_back((*pv)[i]);
    }
    for (unsigned i = 0; i < pv->size(); i++) 
      (*pv)[i].second = 0; 
    pv->clear();
    pv->insert(pv->begin(), temp.begin(), temp.end());

    disc_patch_sets_.push_back(disc_vec);
  }

  vcl_sort(pv->begin(), pv->end(), first_less);

  return true;
}


//: use the real contour to total length ratio to sort the patches
bool dbskr_shock_patch_selector::find_and_sort_wrt_contour_ratio(int depth, float contour_rat_thres)
{
  vcl_map<int, int>::iterator iter = depth_index_map_.find(depth);
  if (iter == depth_index_map_.end())
    return false;

  int index = iter->second;
  if (index >= int(patch_sets_.size()))
    return false;

  //vcl_cout << " finding ratios..";
  vcl_vector<vcl_pair<float, dbskr_shock_patch_sptr> > *pv = patch_sets_[index];
  for (unsigned i = 0; i < pv->size(); i++) {
    dbskr_shock_patch_sptr sp = (*pv)[i].second;
    (*pv)[i].first = 1.0f - sp->contour_ratio();  // the less the better now!!
  }

  vcl_cout << " deleting using ratio..";
  if (contour_rat_thres > 0) {
    vcl_vector<vcl_pair<float, dbskr_shock_patch_sptr> > temp;
    vcl_vector<vcl_pair<float, dbskr_shock_patch_sptr> > *disc_vec = new vcl_vector<vcl_pair<float, dbskr_shock_patch_sptr> >();
    for (unsigned i = 0; i < pv->size(); i++) {
      dbskr_shock_patch_sptr sp = (*pv)[i].second;
      if ((*pv)[i].first < contour_rat_thres)
        temp.push_back((*pv)[i]);
      else
        disc_vec->push_back((*pv)[i]);
    }
    for (unsigned i = 0; i < pv->size(); i++) 
      (*pv)[i].second = 0; 
    pv->clear();
    pv->insert(pv->begin(), temp.begin(), temp.end());

    disc_patch_sets_.push_back(disc_vec);
  }

  //vcl_cout << " sorting..";
  vcl_sort(pv->begin(), pv->end(), first_less);
  //vcl_cout << " done..\n";

  return true;
}

//: given a patch, prune all the patches at that depth whose roots are within the pruning depth of it, 
//  use the ordering in the vector of patches
bool dbskr_shock_patch_selector::prune(int depth, int pruning_depth, bool keep_pruned)
{
  if (pruning_depth > depth)
    return false;

  vcl_map<int, int>::iterator iter = depth_index_map_.find(depth);
  if (iter == depth_index_map_.end())
    return false;

  int index = iter->second;
  if (index >= int(patch_sets_.size()))
    return false;

  vcl_vector<vcl_pair<float, dbskr_shock_patch_sptr> > *pv = patch_sets_[index];
  vcl_map<int, vcl_pair<dbskr_shock_patch_sptr, dbsk2d_shock_node_sptr> > *pvm = patch_set_id_maps_[index];

  vcl_map<dbskr_shock_patch_sptr, bool> deleted_patches;
  
  //: get the sorted patch vector
  //  if the current patch is not in deleted patches then it hasn't been removed, find its list of patches to be pruned
  
  vcl_map<int, vcl_pair<dbskr_shock_patch_sptr, dbsk2d_shock_node_sptr> >::iterator pvm_itr;
  //vcl_cout << "pruning: ";
  for (unsigned i = 0; i < pv->size(); i++) {
    //vcl_cout << ".";
    dbskr_shock_patch_sptr sp = (*pv)[i].second;

    vcl_map<dbskr_shock_patch_sptr, bool>::iterator iter = deleted_patches.find(sp);
    if (iter != deleted_patches.end()) {  // it's been deleted 
      // it doesn't need to crowd the deleted patches list, since the order in pv is fixed
      deleted_patches.erase(iter);
      (*pv)[i].first = -1.0f;  // signify deletion
      continue;
    }
    
    vcl_map<dbskr_shock_patch_sptr, vcl_vector<dbskr_shock_patch_sptr>*>::iterator dpm_itr;
    vcl_vector<dbskr_shock_patch_sptr>* tv;
    //: keep all the patches that are deleted due to this patch for debugging purposes
    if (keep_pruned) {
      dpm_itr = disc_patch_map_.find(sp);
      if (dpm_itr == disc_patch_map_.end()) {
        tv = new vcl_vector<dbskr_shock_patch_sptr>();
        disc_patch_map_[sp] = tv;
      } else {
        tv = dpm_itr->second;
      }
    }

    vcl_pair<dbskr_shock_patch_sptr, dbsk2d_shock_node_sptr> pp = (*pvm)[sp->start_node_id()];
    dbskr_v_graph_sptr vg = construct_v_graph(sg_, pp.second, pruning_depth);

    //: delete all the patches which are covered by this tree at the pruning_depth rooted at our patch's root node
    for (dbskr_v_graph::vertex_iterator v_itr = vg->vertices_begin(); v_itr != vg->vertices_end(); v_itr++) {
      if ((*v_itr)->id_ == sp->start_node_id())  // do not remove itself
        continue;

      pvm_itr = pvm->find((*v_itr)->id_);  // find the patch rooted at this node
      if (pvm_itr != pvm->end()) { // we found a patch with this start node id, delete it
        dbskr_shock_patch_sptr dsp = (pvm_itr->second).first;
        deleted_patches[dsp] = true;
        if (keep_pruned) 
          tv->push_back(dsp);
      }
    }
  }

  //: now actually delete all the deleted ones from the patch_sets_
  vcl_vector<vcl_pair<float, dbskr_shock_patch_sptr> > temp;
  vcl_vector<vcl_pair<float, dbskr_shock_patch_sptr> > *disc_vec = new vcl_vector<vcl_pair<float, dbskr_shock_patch_sptr> >();
  for (unsigned i = 0; i < pv->size(); i++) {
    float first = (*pv)[i].first;
    if (first < 0)  // deleted
      disc_vec->push_back((*pv)[i]);
    else
      temp.push_back((*pv)[i]);
  }

  for (unsigned i = 0; i < pv->size(); i++) 
    (*pv)[i].second = 0; 
  pv->clear();
  pv->insert(pv->begin(), temp.begin(), temp.end());  // add back the kept ones
  disc_patch_sets_.push_back(disc_vec);

  return true;
}

//: prune all the patches at this depth for which all the nodes in v_graph's are overlapping
bool dbskr_shock_patch_selector::prune_same_patches(int depth)
{
  vcl_map<int, int>::iterator iter = depth_index_map_.find(depth);
  if (iter == depth_index_map_.end())
    return false;

  int index = iter->second;
  if (index >= int(patch_sets_.size()))
    return false;

  //vcl_cout << " finding ratios..";
  vcl_vector<vcl_pair<float, dbskr_shock_patch_sptr> > *pv = patch_sets_[index];
  for (unsigned i = 0; i < pv->size(); i++) {
    dbskr_shock_patch_sptr sp = (*pv)[i].second;
      if ((*pv)[i].first == 2.0f)  // CAUTION: make sure initially these values are set to some value other than 2 (even after sorting contrast value can never be 2!!)
        continue;
       
      for (unsigned j = i+1; j <pv->size(); j++) {
        dbskr_shock_patch_sptr sp2 = (*pv)[j].second;
        if ((*pv)[j].first == 2.0f)
          continue;

        if (sp->v_graph_same(sp2))  // delete this one
          (*pv)[j].first = 2.0f;
      }
  }

  //: now actually delete all the deleted ones from the patch_sets_
  vcl_vector<vcl_pair<float, dbskr_shock_patch_sptr> > temp;
  vcl_vector<vcl_pair<float, dbskr_shock_patch_sptr> > *disc_vec = new vcl_vector<vcl_pair<float, dbskr_shock_patch_sptr> >();
  for (unsigned i = 0; i < pv->size(); i++) {
    float first = (*pv)[i].first;
    if (first == 2.0f)  // deleted
      disc_vec->push_back((*pv)[i]);
    else
      temp.push_back((*pv)[i]);
  }

  for (unsigned i = 0; i < pv->size(); i++) 
    (*pv)[i].second = 0; 
  pv->clear();
  pv->insert(pv->begin(), temp.begin(), temp.end());  // add back the kept ones
  disc_patch_sets_.push_back(disc_vec);

  return true;
}


//: prune all the patches at all depths whose virtual graphs share 90% of their nodes 
//  use the sorting measure to pick the bests
bool dbskr_shock_patch_selector::prune_overlaps(float overlap_ratio, bool keep_pruned, bool kill_v_graphs)
{
  for (unsigned index = 0; index < patch_sets_.size(); index++) {
    vcl_vector<vcl_pair<float, dbskr_shock_patch_sptr> > *pv = patch_sets_[index];
    
    for (unsigned i = 0; i < pv->size(); i++) {
      dbskr_shock_patch_sptr sp = (*pv)[i].second;
      if ((*pv)[i].first < 0)
        continue;
       
      for (unsigned j = i+1; j <pv->size(); j++) {
        dbskr_shock_patch_sptr sp2 = (*pv)[j].second;
        if ((*pv)[j].first < 0)
          continue;

        if (sp->v_graph_node_overlap(sp2) >= overlap_ratio)  // delete this one
          (*pv)[j].first = -1;
      }
    }
  }

  //: CAUTION: the following code assumes that depths are in increasing order
  //  in the patch_sets_ vector
  for (int index = patch_sets_.size()-1; index >= 0; index--) {
    vcl_vector<vcl_pair<float, dbskr_shock_patch_sptr> > *pv = patch_sets_[index];
    
    for (unsigned i = 0; i < pv->size(); i++) {

      dbskr_shock_patch_sptr sp = (*pv)[i].second;
      if ((*pv)[i].first < 0)
        continue;
        
      // now go through all the remaining depths
      for (int ind2 = index-1; ind2 >= 0; ind2--) { 
        vcl_vector<vcl_pair<float, dbskr_shock_patch_sptr> > *pv2 = patch_sets_[ind2];
         
        for (unsigned j = 0; j <pv2->size(); j++) {
          dbskr_shock_patch_sptr sp2 = (*pv2)[j].second;
          if ((*pv2)[j].first < 0)
            continue;

          if (sp->v_graph_node_overlap(sp2) >= overlap_ratio) {  // delete the one with less contrast --> contrast measure is larger then
            if ((*pv)[i].first < (*pv2)[j].first)  // delete pv2
              (*pv2)[j].first = -1.0f;
            else
              (*pv)[i].first = -1.0f;
          }
          
        }
      }
    }
  }

  //: also remove the ones with the exact same "traced boundary", 
  //  there are cases such that subgraphs are different but the boundaries of outer faces of the graphes are exactly the same
  //  e.g. one traced from outer shocks of an object, and one traced from inner shocks
  //  check if areas and bounding box centers are almost exactly the same
  for (unsigned index = 0; index < patch_sets_.size(); index++) {
    vcl_vector<vcl_pair<float, dbskr_shock_patch_sptr> > *pv = patch_sets_[index];
    
    for (unsigned i = 0; i < pv->size(); i++) {
      dbskr_shock_patch_sptr sp = (*pv)[i].second;
      if ((*pv)[i].first < 0)
        continue;
       
      for (unsigned j = i+1; j <pv->size(); j++) {
        dbskr_shock_patch_sptr sp2 = (*pv)[j].second;
        if ((*pv)[j].first < 0)
          continue;

        if (sp->same_real_boundaries(sp2))  // delete this one
          (*pv)[j].first = -1.0f;
      }
    }
  }

  for (int index = patch_sets_.size()-1; index >= 0; index--) {
    vcl_vector<vcl_pair<float, dbskr_shock_patch_sptr> > *pv = patch_sets_[index];
    
    for (unsigned i = 0; i < pv->size(); i++) {

      dbskr_shock_patch_sptr sp = (*pv)[i].second;
      if ((*pv)[i].first < 0)
        continue;
        
      // now go through all the remaining depths
      for (int ind2 = index-1; ind2 >= 0; ind2--) { 
        vcl_vector<vcl_pair<float, dbskr_shock_patch_sptr> > *pv2 = patch_sets_[ind2];
         
        for (unsigned j = 0; j <pv2->size(); j++) {
          dbskr_shock_patch_sptr sp2 = (*pv2)[j].second;
          if ((*pv2)[j].first < 0)
            continue;

          if (sp->same_real_boundaries(sp2)) {  // delete the one with less contrast --> contrast measure is larger then
            (*pv2)[j].first = -1.0f;
          }
          
        }
      }
    }
  }

  for (unsigned index = 0; index < patch_sets_.size(); index++) {
    vcl_vector<vcl_pair<float, dbskr_shock_patch_sptr> > *pv = patch_sets_[index];
    
    vcl_vector<vcl_pair<float, dbskr_shock_patch_sptr> > patches_to_keep;
    if (keep_pruned) {
      vcl_vector<vcl_pair<float, dbskr_shock_patch_sptr> > *disc_vec = new vcl_vector<vcl_pair<float, dbskr_shock_patch_sptr> >();
      for (unsigned i = 0; i < pv->size(); i++) {
        if ((*pv)[i].first < 0)
          disc_vec->push_back((*pv)[i]);
        else
          patches_to_keep.push_back((*pv)[i]);
      }
      
    disc_patch_sets_.push_back(disc_vec); 
    } else {
      for (unsigned i = 0; i < pv->size(); i++) {
        if ((*pv)[i].first >= 0)
          patches_to_keep.push_back((*pv)[i]);
      }
    }

    for (unsigned i = 0; i < pv->size(); i++) 
      (*pv)[i].second = 0; 
    pv->clear();
    pv->insert(pv->begin(), patches_to_keep.begin(), patches_to_keep.end());  // add back the kept ones
  }

  // now prune over all depths
  if (kill_v_graphs) {
    for (unsigned index = 0; index < patch_sets_.size(); index++) {
      vcl_vector<vcl_pair<float, dbskr_shock_patch_sptr> > *pv = patch_sets_[index];
      for (unsigned i = 0; i < pv->size(); i++) {
        (*pv)[i].second->kill_v_graph();
      }

    }
  }
  return true;
}


//: return the patch set pruned due to a given patch
vcl_vector<dbskr_shock_patch_sptr>* dbskr_shock_patch_selector::pruned_set(dbskr_shock_patch_sptr sp)
{
  vcl_map<dbskr_shock_patch_sptr, vcl_vector<dbskr_shock_patch_sptr>*>::iterator iter = disc_patch_map_.find(sp);
  if (iter == disc_patch_map_.end())
    return 0;
  else
    return iter->second;
}

//: add the patches that are kept at this depth to the storage
bool dbskr_shock_patch_selector::create_shocks_and_add_to_storage(int depth, dbskr_shock_patch_storage_sptr str)
{
  vcl_map<int, int>::iterator iter = depth_index_map_.find(depth);
  if (iter == depth_index_map_.end())
    return false;

  int index = iter->second;
  if (index >= int(patch_sets_.size()))
    return false;

  vcl_vector<vcl_pair<float, dbskr_shock_patch_sptr> >* pv = patch_sets_[index];
  for (unsigned i = 0; i < pv->size(); i++) {
    //vcl_cout << "patch: " << i << " ";
    if ((*pv)[i].second->extract_simple_shock() && (*pv)[i].second->shock_graph() && (*pv)[i].second->shock_graph()->number_of_vertices()) {
      str->add_patch((*pv)[i].second);
      //vcl_cout << " number of vertices in the shock graph: " << (*pv)[i].second->shock_graph()->number_of_vertices() << vcl_endl;
    } else {
      //vcl_cout << " problems in shock graph extraction\n";
    }
  }

  return true;
}

//: add the patches that are kept at this depth to the storage
bool dbskr_shock_patch_selector::add_discarded_to_storage(int depth, dbskr_shock_patch_storage_sptr str)
{
  //: discarded index might be different than the patch_sets_ index, so find the index
  bool found_it = false;
  for (unsigned i = 0; i < disc_patch_sets_.size(); i++) {
    vcl_vector<vcl_pair<float, dbskr_shock_patch_sptr> >* pv = disc_patch_sets_[i];
    if (!pv->size())
      continue;

    if ((*pv)[0].second->depth() == depth) {
      for (unsigned i = 0; i < pv->size(); i++) {
        str->add_patch((*pv)[i].second);
      }
      found_it = true;
    }
  }

  return found_it;
}

dbsk2d_shock_graph_sptr read_esf_from_file(vcl_string fname) {
  dbsk2d_xshock_graph_fileio loader;
  dbsk2d_shock_graph_sptr sg = loader.load_xshock_graph(fname);
  return sg;
}
  
void set_images(vil_image_resource_sptr img_sptr,
                vil_image_view<vxl_byte> & I_, 
                vil_image_view<float> & L_, 
                vil_image_view<float> & A_, 
                vil_image_view<float> & B_,
                vil_image_resource_sptr& img_r, 
                vil_image_resource_sptr& img_g, 
                vil_image_resource_sptr& img_b) 
{
  I_ = img_sptr->get_view(0, img_sptr->ni(), 0, img_sptr->nj());
  bool color_image_;

  //make sure these images are one plane images
  if (I_.nplanes() != 3) {
    color_image_ = false;
    img_r = vil_plane(img_sptr, 0);
    img_g = vil_plane(img_sptr, 0);
    img_b = vil_plane(img_sptr, 0);
  } else {
    convert_RGB_to_Lab(I_, L_, A_, B_);
    color_image_ = true;
    img_r = vil_plane(img_sptr, 0);
    img_g = vil_plane(img_sptr, 1);
    img_b = vil_plane(img_sptr, 2);
  }
  vcl_cout << "loaded and processed images...\n";
}

void save_image_poly(dbskr_shock_patch_sptr sp, 
                     vcl_string name_initial, 
                     vil_image_resource_sptr img_r, 
                     vil_image_resource_sptr img_g, 
                     vil_image_resource_sptr img_b) {
  //vsol_polygon_2d_sptr poly = sp->get_traced_boundary();
  //if (!poly) {
  //  sp->trace_outer_boundary();
  //  poly = sp->get_traced_boundary();
  //}

  //: trimmed the shock graph samples based on support from real boundaries,
  //  use the shock graph to visualize shock patch
  dbsk2d_shock_graph_sptr sg = sp->shock_graph();
  bool binterpolate = true;
  double interpolate_ds = 1.0;
  bool subsample = true;
  double subsample_ds = 1.0;
  double poly_area_threshold = 20.0f;

  if (!sg || !sg->number_of_vertices() || !sg->number_of_edges() ) {
    vcl_cout << "Patch graph sg is not computed or has 0 nodes and endges !! IMAGE NOT SAVED for " << sp->id() << "\n";
    return; 
  }
  
  //vsol_polygon_2d_sptr poly = trace_boundary_from_graph(sg, binterpolate, subsample, 
  //  interpolate_ds, subsample_ds, poly_area_threshold);
  
  vsol_polygon_2d_sptr poly = sp->get_outer_boundary();
/*
   dbskr_tree_sptr tree = new dbskr_tree((float)subsample_ds, (float)interpolate_ds);
   bool circular_ends = false;  // cause while matching no circular ends completions for patches!!!
   tree->acquire(sg, false, circular_ends, false);
   vsol_polygon_2d_sptr poly = tree->compute_reconstructed_boundary_polygon(circular_ends);
*/
  if (poly) {

      poly->compute_bounding_box();
      vsol_box_2d_sptr bbox = poly->get_bounding_box();
      double minx = bbox->get_min_x()-5 < 0 ? 0 : bbox->get_min_x()-5;
      double miny = bbox->get_min_y()-5 < 0 ? 0 : bbox->get_min_y()-5;

      vil_image_view<vil_rgb<vxl_byte> > temp((int)vcl_ceil(bbox->width() + 10), (int)vcl_ceil(bbox->height() + 10), 1); 
      vil_rgb<vxl_byte> bg_col(255, 255, 255);
      temp.fill(bg_col);

      vil_image_view<vxl_byte> img_rv = img_r->get_view();
      vil_image_view<vxl_byte> img_gv = img_g->get_view();
      vil_image_view<vxl_byte> img_bv = img_b->get_view();

      vgl_polygon<double> pp = bsol_algs::vgl_from_poly(poly);
      vgl_polygon_scan_iterator<double> psi(pp, false);  // do not include boundary
      for (psi.reset(); psi.next(); ) {
        int y = psi.scany();
        for (int x = psi.startx(); x <= psi.endx(); ++x) {
          if (x < 0 || y < 0)
            continue;
          if (x >= int(img_r->ni()) || y >= int(img_r->nj())) 
            continue;
          int xx = (int)vcl_floor(x - minx + 0.5), yy = (int)vcl_floor(y - miny + 0.5);
          if (xx < 0 || yy < 0)
            continue;
          if (double(xx) > bbox->width() || double(yy) > bbox->height())
            continue;
          temp(xx,yy) = vil_rgb<vxl_byte>(img_rv(x,y), img_gv(x,y), img_bv(x,y));
        }
      }

      vil_image_resource_sptr out_img = vil_new_image_resource_of_view(temp);

/*  
      dbinfo_observation_sptr obs_r = new dbinfo_observation(0, img_r, poly, true, false, false);
      dbinfo_observation_sptr obs_g = new dbinfo_observation(0, img_g, poly, true, false, false);
      dbinfo_observation_sptr obs_b = new dbinfo_observation(0, img_b, poly, true, false, false);
      vil_image_resource_sptr out_img_r = obs_r->image_cropped(false);  // no background noise
      vil_image_resource_sptr out_img_g = obs_g->image_cropped(false);  // no background noise
      vil_image_resource_sptr out_img_b = obs_b->image_cropped(false);  // no background noise
      vil_image_view<vil_rgb<vxl_byte> > combined = brip_vil_float_ops::combine_color_planes(out_img_r, out_img_g, out_img_b);
      vil_image_resource_sptr out_img = vil_new_image_resource_of_view(combined);
*/   


      char buffer[1000];
      sprintf(buffer, "%d", sp->id());
      vcl_string cnt_str = buffer;
      sprintf(buffer, "%d", sp->depth());
      vcl_string d_str = buffer;
      vil_save_image_resource(out_img, (name_initial+cnt_str+"_"+d_str+".png").c_str()); 
  } else {
    vcl_cout << "Patch graph outer boundary could not be traced!! IMAGE NOT SAVED for " << sp->id() << "\n";
  }
}

void extract_subgraph_and_find_shock_patches(vcl_string image_file, 
                                             vcl_string esf_file, 
                                             vcl_string boundary_file,
                                             vcl_string kept_dir_name, 
                                             vcl_string discarded_dir_name, 
                                             vcl_string output_name,
                                             bool contour_ratio,
                                             bool circular_ends, 
                                             float area_threshold_ratio,
                                             float overlap_threshold,
                                             int min_depth, int max_depth, int depth_int, int pruning_depth, float sort_threshold,
                                             bool keep_pruned,
                                             bool save_images, 
                                             bool save_discarded_images)
{
  //: load esf and create trees
  dbsk2d_shock_graph_sptr sg = read_esf_from_file(esf_file.c_str());
  vcl_cout << "loaded esf...\n";

  //get the bounding box from the saved contour set (assuming that bnd name can be deduced from the esf name)
  vsol_box_2d_sptr bbox;
  
  if (vul_file::exists(boundary_file)) {
    // new vector to store the contours
    vcl_vector< vsol_spatial_object_2d_sptr > geoms;

    dbsk2d_file_io::load_bnd_v3_0(boundary_file, geoms);

    //: find the bounding box 
    bbox = new vsol_box_2d();
    for (unsigned i = 0; i <geoms.size(); i++) {
      if (geoms[i]->cast_to_curve())
        if (geoms[i]->cast_to_curve()->cast_to_line()) {
          vsol_line_2d_sptr line = geoms[i]->cast_to_curve()->cast_to_line();
          bbox->add_point(line->p0()->x(), line->p0()->y());
          bbox->add_point(line->p1()->x(), line->p1()->y());
        }
    }
  } else { // very risky because finds the box from the samples, they may not be exactly on the original bounding box of the curve set
    dbsk2d_compute_bounding_box(sg);
    bbox = sg->get_bounding_box();
  }


  find_shock_patches(image_file, sg, bbox, kept_dir_name, discarded_dir_name, output_name, 
                     contour_ratio, circular_ends, area_threshold_ratio, overlap_threshold, min_depth, max_depth, depth_int, pruning_depth, 
                     sort_threshold, keep_pruned, save_images, save_discarded_images);
}


bool find_shock_patches(vcl_string image_file, 
                       dbsk2d_shock_graph_sptr sg, 
                       vsol_box_2d_sptr bbox,
                       vcl_string kept_dir_name, 
                       vcl_string discarded_dir_name, 
                       vcl_string output_name,
                       bool contour_ratio,
                       bool circular_ends, 
                       float area_threshold_ratio,
                       float overlap_threshold,
                       int min_depth, int max_depth, int depth_int, int pruning_depth, float sort_threshold,
                       bool keep_pruned,
                       bool save_images, 
                       bool save_discarded_images)
{

  vcl_cout << "extracting from an image NOT from a MASK, sd:  " << min_depth << " md: " << max_depth << " di: " << depth_int << " pd: " << pruning_depth << "\n";

  //: load the image
  vil_image_view<float> L_, A_, B_;
  vil_image_view<vxl_byte> I_;
  vil_image_resource_sptr img_sptr, img_r, img_g, img_b;
  img_sptr = vil_load_image_resource(image_file.c_str());
  if (!img_sptr) {
    vcl_cout << "image file: " << image_file << "could not be found\n";
    return false;
  }

  set_images(img_sptr, I_, L_, A_, B_, img_r, img_g, img_b);     
  
 
 // create the output storage class
  dbskr_shock_patch_storage_sptr output = dbskr_shock_patch_storage_new();
  dbskr_shock_patch_storage_sptr discarded = dbskr_shock_patch_storage_new();
  
  int cnt = 0;  
  
  dbskr_shock_patch_selector selector(sg);
  
  float area_threshold;
  if (img_sptr)
    area_threshold = (img_sptr->ni()*img_sptr->nj())*area_threshold_ratio;
  else {
    if (!sg->get_bounding_box())
      dbsk2d_compute_bounding_box(sg);
    area_threshold = (float)sg->get_bounding_box()->area()*area_threshold_ratio;
  }
  selector.set_area_threshold(area_threshold);
  
  selector.set_image(img_sptr);
  
  for (int d = min_depth; d <= max_depth; d += depth_int) {
    vcl_cout << "d: " << d << "..";
    if (!selector.extract(d, circular_ends)) 
      return false;

    if (!selector.prune_same_patches(d))
      return false;

    //selector.prune_bounding_box_patches(d, bbox);
    if (!selector.clear_bounding_box_contours(d, bbox))
      return false;

    if (!contour_ratio) {
      if (!selector.find_and_sort_wrt_color_contrast(d, sort_threshold))
        selector.find_and_sort_wrt_app_contrast(d, sort_threshold);
    } else
      selector.find_and_sort_wrt_contour_ratio(d, sort_threshold);

    selector.prune(d, pruning_depth, keep_pruned);
    
    vcl_cout << ".DONE! ";
  }
  selector.prune_overlaps(overlap_threshold, keep_pruned, true);
  for (int d = min_depth; d <= max_depth; d += depth_int) {
    selector.create_shocks_and_add_to_storage(d, output);
    if (keep_pruned && save_discarded_images)
      selector.add_discarded_to_storage(d, discarded);
  }
    
  if (save_images) {
    vcl_cout << "saving images, total # of patches: " << output->size() << "... \n";
  for (unsigned i = 0; i < output->size(); i++) {
    dbskr_shock_patch_sptr sp = output->get_patch(i);
    save_image_poly(sp, kept_dir_name, img_r, img_g, img_b);
    vcl_cout << ".";

    if (keep_pruned && save_discarded_images) {
      vcl_cout << " disc: ";
    vcl_vector<dbskr_shock_patch_sptr>* pruned_set = selector.pruned_set(sp);
    if (pruned_set) {
      for (unsigned i = 0; i < pruned_set->size(); i++) {
        dbskr_shock_patch_sptr psp = (*pruned_set)[i];
        char buffer[1000];
        sprintf(buffer, "%d", sp->id());
        vcl_string cnt_str = buffer;
        sprintf(buffer, "%d", sp->depth());
        vcl_string d_str = buffer;
        vcl_string discarded_dir_name_patch = kept_dir_name+cnt_str+"_"+d_str+"_prn_set/";
        vul_file::make_directory_path(discarded_dir_name_patch);
        save_image_poly(psp, discarded_dir_name_patch, img_r, img_g, img_b);
        vcl_cout << ".";
      }
    }
    }
    
  }
  }

  if (save_discarded_images) {
    vcl_cout << " saving discarded images, total #: " << discarded->size() << "... ";
    for (unsigned i = 0; i < discarded->size(); i++) {
      dbskr_shock_patch_sptr sp = discarded->get_patch(i);
      save_image_poly(sp, discarded_dir_name, img_r, img_g, img_b);
      vcl_cout << ".";
    }
  }
  vcl_cout <<"\n";
    
  vsl_b_ofstream bfs(output_name.c_str());
  output->b_write(bfs);
  bfs.close();

  vcl_cout << output->size() << " patches in the storage, saving shocks..\n";
  //: save esfs for each patch to load later
  //vcl_string str_name_end = "patch_storage.bin";
  vcl_string str_name_end = "patch_strg.bin";
  for (unsigned i = 0; i < output->size(); i++) {
    dbskr_shock_patch_sptr sp = output->get_patch(i);
    vcl_string patch_esf_name = output_name.substr(0, output_name.length()-str_name_end.size());
    vcl_ostringstream oss;
    oss << sp->id();
    patch_esf_name = patch_esf_name+oss.str()+".esf";
    dbsk2d_xshock_graph_fileio file_io;
    file_io.save_xshock_graph(sp->shock_graph(), patch_esf_name);
  }
 
  selector.clear();
  output->clear();
  discarded->clear();

  return true;
}



