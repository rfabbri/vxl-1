project( coresys )

set(SOURCES 
  parameters/params.cpp
  compressed/blocks.cpp
  compressed/codestream.cpp
  compressed/compressed.cpp
  coding/block_coding_common.cpp
  coding/block_decoder.cpp
  coding/block_encoder.cpp
  coding/decoder.cpp
  coding/encoder.cpp
  coding/mq_decoder.cpp
  coding/mq_encoder.cpp
  transform/analysis.cpp
  transform/colour.cpp
  transform/synthesis.cpp
  kernels/kernels.cpp
  messaging/messaging.cpp
  roi/roi.cpp
  common/kdu_block_coding.h
  common/kdu_compressed.h
  common/kdu_elementary.h
  common/kdu_kernels.h
  common/kdu_messaging.h
  common/kdu_roi_processing.h
  common/kdu_sample_processing.h
  common/kdu_utils.h
  parameters/params_local.h
  transform/transform_local.h
  transform/synthesis_local.h
  transform/msvc_dwt_mmx_local.h
  transform/analysis_local.h
)
add_definitions( 
  -DCORESYS_EXPORTS
  -DWIN32
  -#-D_USRDLL
  -D_WINDOWS
  -D_CRT_SECURE_NO_WARNINGS
)

ADD_LIBRARY( kdu_coresys STATIC ${SOURCES} )
SET_TARGET_PROPERTIES(kdu_coresys PROPERTIES OUTPUT_NAME kdu_coresys DEBUG_POSTFIX d)

