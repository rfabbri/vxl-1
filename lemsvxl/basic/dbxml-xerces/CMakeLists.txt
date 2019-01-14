# This is basic/dbxml-xerces/CMakeLists.txt

# Place holder for depreciated codes
# All these files used to reside in basic/dbxml. However, they are not used by anyone and have been depreciated.
# They are moved here for archiving purpose. Eventually, they should be removed completely.


include_directories( ${VXL_BRL_INCLUDE_DIR} )
include_directories( ${LEMS_SOURCE_DIR}/algo )
include_directories( ${LEMS_SOURCE_DIR}/basic )
include_directories( ${LEMS_SOURCE_DIR}/breye )

include_directories( ${BRL_INCLUDE_DIR} )
include_directories( ${GEL_INCLUDE_DIR} )
include_directories( ${BRL_INCLUDE_DIR}/bbas )

# Optionally Build this library
option(BUILD_DBXML "Build XML support" NO)

if(BUILD_DBXML)

# find the path to the xerces libraries and includes
include( ${LEMS_CMAKE_DIR}/Modules/find_xerces.cmake )


if(HAS_XERCES)

set(dbxml_sources
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

aux_source_directory(Templates dbxml_sources)


if( BUILD_EXAMPLES )
  add_subdirectory( examples )
endif( BUILD_EXAMPLES )

add_library(dbxml  ${dbxml_sources})

target_link_libraries(dbxml ${XERCES_LIBRARIES} vul vbl )

endif(HAS_XERCES)

endif(BUILD_DBXML)
