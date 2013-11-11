// This is brcv/shp/dbsk2d/dbsk2d_ishock_bline.cxx

//:
// \file

#include "dbsk2d_ishock_bline.h"

#include <vcl_cstdio.h>
#include <dbsk2d/dbsk2d_ishock_edge.h>
#include <dbsk2d/dbsk2d_bnd_edge.h>


//-------------------------------------------------------------------
dbsk2d_ishock_bline::
dbsk2d_ishock_bline (dbsk2d_ishock_bpoint* startpt, 
                     dbsk2d_ishock_bpoint* endpt, 
                     int id, 
                     bool bGUI) :
dbsk2d_ishock_bcurve(startpt, endpt, id, bGUI)
{
  _type    = BLINE;
  
  this->compute_cached_params();

  //now we need to link the BPOINTs to the line
  s_pt()->connectTo(this);
  e_pt()->connectTo(this);

  // for gui purpose
  compute_extrinsic_locus();
}

//-------------------------------------------------------------------
//: compute local copies of commonly used parameters
void dbsk2d_ishock_bline::
compute_cached_params()
{
  _u      = _vPointPoint (this->start(), this->end());
  _n      = angle0To2Pi (_u + vnl_math::pi_over_2);
  _l      = _distPointPoint (this->start(), this->end());
  return;
}


//-------------------------------------------------------------------
//: Compute the bounding box of this bline
// inherited from bcurve
void dbsk2d_ishock_bline::
compute_bounding_box(vbl_bounding_box<double, 2 >& box ) const
{
  box.update(this->start().x(), this->start().y());
  box.update(this->end().x(), this->end().y());
  return; 
}

//Remember to recompute _u, _n, and _l for this dbsk2d_ishock_bline
void dbsk2d_ishock_bline::
reconnect(dbsk2d_ishock_bpoint* oldPt, dbsk2d_ishock_bpoint* newPt)
{
  if(oldPt==s_pt()) 
  {
    this->set_s_pt(newPt);
  }
  else
  {
    dbsk2d_assert (oldPt==e_pt());
    this->set_e_pt(newPt);
  }
  _u      = _vPointPoint (this->start(), this->end());
  _n      = angle0To2Pi (_u + vnl_math::pi_over_2);
  _l      = _distPointPoint (this->start(), this->end());
}

