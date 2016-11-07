# brcv/trk/CMakeLists.txt
# Brown Tracking Libraries.

include_directories( ${lems_SOURCE_DIR}/brcv/trk )

# Brown Epipolar Curve Tracking Library
add_subdirectory( dbetl )

# Brown shape Curve Tracking Library
add_subdirectory( dbctrk )

# Brown edge Tracking Library
add_subdirectory( dbetrk )

# Brown Markov Random Field Tracking
# Deprecated Library
# add_subdirectory( dbmrf )

# Brown Mutual Information Tracking
add_subdirectory( dbinfo )

