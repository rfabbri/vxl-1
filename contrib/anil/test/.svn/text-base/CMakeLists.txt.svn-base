# This is contrib/anil/shock_flip/CMakeLists.txt
# Brown Shock Recognition Library Executables

INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/brcv/rec)
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/basic)
INCLUDE_DIRECTORIES(${VXL_BRL_INCLUDE_DIR}/bbas)

SET(test_cvmatch_sources
	test_cvmatch.cxx)

SET(test_cvmatch_intensity_sources
	dbcvr_cvmatch_intensity.h
	dbcvr_cvmatch_intensity.cxx
	test_cvmatch_intensity.cxx)

SET(test_cvmatch_even_sources
	test_cvmatch_even.cxx)

ADD_EXECUTABLE (test_cvmatch ${test_cvmatch_sources})
ADD_EXECUTABLE (test_cvmatch_intensity ${test_cvmatch_intensity_sources})
ADD_EXECUTABLE (test_cvmatch_even ${test_cvmatch_even_sources})

TARGET_LINK_LIBRARIES (test_cvmatch dbcvr dbsol bsol)
TARGET_LINK_LIBRARIES (test_cvmatch_intensity dbcvr dbsol bsol)
TARGET_LINK_LIBRARIES (test_cvmatch_even dbcvr dbsol dbsol_algo vsol)
