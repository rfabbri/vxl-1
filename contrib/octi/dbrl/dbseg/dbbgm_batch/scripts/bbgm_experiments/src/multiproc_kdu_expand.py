from utils.bbgm_utils import *
from multiprocessing_utils.bbgm_multiprocessing import measureProbQueue,GUIInvoker,executeJobs
from multiprocessing_utils.bbgm_jobs import *
#from mutliprocessing_utils.bbgm_workers import *
from multiprocessing import Queue
import time
import multiprocessing

#path="C:/Users/octi/Documents/Mundy_Group/imagedata/westin/full/";
path="C:/Users/octi/Documents/Mundy_Group/imagedata/Argus/argus_quant/registered/";
command_args="KDU_IX -quiet -precise "; 
ext=".tif"
frame_num=14;
ImageList=[];
job_list=[];
t1=time.time();
work_queue=Queue();
num_cores=8;
dbbgm_batch.register_processes();
dbbgm_batch.register_datatypes();
model=loadModel(path+"/out_wv/model.mdl");
#myWavelet=loadWavelet(path+"out_wv/wavelet.wv");
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
#    base_args=command_args
#    base_args+="-i "+path+"/out_j2c/out_model_"+str(11)+".j2c"
#    myKDUWavelet=wvXform("bbgm_image_sptr",model,-1,5,3,base_args);
#    dbbgm_batch.print_db() 
#    saveWavelet(path+"out_wv/wavelet.wv",myKDUWavelet);
    rate_list=[0.05, 0.075, 0.1,1, 2, 5, 10, 15, 20, 25, 32]  
    thresh=[0.8]
    for j in thresh:
        job_list=[];
        for i in range(0,99):
            base_args=command_args
            base_args+="-i "+path+"/out_j2c/model_5bps.j2c"
            
    #        current_job=kduMeasureProbWvLookupJob(base_args,model,"probability",0.15,"bil",path+"/out_stats/",path+"frames_0091"+ext,
    #                                               path+"/out_pmap/"+frameSuffix(i,2)+".tiff",frameSuffix(i,2),0.5)
            current_job=kduMeasureProbWvLookupJob(base_args,model,"probability",0.2,"bil",path+"/out_stats/",path+"reg"+frameSuffix(i,5)+ext,
                                                   path+"/out_pmap/pmap_"+frameSuffix(i,5)+".tiff","frame"+frameSuffix(i,5),j)
    #        current_job=kduExpandJob(model,0.15,base_args,path+"frames_0091"+ext,
    #                                 path+"/out_pmap/"+frameSuffix(i,2)+".tiff")
            job_list.append(current_job)
            
        executeJobs(job_list,"kduMeasureProbWvLookup",8)  
#    executeJobs(job_list,"bbgmMeasureProbWvLookup",2)  

   

   
