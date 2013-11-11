//:
// \file
// \brief class to encapsulate composition strategies
//
//
// \author Ozge C. Ozcanli (Brown)
// \date   April 23, 2009
//
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim
//
//


#if !defined(_dbrec_compositor_h_)
#define _dbrec_compositor_h_

#include <vbl/vbl_ref_count.h>
#include <bxml/bxml_document.h>
#include "dbrec_compositor_sptr.h"

class dbrec_compositor : public vbl_ref_count {
public:

  //: return self as a bxml_data_sptr
  virtual bxml_data_sptr xml_element() const;
  //: read self from the bxml_data_sptr
  static dbrec_compositor_sptr xml_parse_element(bxml_data_sptr data);

protected:
  
};



#endif  //_dbrec_compositor_h_
