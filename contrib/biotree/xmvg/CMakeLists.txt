INCLUDE_DIRECTORIES(${BIOTREE_SOURCE_DIR} ${lems_SOURCE_DIR}/brcv/mvg)
INCLUDE_DIRECTORIES( ${VXL_GEL_INCLUDE_DIR}/mrc )
INCLUDE_DIRECTORIES(${VXL_BRL_INCLUDE_DIR}/bbas)

SET(xmvg_src
xmvg_filter_descriptor.cxx              xmvg_filter_descriptor.h
xmvg_composite_filter_descriptor.cxx    xmvg_composite_filter_descriptor.h                               
                                        xmvg_atomic_filter_3d.h
                                        xmvg_composite_filter_3d.h
                                        xmvg_composite_filter_2d.h
                                        xmvg_atomic_filter_2d_base.h
                                        xmvg_atomic_filter_2d.h
xmvg_filter_response.txx                xmvg_filter_response.h
xmvg_source.cxx                         xmvg_source.h
xmvg_perspective_camera.txx             xmvg_perspective_camera.h
xmvg_no_noise_filter_descriptor.cxx     xmvg_no_noise_filter_descriptor.h
xmvg_gaussian_filter_descriptor.cxx     xmvg_gaussian_filter_descriptor.h
xmvg_no_noise_filter_3d.cxx             xmvg_no_noise_filter_3d.h
xmvg_gaussian_filter_3d.cxx             xmvg_gaussian_filter_3d.h
xmvg_svd.txx                            xmvg_svd.h

xmvg_point_filter_descriptor.cxx        xmvg_point_filter_descriptor.h
xmvg_point_filter_3d.cxx                xmvg_point_filter_3d.h
xmvg_transform_camera.cxx               xmvg_transform_camera.h
xmvg_cylinder_line_integrator.cxx       xmvg_cylinder_line_integrator.h
xmvg_parallel_beam_filter_3d.txx        xmvg_parallel_beam_filter_3d.h
                                        xmvg_difference_line_integrator.h
                                        xmvg_gaussian_integrating_filter_3d.h
                                        xmvg_scalar_multiplier_line_integrator.h
xmvg_pipe_line_integrator.cxx           xmvg_pipe_line_integrator.h
xmvg_pipe_filter_3d.h

xmvg_icosafilter_nonoise_3d.cxx         xmvg_icosafilter_nonoise_3d.h
xmvg_icosafilter_response.cxx           xmvg_icosafilter_response.h
                                        xmvg_gaussian_line_integrator.h

xmvg_gaussian_edge_detector_descriptor.cxx   xmvg_gaussian_edge_detector_descriptor.h
xmvg_gaussian_edge_detector_x_3d.cxx         xmvg_gaussian_edge_detector_x_3d.h
xmvg_gaussian_edge_detector_y_3d.cxx         xmvg_gaussian_edge_detector_y_3d.h
xmvg_gaussian_edge_detector_z_3d.cxx         xmvg_gaussian_edge_detector_z_3d.h

                                        xmvg_multiscale_filter_3d.h
xmvg_splat_filter.cxx                   xmvg_splat_filter.h
                                        xmvg_filter_3d_wrapper.h
)

AUX_SOURCE_DIRECTORY(Templates xmvg_src)
ADD_LIBRARY(xmvg ${xmvg_src})

TARGET_LINK_LIBRARIES(xmvg vpgl vpgl_algo vgl vgl_xio vil vnl vnl_xio vsl)

SUBDIRS(algo)

SUBDIRS(io)

IF(BUILD_TESTING)
  SUBDIRS(tests)
ENDIF(BUILD_TESTING)
