import os
from utils.bbgm_utils import *
dbbgm_batch.register_processes();
dbbgm_batch.register_datatypes();
'''
Created on Apr 9, 2011

@author: octi
'''
path="C:/Users/octi/Documents/Mundy_Group/imagedata/westin/full/"
pathlist=os.listdir(path+"/out_pmap/");
model=loadModel(path+"/out_wv/model.mdl");
token="_th_";

[test_image,ni,nj]=openImage(path+"frames_0062.jpg");
truth_map=measure(model,test_image,"probability",0.15)
rate_list=[0.05, 0.075, 0.1,1, 2, 5, 10, 15, 20, 25, 32]; 
thresh=[0.1, 0.3, 0.5, 0.7 ,0.9] 
for j in rate_list:
    roc_path=path+"/out_stats/rate_"+str(j)+"_ROC.txt";
    if (os.path.exists(roc_path)):
        os.remove(roc_path)
    for i in thresh:
    #    if (i==0.1):
         [tmp_image,ni,nj]=openImage(path+"/out_pmap/rate_"+str(j)+token+str(i)+".tiff");
         evalROC(truth_map,tmp_image,0.8,roc_path);