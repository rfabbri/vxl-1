# This is brcv/rec/dborl/bin/CMakeLists.txt
# Brown ORL Executables

INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/breye1 )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/brcv/trk )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/brcv/shp )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/brcv/rec )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/brcv/seg )
INCLUDE_DIRECTORIES( ${VXL_BRL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${VXL_EXPAT_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${VXL_MUL_INCLUDE_DIR} )

ADD_EXECUTABLE(dborl_command_line_utilities
dborl_prepare_orl_dataset.cxx
dborl_command_line.h
dborl_command_line_main.cxx
)

TARGET_LINK_LIBRARIES( dborl_command_line_utilities dbskr dborl_algo dbskr_algoio vil  )

ADD_EXECUTABLE(dborl_cmd_line_dataset_procs
dborl_dataset_procs.cxx
dborl_dataset_procs.h
dborl_dataset_procs_main.cxx
)

TARGET_LINK_LIBRARIES( dborl_cmd_line_dataset_procs dbskr_pro dbsk2d dbsk2d_algo dbskr  dborl_algo dbskr_algoio vil  )


