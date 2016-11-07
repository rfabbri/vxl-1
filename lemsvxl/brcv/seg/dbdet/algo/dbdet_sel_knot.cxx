#include "dbdet_sel_knot.h"

#include <vcl_iostream.h>
#include <vcl_fstream.h>
#include <vcl_cassert.h>
#include <vcl_deque.h>
#include <vcl_algorithm.h>

#include <vgl/vgl_polygon.h>
#include <vgl/vgl_distance.h>
#include <vgl/vgl_clip.h>
#include <vgl/vgl_area.h>

#include "dbdet_sel_utils.h"
#include <dbgl/algo/dbgl_eulerspiral.h>

