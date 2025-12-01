#ifdef _MSC_VER
#  include "vcl_msvc_warnings.h"
#endif
#include <vbl/vbl_smart_ptr.h>
#include <vpgl/vpgl_camera.h>
#include <brdb/brdb_value.hxx>

BRDB_VALUE_INSTANTIATE_LONG_FORM(vbl_smart_ptr<vpgl_camera<double> >, vpgl_camera_double_sptr, "vpgl_camera_double_sptr");
