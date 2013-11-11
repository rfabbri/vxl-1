INCLUDE_DIRECTORIES( /vision/packages/Python/Python-1.4/Include )
INCLUDE_DIRECTORIES( /vision/packages/arch/SunOS-5.5-sparc/lib/python1.4/config)
INCLUDE_DIRECTORIES( /vision/packages/Python/Extensions-1.4/NumPy/Include)

SET( eno_tracer_sources 
  eno_tracer/1d_eno_types.h  eno_tracer/eno_image.c  eno_tracer/enoimagemodule.c  eno_tracer/eno_interpolant.c  
  eno_tracer/eno_interval.c  eno_tracer/eno_shock.c  eno_tracer/eno_utils.c  eno_tracer/eno_zero_crossing.c  
  eno_tracer/twod_dpoint.c  eno_tracer/twod_dpoint.h   
)
ADD_LIBRARY( engn2500ENOImage ${eno_tracer_sources} )

SET( surface_tracer_sources
gl_main.cxx                make_normals.cxx    polygonize_voxel.cxx  read_find_eno.cxx        surface_tracer.cxx      surface_tracer_types.h  write_vtk_poly_file.cxx
init_wave_front.cxx        make_triangles.cxx  propagate_wave.cxx    read_inputs_file.cxx     surface_tracer_funcs.h  utility.cxx
get_polygon.cxx   make_voxels.cxx     read_dt_file.cxx      surface_tracer_consts.h  surface_tracer.h        utility.h
)

ADD_EXECUTABLE(engn2500_surface_tracer ${surface_tracer_sources})
TARGET_LINK_LIBRARIES (engn2500_surface_tracer engn2500ENOImage glut X11 GL GLU Xmu)