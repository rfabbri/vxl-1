// This is shp/dbsksp/xio/tests/test_shock_graph_xio.cxx

// \author Nhon Trinh
// \date June 27, 2007

#include <testlib/testlib_test.h>
#include <dbsksp/dbsksp_shock_graph.h>
#include <dbsksp/dbsksp_shock_graph_sptr.h>
#include <dbsksp/xio/dbsksp_xio_shock_graph.h>
#include <dbsksp/dbsksp_shock_fragment.h>




void test_shock_graph_xio()
{
  vcl_string xml_file = "V:/projects/kimia/shockshape/symmetry-shape-model/results/shock-graphs-xml/one_A12_fragment.xml";

  dbsksp_shock_graph_sptr shock_graph = 0;
  if ( !x_read(xml_file, shock_graph) )
  {
    vcl_cerr << "Loading shock graph XML file failed.\n";
    shock_graph = 0;
    return;
  }
  else
  {
    vcl_cout << "Loading shock graph XML file completed.\n";
    shock_graph->compute_all_dependent_params();
  };


  int num_pts = 100;
  for (int i=1; i< num_pts ; ++i)
  {
    dbsksp_shock_graph_sptr g = new dbsksp_shock_graph(*shock_graph);
    
    // Identify the main edge
    dbsksp_shock_edge_sptr e0 = 0;
    for (dbsksp_shock_graph::edge_iterator eit = g->edges_begin();
      eit != g->edges_end(); ++eit)
    {
      if ((*eit)->is_terminal_edge()) continue;
      e0 = *eit;
    }

    if (!e0)
    {
      vcl_cout << "ERROR: no non-terminal edge in the graph.\n";
      return;
    }


    //e0->print(vcl_cout);
    double m0 = e0->param_m();
    double len0 = e0->chord_length();


    double t = double(i) / num_pts;
    dbsksp_shock_node_sptr v0 = g->insert_shock_node(e0, t);
    g->compute_all_dependent_params();

    dbsksp_shock_edge_sptr e1 = v0->edge_list().front();
    dbsksp_shock_edge_sptr e2 = v0->edge_list().back();

    double m1 = e1->param_m();
    double len1 = e1->chord_length();

    double m2 = e2->param_m();
    double len2 = e2->chord_length();

    vcl_cout << "i= " << i 
      << " m0 m1 m2 (m1+m2-m0) m0/len0 m1/len1 m2/len2: "
      << m0 << " " 
      << m1 << " "
      << m2 << " "
      << m1 + m2 - m0 << " "
      << m0 / len0 << " "
      << m1 / len1 << " "
      << m2 / len2 << " "
      << vcl_endl;    
  }


  // Identify the main edge
  dbsksp_shock_edge_sptr e0 = 0;
  for (dbsksp_shock_graph::edge_iterator eit = shock_graph->edges_begin();
    eit != shock_graph->edges_end(); ++eit)
  {
    if ((*eit)->is_terminal_edge()) continue;
    e0 = *eit;
  }

  if (!e0)
  {
    vcl_cout << "ERROR: no non-terminal edge in the graph.\n";
    return;
  }

  e0->form_fragment();
  dbsksp_shapelet_sptr s = e0->fragment()->get_shapelet();
  dbgl_conic_arc shock_curve = s->shock_geom();

  for (int i=1; i< num_pts ; ++i)
  { 
    double t = double(i) / num_pts;
    double k = shock_curve.curvature_at(t);
    double phi = s->phi_at(t);
    vcl_cout << "t k phi k/sin(phi): "
      << t << " "
      << k << " "
      << phi << " "
      << k / vcl_sin(phi) << vcl_endl;
  }
}



//: Test closest point functions
MAIN( test_shock_graph_xio )
{
  START (" Test shock graph I/O from xml file");
  //test_shock_graph_xio();
  SUMMARY();
}
