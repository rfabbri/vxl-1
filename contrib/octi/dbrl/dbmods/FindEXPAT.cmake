#
# Find an Expat library
#
#
# This file is used to manage using either a natively provided Expat library 
# or the one in b3p if provided.
#
#
# As per the standard scheme the following definitions are used
# EXPAT_INCLUDE_DIR - where to find expat.h
# EXPAT_LIBRARIES   - the set of libraries to include to use Expat.
# EXPAT_FOUND       - TRUE, if available somewhere on the system.

# Additionally
# VXL_USING_NATIVE_EXPAT  - True if we are using an Expat library provided outside vxl (or b3p)

# If this FORCE variable is unset or is FALSE, try to find a native library.
if( VXL_FORCE_B3P_EXPAT )
else( VXL_FORCE_B3P_EXPAT )
  include( ${CMAKE_ROOT}/Modules/FindEXPAT.cmake )
endif( VXL_FORCE_B3P_EXPAT )

if(EXPAT_FOUND)

  set(VXL_USING_NATIVE_EXPAT "YES")

else(EXPAT_FOUND)

  #
  # At some point, in a "release" version, it is possible that someone
  # will not have the b3p expat library
  #

  if(EXISTS ${vxl_SOURCE_DIR}/contrib/brl/b3p/expat/expat.h)

    set( EXPAT_FOUND "YES" )
    set( EXPAT_LIBRARIES expat )  
    set( EXPAT_LIBRARY expat )  
    set( EXPAT_INCLUDE_DIRS ${vxl_SOURCE_DIR}/contrib/brl/b3p/expat ${vxl_BINARY_DIR}/contrib/brl/b3p/expat )
    set( EXPAT_INCLUDE_DIR ${vxl_SOURCE_DIR}/contrib/brl/b3p/expat ${vxl_BINARY_DIR}/contrib/brl/b3p/expat )
    set( EXPAT_INSTALL_INCLUDE_DIR ${CMAKE_INSTALL_PREFIX}/include/vxl/contrib/brl/b3p/expat)
        
  endif(EXISTS ${vxl_SOURCE_DIR}/contrib/brl/b3p/expat/expat.h)

endif(EXPAT_FOUND)
