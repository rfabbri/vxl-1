#include "dbdet_sel_knot.h"

#include <iostream>
#include <fstream>
#include <cassert>
#include <deque>
#include <algorithm>

#include <vgl/vgl_polygon.h>
#include <vgl/vgl_distance.h>
#include <vgl/vgl_clip.h>
#include <vgl/vgl_area.h>

#include "dbdet_sel_utils.h"
#include <bgld/algo/bgld_eulerspiral.h>

