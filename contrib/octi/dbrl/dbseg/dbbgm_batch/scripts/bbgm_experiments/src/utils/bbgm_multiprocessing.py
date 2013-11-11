'''
Created on Nov 15, 2010

@author: octi
'''
import dbbgm_batch
import multiprocessing
import Queue 
from bbgm_utils import saveImg,dbvalue,openImage,frameSuffix,frameSuffixWrite,wvXform,measure,getSubband,GUI,displayImg
        
class GUIInvoker(multiprocessing.Process):
    def __init__(self,model,inv_model,fwd_model,pMap):
        self.mdl=model
        self.invmdl=inv_model
        self.fwd=fwd_model
        self.pmap=pMap
        multiprocessing.Process.__init__(self)
    def run(self):
        dbbgm_batch.init_process("bbgmInvokeGUIProcess");
        dbbgm_batch.set_input_from_db(0,self.mdl);
        dbbgm_batch.set_input_from_db(1,self.invmdl);
        dbbgm_batch.set_input_from_db(2,self.fwd);
        dbbgm_batch.set_input_from_db(3,self.pmap);
        dbbgm_batch.run_process();
        return []   

def executeJobs(jobs,type, num_procs=5):
    work_queue=multiprocessing.Queue();
    result_queue=multiprocessing.Queue();
    for job in jobs:
        work_queue.put(job)
    
    for i in range(num_procs):
        if (type=="measureProb"):
            worker= measureProbWorker(work_queue,result_queue)
        elif(type=="kduCompressDisplay"):
            worker= kduCompressDisplayWorker(work_queue,result_queue)
        elif (type=="kduExpandDisplay"):
            worker= kduExpandDisplayWorker(work_queue,result_queue)
        worker.start();
        print("worker with name ",worker.name," started!")
    
    
    
class bbgmJob():
    def __init__(self,wavelet,attribute,tolerance,interp_functor,data_path,input_path,output_path,frameSuffix,threshold):
        self.wavelet=wavelet;
        self.test_image_path=input_path;
        self.attribute=attribute;
        self.tolerance=tolerance;
        self.interp_functor=interp_functor;
        self.data_path=data_path;
        self.output_path=output_path;
        self.frameSuffix=frameSuffix;
        self.threshold=threshold

class kduExpandJob():
    def __init__(self,model,tolerance,args,image_path,out_path):
        self.model=model;
        self.image_path=image_path;
        self.tolerance=tolerance;
        self.args=args;
        self.out_path=out_path;

class kduCompressJob():
    def __init__(self,model,args):
        self.model=model;
        self.args=args;

        
class Worker(multiprocessing.Process):
    def __init__(self,work_queue,result_queue):
        multiprocessing.Process.__init__(self)
        # job management stuff
        self.work_queue = work_queue
        self.result_queue = result_queue
        self.kill_received=False;
              
class measureProbWorker(Worker):
 
    def __init__(self,work_queue,result_queue):
        # base class initialization
        Worker.__init__(self,work_queue,result_queue)
        # job management stuff
          
    def run(self):
        while not self.kill_received:
 
            # get a task
            try:
                job = self.work_queue.get_nowait()
            except Queue.Empty:
                break
            # store the result
            [test_image,ni,nj]=openImage(job.test_image_path);
            dbbgm_batch.init_process("bbgmMeasureWvLookupProcess");
            dbbgm_batch.set_input_from_db(0,job.wavelet);
            dbbgm_batch.set_input_from_db(1,test_image);
            dbbgm_batch.set_input_string(2,job.attribute);
            dbbgm_batch.set_input_float(3,job.tolerance);
            dbbgm_batch.set_input_string(4,job.interp_functor);
            dbbgm_batch.set_input_string(5,job.data_path);
            dbbgm_batch.set_input_string(6,job.frameSuffix);
            dbbgm_batch.set_input_float(7,job.threshold);
            dbbgm_batch.run_process();
            out_image=dbvalue(0,"")
            (out_image.id,out_image.type)=dbbgm_batch.commit_output(0);
            saveImg(out_image,job.output_path)   

class measureProbQueue(multiprocessing.Process):  
    def __init__(self,wavelet,attribute,tolerance,interp_functor,data_path,input_path,output_path,input_queue,frameSuffix,threshold):
        multiprocessing.Process.__init__(self)
        self.wavelet=wavelet;
        self.input_path=input_path
        self.attribute=attribute;
        self.tolerance=tolerance;
        self.interp_functor=interp_functor;
        self.data_path=data_path;
        self.input_queue=input_queue;
        self.output_path=output_path;
        self.kill_received=False
        self.frameSuffix=frameSuffix;
        self.threshold=threshold
   
    def run(self):
        while not self.kill_received:
            try:
                index = self.input_queue.get_nowait()
                
            except Queue.Empty:
                break
            # store the result
            [test_image,ni,nj]=openImage(self.input_path+frameSuffix(index,5)+".tiff");
            dbbgm_batch.init_process("bbgmMeasureWvLookupProcess");
            dbbgm_batch.set_input_from_db(0,self.wavelet);
            dbbgm_batch.set_input_from_db(1,test_image);
            dbbgm_batch.set_input_string(2,self.attribute);
            dbbgm_batch.set_input_float(3,self.tolerance);
            dbbgm_batch.set_input_string(4,self.interp_functor);
            dbbgm_batch.set_input_string(5,self.data_path);
            dbbgm_batch.set_input_string(6,self.frameSuffix);
            dbbgm_batch.set_input_float(7,self.threshold);
            dbbgm_batch.run_process();
            out_image=dbvalue(0,"")
            (out_image.id,out_image.type)=dbbgm_batch.commit_output(0)
            saveImg(out_image,self.output_path+frameSuffix(index,3)+".tiff")   

class kduExpandDisplayWorker(Worker):  
    def __init__(self,work_queue,result_queue):
        # base class initialization
         Worker.__init__(self,work_queue,result_queue)
        # job management stuff
        
    
    def run(self):
        while not self.kill_received:
 
            # get a task
            try:
                job = self.work_queue.get_nowait()
            except Queue.Empty:
                break
            # store the result
            [myKDUWavelet,waveletDomain3]=wvXform("bbgm_image_sptr",job.model,-1,5,3,job.args);
            mySubband=getSubband("bbgm_image_sptr",3,"forward",0,myKDUWavelet);
            [test_image,ni,nj]=openImage(job.image_path);
            inverse_map=measure(mySubband,test_image,"probability",job.tolerance)
            saveImg(inverse_map,job.out_path)
            Mean=displayImg(job.model,"mean",0,1);
            GUI(job.model,mySubband,0,inverse_map);

class kduCompressDisplayWorker(Worker):  
    def __init__(self,work_queue,result_queue):
        # base class initialization
         Worker.__init__(self,work_queue,result_queue)
        # job management stuff
            
    def run(self):
        while not self.kill_received:
 
            # get a task
            try:
                job = self.work_queue.get_nowait()
            except Queue.Empty:
                break
            # store the result
            [myKDUWavelet,waveletDomain3]=wvXform("bbgm_image_sptr",job.model,2,5,3,job.args);
            mySubband=getSubband("bbgm_image_sptr",3,"forward",0,myKDUWavelet);
            Mean=displayImg(job.model,"mean",0,1);
            GUI(job.model,0,mySubband,0);
