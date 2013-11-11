#include <Inventor/nodes/SoGroup.h>
#include <VolumeViz/nodes/SoVolumeRender.h>
#include <VolumeViz/nodes/SoVolumeData.h>
#include <VolumeViz/nodes/SoVolumeRender.h>
#include <VolumeViz/nodes/SoVolumeRendering.h>
#include <VolumeViz/nodes/SoTransferFunction.h>
#include <Inventor/nodes/SoCylinder.h>



biov_examiner_tableau_new setup_biov_examiner_tableau(SbVec3s dim,uint8_t * voxels);

uint8_t * read_ede_responses(vcl_string fbase,unsigned int marginx,unsigned int marginy,
                        unsigned int marginz,SbVec3s & dim);
                        

uint8_t *  read_filter_responses(vcl_string fbase,unsigned int filter_num,unsigned int marginx,
                           unsigned int marginy, unsigned int marginz,SbVec3s & dim);

uint8_t *  read_splr_responses(vcl_string fname,unsigned int filter_num,double resolution,
                           SbVec3s & dim);

