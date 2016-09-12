#include <biob/biob_worldpt_field.hxx>
#include <vnl/xio/vnl_xio_quaternion.hxx>

typedef vnl_quaternion<double> vnl_quaternion_double;
BIOB_WORLDPT_FIELD_INSTANTIATE(vnl_quaternion_double);
//VNL_XIO_QUATERNION_INSTANTIATE(double);
