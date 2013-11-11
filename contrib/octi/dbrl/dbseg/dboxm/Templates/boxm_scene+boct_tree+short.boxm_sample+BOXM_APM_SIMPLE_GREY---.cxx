#include <dboxm/boxm_scene.txx>
#include <boct/boct_tree.h>
#include <dboxm/boxm_sample.h>

typedef boct_tree<short, boxm_sample<BOXM_APM_SIMPLE_GREY> >  tree_type;
BOXM_SCENE_INSTANTIATE(tree_type);
