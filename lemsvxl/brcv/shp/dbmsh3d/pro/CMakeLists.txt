# This is brcv/shp/dbmsh3d/pro/CMakeLists.txt
# Brown Mesh Processing library

include_directories( ${LEMS_SOURCE_DIR}/shp)

set ( dbmsh3d_pro_sources

    dbmsh3d_cmdpara.cxx         dbmsh3d_cmdpara.h
    dbmsh3d_process.cxx         dbmsh3d_process.h

)


add_library ( dbmsh3d_pro ${dbmsh3d_pro_sources} )

target_link_libraries ( dbmsh3d_pro dbmsh3d dbmsh3d_algo )

