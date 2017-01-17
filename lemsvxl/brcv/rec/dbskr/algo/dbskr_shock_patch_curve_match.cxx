#include <vcl_iostream.h>
#include <vcl_algorithm.h>
#include <dbskr/algo/dbskr_shock_patch_curve_match.h>
#include <dbskr/dbskr_shock_patch.h>
#include <dbcvr/dbcvr_cv_cor.h>

#include <dbskr/dbskr_utilities.h>

#include <vsol/vsol_polygon_2d.h>
#include <vsol/vsol_polyline_2d.h>
#include <vsol/vsol_box_2d.h>

#include <vgl/vgl_distance.h>
#include <vgl/vgl_polygon.h>

/*#include <vil/algo/vil_structuring_element.h>
#include <vil/algo/vil_binary_erode.h>
#include <vil/algo/vil_binary_dilate.h>
#include <vil/vil_save.h>
#include <vil/vil_image_resource.h>


#include <dbsol/algo/dbsol_img_curve_algs.h>
#include <dbsol/algo/dbsol_curve_algs.h>
#include <dbsol/dbsol_interp_curve_2d.h>

#include <dbru/algo/dbru_object_matcher.h>
*/

void dbskr_shock_patch_curve_match::clear()
{
   for (vcl_map<int, vcl_vector<vcl_pair<int, vcl_vector<dbcvr_cv_cor_sptr> > >* >::const_iterator iter = patch_curve_cor_map_.begin(); 
    iter != patch_curve_cor_map_.end(); iter++) {
      vcl_vector<vcl_pair<int, vcl_vector<dbcvr_cv_cor_sptr> > >* v_ptr = iter->second;
      delete v_ptr;
   }
   patch_curve_cor_map_.clear();

   map1_.clear();
   map2_.clear();

   patch_set2_.clear();
   return;
}
inline 
bool norm_cost_less(const vcl_pair<int, vcl_vector<dbcvr_cv_cor_sptr> >& p1,
                    const vcl_pair<int, vcl_vector<dbcvr_cv_cor_sptr> >& p2) {
  
  //: take the sum for now
  double cost1 = 0;
  double total_len = 0;
  for (unsigned i = 0; i < (p1.second).size(); i++) {
    cost1 += ((p1.second)[i]->final_norm_cost_)*(p1.second)[i]->length1_;
    total_len += (p1.second)[i]->length1_;
  }
  cost1 /= total_len;
  
  double cost2 = 0;
  total_len = 0;
  for (unsigned i = 0; i < (p2.second).size(); i++) {
    cost2 += ((p2.second)[i]->final_norm_cost_)*(p2.second)[i]->length1_;
    total_len += (p2.second)[i]->length1_;
  }
  cost2 /= total_len;  // total length of second set of curve1s

  return (cost1 < cost2);
}

//: sort again wrt norm costs
void dbskr_shock_patch_curve_match::resort_wrt_costs()
{
  for (vcl_map<int, vcl_vector<vcl_pair<int, vcl_vector<dbcvr_cv_cor_sptr> > >* >::const_iterator iter = patch_curve_cor_map_.begin(); 
    iter != patch_curve_cor_map_.end(); iter++) {
      vcl_vector<vcl_pair<int, vcl_vector<dbcvr_cv_cor_sptr> > >* v_ptr = iter->second;
      vcl_sort((*v_ptr).begin(), (*v_ptr).end(), norm_cost_less);
   }

}
/*
  //: get the top n best match of the patch with this id
vcl_vector< vcl_pair<int, dbskr_sm_cor_sptr> >* dbskr_shock_patch_curve_match::get_best_n_match(int patch_id, int n)
{
  vcl_vector< vcl_pair<int, dbskr_sm_cor_sptr> >* temp = new vcl_vector< vcl_pair<int, dbskr_sm_cor_sptr> >();
  temp->insert(temp->begin(), (*patch_curve_cor_map_[patch_id]).begin(), (*patch_curve_cor_map_[patch_id]).begin()+n);
  return temp;
}
*/

