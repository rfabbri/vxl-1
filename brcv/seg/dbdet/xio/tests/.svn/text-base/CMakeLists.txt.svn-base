# medial_livewire/xio/tests/CMakeLists.txt

INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/contrib )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/contrib/medial_livewire )

ADD_EXECUTABLE( dbdet_xio_test_all
  test_driver.cxx
  test_xml_write_read.cxx
)

TARGET_LINK_LIBRARIES( dbdet_xio_test_all dbdet_xio testlib )


ADD_EXECUTABLE( dbdet_xio_test_include test_include.cxx )
TARGET_LINK_LIBRARIES( dbdet_xio_test_include dbdet_xio vsl )
