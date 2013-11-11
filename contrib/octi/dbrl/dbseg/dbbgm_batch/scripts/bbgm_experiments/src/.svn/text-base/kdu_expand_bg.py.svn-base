from utils.bbgm_utils import *
import time
import os

dbbgm_batch.register_processes();
dbbgm_batch.register_datatypes();

path="C:/Users/octi/Documents/Mundy_Group/imagedata/westin/full/";
command_argse="KDU_IX -i "+path+"/out_j2c/model_5bps.j2c -quiet"; 

ext="jpg"
#model=updateImageStream(path,300,0.1,0.02,0,-1,ext);
#saveModel(path+"/out_wv/model.mdl",model);
model=loadModel(path+"/out_wv/model.mdl");
Mean=displayImg(model,"mean",0,1);
#saveImg(Mean,path+"/out_stats/mean2.bmp");
#time.sleep(5)
#weightWavelet=loadWavelet(path+"out_wv/weight_wavelet.wv");

#weight_domain=getSubband("bbgm_image_sptr",3,"forward",0,weightWavelet);
[myKDUWavelet,initModel]=wvXform("bbgm_image_sptr",model,-1,5,3,command_argse);
#saveWavelet(path+"out_wv/wavelet.wv",myWavelet);
mySubband=getSubband("bbgm_image_sptr",3,"forward",0,myKDUWavelet);
 
#myWavelet=loadWavelet(path+"out_wv/weight_wavelet.wv");
#mySubband=getSubband("bbgm_image_sptr",3,"LL",0,myWavelet);

#waveletDomain=getSubband("bbgm_image_sptr",3,"forward",0,myWavelet);
[test_image,ni,nj]=openImage(path+"frames_0380.jpg");

#truth_map=measure(model,test_image,"probability",0.2)
prob_map=measureWvLookup(myKDUWavelet,test_image,"probability",0.15,"bil",path+"/out_stats/","myframe380",0.9);
#prob_map=measure(mySubband,test_image,"probability",0.2)
#saveImg(truth_map,path+"/out_pmap/truth_map.tiff")
#saveImg(inverse_map,path+"/out_pmap/inverse_map.tiff")
#[roc_image,ni,nj]=openImage(path+"/out_ROC/"+"frame_1_thresh0.8575.tiff");
#dbbgm_batch.print_db();



#prob_map=measureWvLookup(myWavelet,test_image,"probability",0.15,"bil",path+"/out_stats/","frame"+str(1),0.5);
#truth_map=measure(model,test_image,"probability",0.2)

saveImg(prob_map,path+"/out_pmap/my_frame.tiff")
GUI(model,mySubband,0,prob_map);






 
