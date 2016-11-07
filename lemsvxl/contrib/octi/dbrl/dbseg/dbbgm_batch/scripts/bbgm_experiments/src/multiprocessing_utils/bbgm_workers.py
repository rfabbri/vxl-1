'''
Created on Apr 8, 2011

@author: octi
'''
import dbbgm_batch
import multiprocessing
import Queue 
from utils.bbgm_utils import saveImg,dbvalue,openImage,frameSuffix,frameSuffixWrite,wvXform,measure,getSubband,GUI,displayImg
class Worker(multiprocessing.Process):
    def __init__(self,work_queue,result_queue):
        multiprocessing.Process.__init__(self)
        # job management stuff
        self.work_queue = work_queue
        self.result_queue = result_queue
        self.kill_received=False;

class kduExpandWorker(Worker):  
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
            myKDUWavelet=wvXform("bbgm_image_sptr",job.model,-1,5,3,job.args);
#            mySubband=getSubband("bbgm_image_sptr",3,"forward",0,myKDUWavelet);
            [test_image,ni,nj]=openImage(job.image_path);
            
#            inverse_map=measure(mySubband,test_image,"probability",job.tolerance)
#            saveImg(inverse_map,job.out_path)
#            Mean=displayImg(job.model,"mean",0,1);
#            GUI(job.model,mySubband,0,inverse_map);

class kduCompressWorker(Worker):  
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
            myKDUWavelet=wvXform("bbgm_image_sptr",job.model,2,5,3,job.args);
#            mySubband=getSubband("bbgm_image_sptr",3,"forward",0,myKDUWavelet);
#            Mean=displayImg(job.model,"mean",0,1);
#            GUI(job.model,0,mySubband,0);        
              
class measureProbWvLookupWorker(Worker):
 
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
            dbbgm_batch.print_db()
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



class kduMeasureProbWvLookupWorker(Worker):  
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
            [myKDUWavelet,forwardm]=wvXform("bbgm_image_sptr",job.model,-1,5,3,job.args);
            [test_image,ni,nj]=openImage(job.test_image_path);
            dbbgm_batch.init_process("bbgmMeasureWvLookupProcess");
            dbbgm_batch.set_input_from_db(0,myKDUWavelet);
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
            self.result_queue.put_nowait(out_image)
            saveImg(out_image,job.output_path)  
            dbbgm_batch.remove_data(test_image.id);
            dbbgm_batch.remove_data(myKDUWavelet.id)
            dbbgm_batch.remove_data(forwardm.id) 

class kduCompressFrameWorker(Worker):  
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
            [probability_map,nii,njj]=openImage(job.probability_map_path);
            [test_image,ni,nj]=openImage(job.input_frame_path);
            dbbgm_batch.init_process("bbgmThreshAndEncodeProcess");
            dbbgm_batch.set_input_from_db(0,probability_map)
            dbbgm_batch.set_input_from_db(1,test_image)
            dbbgm_batch.set_input_float(2,job.threshold)
            dbbgm_batch.set_input_string(3,job.kdu_args)
            dbbgm_batch.set_input_int(4,job.roi)
            dbbgm_batch.set_input_string(5,job.mask_args)
            dbbgm_batch.run_process()
            dbbgm_batch.remove_data(probability_map.id)
            dbbgm_batch.remove_data(test_image.id)