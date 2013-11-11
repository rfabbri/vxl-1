#include <splr/splr_basic_pizza_slice_splat_collection.txx>
#include <splr/tests/dummy_filter_2d.h>
#include <xscan/xscan_dummy_scan.h>
#include <splr/tests/dummy_filter_3d.h>
#include <xmvg/xmvg_perspective_camera.h>

SPLR_BASIC_PIZZA_SLICE_SPLAT_COLLECTION_WITH_ALL_ARGS_INSTANTIATE(double, int, xscan_dummy_scan, dummy_filter_2d, dummy_filter_3d, xmvg_perspective_camera<double> );
