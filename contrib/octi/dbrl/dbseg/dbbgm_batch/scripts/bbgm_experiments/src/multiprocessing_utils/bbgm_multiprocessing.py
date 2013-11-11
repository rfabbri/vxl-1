'''
Created on Nov 15, 2010

@author: octi
'''
import dbbgm_batch
import multiprocessing
import Queue 
from utils.bbgm_utils import saveImg,dbvalue,openImage,frameSuffix,frameSuffixWrite,wvXform,measure,getSubband,GUI,displayImg
from bbgm_workers import *        
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
    worker_list=[];
    for job in jobs:
        work_queue.put(job)
    
    for i in range(num_procs):
        if (type=="bbgmMeasureProbWvLookup"):
            worker= measureProbWvLookupWorker(work_queue,result_queue)
        elif(type=="kduCompress"):
            worker= kduCompressWorker(work_queue,result_queue)
        elif (type=="kduExpand"):
            worker= kduExpandWorker(work_queue,result_queue)
        elif (type=="kduMeasureProbWvLookup"):
            worker= kduMeasureProbWvLookupWorker(work_queue,result_queue)
        elif (type=="kduCompressFrameJob"):
            worker= kduCompressFrameWorker(work_queue,result_queue)
        worker_list.append(worker)
        worker.start();
        print("worker with name ",worker.name," started!")
    
    for i in worker_list:
        i.join(); 
        print("worker with name ",worker.name," done!")
          


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


