/*******************************************************************************
* 
*******************************************************************************/

#ifndef SURFACE_TRACER_FUNCS_H
#define SURFACE_TRACER_FUNCS_H

/******************************************************************************/
/* Libraries */

#include <vector>
#include <deque>
using namespace std;

#include "surface_tracer_consts.h"
#include "surface_tracer_types.h"

/******************************************************************************/
/* Function Prototypes */

// in surface_tracer.cxx:
void SurfaceTracer(vector<double> & eno_coords,
                   vector<EnoPoint> & eno_array,
                   vector<unsigned int> & poly_list,
                   vector<double> & normal_list,
                   unsigned int file_dimensions[NUM_DIMENSIONS]);

bool SurfaceTracerIncrement(vector<double> & eno_coords,
                            vector<EnoPoint> & eno_array,
                            vector<unsigned int> & poly_list,
                            vector<double> & normal_list,
                            unsigned int file_dimensions[NUM_DIMENSIONS]);

// in read_inputs_file.cxx
void ReadInputsFile(const char inputs_file_name[],
                    char dt_file_dir[],
                    char dt_file_name[],
                    char eno_file_dir[],
                    char eno_file_prefix[],
                    char output_file_dir[],
                    char output_file_name[],
                    bool & dt_file_has_header,
                    bool & dt_file_has_shocks,
                    unsigned int file_dimensions[NUM_DIMENSIONS]);

// in read_dt_file.cxx
float *** ReadDTFile(const char file_path[],
                     const char file_name[],
                     const bool file_has_header,
                     const bool file_has_shocks,
                     unsigned int file_dimensions[NUM_DIMENSIONS]);

// in read_find_eno.cxx
SpaceUnit *** ReadENOFiles(const char file_path[],
                           const char file_prefix[],
                           const unsigned int file_dimensions[NUM_DIMENSIONS],
                           vector<EnoPoint> & eno_array,
                           vector<double> & eno_coords);

SpaceUnit *** FindENOPoints(float *** distance_transform,
                            const unsigned int file_dimensions[NUM_DIMENSIONS],
                            vector<EnoPoint> & eno_array,
                            vector<double> & eno_coords);

// in make_voxels.cxx
Voxel *** MakeVoxels(SpaceUnit *** space_array,
                     const unsigned int file_dimensions[NUM_DIMENSIONS]);

// in init_wave_front.cxx
bool InitWaveFront(float *** distance_transform,       // input only
                   Voxel *** voxel_array,              // eno_used flags set
                   const unsigned int file_dimensions[NUM_DIMENSIONS], // input
                   vector<EnoPoint> & eno_array,       // used flags set
                   const vector<double> & eno_coords,  // input only
                   deque<WaveElement> & wave_front,    // output only
                   vector<unsigned int> & poly_list,   // new polys added
                   vector<double> & normal_list);      // new normals added

// in propagate_wave.cxx
bool PropagateWave(Voxel *** voxel_array,              // eno_used flags set
                   const unsigned int file_dimensions[NUM_DIMENSIONS], // input
                   vector<EnoPoint> & eno_array,       // used flags set
                   const vector<double> & eno_coords,  // input only
                   deque<WaveElement> & wave_front,    // get next and add new
                   vector<unsigned int> & poly_list,   // new polys added
                   vector<double> & normal_list);      // new normals added

// in get_polygon.cxx
unsigned int
GetUnambiguousPolygon(const Voxel * voxel_ptr,    // DON'T set eno_used flags
                      const vector<EnoPoint> & eno_array,
                      const unsigned int voxel_idxs[NUM_DIMENSIONS], // input
                      unsigned int poly_idxs[MAX_POINTS_PER_UNAMB_POLY],//output
                      unsigned int poly_dims[MAX_POINTS_PER_UNAMB_POLY],
                      unsigned int poly_sides[MAX_POINTS_PER_UNAMB_POLY],
                      unsigned int poly_face_enos[2*MAX_POINTS_PER_UNAMB_POLY]);

unsigned int
GetAmbiguousPolygon(const double in_normal[NUM_DIMENSIONS],
                    const vector<double> & eno_coords,
                    Voxel * voxel_ptr,            // set eno_used flags
                    vector<EnoPoint> & eno_array, // set used flags
                    const unsigned int voxel_idxs[NUM_DIMENSIONS], // input only
                    unsigned int poly_idxs[MAX_POINTS_PER_AMB_POLY+1], //outputs
                    unsigned int poly_dims[MAX_POINTS_PER_AMB_POLY+1],
                    unsigned int poly_sides[MAX_POINTS_PER_AMB_POLY],
                    unsigned int poly_face_enos[2*MAX_POINTS_PER_AMB_POLY]);

// in make_triangles.cxx
void MakeTriangles(const vector<double> & eno_coords, // inputs
                   vector<EnoPoint> & eno_array, // set used flags
                   const unsigned int * poly_idxs,
                   const unsigned int * poly_dims,
                   const unsigned int * poly_sides,
                   const unsigned int * poly_face_enos,
                   const unsigned int poly_length,
                   Voxel *** voxel_array, // set eno_used flags in faces
                   const unsigned int voxel_idxs[NUM_DIMENSIONS],
                   const unsigned int file_dimensions[NUM_DIMENSIONS],
                   deque<WaveElement> & wave_front, // (in-)outputs
                   vector<unsigned int> & poly_list,
                   vector<double> & normal_list);

void PolygonizeVoxel(const WaveElement & in_element,
                     Voxel *** voxel_array,
                     const unsigned int file_dimensions[NUM_DIMENSIONS],
                     const vector<double> & eno_coords,
                     vector<EnoPoint> & eno_array, // set used flags
                     deque<WaveElement> & wave_front, // for outputs
                     vector<unsigned int> & poly_list,
                     vector<double> & normal_list);

void WriteVTKPolyFile(const char * file_path,
                      const double * coords,
                      const unsigned int num_coords,
                      const unsigned int * triangles,
                      const unsigned int num_triangles,
                      const char * header = "");
                       

/******************************************************************************/

#endif
