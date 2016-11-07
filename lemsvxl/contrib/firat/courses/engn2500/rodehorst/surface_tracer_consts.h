/*******************************************************************************
* 
*******************************************************************************/

#ifndef SURFACE_TRACER_CONSTS_H
#define SURFACE_TRACER_CONSTS_H

/******************************************************************************/
/* Global File Constants */
/*
const char DT_FILE_DIR[] = "/vision/projects/kimia/3dVisualize/Subpixel-3d-Reconstruction/DATA-Test/";
const char ENO_FILE_DIR[] = "/vision/projects/kimia/3dVisualize/Subpixel-3d-Reconstruction/DATA-Test/";

const char DT_FILE_NAME[] = "FredData/Medical/rad-80-dt.raw";
//"FredData/Medical/carot-b-80-dt.raw"
//"FredData/Medical/rad-100-dt.raw"
//"FredData/Medical/rad-80-dt.raw"
//"FredData/Medical/radius-80-dt.raw" <--- ALL ZEROS! (except 2 pts)
//"simple_test/simple_test.raw"
//"simple_test/simple_test2.raw"
//"CylinderConeZ15/cylinderZ.raw"
//"Sphere1-d15-r43/sphere1-d15-r43.raw"
//"Sphere1-d50-c245-r2205/sphere1-dim50-center245-r2205.raw"
//"CubeTiltY/cube_tiltY.raw"
//"TwoSphere20-noSub/sphere20_c1888r55_c214r38.raw"
const bool DT_FILE_HAS_HEADER = true;
const unsigned int DT_FILE_HEADER_NUM_LINES = 4;

const char ENO_FILE_PREFIX[] = "INVALID";
//"simple_test/simple_test"
//"simple_test/simple_test2"
//"CylinderConeZ15/cylinderZ"
//"Sphere1-d15-r43/sphere1-d15-r43"
//"Sphere1-d50-c245-r2205/sphere1-dim50-center245-r2205"
//"CubeTiltY/cube_tiltY"
//"TwoSphere20-noSub/sphere20_c1888r55_c214r38"
*/
/******************************************************************************/
/* Global Constants */

#include <float.h>
#define DEBUG6

const char INPUTS_FILE_NAME[] = "/home/firat/lemsvxl/src/contrib/firat/courses/engn2500/rodehorst/input_files/surface_tracer_inputs2.txt";
const unsigned int DT_FILE_HEADER_NUM_LINES = 4;

const unsigned int NUM_DIMENSIONS = 3;
const unsigned int DIM_X = 0;
const unsigned int DIM_Y = 1;
const unsigned int DIM_Z = 2;

const unsigned int FACES_PER_DIMENSION = 2;
const unsigned int EDGES_PER_FACE = 4;
const unsigned int MAX_ENOS_PER_EDGE = 2;
const unsigned int MAX_ENOS_PER_FACE = EDGES_PER_FACE*MAX_ENOS_PER_EDGE;
const unsigned int MAX_ENOS_PER_VOXEL = 3*MAX_ENOS_PER_FACE;
const unsigned int MAX_POINTS_PER_UNAMB_POLY = 6;
const unsigned int MAX_POINTS_PER_AMB_POLY = 24;
const unsigned int POINTS_PER_TRIANGLE = 3;

// This number must be large enough to not result in degenerate triangles
// under float precision.
const double REPLACE_ZERO_VALUE = -1e-03;

const double INIT_SCORE = -3.0;
const double HIGHEST_SCORE = DBL_MAX;//+2.0;
const double LOWEST_SCORE = -2.0;

const bool TRUE = 1;
const bool FALSE = 0;

const unsigned int MAX_STRING_LENGTH = 1024;
const double FLOAT_ERROR_MARGIN = 1e-05;

// size of file in x, y, z
const unsigned int DEFAULT_FILE_DIMENSIONS[NUM_DIMENSIONS] = {15, 15, 15};
const char DIMENSION_CHARS[NUM_DIMENSIONS] = {'x','y','z'};

/******************************************************************************/

#endif
