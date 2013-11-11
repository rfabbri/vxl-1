// This is brcv/shp/dbsk2d/dbsk2d_shock_node.cxx

//:
// \file

#include <vcl_cstdio.h>
#include <dbsk2d/dbsk2d_shock_node.h>

//: Constructor
dbsk2d_shock_node::dbsk2d_shock_node() : dbsk2d_base_gui_geometry(), dbgrl_vertex<dbsk2d_shock_edge>(), 
  id_(-1), pt_(-1.0, -1.0), radius_ (-1), descriptor_list_(0)
{
}

//: Destructor
dbsk2d_shock_node::~dbsk2d_shock_node() 
{
  descriptor_list_.clear();
}

//: return the type of this node
dbsk2d_shock_node::shock_node_type 
dbsk2d_shock_node::type() 
{ 
  int in_deg = in_degree();
  int out_deg = out_degree();

  if (in_deg==0 && out_deg==2) return SOURCE;
  if (in_deg >1 && out_deg==0) return SINK;
  if (in_deg >0 && out_deg >0) return JUNCT;
  if (in_deg==0 && out_deg==1) return A3;
  if (in_deg==1 && out_deg==0) return TERMINAL;

  dbsk2d_assert(0);
  return A3;
}

//: form shock fragments on this node
void dbsk2d_shock_node::form_shock_fragments()
{
  //traverse the descriptor list and form shock fragments for the 
  //degenerate descriptors
  vcl_list<dbsk2d_shock_node_descriptor>::iterator p_itr = descriptor_list_.begin();
  for (; p_itr != descriptor_list_.end(); ++ p_itr){
    dbsk2d_shock_node_descriptor* cur_descriptor = &(*p_itr);

    if (cur_descriptor->edge == 0)//signals A-inf
    {  
      // instantiate the shock fragment
      cur_descriptor->fragment = new dbsk2d_shock_fragment(this);

      //now form the polygon of the arc sector
      cur_descriptor->fragment->ex_pts().push_back(this->pt());
      
      // only the tangents are sweeping the range between the two tangents (CCW)
      double cur_tan = cur_descriptor->tangent;
      double end_tan = cur_descriptor->tangent2;
      
      if (end_tan < cur_tan)
        end_tan += 2*vnl_math::pi;

      while (cur_tan < end_tan)
      {
        cur_descriptor->fragment->ex_pts().push_back(_translatePoint(this->pt(), cur_tan, this->radius()));
        cur_tan += 0.02; //increment the tangent angle (going CCW)
      }
      //endpoint
      cur_descriptor->fragment->ex_pts().push_back(_translatePoint(this->pt(), end_tan, this->radius()));
    }
  }
}

//: Return some information about the element
void dbsk2d_shock_node::getInfo (vcl_ostream& ostrm)
{
  char s[1024];

  ostrm << "\n==============================\n";
  ostrm << "N: [" << id_ << "]" << vcl_endl;
  vcl_sprintf(s, "Position : (%.3f, %.3f)\n", pt_.x(), pt_.y()); ostrm << s;
  ostrm << "Radius : " << radius_ <<vcl_endl;

  // connected edges in order
  ostrm << "Adj. edges CCW [ ";
  //incoming edges
  for (edge_iterator e_itr = in_edges_.begin();
       e_itr != in_edges_.end(); ++ e_itr)
    ostrm << (*e_itr)->id() << " ";

  //outgoing edges
  for (edge_iterator e_itr = out_edges_.begin();
       e_itr != out_edges_.end(); ++ e_itr)
    ostrm << (*e_itr)->id() << " ";

  ostrm << "]" << vcl_endl;

  //connectivity and intrinsic parameters
  ostrm << "Intrinsic Parameters: " << vcl_endl;
  ostrm << "....................." << vcl_endl;

  vcl_list<dbsk2d_shock_node_descriptor>::iterator p_itr = descriptor_list_.begin();
  for (; p_itr != descriptor_list_.end(); ++ p_itr){
    dbsk2d_shock_node_descriptor cur_descriptor = (*p_itr);

    if (cur_descriptor.edge){
      ostrm << cur_descriptor.edge->id();
      ostrm << ": [T=" << cur_descriptor.tangent;
    }
    else {
      ostrm << "A-inf";
      ostrm << ": [T1=" << cur_descriptor.tangent << ", T2=" << cur_descriptor.tangent2;
    }
    ostrm << ", phi=" << cur_descriptor.phi << "]" << vcl_endl;
    ostrm << "Frag? : " << (cur_descriptor.fragment==0?"no":"yes") << vcl_endl;
  }
}

