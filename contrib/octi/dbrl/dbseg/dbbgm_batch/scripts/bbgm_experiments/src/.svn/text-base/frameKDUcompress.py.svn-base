import os
from utils.bbgm_utils import *
dbbgm_batch.register_processes();
dbbgm_batch.register_datatypes();
'''
Created on Apr 14, 2011

@author: octi
'''
in_path="C:/Users/octi/Documents/Mundy_Group/imagedata/westin/full/";
out_path="C:/Users/octi/Documents/Mundy_Group/imagedata/westin/full/out_compressed/";
#in_path="C:/Users/octi/Documents/Mundy_Group/imagedata/Argus/argus_quant/registered/";
#out_path="C:/Users/octi/Documents/Mundy_Group/imagedata/Argus/argus_quant/registered/out_compressed/";
command_args="KDU_X Clevels=5 Sprecision=16 Ssigned=no Creversible=no -rate 0.05 ";
mask_args="KDU_X Clevels=5 Sprecision=8 Ssigned=no  Creversible=yes ";
command_args_i="KDU_IX ";
model=loadModel(in_path+"/out_wv/model.mdl");
mean=displayImg(model,"mean",0,1)
#saveImg(mean,in_path+"out_bg/mean.tiff")
#ext=".jpg"
#[test_image,ni,nj]=openImage(in_path+"frames_"+frameSuffix(380,4)+ext);
#truth_map=measure(model,test_image,"probability",0.15)
#saveImg(truth_map,in_path+"/out_pmap/frames_"+frameSuffix(380,4)+".tiff")
#for i in range (1,601):
#    temp_args=command_args;
#    temp_args+=" -o "+out_path+"compressed_"+frameSuffix(i,4)+".j2c"
#    temp_mask_args=mask_args;
#    temp_mask_args+=" -o "+out_path+"mask_"+frameSuffix(i,4)+".j2c"
##    [test_image,ni,nj]=openImage(in_path+"frames_"+frameSuffix(i,4)+ext);
##    truth_map=measure(model,test_image,"probability",0.15)
#    kduCompressFrame(truth_map,test_image,0.8,temp_args,temp_mask_args,1)
    
for i in range (380,381):
    temp_args=command_args_i;
    temp_args+="-i "+out_path+"compressed_"+frameSuffix(i,4)+".j2c"
    kduDecompressFrame(mean,temp_args,"",out_path+"foreground_"+frameSuffixWrite(i,99)+".jpg",out_path+"reconstructed_"+frameSuffixWrite(i,99)+".jpg")