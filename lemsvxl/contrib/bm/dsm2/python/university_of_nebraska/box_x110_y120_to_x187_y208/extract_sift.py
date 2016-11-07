'''
Author: Brandon A. Mayer
email: b.mayer1@gmail.com
Date: 10/6/2011

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



    
    xmin = 110
    xmax = 187
    ymin = 120
    ymax = 208
    ndims_to_keep = 128
    
    base_filename='box_x%d_y%d_to_x%d_y%d' % (xmin,xmax,ymin,ymax)
    
#    result_dir = (os.path.
#    normpath(os.path.join(c.results_dir,
#    os.path.basename(os.getcwd()) ) ) 
#    )
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
    
    print'\npixel_time_series_bin_d2_file = %s' %\
        pixel_time_series_bin_d2_file
    
    print'Extracting sift feature around box:'
    print'xmin = %d' % xmin
    print'xmax = %d' % xmax
    print'ymin = %d' % ymin
    print'ymax = %d\n' % ymax
    print'ndims_to_keep = % d\n' % ndims_to_keep
    
    dsm2_batch.init_process("dtsPixelTimeSeriesExtractSiftVideoPcaProcess")
    dsm2_batch.set_input_string(0,video_glob)
    dsm2_batch.set_input_string(1,"unsigned")
    dsm2_batch.set_input_unsigned(2,2)
    dsm2_batch.set_input_unsigned(3,xmin)
    dsm2_batch.set_input_unsigned(4,xmax)
    dsm2_batch.set_input_unsigned(5,ymin)
    dsm2_batch.set_input_unsigned(6,ymax)
    dsm2_batch.run_process()
    (id,type) = dsm2_batch.commit_output(0)
    pixel_time_series_sptr = dbvalue(id,type)
    
    dsm2_batch.init_process("dtsPixelTimeSeriesWriteBinProcess")
    dsm2_batch.set_input_from_db(0,pixel_time_series_sptr)
    dsm2_batch.set_input_string(1,pixel_time_series_bin_d2_file)
    dsm2_batch.run_process()
    

if __name__ == '__main__':
    main()
