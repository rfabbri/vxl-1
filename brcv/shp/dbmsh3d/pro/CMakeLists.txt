# This is brcv/shp/dbmsh3d/pro/CMakeLists.txt
# Brown Mesh Processing library

INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/shp)

SET ( dbmsh3d_pro_sources

    dbmsh3d_cmdpara.cxx         dbmsh3d_cmdpara.h
    dbmsh3d_process.cxx         dbmsh3d_process.h

)


ADD_LIBRARY ( dbmsh3d_pro ${dbmsh3d_pro_sources} )

TARGET_LINK_LIBRARIES ( dbmsh3d_pro dbmsh3d dbmsh3d_algo )

