# brcv/trk/CMakeLists.txt
# Brown Tracking Libraries.

INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/brcv/trk )

# Brown Epipolar Curve Tracking Library
SUBDIRS( dbetl )

# Brown shape Curve Tracking Library
SUBDIRS( dbctrk )

# Brown edge Tracking Library
SUBDIRS( dbetrk )

# Brown Markov Random Field Tracking
# Deprecated Library
# SUBDIRS( dbmrf )

# Brown Mutual Information Tracking
SUBDIRS( dbinfo )

