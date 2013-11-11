//:
// \file

#include "dbacm3d_xio_geodesic_active_contour.h"
#include <vsl/vsl_basic_xml_element.h>

//============================================================================
//: Binary save self to stream.
void x_write(vcl_ostream &os, const dbacm3d_geodesic_active_contour & g, vcl_string element_name)
{  
    vsl_basic_xml_element xml_element(element_name);
    xml_element.add_attribute("driver", "dbacm3d_geodesic_active_contour");
    xml_element.add_attribute("iters", static_cast<int>(g.num_iterations()));
    xml_element.add_attribute("timestep ",g.timestep());
    xml_element.add_attribute("direction ",g.direction());
    xml_element.add_attribute("gradient_gauss_sigma ",g.gradient_gauss_sigma());
    xml_element.add_attribute("inflation_force ",g.inflation_weight());
    xml_element.add_attribute("T ",g.gradient_norm());
    xml_element.add_attribute("exponent ",g.malladi_exponent());
    xml_element.add_attribute("curvature_beta ",g.curvature_weight());
    xml_element.add_attribute("use_geodesic ",g.use_geodesic());
    xml_element.x_write(os);
}
