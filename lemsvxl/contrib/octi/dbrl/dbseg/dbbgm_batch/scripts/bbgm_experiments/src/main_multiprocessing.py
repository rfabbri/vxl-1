from utils.bbgm_utils import *
from utils.bbgm_multiprocessing import measureProbQueue,bbgmJob,executeJobs,GUIInvoker
from multiprocessing import Queue
import time
import multiprocessing

def interlude(path):
    
    return myWavelet
path="C:/Users/octi/Documents/Mundy_Group/imagedata/tree_sequence/";
frame_num=20;
ImageList=[];
job_list=[];
t1=time.time();
work_queue=Queue();
num_cores=8;
dbbgm_batch.register_processes();
dbbgm_batch.register_datatypes();
myWavelet=loadWavelet(path+"out_wv/wavelet.wv");
[test_image,ni,nj]=openImage(path+"00001.tiff");
model=loadModel(path+"/out_wv/model.mdl");
#prob_map=measureWvLookup(myWavelet,test_image,"probability",0.15,"bil",path+"/out_stats/","frame"+str(1),0.5);   
#myWavelet=wvXform("bbgm_image_sptr",model,2,5,3);
#saveWavelet(path+"out_wv/wavelet.wv",myWavelet);
#model=updateImageStream(path,300,0.1,0.02,0,-1);
#saveModel(path+"/out_wv/model.mdl",model);

#mySubband=getSubband("bbgm_image_sptr",3,"LL",2,myWavelet);
#inverse=getSubband("bbgm_image_sptr",3,"LL",0,myWavelet);
#waveletDomain=getSubband("bbgm_image_sptr",3,"forward",0,myWavelet);
#Mean=displayImg(model,"mean",0,1);
#Mean2=displayImg(model,"mean",1,1);
#saveImg(Mean,path+"/out_stats/mean.jpg");
#saveImg(Mean2,path+"/out_stats/mean2.jpg");

#for x in range(1,frame_num):
#    [current_im,cur_ni,cur_nj]=openImage(path+frameSuffix(x,5)+".tiff");
#    ImageList.append(current_im);
 
if __name__=="__main__": #you really need this if you want to do multiprocessing. Otherwise unthinkable, evil things will occur on your machine 
    #There should be only one example running at a time. Uncomment the block to run it.
 
    #dbbgm_batch.print_db();
   
    

    
######===============================================================================================================================######    
    #Example 1: The easy way to do multiprocessing; Each thread contains the same parameters of the 
    #experiment and only the frame index varies. We first enqueue all the frame indices in a queue
    #initialize each process with the experiment parameters, and start it
   
   
#    for i in range(1,frame_num):
#        work_queue.put(i)
#    
#    for i in range(num_cores):
#        current_process=measureProbQueue(myWavelet,"probability",0.15,"bil",path+"/out_stats/",path,path+"/out_pmap/",work_queue,frameSuffix(i,3),0.5)
#        current_process.start()

######===============================================================================================================================###### 
    
    #Example 2: The more general/Object-Oriented way to do multiprocessing; Each thread contains an input and an output Queue. The input Queue contains Job objects that have the experiment
    #parameters. We create the Job objects, Enqueue them in the input queue and then start each process. The output Queue is empty for this particular example.   
    
#    for i in range(1,frame_num):
#        current_job=bbgmJob(myWavelet,"probability",0.15,"bil",path+"/out_stats/",path+frameSuffix(i,5)+".tiff",path+"/out_pmap/"+frameSuffix(i,3)+".tiff",frameSuffix(i,3),0.5) 
#        job_list.append(current_job)
#    executeJobs(job_list,num_cores,"measureProb")  

######===============================================================================================================================###### 
    
    ##Example 3: Load three different GUI windows (possibly with different tableau's in each one)
#    GUIInvoker(model,mySubband,waveletDomain,prob_map).start();  
#    GUIInvoker(model,inverse,waveletDomain,0).start(); 
#    GUIInvoker(model,0,waveletDomain,0).start();   
#   
#    dbbgm_batch.print_db(); 
#print dbbgm_batch.remove_data(model.id);

   

    numPts=20;
    hi=0.05;
    lo=1
    increment=float((hi-lo)/numPts)
    s=lo;
    for i in range(1,numPts):
        current_job=bbgmJob(myWavelet,"probability",0.15,"bil",path+"/out_stats/",path+frameSuffix(1,5)+".tiff",path+"/out_ROC/frame_1_thresh"+str(s)+".tiff",frameSuffix(1,3),s) 
        job_list.append(current_job)
        s=s+increment
    executeJobs(job_list,num_cores,"measureProb") 