//: Return information about the object
void dbsk2d_ishock_bline::
getInfo (vcl_ostream& ostrm)
{
  char s[1024];

  ostrm << "\n==============================\n";
  ostrm << "BL: [" << _id << "] Twin: [" << twinLine()->id() << "] s_pt: [";
  ostrm << s_pt()->id() << "] e_pt: [" << e_pt()->id() << "]\n";
  vcl_sprintf (s, 
    "S-E:(%.3f, %.3f)-(%.3f, %.3f)\n", 
    this->start().x(), 
    this->start().y(), 
    this->end().x(), 
    this->end().y()); 
  ostrm << s;
  vcl_sprintf (s, "u: %.5f\n", _u); ostrm<<s;
  vcl_sprintf (s, "n: %.5f (u+pi/2)\n", _n); ostrm<<s;
  vcl_sprintf (s, "length: %.5f\n", _l); ostrm<<s;
  vcl_sprintf (s, "bGUIElm: %s\n\n", _bGUIElm ? "yes" : "no"); ostrm<<s;

  //bnd_ishock_map
  bnd_ishock_map_iter curS = shock_map_.begin();
  ostrm << "ShockMap: [" << id() << "]" << vcl_endl;
  for (; curS!=shock_map_.end(); ++curS){
    vcl_sprintf (s, "%.5f -> %d (%s)\n", 
      curS->first.s_eta, curS->second->id(), 
      (curS->first).type_string().c_str()); 
    ostrm<<s;
  }
  ostrm << "\n";

  dbsk2d_ishock_belm* twL = twinLine();
  ostrm << "Twin ShockMap: [" << twL->id() << "]" << vcl_endl;
  for (curS = twL->shock_map().begin(); curS!=twL->shock_map().end(); ++curS){
    vcl_sprintf (s, "%.5f -> %d (%s)\n", 
      curS->first.s_eta, curS->second->id(), 
      (curS->first).type_string().c_str()); 
    ostrm<<s;
  }
  ostrm << "\n";

  dbsk2d_ishock_belm* sp = s_pt();
  ostrm << "SPT ShockMap [" << sp->id() << "]" << vcl_endl;
  for (curS = sp->shock_map().begin(); curS!=sp->shock_map().end(); ++curS){
    vcl_sprintf (s, "%.5f -> %d (%s)\n", 
      curS->first.s_eta, curS->second->id(), 
      (curS->first).type_string().c_str()); 
    ostrm<<s;
  }
  ostrm << "\n";

  dbsk2d_ishock_belm* ep = e_pt();
  ostrm << "EPT ShockMap [" << ep->id() << "]" << vcl_endl;
  for (curS = ep->shock_map().begin(); curS!=ep->shock_map().end(); ++curS){
    vcl_sprintf (s, "%.5f -> %d (%s)\n", 
      curS->first.s_eta, curS->second->id(), 
      (curS->first).type_string().c_str());
    ostrm<<s;
  }
  ostrm << "\n";

  /* sprintf (s, "Neighboring BElements::\n"); ostrm<<s;
  belm_list neighboringBElms = getAllNeighboringBElements();
  curB = neighboringBElms.begin();
   for (; curB!=neighboringBElms.end(); ++curB) {
      sprintf (s, "%d, ", (*curB)->id()); ostrm<<s;
  }
  sprintf (s, "\n\n"); ostrm<<s;

  sprintf (s, "Neighboring dbsk2d_ishock_belm(s) Linked to this dbsk2d_ishock_bline (id=%d): ", _id); ostrm<<s;
  belm_list neighboringBElms = getAllNeighboringBElements();
  dbsk2d_belm_list_iter curB = neighboringBElms.begin();
   for (; curB!=neighboringBElms.end(); ++curB) {
      sprintf (s, "%d, ", (*curB)->id()); ostrm<<s;
  }
  sprintf (s, "\n"); ostrm<<s;

  if (twinLine()){
    sprintf (s, "Neighboring dbsk2d_ishock_belm(s) Linked to the twinLine (id=%d): ", twinLine()->id()); ostrm<<s;
    neighboringBElms.clear();
    neighboringBElms = twinLine()->getAllNeighboringBElements();
    curB = neighboringBElms.begin();
    for (; curB!=neighboringBElms.end(); ++curB) {
      sprintf (s, "%d, ", (*curB)->id()); ostrm<<s;
    }
    sprintf (s, "\n"); ostrm<<s;
  }

   sprintf (s, "Neighboring dbsk2d_ishock_belm(s) Linked to the s_pt() (id=%d): ", s_pt()->id()); ostrm<<s;
  neighboringBElms.clear();
  neighboringBElms = s_pt()->getAllNeighboringBElements();
  curB = neighboringBElms.begin();
   for (; curB!=neighboringBElms.end(); ++curB) {
      sprintf (s, "%d, ", (*curB)->id()); ostrm<<s;
  }
  sprintf (s, "\n"); ostrm<<s;
  
   sprintf (s, "Neighboring dbsk2d_ishock_belm(s) Linked to the e_pt() (id=%d): ", e_pt()->id()); ostrm<<s;
  neighboringBElms.clear();
  neighboringBElms = e_pt()->getAllNeighboringBElements();
  curB = neighboringBElms.begin();
   for (; curB!=neighboringBElms.end(); ++curB) {
      sprintf (s, "%d, ", (*curB)->id()); ostrm<<s;
  }
  sprintf (s, "\n\n"); ostrm<<s;*/

  ////\TODO need to add more stuffs here
  ////bnd_cells
  //ostrm << "Bnd cells: [" << this->bnd_edge()->cells().size() << "]" << vcl_endl;
  //ostrm << "\n";

  this->bnd_edge()->print_cell_info(ostrm);

}



//: extrinsic points for drawing purposes
void dbsk2d_ishock_bline::
compute_extrinsic_locus()
{
  //clear existing points and recompute in case it was modified
  ex_pts_.clear();
  ex_pts_.push_back(this->start());
  ex_pts_.push_back(this->end());
}

int dbsk2d_ishock_bline::get_contour_id()
{
    const vcl_list< vtol_topology_object * > * 
        superiors  = bnd_edge_->superiors_list();
    vcl_list<vtol_topology_object*>::const_iterator tit;
    tit=(*superiors).begin();
    return (*tit)->get_id();
}

