# breye/dbpro/CMakeLists.txt

PROJECT(dbpro)

INCLUDE_DIRECTORIES( ${VXL_BRL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/basic )

FIND_PACKAGE( Threads )

CONFIGURE_FILE( ${dbpro_SOURCE_DIR}/dbpro_config.h.in
                ${dbpro_BINARY_DIR}/dbpro_config.h )


SET(dbpro_sources
   dbpro_mutex.cxx                       dbpro_mutex.h 
   dbpro_parameters.cxx                  dbpro_parameters.h                  dbpro_parameters_sptr.h
   dbpro_parameters.txx
   dbpro_process.cxx                     dbpro_process.h                     dbpro_process_sptr.h
                                         dbpro_basic_processes.h
                                         dbpro_vsl_processes.h
                                         dbpro_ios_processes.h
   dbpro_process_factory.cxx             dbpro_process_factory.h
                                         dbpro_storage.h                     dbpro_storage_sptr.h
                                         dbpro_observer.h                    dbpro_observer_sptr.h
   dbpro_connector.cxx                   dbpro_connector.h                   dbpro_connector_sptr.h
   dbpro_delay.cxx                       dbpro_delay.h
   dbpro_try_option.cxx                  dbpro_try_option.h
   dbpro_executive.cxx                   dbpro_executive.h
)



AUX_SOURCE_DIRECTORY(Templates dbpro_sources)

ADD_LIBRARY(dbpro  ${dbpro_sources})


TARGET_LINK_LIBRARIES(dbpro vul vbl)

IF( CMAKE_USE_PTHREADS_INIT )
  SET_TARGET_PROPERTIES(dbpro PROPERTIES COMPILE_FLAGS -pthread)
  TARGET_LINK_LIBRARIES(dbpro pthread)
ENDIF( CMAKE_USE_PTHREADS_INIT )


SUBDIRS( vis )

SUBDIRS( xio )

IF(BUILD_TESTING)
  SUBDIRS(tests)
ENDIF(BUILD_TESTING)