//: construct the match for the second image, does not sort wrt any type of cost
dbskr_shock_patch_curve_match_sptr dbskr_shock_patch_curve_match::construct_curve_match_just_cost()
{
  dbskr_shock_patch_curve_match_sptr new_match = new dbskr_shock_patch_curve_match();
  patch_curve_cor_map_type& new_map = new_match->get_map();
  patch_curve_cor_map_iterator it;
  for (vcl_map<int, vcl_vector<vcl_pair<int, vcl_vector<dbcvr_cv_cor_sptr> > >* >::const_iterator iter = patch_curve_cor_map_.begin(); 
    iter != patch_curve_cor_map_.end(); iter++) {
      vcl_vector<vcl_pair<int, vcl_vector<dbcvr_cv_cor_sptr> > >* v = (iter->second);
      for (unsigned i = 0; i < v->size(); i++) {
        it = new_map.find((*v)[i].first);
        if (it == new_map.end()) { // insert it
          vcl_vector<vcl_pair<int, vcl_vector<dbcvr_cv_cor_sptr> > >* new_v = new vcl_vector<vcl_pair<int, vcl_vector<dbcvr_cv_cor_sptr> > >();
          vcl_pair<int, vcl_vector<dbcvr_cv_cor_sptr> > p;
          p.first = iter->first;
          p.second = (*v)[i].second;  // the scurve correspondence is totally wrong but the cost is fine
          new_v->push_back(p);
          new_map[(*v)[i].first] = new_v;
        } else {
          vcl_pair<int, vcl_vector<dbcvr_cv_cor_sptr> > p;
          p.first = iter->first;
          p.second = (*v)[i].second;  // the scurve correspondence is totally wrong but the cost is find
          (it->second)->push_back(p);
        }
      }
  }

  return new_match;
}


//: Binary save self to stream.
void dbskr_shock_patch_curve_match::b_write(vsl_b_ostream &os) const
{
  vsl_b_write(os, version());

  vsl_b_write(os, n_);
  vsl_b_write(os, patch_curve_cor_map_.size());
  for (vcl_map<int, vcl_vector<vcl_pair<int, vcl_vector<dbcvr_cv_cor_sptr> > >* >::const_iterator iter = patch_curve_cor_map_.begin(); 
    iter != patch_curve_cor_map_.end(); iter++) {
    vsl_b_write(os, iter->first);
    vcl_vector<vcl_pair<int, vcl_vector<dbcvr_cv_cor_sptr> > >* v = (iter->second);
    vsl_b_write(os, v->size());
    for (unsigned i = 0; i < v->size(); i++) {
      vsl_b_write(os, (*v)[i].first);
      vsl_b_write(os, ((*v)[i].second).size());
      for (unsigned j = 0; j < ((*v)[i].second).size(); j++)
        ((*v)[i].second)[i]->b_write(os);
    }
  }

  return;
}

//: Binary load self from stream.
void dbskr_shock_patch_curve_match::b_read(vsl_b_istream &is)
{
  short ver;
  vsl_b_read(is, ver);
  switch (ver)
    {
    case 1:
      {
        vsl_b_read(is, n_);
        unsigned cnt;
        vsl_b_read(is, cnt);
        for (unsigned i = 0; i < cnt; i++) {
          int id1;
          vsl_b_read(is, id1);
          vcl_vector<vcl_pair<int, vcl_vector<dbcvr_cv_cor_sptr> > > *v = new vcl_vector<vcl_pair<int, vcl_vector<dbcvr_cv_cor_sptr> > >();
          unsigned v_cnt;
          vsl_b_read(is, v_cnt);
          for (unsigned j = 0; j < v_cnt; j++) {
            vcl_pair<int, vcl_vector<dbcvr_cv_cor_sptr> > pp;
            vsl_b_read(is, pp.first);
            vcl_vector<dbcvr_cv_cor_sptr> m_vec;
            unsigned m_cnt;
            vsl_b_read(is, m_cnt);
            for (unsigned k = 0; k < m_cnt; k++) {
              dbcvr_cv_cor_sptr cv = new dbcvr_cv_cor();
              cv->b_read(is);
              m_vec.push_back(cv);
            }
            pp.second = m_vec;
            v->push_back(pp);
          }
          patch_curve_cor_map_[id1] = v;
        }
        
        break;
      }
  }
}



