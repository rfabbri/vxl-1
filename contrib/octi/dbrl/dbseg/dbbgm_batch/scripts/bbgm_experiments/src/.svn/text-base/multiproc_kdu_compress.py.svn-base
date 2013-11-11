from utils.bbgm_utils import *
from multiprocessing_utils.bbgm_multiprocessing import measureProbQueue,GUIInvoker,executeJobs
from multiprocessing_utils.bbgm_jobs import *
from multiprocessing import Queue
import time
import multiprocessing

#path="C:/Users/octi/Documents/Mundy_Group/imagedata/westin/full/";
#command_args="KDU_X Clevels=5 Ckernels=W5X3 Qguard=2 -precise ";
#out_path="C:/Users/octi/Documents/Mundy_Group/imagedata/westin/full/out_compressed/";
foreground_args="KDU_X Clevels=5 Sprecision=8 Ssigned=no Creversible=yes  ";
ext=".tif"

path="C:/Users/octi/Documents/Mundy_Group/imagedata/Argus/argus_quant/registered/";
command_args="KDU_X Clevels=5 Ckernels=W5X3 Qguard=2 -precise ";
out_path=path+"/out_compressed/";
frame_num=20;
ImageList=[];
job_list=[];
t1=time.time();
work_queue=Queue();
num_cores=8;
dbbgm_batch.register_processes();
dbbgm_batch.register_datatypes();
#model=loadModel(path+"/out_wv/model.mdl");
#prob_map=measureWvLookup(myWavelet,test_image,"probability",0.15,"bil",path+"/out_stats/","frame"+str(1),0.5);   
#myWavelet=wvXform("bbgm_image_sptr",model,2,5,3);
#saveWavelet(path+"out_wv/wavelet.wv",myWavelet);
#model=updateImageStream(path,300,0.1,0.02,0,-1);
#saveModel(path+"/out_wv/model.mdl",model);

 
if __name__=="__main__": #you really need this if you want to do multiprocessing. Otherwise unthinkable, evil things will occur on your machine 
    #There should be only one example running at a time. Uncomment the block to run it.
   


######===============================================================================================================================###### 
    
    #Example 2: The more general/Object-Oriented way to do multiprocessing; Each thread contains an input and an output Queue. The input Queue contains Job objects that have the experiment
    #parameters. We create the Job objects, Enqueue them in the input queue and then start each process. The output Queue is empty for this particular example.   
    lo=0.5;
    inc=0.5;
    rate_list=[0.05, 0.075, 0.1,1, 2, 5, 10, 15, 20, 25, 32];  
#    for i in rate_list:
#        base_args=command_args;
#        base_args+="-rate "+str(i)+" "
#        base_args+="-o "+path+"/out_j2c/out_model_"+str(i)+".j2c "
#        current_job=kduCompressJob(model,base_args) 
#        job_list.append(current_job)
#        lo+=inc
#    executeJobs(job_list,"kduCompress",num_cores)  
 
    for i in range(1,99):
        base_args=foreground_args
        base_args+=" -o "+out_path+"/compressed_"+frameSuffix(i,5)+".j2c"
        
        current_job=kduCompressFrameJob(path+"/out_pmap/pmap_"+frameSuffix(i,5)+".tiff",
                                       path+"/reg"+frameSuffix(i,5)+ext,0.8,base_args,"",0)
        #      
        job_list.append(current_job)
    
    executeJobs(job_list,"kduCompressFrameJob",8)  

