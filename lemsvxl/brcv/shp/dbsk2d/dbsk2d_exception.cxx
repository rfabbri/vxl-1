// This is brcv/shp/dbsk2d/dbsk2d_exception.cxx

#include <dbsk2d/dbsk2d_exception.h>
#include <dbsk2d/dbsk2d_assert.h>

dbsk2d_exception_abort::dbsk2d_exception_abort(const std::string& comment): std::logic_error(comment) 
{}


dbsk2d_exception_topology_error::dbsk2d_exception_topology_error(const std::string& comment):
  std::logic_error(comment) 
{
  std::cout << "Irrecoverable shock computation failure at: ";
}
