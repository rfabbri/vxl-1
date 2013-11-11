//:
// \file
// \author Isabel Restrepo
// \date 16-Jun-2010
// All dbrec3d brdb instantiations


#include <brdb/brdb_value.txx>
#include <vbl/io/vbl_io_smart_ptr.h>
#include <dbrec3d/dbrec3d_parts_manager.h>
#include <dbrec3d/dbrec3d_context_manager.h>

BRDB_VALUE_INSTANTIATE(dbrec3d_part_sptr, "dbrec3d_part_sptr");
BRDB_VALUE_INSTANTIATE(dbrec3d_context_sptr, "dbrec3d_context_sptr");
BRDB_VALUE_INSTANTIATE(dbrec3d_parts_manager_sptr, "dbrec3d_parts_manager_sptr");
BRDB_VALUE_INSTANTIATE(dbrec3d_context_manager_sptr, "dbrec3d_context_manager_sptr");