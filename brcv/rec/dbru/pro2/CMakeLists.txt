# This is lemsvxlsrc/brcv/rec/dbru/pro2/CMakeLists.txt
# Brown Recognition Utilities Process Library - new process format of contrib/brl/bpro

INCLUDE_DIRECTORIES( ${VXL_BRL_INCLUDE_DIR}/bpro )

SET(dbru_pro2_sources
    dbru_register.h                       dbru_register.cxx
    dbru_processes.h
)

AUX_SOURCE_DIRECTORY(Templates dbru_pro2_sources)

AUX_SOURCE_DIRECTORY(processes dbru_pro2_sources)

ADD_LIBRARY(dbru_pro2 ${dbru_pro2_sources})

TARGET_LINK_LIBRARIES(dbru_pro2 dbru bprb brdb bvgl)

#IF( BUILD_TESTING )
#  SUBDIRS(tests)
#ENDIF( BUILD_TESTING )

