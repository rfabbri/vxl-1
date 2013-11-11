// (c) 2005 Bio-Tree Systems, Inc.  All rights reserved.
// No part of this software may be used, reproduced or transmitted for
// any purpose, in any form or by any means, except as expressly
// permitted in writing by Bio-Tree Systems, Inc.

#include <proc/biocluster/biocluster_volpart_filtering_proc.txx>
#include <xmvg/xmvg_parallel_beam_filter_3d.h>
#include <xmvg/xmvg_pipe_filter_3d.h>
typedef  xmvg_parallel_beam_filter_3d<xmvg_pipe_filter_3d> xmvg_parallel_beam_filter_3d_xmvg_pipe_filter_3d;
BIOCLUSTER_VOLPART_FILTERING_PROC_INSTANTIATE(double, xmvg_parallel_beam_filter_3d_xmvg_pipe_filter_3d );

