'''
Author: Brandon A. Mayer
email: b.mayer1@gmail.com
Date: 10/20/2011

'''
import dsm2_batch
import config
import os 
import time

dsm2_batch.register_datatypes();
dsm2_batch.register_processes();

class dbvalue:
    def __init__(self, index, type):
        self.id = index #unsigned integer
        self.type = type #string
        
def main():
    c = config.Config('lems')
    img_dir = c.datasets_dir + '/university_of_nebraska'  +'/grey'
    video_glob = img_dir +'/*.tif'
    
    result_dir = (os.path.
    normpath(os.path.join(c.results_dir,
    os.path.basename(os.getcwd()) ) ) 
    )
    
    pixel_time_series_bin_file = 'E:\\research\\changeDetection\\results\\box_x110_y187_to_x120_y208\\test_ts_sptr.bin'
    
    
    
    xmin = 110
    xmax = 187
    ymin = 120
    ymax = 208
    
    print'Extracting sift feature around box:'
    print'xmin = %d' % xmin
    print'xmax = %d' % xmax
    print'ymin = %d' % ymin
    print'ymax = %d\n' % ymax
    
    dsm2_batch.init_process("dtsPixelTimeSeriesCreateSptrProcess")
    dsm2_batch.set_input_string(0,"unsigned")
    dsm2_batch.set_input_string(1,"unsigned")
    dsm2_batch.set_input_string(2,"double")
    dsm2_batch.set_input_unsigned(3,128)
    dsm2_batch.run_process()
    (id,type)=dsm2_batch.commit_output(0)
    pixel_time_series_sptr=dbvalue(id,type)
#    dsm2_batch.init_process("dtsExtractSiftVideoBoxProcess")
#    dsm2_batch.set_input_string(0,video_glob)
#    dsm2_batch.set_input_string(1,"unsigned")
#    dsm2_batch.set_input_unsigned(2,xmin)
#    dsm2_batch.set_input_unsigned(3,xmax)
#    dsm2_batch.set_input_unsigned(4,ymin)
#    dsm2_batch.set_input_unsigned(5,ymax)
#    dsm2_batch.run_process()
#    (id,type)=dsm2_batch.commit_output(0)
#    pixel_time_series_sptr = dbvalue(id,type)
#    
#    
    dsm2_batch.init_process("dtsPixelTimeSeriesWriteBinProcess")
    dsm2_batch.set_input_from_db(0,pixel_time_series_sptr)
    dsm2_batch.set_input_string(1,pixel_time_series_bin_file)
    dsm2_batch.run_process()
    
    
    
if __name__ == '__main__':
    main()    
    