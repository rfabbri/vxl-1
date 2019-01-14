// This is brcv/shp/dbsk2d/dbsk2d_exception.cxx

#include <dbsk2d/dbsk2d_exception.h>

#if !VCL_HAS_EXCEPTIONS

dbsk2d_exception_abort::dbsk2d_exception_abort(const std::string& comment):
  msg_(comment) 
{
  dbsk2d_assert(false);
}

#else

dbsk2d_exception_abort::dbsk2d_exception_abort(const std::string& comment): std::logic_error(comment) 
{}

#endif


#if !VCL_HAS_EXCEPTIONS

dbsk2d_exception_topology_error::dbsk2d_exception_topology_error(const std::string& comment):
  msg_(comment) {}

#else

dbsk2d_exception_topology_error::dbsk2d_exception_topology_error(const std::string& comment):
  std::logic_error(comment) 
{
  std::cout << "Irrecoverable shock computation failure at: ";
}

#endif



