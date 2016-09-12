#set(CMAKE_CXX_FLAGS -O3)
set(CMAKE_CXX_FLAGS -g)
link_directories(${LEMS_LIBRARY_DIR} ${VXL_LIBRARY_DIR})

    include_directories( ${VXL_CORE_INCLUDE_DIR} 
                    ${VXL_VGUI_INCLUDE_DIR} 
                    ${VXL_GEL_INCLUDE_DIR}
                    ${VXL_BRL_INCLUDE_DIR} 
                    ${VXL_BRL_INCLUDE_DIR}/bbas 
                    ${VXL_MUL_INCLUDE_DIR} 
                    ${VXL_GEL_INCLUDE_DIR} 
                    ${LEMS_SOURCE_DIR}/basic
                    ${LEMS_SOURCE_DIR}/algo
                    ${LEMS_SOURCE_DIR}/brcv/shp
                    ${LEMS_SOURCE_DIR}/brcv/seg/
                    ${LEMS_SOURCE_DIR}/db3p 
                    )
    set(histogrampick_sources
                    main.cxx 
                    histogrampick_manager.h histogrampick_manager.cxx
                    histogrampick_menus.h histogrampick_menus.cxx
                    )
add_executable(histogrampick ${histogrampick_sources} )
#aux_source_directory(Templates ${histogrampick_sources})
target_link_libraries(histogrampick vgl vgui vcl vil vil3d bsta  expatpp vsl
bgui slice)

