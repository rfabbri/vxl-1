# This is contrib/anil/shock_flip/CMakeLists.txt
# Brown Shock Recognition Library Executables

include_directories( ${lems_SOURCE_DIR}/brcv/rec)
include_directories( ${lems_SOURCE_DIR}/basic)
include_directories(${VXL_BRL_INCLUDE_DIR}/bbas)

set(test_cvmatch_sources
	test_cvmatch.cxx)

set(test_cvmatch_intensity_sources
	dbcvr_cvmatch_intensity.h
	dbcvr_cvmatch_intensity.cxx
	test_cvmatch_intensity.cxx)

set(test_cvmatch_even_sources
	test_cvmatch_even.cxx)

add_executable (test_cvmatch ${test_cvmatch_sources})
add_executable (test_cvmatch_intensity ${test_cvmatch_intensity_sources})
add_executable (test_cvmatch_even ${test_cvmatch_even_sources})

target_link_libraries (test_cvmatch dbcvr dbsol bsol)
target_link_libraries (test_cvmatch_intensity dbcvr dbsol bsol)
target_link_libraries (test_cvmatch_even dbcvr dbsol dbsol_algo vsol)
