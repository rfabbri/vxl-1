// This is brcv/shp/dbsk2d/dbsk2d_exception.cxx

#include <dbsk2d/dbsk2d_exception.h>

#if !VCL_HAS_EXCEPTIONS

dbsk2d_exception_abort::dbsk2d_exception_abort(const vcl_string& comment):
  msg_(comment) 
{
  dbsk2d_assert(false);
}

#else

dbsk2d_exception_abort::dbsk2d_exception_abort(const vcl_string& comment): vcl_logic_error(comment) 
{}

#endif


#if !VCL_HAS_EXCEPTIONS

dbsk2d_exception_topology_error::dbsk2d_exception_topology_error(const vcl_string& comment):
  msg_(comment) {}

#else

dbsk2d_exception_topology_error::dbsk2d_exception_topology_error(const vcl_string& comment):
  vcl_logic_error(comment) 
{
  vcl_cout << "Irrecoverable shock computation failure at: ";
}

#endif



