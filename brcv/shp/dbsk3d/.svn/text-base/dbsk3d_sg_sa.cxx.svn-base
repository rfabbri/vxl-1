// This is 3DShock_Scaffold_Graph_SA.cxx
//: MingChing Chang
//  Nov. 23, 2004        Creation

#include <vcl_iostream.h>
#include <vul/vul_printf.h>

#include <dbsk3d/dbsk3d_fs_mesh.h>
#include <dbsk3d/dbsk3d_ms_hypg.h>
#include <dbsk3d/dbsk3d_sg_sa.h>

bool dbsk3d_sg_sa::check_integrity ()
{
  if (dbmsh3d_graph::check_integrity() == false)
    return false;

  vcl_map<int, dbsk3d_fs_vertex*>::iterator it = FV_map_.begin();
  for (; it != FV_map_.end(); it++) {
    dbsk3d_fs_vertex* FV = (*it).second;
    if (FV->id() != (*it).first)
      return false;
    if (FV->check_integrity() == false)
      return false;
  }

  return true;
}

//: Compute the median of link length from the fine-scale graph.
void dbsk3d_sg_sa::compute_median_edge_len ()
{
  if (median_edge_len_ != VALUE_NOT_DETERMINED)
    return; //If already computed, return.

  vul_printf (vcl_cout, "  compute_median_edge_len(): %u fs_edges.\n", edgemap_.size());
  assert (edgemap_.size() != 0);

  //Loop through all ms_cruves and their fs_edges and compute the median.
  vcl_vector<double> vals;

  vcl_map<int, dbmsh3d_edge*>::iterator eit = edgemap_.begin();
  for (; eit != edgemap_.end(); eit++) {
    dbsk3d_ms_curve* MC = (dbsk3d_ms_curve*) (*eit).second;

    if (MC->data_type() == C_DATA_TYPE_EDGE) {
      //Put each link element's length into vector.
      for (unsigned int i=0; i<MC->E_vec().size(); i++) {
        const dbsk3d_fs_edge* FE = (dbsk3d_fs_edge*) MC->E_vec(i);
        vals.push_back (FE->length());
      }
    }
    else {
      assert (MC->data_type() == C_DATA_TYPE_VERTEX);
      //Compute the link length from node elements.
      for (unsigned int i=0; i<MC->V_vec().size()-1; i++) {
        const dbsk3d_fs_vertex* FV = (dbsk3d_fs_vertex*) MC->V_vec(i);
        const dbsk3d_fs_vertex* N2 = (dbsk3d_fs_vertex*) MC->V_vec(i+1);
        double d = vgl_distance (FV->pt(), N2->pt());
        vals.push_back (d);
      }
    }
  }

  //Find the median of a vector of doubles:
  vcl_vector<double>::iterator medianIter=vals.begin() + vals.size()/2;
  vcl_nth_element (vals.begin(),medianIter,vals.end());
  median_edge_len_ = (float) *medianIter;

  vul_printf (vcl_cout, "\tmedian fine-scale link length = %f\n", median_edge_len_);
}

//##################################################################

void clone_sg_sa (dbsk3d_sg_sa* targetSG, dbsk3d_sg_sa* inputSG)
{
  //Clone the dbmsh3d_graph structure.
  clone_graph (targetSG, inputSG);

  //Clone the graph's FV_map[].  
  vcl_map<int, dbsk3d_fs_vertex*>::iterator it = inputSG->FV_map().begin();
  for (; it != inputSG->FV_map().end(); it++) {
    dbsk3d_fs_vertex* FV = (dbsk3d_fs_vertex*) (*it).second;
    dbsk3d_fs_vertex* newN = targetSG->_new_FV (FV->id());
    assert (newN->id() == FV->id());
    newN->set_pt (FV->pt());
    newN->set_v_type (FV->v_type());
    assert (FV->asgn_G_list() == NULL);
    targetSG->_add_FV (newN);
  }
  targetSG->set_vertex_id_counter (inputSG->vertex_id_counter());
  
  //More on cloning each ms_curve: type, fs_vertices[], etc.
  vcl_map<int, dbmsh3d_vertex*>::iterator vit = inputSG->vertexmap().begin();
  for (; vit != inputSG->vertexmap().end(); vit++) {
    dbsk3d_ms_node* MN = (dbsk3d_ms_node*) (*vit).second;
    dbsk3d_ms_node* newSV = (dbsk3d_ms_node*) targetSG->vertexmap (MN->id());
    assert (newSV->id() == MN->id());
    clone_ms_vertex_sg_sa (newSV, MN, targetSG);
  }

  //More on cloning each ms_curve: type, fs_vertices[], etc.
  vcl_map<int, dbmsh3d_edge*>::iterator eit = inputSG->edgemap().begin();
  for (; eit != inputSG->edgemap().end(); eit++) {
    dbsk3d_ms_curve* MC = (dbsk3d_ms_curve*) (*eit).second;
    dbsk3d_ms_curve* newSC = (dbsk3d_ms_curve*) targetSG->edgemap (MC->id());
    assert (newSC->id() == MC->id());
    clone_ms_curve_sg_sa (newSC, MC, targetSG);
  }
}
