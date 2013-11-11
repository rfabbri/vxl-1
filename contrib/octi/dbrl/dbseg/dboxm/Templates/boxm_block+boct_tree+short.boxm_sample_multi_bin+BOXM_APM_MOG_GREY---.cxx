#include <dboxm/boxm_block.txx>
#include <dboxm/boxm_apm_traits.h>
#include <dboxm/boxm_sample_multi_bin.h>
#include <boct/boct_tree.h>

typedef boct_tree<short,boxm_sample_multi_bin<BOXM_APM_MOG_GREY> >  tree_type;
BOXM_BLOCK_INSTANTIATE(tree_type);
