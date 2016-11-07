#include "xmvg_source.h"
#include <vsl/vsl_basic_xml_element.h>

void x_write(vcl_ostream& os, xmvg_source source)
{
  vsl_basic_xml_element element("xmvg_source");
  element.add_attribute("spot_radius", source.spot_radius());
  element.add_attribute("kv", source.kv());
  element.add_attribute("ma", source.ma());
  element.add_attribute("poisson_lambda", source.poisson_lambda());
  element.x_write(os);
}

