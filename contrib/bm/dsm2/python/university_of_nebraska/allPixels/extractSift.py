'''
Author: Brandon A. Mayer
email: b.mayer1@gmail.com
Date: 11/8/2011

'''
import dsm2_batch
import config
import os
import time

class dbvalue:
    def __init__(self, index, type):
        self.id = index #unsigned integer
        self.type = type #string
        
def main():
    c = config.Config('lems')
    
    dsm2_batch.register_datatypes()
    dsm2_batch.register_processes()
    
    base_filename = "allPixels"
    
    result_dir = os.path.join(c.results_dir,base_filename)
    
    print'\nresult_dir = %s \n' % result_dir
    
    if not os.path.isdir(result_dir):
        os.makedirs(result_dir)
        
    video_glob = c.university_of_nebraska_60frames_grey
    print'\nvideo_glob = %s \n' % video_glob
    
    pixel_time_series_bin_128_file = os.path.join(result_dir,
                                                  'pixel_time_series_128.bin')
    
    print'\npixel_time_series_bin_128_file = %s' %\
        pixel_time_series_bin_128_file
        
    pixel_time_series_bin_d2_file = os.path.join(result_dir,
                                                 'pixel_time_series_d2.bin')
    
    print'\npixel_time_series_bin_d2_file = %s \n' %\
        pixel_time_series_bin_d2_file
        
    pixel_type = "unsigned"
    time_type = "unsigned"
    math_type = "double"
        
#    dsm2_batch.init_process("dtsPixelTimeSeriesExtractSiftVideoFrameProcess")
#    dsm2_batch.set_input_string(0,video_glob)
#    dsm2_batch.set_input_string(1,'unsigned')
#    dsm2_batch.run_process()
#    (id,type) = dsm2_batch.commit_output(0)
#    pixel_time_series_sptr = dbvalue(id,type)
    

#    dsm2_batch.init_process("dtsPixelTimeSeriesWriteBinProcess")
#    dsm2_batch.set_input_from_db(0,pixel_time_series_sptr)
#    dsm2_batch.set_input_string(1,pixel_time_series_bin_128_file)
#    dsm2_batch.run_process()

    dsm2_batch.init_process("dtsPixelTimeSeriesReadBinProcess")
    dsm2_batch.set_input_string(0,pixel_time_series_bin_128_file)
    dsm2_batch.set_input_string(1,pixel_type)
    dsm2_batch.set_input_string(2,time_type)
    dsm2_batch.set_input_string(3,math_type)
    dsm2_batch.set_input_unsigned(4,128)
    dsm2_batch.run_process()
    (id,type) = dsm2_batch.commit_output(0)
    pixel_time_series_sptr_128d = dbvalue(id,type)
    
    dsm2_batch.init_process("dtsPixelTimeSeriesPcaProcess")
    dsm2_batch.set_input_from_db(0,pixel_time_series_sptr_128d)
    dsm2_batch.set_input_string(1,pixel_type)
    dsm2_batch.set_input_string(2,time_type)
    dsm2_batch.set_input_string(3,math_type)
    dsm2_batch.set_input_unsigned(4,128)
    dsm2_batch.set_input_unsigned(5,2)
    dsm2_batch.run_process()
    (id,type) = dsm2_batch.commit_output(0)
    pixel_time_series_sptr_2d = dbvalue(id,type)

    dsm2_batch.init_process("dtsPixelTimeSeriesWriteBinProcess")
    dsm2_batch.set_input_from_db(0,pixel_time_series_sptr_2d)
    dsm2_batch.set_input_string(1,pixel_time_series_bin_d2_file)
    dsm2_batch.run_process()
    
    
if __name__ == '__main__':
    main()