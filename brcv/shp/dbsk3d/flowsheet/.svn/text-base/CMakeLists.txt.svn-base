# This is brcv/shp/dbsk3d/algo/CMakeLists.txt
# Brown Shock3D Algorithm Library

INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/shp)

SET( dbsk3d_flowsheet_sources

dbsk3d_fs_flowsheet.cxx             dbsk3d_fs_flowsheet.h
dbsk3d_fs_flowsheet_bucket.cxx      dbsk3d_fs_flowsheet_bucket.h

#Frederic's code
barycentric.cxx                     barycentric.h                 
blue_shocks_utils.cxx               blue_shocks_utils.h           
buck_utils.cxx                      buck_utils.h                  
circum.cxx                          circum.h                      
fetch_green_shocks.cxx              fetch_green_shocks.h          
fetch_green_shocks_v2.cxx           fetch_green_shocks_v2.h       
fetch_pairs_internal.cxx            fetch_pairs_internal.h        
fetch_pairs_via_layers.cxx          fetch_pairs_via_layers.h      
fetch_red_shocks.cxx                fetch_red_shocks.h            
fetch_red_shocks_v2.cxx             fetch_red_shocks_v2.h         
fetch_shocks_fst_layer.cxx          fetch_shocks_fst_layer.h      
fetch_shocks_int.cxx                fetch_shocks_int.h            
fetch_shocks_oth_layers.cxx         fetch_shocks_oth_layers.h     
sheet_flow_io_v2.cxx                sheet_flow_io_v2.h            
sheet_flow_mbuck_mres.cxx           sheet_flow_mbuck_mres.h      
shock_connect.cxx                   shock_connect.h               
shock_utils.cxx                     shock_utils.h                 
valid_shocks.cxx                    valid_shocks.h                

)


ADD_LIBRARY(dbsk3d_flowsheet ${dbsk3d_flowsheet_sources})

TARGET_LINK_LIBRARIES(dbsk3d_flowsheet dbsk3d dbmsh3d_algo)

