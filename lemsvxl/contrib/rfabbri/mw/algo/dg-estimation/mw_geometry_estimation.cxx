#include "mw_geometry_estimation.h"
#include <dbdet/sel/dbdet_sel.h>

#include <vsol/vsol_line_2d.h>


//:  k[i][j] == curvature at edgel i, valid quad number j
//
// \param[in] lines : input edgels
// \param[in] nrows, ncols, nrad, dtheta, dpos: parameters to SEL 
//
// All other parameters are out.
//
void mw_geometry_estimation::
sel_quad(
    vcl_vector<vsol_line_2d_sptr> &lines,
    unsigned  nrows,
    unsigned  ncols,
    unsigned  nrad,
    double    dtheta,
    double    dpos,
    vcl_vector<vcl_vector<double> > &k, 
    vcl_vector<vcl_vector<double> > &kdot,
    vcl_vector<vcl_vector<double> > &k_stdv, 
    vcl_vector<vcl_vector<double> > &kdot_stdv,
    dbdet_sel_sptr &sel
    )
{
  //: compute SEL
  
  vcl_vector<dbdet_edgel *> all_edgels(lines.size());


  for (unsigned j=0; j < lines.size(); ++j) { 
    vsol_line_2d_sptr eline = lines[j];

    vgl_point_2d<double> spt(eline->p0()->x(), eline->p0()->y());
    vgl_point_2d<double> ept(eline->p1()->x(), eline->p1()->y());
    double tan = dbdet_vPointPoint(spt, ept);
    
    all_edgels[j] = new dbdet_edgel(lines[j]->middle()->get_p(), tan);
  }

  sel = new dbdet_sel<dbdet_ES_curve_model>(nrows, ncols, nrad, dtheta, dpos); //: < for now use hardcoded values
  sel->build_edgel_neighborhoods(all_edgels);
  sel->build_pairs();
  sel->build_triplets();
  sel->build_quadruplets();
  sel->report_stats();

  sel_quad(sel, k, kdot, k_stdv, kdot_stdv); 
}

//: same as above but with precomputed SEL
void  mw_geometry_estimation::
sel_quad(
    const dbdet_sel_sptr &sel,
    vcl_vector<vcl_vector<double> > &k, 
    vcl_vector<vcl_vector<double> > &kdot,
    vcl_vector<vcl_vector<double> > &k_stdv, 
    vcl_vector<vcl_vector<double> > &kdot_stdv
    )
{
  k.resize(sel->get_edgels().size());
  kdot.resize(k.size());
  k_stdv.resize(k.size());
  kdot_stdv.resize(k.size());

  //: get the estimates
  for (unsigned  i=0; i < sel->get_edgels().size(); ++i) {
    bool stat = differential_geometry_at(sel->get_edgels()[i], k[i], kdot[i]);
    if (! stat)
      vcl_cout <<"Warning: edgel " << i << " has no valid quads\n";
  }
}

//: \return true if there were any valid measures for k, kdot at given edgel,
// based on quads
bool mw_geometry_estimation::
differential_geometry_at(dbdet_edgel *e1, vcl_vector<double> &k, vcl_vector<double> &kdot)
{
  const int cl=2; //:< curve level = quad
  
#define my_abs(a)  (((int)a<0)?-((int)a):((int)a) )
//#define my_abs_mod(a)  ( my_min(my_abs(a), nn - my_abs(a) ) )
  
  curvelet_list_const_iter it = e1->local_curvelets[cl].begin();
  for (; it != e1->local_curvelets[cl].end(); ++it) {
    /* XXX Skipping non-consecutive quads*/
    if (   my_abs( (*it)->edgel_chain[0]->id - (*it)->edgel_chain[1]->id ) == 1 
        && my_abs( (*it)->edgel_chain[1]->id - (*it)->edgel_chain[2]->id ) == 1 
        && my_abs( (*it)->edgel_chain[2]->id - (*it)->edgel_chain[3]->id ) == 1 
        ) {
        if ( (*it)->curve_model && (*it)->ref_edgel->id == e1->id  ) {

          dbdet_ES_curve_model *crv_model = dynamic_cast<dbdet_ES_curve_model *> ((*it)->curve_model);
          if (!crv_model) {
            vcl_cerr << "Curve model must be Eulerspiral\n";
            abort();
          }

          k.push_back(crv_model->k);
          kdot.push_back(crv_model->gamma);
        }
    }
  }

  return k.size() != 0;
}


