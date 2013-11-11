//---------------------------------------------------------------------
// This is brcv/rec/dbasn/dbasn_graph.cxx
//:
// \file
// \brief a simple graph structure
//
// \author
//  O.C. Ozcanli - January 21, 2004
//
// \verbatim
//  Modifications
//   <none>
// \endverbatim
//
//-------------------------------------------------------------------------

#include <vcl_cmath.h>
#include <vcl_vector.h>
#include <vcl_cassert.h>
#include <vcl_cfloat.h>
#include <vcl_stack.h>
#include <vcl_iostream.h>
#include <vnl/vnl_math.h>
#include <vul/vul_printf.h>

#include <dbasn/dbasn_graph.h>

//: allocate the links_[][] array according to the size of nodes_[].
void dbasn_graph::alloc_links ()
{
  const unsigned int nN = nodes_.size();
  links_ = new float*[nN];
  for (unsigned int i = 0; i<nN; i++)
    links_[i] = new float[nN];

  for (unsigned int i = 0; i<nN; i++)
    for (unsigned int j = 0; j<nN; j++)
      links_[i][j] = 0.0;
}

void dbasn_graph::get_node_cost_max_min (float& max, float& min) const
{
  max = -FLT_MAX;
  min = FLT_MAX;
  //Note there the min is the non-zero min.
  //We can't have node with 0 radius!
  for (unsigned int i=0; i<nodes_.size(); i++) {
    const float cost = nodes_[i]->cost();
    if (cost > max)
      max = cost;
    if (cost>0 && cost < min)
      min = cost;
  }
}

void dbasn_graph::get_link_cost_max_min (float& max, float& min) const
{
  max = -FLT_MAX;
  min = FLT_MAX;
  //Note there the min is the non-zero min.
  //We can't have link with 0 length!
  //This also skip the zero entries in the table!!
  const unsigned int nN = nodes_.size();
  for (unsigned int i=0; i<nN; i++) {
    for (unsigned int j=0; j<nN; j++) {      
      const float cost = links_[i][j];
      if (cost > max)
        max = cost;
      if (cost> 0 && cost < min)
        min = cost;
    }
  }
}

//###############################################################

void dbasn_graph::normalize_node_cost (const double max, const double min, const int verbose)
{
  //Normalize the link cost to be within [0 ~ 1].
  if (max == 0)
    return;

  ///const double log_min = vcl_log (min);

  if (verbose)
    vul_printf (vcl_cout, "  %s: normalize_node_cost(): max %.3lf, min = %.3lf.\n", 
                b_1st_graph_ ? "G" : "g", max, min);

  if (verbose > 1) {
    vcl_cout << "  ----- before normalization -----\n";
    print_node_cost ();
  }

  for (unsigned int i=0; i<nodes_.size(); i++) {
    ///double log_cost = vcl_log (nodes_[i]->cost() / max); //should be negative.
    double updated_cost = vcl_sqrt (nodes_[i]->cost() / max);
    ///assert (vnl_math_isnan(log_cost) == false);
    nodes_[i]->set_cost (updated_cost); ///(log_min + log_cost) / log_min); //now cost is between 0 and 1
  }

  if (verbose > 1) {    
    vcl_cout << "  ----- after normalization -----\n";
    print_node_cost ();
  }
}

void dbasn_graph::normalize_link_cost (const double max, const double min, const int verbose)
{
  //Normalize the link cost to be within [0 ~ 1].
  if (max == 0)
    return;
  
  ///const double log_min = vcl_log (min);

  if (verbose)
    vul_printf (vcl_cout, "  %s: normalize_link_cost(): max %.3lf, min = %.3lf.\n", 
                b_1st_graph_ ? "G" : "g", max, min);

  if (verbose > 1) {
    vcl_cout << "  ----- before normalization -----\n";
    print_link_cost ();
  }

  const unsigned int nN = nodes_.size();
  for (unsigned int i=0; i<nN; i++) {
    for (unsigned int j=0; j<nN; j++) {
      ///double log_cost = vcl_log (links_[i][j] / max); //should be negative. normalize_node_cost
      ///assert (vnl_math_isnan(log_cost) == false);
      ///(log_min + log_cost) / log_min; //now link[][] is between 0 and 1
      double updated_cost = vcl_sqrt (links_[i][j] / max);      
      links_[i][j] = updated_cost;       
    }
  }

  if (verbose > 1) {    
    vcl_cout << "  ----- after normalization -----\n";
    print_link_cost ();
  }
}

void dbasn_graph::print_node_cost (void)
{
  vul_printf (vcl_cout, "\n  %s->print_node_cost():\n",
        b_1st_graph_ ? "G" : "g");
  vul_printf (vcl_cout, "    # of nodes: %u\n", (unsigned int)nodes_.size());
  for (int i = 0; i<(int)nodes_.size(); i++) {
    assert (i == nodes_[i]->nid());
    vul_printf (vcl_cout, "    node %d cost %1.3f\n", i, nodes_[i]->cost());
  }
}

void dbasn_graph::print_link_cost ()
{
  vul_printf (vcl_cout, "\n  %s->print_link_cost():\n", b_1st_graph_ ? "G" : "g");
  int count = 0;
  const unsigned int nN = nodes_.size();
  for (unsigned int i=0; i<nN; i++)
    for (unsigned int j=0; j<nN; j++) {
        if (links_[i][j] != 0) {
          vul_printf (vcl_cout, "    links[%d][%d]=%1.3f \n", i, j, links_[i][j]);
          count++;
        }
      }
  vul_printf (vcl_cout, "    # of links: %d.\n", count);
}


