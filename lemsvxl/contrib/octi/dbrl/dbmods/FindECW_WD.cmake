# Find the ECW library for release and debug - Enhanced Compression Wavelets for JPEG2000 
#
# Sets
#   ECW_FOUND.  If false, don't try to use ecw
#   ECW_INCLUDE_DIR
#   ECW_LIBRARIES

# The original sponsorring website of this library appears to have vanished,
# but there are still traces at http://www.gdal.org/frmt_ecw.html and a
# distribution at https://svn.zib.de/lenne3d/lib/libecw/current - IMS  7-Dec-2009.

set( ECW_FOUND "NO" )
set (ECW_DEBUG_FOUND "NO")
find_path( ECW_INCLUDE_DIR NCSEcw.h
  /usr/include
  /usr/local/include
)

if( ECW_INCLUDE_DIR )
 find_library( ECW_ncsutil_DEBUG_LIBRARY NCSUtild
    /usr/lib
    /usr/local/lib
    /usr/lib64
    /usr/local/lib64
  )
  find_library( ECW_ncsutil_LIBRARY NCSUtil

	 /usr/lib
    /usr/local/lib
    /usr/lib64
    /usr/local/lib64

)
  find_library( ECW_ncsecw_DEBUG_LIBRARY NCSEcwd
    /usr/lib
    /usr/local/lib
    /usr/lib64
    /usr/local/lib64
  )
  find_library( ECW_ncsecw_LIBRARY NCSEcw
    /usr/lib
    /usr/local/lib
    /usr/lib64
    /usr/local/lib64
  )
  if( ECW_ncsutil_LIBRARY )
  if( ECW_ncsecw_LIBRARY )

    set( ECW_FOUND "YES" )
	#set( ECW_LIBRARIES ${ECW_ncsecw_LIBRARY} )
    set(ECW_LIBRARY  ${ECW_ncsecw_LIBRARY} )
	set(ECW_UTIL_LIBRARY ${ECW_ncsutil_LIBRARY})
	set (ECW_LIBRARIES ${ECW_LIBRARY} ${ECW_UTIL_LIBRARY}) 
  endif( ECW_ncsecw_LIBRARY )
  endif( ECW_ncsutil_LIBRARY )
  

  if( ECW_ncsutil_DEBUG_LIBRARY )
  if( ECW_ncsecw_DEBUG_LIBRARY )

    set( ECW_FOUND "YES" )
	set(ECW_DEBUG_FOUND "YES")
    #set( ECW_DEBUG_LIBRARIES  ${ECW_ncsecw_DEBUG_LIBRARY} )
	set(ECW_DEBUG_LIBRARY  ${ECW_ncsecw_DEBUG_LIBRARY} )
	set(ECW_UTIL_DEBUG_LIBRARY ${ECW_ncsutil_DEBUG_LIBRARY})
  endif( ECW_ncsecw_DEBUG_LIBRARY )
  endif( ECW_ncsutil_DEBUG_LIBRARY )
 
  

 endif( ECW_INCLUDE_DIR )