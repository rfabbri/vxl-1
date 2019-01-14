# medial_livewire/xio/tests/CMakeLists.txt

include_directories( ${LEMS_SOURCE_DIR}/contrib )
include_directories( ${LEMS_SOURCE_DIR}/contrib/medial_livewire )

add_executable( dbdet_xio_test_all
  test_driver.cxx
  test_xml_write_read.cxx
)

target_link_libraries( dbdet_xio_test_all dbdet_xio testlib )


add_executable( dbdet_xio_test_include test_include.cxx )
target_link_libraries( dbdet_xio_test_include dbdet_xio vsl )
