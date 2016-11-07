#include <testlib/testlib_test.h>
#include <dbsks/xio/dbsks_xio_xgraph_ccm_model.h>
#include <dbsks/xio/dbsks_xio_xgraph_geom_model.h>
#include <dbsks/dbsks_xgraph_ccm_model.h>
#include <dbsks/dbsks_xgraph_geom_model.h>
#include <dbsks/dbsks_xnode_geom_model.h>
#include <dbsks/dbsks_xfrag_geom_model.h>
#include <vcl_string.h>

#include <vnl/vnl_math.h>

void test_xio_xgraph_ccm_model()
{
  // Test loading a ccm model file
  vcl_string ccm_xml_file = "V:\\projects\\kimia\\shockshape\\symseg\\results\\ETHZ-dataset\\xshock-graph\\applelogos-xgraph\\applelogos_prototype1_ccm_cost-closest_oriented_edge-kovesi_len_10_thresh_15-2009april20_pos.xml";
  dbsks_xgraph_ccm_model_sptr xgraph_ccm = 0;

  x_read(ccm_xml_file, xgraph_ccm);
  if (xgraph_ccm)
  {
    xgraph_ccm->print(vcl_cout);
  }
  // TEST_NEAR("Read CCM model file", 1, 1.01, 0.01);


  // Test loading a geom_model file
  vcl_string geom_xml_file = "V:\\projects\\kimia\\shockshape\\symseg\\results\\ETHZ-dataset\\xshock-graph\\applelogos-xgraph\\xgraph_geom_model-applelogos_prototype1.xml";

  vcl_cout << "\nLoading xgraph_geom_file = " << geom_xml_file << "\n";
  dbsks_xgraph_geom_model_sptr xgraph_geom = 0;
  x_read(geom_xml_file, xgraph_geom);

  
  // Display the ratio between max and min values for each length parameter
  vcl_cout << ">> Log2 length ratios = ";
  for (vcl_map<unsigned, dbsks_xnode_geom_model_sptr >::iterator iter = xgraph_geom->map_node2geom().begin();
    iter != xgraph_geom->map_node2geom().end(); ++iter)
  {
    dbsks_xnode_geom_model_sptr xnode_geom = iter->second;
    double min_psi, max_psi;
    double min_radius, max_radius;
    double min_phi, max_phi;
    double min_phi_diff, max_phi_diff;
    double graph_size;
    xnode_geom->get_param_range(min_psi, max_psi, min_radius, max_radius, 
      min_phi, max_phi, min_phi_diff, max_phi_diff, graph_size);

    vcl_cout << " " << vcl_log(max_radius / min_radius) / vnl_math::ln2;
    graph_size;
  }

  for (vcl_map<unsigned, dbsks_xfrag_geom_model_sptr >::iterator iter = xgraph_geom->map_edge2geom().begin();
    iter != xgraph_geom->map_edge2geom().end(); ++iter)
  {
    dbsks_xfrag_geom_model_sptr xfrag_geom = iter->second;
    double min_psi_start, max_psi_start;
    double min_r_start, max_r_start;
    double min_phi_start, max_phi_start;
    double min_alpha_start, max_alpha_start;
    double min_chord, max_chord;
    double min_dpsi, max_dpsi;
    double min_r_end, max_r_end;
    double min_phi_end, max_phi_end;
    double graph_size;

    xfrag_geom->get_param_range(min_psi_start, max_psi_start,
                min_r_start, max_r_start,
                min_phi_start, max_phi_start,
                min_alpha_start, max_alpha_start,
                min_chord, max_chord,
                min_dpsi, max_dpsi,
                min_r_end, max_r_end,
                min_phi_end, max_phi_end,
                graph_size);
    vcl_cout << " " << vcl_log(max_chord / min_chord) / vnl_math::ln2;
    graph_size;

  }
  vcl_cout << "\n";

  
  


  float a0 = vnl_huge_val(float());
  float a1 = vnl_huge_val(float());
  float a2 = 100;

  vcl_cout << "a0 = huge_val = " << a0 << vcl_endl;
  vcl_cout << "a1 = huge_val = " << a1 << vcl_endl;
  vcl_cout << "a2 = " << a2 << vcl_endl;

  vcl_cout << "a0 + a1 = " << a0 + a1 << vcl_endl;
  vcl_cout << "a0 + a2 = " << a0 + a2 << vcl_endl;

  vcl_cout << "a0 - a1 = " << a0 - a1 << vcl_endl;
  vcl_cout << "a0 - a2 = " << a0 - a2 << vcl_endl;

  vcl_cout << "is a0 > a2 ? answer = " << ((a0 > a2) ? "true" : "false") << vcl_endl;
  vcl_cout << "is (a0 + a2) > a0 ? answer = " << ((a0 + a2) > a0 ? "true" : "false") << vcl_endl;
  vcl_cout << "is (a0 + a2) >= a0 ? answer = " << ((a0 + a2) >= a0 ? "true" : "false") << vcl_endl;
  vcl_cout << "is (a0 + a1) > a0 ? answer = " << ((a0 + a1) > a0 ? "true" : "false") << vcl_endl;
  vcl_cout << "is (a0 + a1) >= a0 ? answer = " << ((a0 + a1) >= a0 ? "true" : "false") << vcl_endl;
  vcl_cout << "is (2*a0) > a0 ? answer = " << ( 2*a0 > a0 ? "true" : "false") << vcl_endl;
  vcl_cout << "is (2*a0) >= a0 ? answer = " << ( 2*a0 >= a0 ? "true" : "false") << vcl_endl;

}


MAIN( test_xio_xshock )
{
  START ("xio_xshock");
  test_xio_xgraph_ccm_model();
  SUMMARY();
}

