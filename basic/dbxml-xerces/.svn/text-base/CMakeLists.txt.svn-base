# This is basic/dbxml-xerces/CMakeLists.txt

# Place holder for depreciated codes
# All these files used to reside in basic/dbxml. However, they are not used by anyone and have been depreciated.
# They are moved here for archiving purpose. Eventually, they should be removed completely.


INCLUDE_DIRECTORIES( ${VXL_BRL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/algo )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/basic )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/breye )

INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${GEL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/bbas )

# Optionally Build this library
OPTION(BUILD_DBXML "Build XML support" NO)

IF(BUILD_DBXML)

# find the path to the xerces libraries and includes
INCLUDE( ${LEMS_CMAKE_DIR}/Modules/find_xerces.cmake )


IF(HAS_XERCES)

SET(dbxml_sources
dbxml_input_converter.cxx  dbxml_input_converter.h
                                                                                        dbxml_dom.h
                                                                                                DOMPrintErrorHandler.hpp
                                                                                                DOMPrintFilter.hpp
                                                                                                DOMTreeErrorReporter.hpp
                                                                                                dbxml_generic_ptr.h

dbxml_param_input_converter.cxx                         dbxml_param_input_converter.h
                                                                                                                    dbxml_param.h
                                            dbxml_input_converter_sptr.h

dbxml_track_input_converter.cxx                         dbxml_track_input_converter.h
                                                                                                                    dbxml_track.h
                                                                                                                    dbxml_tracks.h
                                            dbxml_input_converter_sptr.h

dbxml_io.cxx                                dbxml_io.h

)

AUX_SOURCE_DIRECTORY(Templates dbxml_sources)


IF( BUILD_EXAMPLES )
  SUBDIRS( examples )
ENDIF( BUILD_EXAMPLES )

ADD_LIBRARY(dbxml  ${dbxml_sources})

TARGET_LINK_LIBRARIES(dbxml ${XERCES_LIBRARIES} vul vbl )

ENDIF(HAS_XERCES)

ENDIF(BUILD_DBXML)
