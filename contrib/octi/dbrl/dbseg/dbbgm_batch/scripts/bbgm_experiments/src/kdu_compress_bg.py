from utils.bbgm_utils import *
import time
import os

dbbgm_batch.register_processes();
dbbgm_batch.register_datatypes();

#path="C:/Users/octi/Documents/Mundy_Group/imagedata/Argus/argus_quant/registered/";
path="C:/Users/octi/Documents/Mundy_Group/imagedata/westin/full/";
command_args="KDU_X -o "+path+"/out_j2c/model_5bps.j2c -rate 8 Clevels=3 Ckernels=W5X3 Qguard=2 -precise";
ext=".jpg"
#model=updateImageStream(path,300,0.1,0.02,0,-1,ext);
#saveModel(path+"/out_wv/model.mdl",model);
model=loadModel(path+"/out_wv/model.mdl");
Mean=displayImg(model,"mean",0,1);
saveImg(Mean,path+"/out_wv/mean2.bmp");
#time.sleep(5)
[myWavelet,waveletDomain3]=wvXform("bbgm_image_sptr",model,8,5,3,command_args);


#saveWavelet(path+"out_wv/weight_wavelet.wv",myWavelet);

#myWavelet=loadWavelet(path+"out_wv/wavelet.wv");
#mySubband=getSubband("bbgm_image_sptr",3,"LL",2,myWavelet);
forward=getSubband("bbgm_image_sptr",3,"forward",0,myWavelet);
wvdomain=displayImg(forward,"mean",0,1);
saveImg(wvdomain,path+"/out_wv/forward.tiff");
#waveletDomain=getSubband("bbgm_image_sptr",3,"forward",0,myWavelet);
[test_image,ni,nj]=openImage(path+"frames0407.jpg");
#[roc_image,ni,nj]=openImage(path+"/out_ROC/"+"frame_1_thresh0.8575.tiff");
#dbbgm_batch.print_db();



#prob_map=measureWvLookup(myWavelet,test_image,"probability",0.15,"bil",path+"/out_stats/","frame"+str(1),0.5);
truth_map=measure(model,test_image,"probability",0.2)
saveImg(truth_map,path+"/out_pmap/full.tiff")
#pathlist=os.listdir(path+"/out_ROC");
token="frame_1";
#for im_path in pathlist:
#    if ((str.find(im_path,".tiff")!=-1)&(str.find(im_path,token)!=-1)):
#        [tmp_image,ni,nj]=openImage(path+"/out_ROC/"+im_path);
#        evalROC(truth_map,tmp_image,0.5,path+"/out_stats/"+token+"_ROC.txt");
GUI(model,0,forward,truth_map);






 
