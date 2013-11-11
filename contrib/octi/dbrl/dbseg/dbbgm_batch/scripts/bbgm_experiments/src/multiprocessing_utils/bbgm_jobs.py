'''
Created on Apr 8, 2011

@author: octi
'''
class bbgmMeasureProbWvLookupJob():
    def __init__(self,wavelet,attribute,tolerance,interp_functor,data_path,test_image_path,output_path,frameSuffix,threshold):
        self.wavelet=wavelet;
        self.test_image_path=test_image_path;
        self.attribute=attribute;
        self.tolerance=tolerance;
        self.interp_functor=interp_functor;
        self.data_path=data_path;
        self.output_path=output_path;
        self.frameSuffix=frameSuffix;
        self.threshold=threshold

class kduMeasureProbWvLookupJob():
    def __init__(self,args,model,attribute,tolerance,interp_functor,data_path,test_image_path,output_path,data_file_suffix,threshold):
        self.args=args;
        self.model=model;
        self.test_image_path=test_image_path;
        self.attribute=attribute;
        self.tolerance=tolerance;
        self.interp_functor=interp_functor;
        self.data_path=data_path;
        self.output_path=output_path;
        self.frameSuffix=data_file_suffix;
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
        
class kduCompressFrameJob():
    def __init__(self,probability_map_path,input_frame_path,threshold,kdu_args,mask_args,roi):
        self.probability_map_path=probability_map_path
        self.input_frame_path=input_frame_path
        self.threshold=threshold
        self.kdu_args=kdu_args
        self.mask_args=mask_args
        self.roi=roi