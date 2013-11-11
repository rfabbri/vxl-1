#include <proc/bioproc_dummy_splat_resource.txx>
#include <xmvg/xmvg_parallel_beam_filter_3d.h>
#include <xmvg/xmvg_pipe_filter_3d.h>
typedef  xmvg_parallel_beam_filter_3d<xmvg_pipe_filter_3d> xmvg_parallel_beam_filter_3d_xmvg_pipe_filter_3d;
BIOPROC_DUMMY_SPLAT_RESOURCE_INSTANTIATE(double, xmvg_parallel_beam_filter_3d_xmvg_pipe_filter_3d );
