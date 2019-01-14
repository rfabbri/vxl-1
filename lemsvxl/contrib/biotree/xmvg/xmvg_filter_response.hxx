// This is /contrib/biotree/xmvg/xmvg_filter_response.hxx

#ifndef xmvg_filter_response_txx_
#define xmvg_filter_response_txx_

#include "xmvg_filter_response.h"
#include <vsl/vsl_basic_xml_element.h>


template <class T>
void x_write(std::ostream& os, xmvg_filter_response<T> res)
{
  vsl_basic_xml_element element("xmvg_filter_response");
  element.add_attribute("size", (int) res.size());
  for (unsigned i=0; i<res.size(); i++) {
    element.append_cdata(res.get(i));
  }
  element.x_write(os);
}
template <class T>
std::ostream& operator << ( std::ostream& stream, const xmvg_filter_response<T> & resp)
{
  for (unsigned i=0; i<resp.size(); i++) {
    stream << resp.get(i) << " ";
  }
  stream << "\n";
  return stream;
}

// Code for easy instantiation.
#undef XMVG_FILTER_RESPONSE_INSTANTIATE
#define XMVG_FILTER_RESPONSE_INSTANTIATE(T) \
template class xmvg_filter_response<T>; \
template void x_write(std::ostream & , xmvg_filter_response<T>); \
template std::ostream& operator << ( std::ostream& , const xmvg_filter_response<T> &)
#endif
