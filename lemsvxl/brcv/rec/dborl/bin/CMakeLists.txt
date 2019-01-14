# This is brcv/rec/dborl/bin/CMakeLists.txt
# Brown ORL Executables

include_directories( ${LEMS_SOURCE_DIR}/breye1 )
include_directories( ${LEMS_SOURCE_DIR}/brcv/trk )
include_directories( ${LEMS_SOURCE_DIR}/brcv/shp )
include_directories( ${LEMS_SOURCE_DIR}/brcv/rec )
include_directories( ${LEMS_SOURCE_DIR}/brcv/seg )
include_directories( ${VXL_BRL_INCLUDE_DIR} )
include_directories( ${VXL_EXPAT_INCLUDE_DIR} )
include_directories( ${VXL_MUL_INCLUDE_DIR} )

add_executable(dborl_command_line_utilities
dborl_prepare_orl_dataset.cxx
dborl_command_line.h
dborl_command_line_main.cxx
)

target_link_libraries( dborl_command_line_utilities dbskr dborl_algo dbskr_algoio vil  )

add_executable(dborl_cmd_line_dataset_procs
dborl_dataset_procs.cxx
dborl_dataset_procs.h
dborl_dataset_procs_main.cxx
)

target_link_libraries( dborl_cmd_line_dataset_procs dbskr_pro dbsk2d dbsk2d_algo dbskr  dborl_algo dbskr_algoio vil  )


