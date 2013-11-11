#SET(CMAKE_CXX_FLAGS -O3)
SET(CMAKE_CXX_FLAGS -g)
LINK_DIRECTORIES(${LEMS_LIBRARY_DIR} ${VXL_LIBRARY_DIR})

    INCLUDE_DIRECTORIES( ${VXL_CORE_INCLUDE_DIR} 
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
    SET(histogrampick_sources
                    main.cxx 
                    histogrampick_manager.h histogrampick_manager.cxx
                    histogrampick_menus.h histogrampick_menus.cxx
                    )
ADD_EXECUTABLE(histogrampick ${histogrampick_sources} )
#AUX_SOURCE_DIRECTORY(Templates ${histogrampick_sources})
TARGET_LINK_LIBRARIES(histogrampick vgl vgui vcl vil vil3d bsta  expatpp vsl
bgui slice)

