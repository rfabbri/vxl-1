# brcv/trk/CMakeLists.txt
# Brown Tracking Libraries.

include_directories( ${lems_SOURCE_DIR}/brcv/trk )

# Brown Epipolar Curve Tracking Library
subdirs( dbetl )

# Brown shape Curve Tracking Library
subdirs( dbctrk )

# Brown edge Tracking Library
subdirs( dbetrk )

# Brown Markov Random Field Tracking
# Deprecated Library
# subdirs( dbmrf )

# Brown Mutual Information Tracking
subdirs( dbinfo )

