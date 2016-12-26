# This is lemsvxlsrc/brcv/rec/dbru/pro2/CMakeLists.txt
# Brown Recognition Utilities Process Library - new process format of contrib/brl/bpro

include_directories( ${VXL_BRL_INCLUDE_DIR}/bpro )

set(dbru_pro2_sources
    dbru_register.h                       dbru_register.cxx
    dbru_processes.h
)

aux_source_directory(Templates dbru_pro2_sources)

aux_source_directory(processes dbru_pro2_sources)

add_library(dbru_pro2 ${dbru_pro2_sources})

target_link_libraries(dbru_pro2 dbru bprb brdb bvgl)

#if( BUILD_TESTING )
#  add_subdirectory(tests)
#endif( BUILD_TESTING )

