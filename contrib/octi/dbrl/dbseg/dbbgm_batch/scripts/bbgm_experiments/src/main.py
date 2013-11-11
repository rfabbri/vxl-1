from utils.bbgm_utils import *
import time
import os

dbbgm_batch.register_processes();
dbbgm_batch.register_datatypes();

path="C:/Users/octi/Documents/Mundy Group/imagedata/tree_sequence/";

#model=updateImageStream(path,300,0.1,0.02,0,-1);
#saveModel(path+"/out_wv/model.mdl",model);
model=loadModel(path+"/out_wv/model.mdl");

#myWavelet=wvXform("bbgm_image_sptr",model,2,2,3);
#saveWavelet(path+"out_wv/wavelet.wv",myWavelet);
myWavelet=loadWavelet(path+"out_wv/wavelet.wv");
#mySubband=getSubband("bbgm_image_sptr",3,"LL",2,myWavelet);
#inverse=getSubband("bbgm_image_sptr",3,"LL",0,myWavelet);
#waveletDomain=getSubband("bbgm_image_sptr",3,"forward",0,myWavelet);
[test_image,ni,nj]=openImage(path+"00001.tiff");
[roc_image,ni,nj]=openImage(path+"/out_ROC/"+"frame_1_thresh0.8575.tiff");
#dbbgm_batch.print_db();
Mean=displayImg(model,"mean",0,1);
#Mean2=displayImg(model,"mean",1,1);
#saveImg(Mean,path+"/out_stats/mean.jpg");
#saveImg(Mean2,path+"/out_stats/mean2.jpg");
#prob_map=measureWvLookup(myWavelet,test_image,"probability",0.15,"bil",path+"/out_stats/","frame"+str(1),0.5);
truth_map=measure(model,test_image,"probability",0.2)

pathlist=os.listdir(path+"/out_ROC");
token="frame_1";
#for im_path in pathlist:
#    if ((str.find(im_path,".tiff")!=-1)&(str.find(im_path,token)!=-1)):
#        [tmp_image,ni,nj]=openImage(path+"/out_ROC/"+im_path);
#        evalROC(truth_map,tmp_image,0.5,path+"/out_stats/"+token+"_ROC.txt");
GUI(model,0,0,truth_map);






 
