# Find the ECW library for release and debug - Enhanced Compression Wavelets for JPEG2000 
#
# Sets
#   ECW_FOUND.  If false, don't try to use ecw
#   ECW_INCLUDE_DIR
#   ECW_LIBRARIES

# The original sponsorring website of this library appears to have vanished,
# but there are still traces at http://www.gdal.org/frmt_ecw.html and a
# distribution at https://svn.zib.de/lenne3d/lib/libecw/current - IMS  7-Dec-2009.

SET( ECW_FOUND "NO" )
SET (ECW_DEBUG_FOUND "NO")
FIND_PATH( ECW_INCLUDE_DIR NCSEcw.h
  /usr/include
  /usr/local/include
)

IF( ECW_INCLUDE_DIR )
 FIND_LIBRARY( ECW_ncsutil_DEBUG_LIBRARY NCSUtild
    /usr/lib
    /usr/local/lib
    /usr/lib64
    /usr/local/lib64
  )
  FIND_LIBRARY( ECW_ncsutil_LIBRARY NCSUtil

	 /usr/lib
    /usr/local/lib
    /usr/lib64
    /usr/local/lib64

)
  FIND_LIBRARY( ECW_ncsecw_DEBUG_LIBRARY NCSEcwd
    /usr/lib
    /usr/local/lib
    /usr/lib64
    /usr/local/lib64
  )
  FIND_LIBRARY( ECW_ncsecw_LIBRARY NCSEcw
    /usr/lib
    /usr/local/lib
    /usr/lib64
    /usr/local/lib64
  )
  IF( ECW_ncsutil_LIBRARY )
  IF( ECW_ncsecw_LIBRARY )

    SET( ECW_FOUND "YES" )
	#SET( ECW_LIBRARIES ${ECW_ncsecw_LIBRARY} )
    SET(ECW_LIBRARY  ${ECW_ncsecw_LIBRARY} )
	SET(ECW_UTIL_LIBRARY ${ECW_ncsutil_LIBRARY})
	SET (ECW_LIBRARIES ${ECW_LIBRARY} ${ECW_UTIL_LIBRARY}) 
  ENDIF( ECW_ncsecw_LIBRARY )
  ENDIF( ECW_ncsutil_LIBRARY )
  

  IF( ECW_ncsutil_DEBUG_LIBRARY )
  IF( ECW_ncsecw_DEBUG_LIBRARY )

    SET( ECW_FOUND "YES" )
	SET(ECW_DEBUG_FOUND "YES")
    #SET( ECW_DEBUG_LIBRARIES  ${ECW_ncsecw_DEBUG_LIBRARY} )
	SET(ECW_DEBUG_LIBRARY  ${ECW_ncsecw_DEBUG_LIBRARY} )
	SET(ECW_UTIL_DEBUG_LIBRARY ${ECW_ncsutil_DEBUG_LIBRARY})
  ENDIF( ECW_ncsecw_DEBUG_LIBRARY )
  ENDIF( ECW_ncsutil_DEBUG_LIBRARY )
 
  

 ENDIF( ECW_INCLUDE_DIR )