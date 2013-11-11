#include <splr/splr_explicit_splat_collection.txx>
#include <splr/tests/dummy_filter_2d.h>
#include <splr/tests/dummy_scan.h>
#include <splr/tests/dummy_filter_3d.h>
#include <splr/tests/dummy_camera.h>

SPLR_EXPLICIT_SPLAT_COLLECTION_WITH_ALL_ARGS_INSTANTIATE(double, int, dummy_scan, dummy_filter_2d, dummy_filter_3d, dummy_camera);
