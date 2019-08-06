set(mcr_stereo_sources
   mcr.cxx
)

set(pad_sources
   pad.cxx
)

set(generate_synth_sequence_sources
   generate_synth_sequence.cxx
)

set(generate_synth_sequence_2_sources
   generate_synth_sequence_2.cxx
)

set(generate_synth_sequence_3_sources
   generate_synth_sequence_3.cxx
)

set(vox_mcs_stereo_eval_sources
   vox_mcs_eval.cxx vox_mcs_eval.h
   vox_mcs_eval_params.cxx vox_mcs_eval_params.h 
   vox_mcs_eval_params_sptr.h vbl_smart_ptr+vox_eval_mcs_params-.cxx
)

set(badj_sources
   badj.cxx
)

set(pnp_sources
   pnp.cxx
)

add_executable(mcr ${mcr_stereo_sources})
target_link_libraries(mcr mw_pro bmcsd_algo sdetd_io sdetd_algo)

add_executable(pad ${pad_sources})
target_link_libraries(pad vcl)

add_executable(generate_synth_sequence ${generate_synth_sequence_sources})
target_link_libraries(generate_synth_sequence mw_pro sdetd_io sdetd_algo)

add_executable(generate_synth_sequence_2 ${generate_synth_sequence_2_sources})
target_link_libraries(generate_synth_sequence_2 mw_pro sdetd_io sdetd_algo)

add_executable(generate_synth_sequence_3 ${generate_synth_sequence_3_sources})
target_link_libraries(generate_synth_sequence_3 mw_pro sdetd_io sdetd_algo)

add_executable(vox_mcs_eval ${vox_mcs_stereo_eval_sources})
target_link_libraries(vox_mcs_eval mw_pro dborl_algo dbxml sdetd_io sdetd_algo)

add_executable(badj ${badj_sources})
target_link_libraries(badj bmcsd vpgld_io vpgld_algo vpgl_algo buld vul)


if (USE_OpenMVG)
  # set(Boost_USE_STATIC_LIBS   OFF)
  find_package(OpenMVG REQUIRED)
  # find_package( Boost 1.35.0 COMPONENTS iostreams)
endif()

if ( OpenMVG_FOUND )
  MESSAGE("OpenMVG includes: " ${OPENMVG_INCLUDE_DIRS})
  #include_directories(${OPENMVG_INCLUDE_DIRS}/third_party)
  include_directories( ${OPENMVG_INCLUDE_DIRS} )
  
  add_executable(pnp ${pnp_sources})
  target_link_libraries(pnp
    openMVG_multiview
    mw
  )
  #message("FOUND YEAH" "${Boost_LIBRARIES}" "${Boost_INCLUDE_DIRS}" )
else ()
  #message("FOUND NO")
endif()
