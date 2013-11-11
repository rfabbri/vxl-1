'''
Created on May 22, 2011

@author: bm
'''
import glob;
import os;
import dsm_batch;
dsm_batch.register_processes();
dsm_batch.register_datatypes();

class dbvalue:
  def __init__(self, index, type):
    self.id = index    # unsigned integer
    self.type = type   # string

#this function will extract the intensity of a given pixel location and return a 
# pixel/time series map.
# pixels indicating image coordinates of the form:
# pixels = [ [x1, x2, x3, ... , xn],
#          [ [y1, y2, y3, ... , yn] ]

def remove_data(sptr_id):
    dsm_batch.remove_data(sptr_id);
    
def extract_intensity_time_series(pixels, image_sequence_glob):
    print "Extracting Intensity Time Series"
    
    x_keys = pixels[0];
    y_keys = pixels[1];
    
    print "Creating dsm_pixel_time_series_map_sptr"
    dsm_batch.init_process("dsmCreatePixelTimeSeriesMapSptrProcess");
    dsm_batch.run_process();
    (id,type)=dsm_batch.commit_output(0);
    pixel_time_series_map_sptr = dbvalue(id,type);
    
    time_step=0;
    for frame in glob.glob(image_sequence_glob):
        frame = os.path.abspath(frame);
        
        print "----- Processing Frame %s -----" % time_step
        
        print "\t Loading Image %s" % os.path.abspath(frame);
        dsm_batch.init_process("vilLoadImageViewProcess")
        dsm_batch.set_input_string(0,frame);
        dsm_batch.run_process();
        (id,type)=dsm_batch.commit_output(0);
        img_view_sptr=dbvalue(id,type);    
        
        for key_idx in range(len(x_keys)):
            print "Extracting intensity dsift_feature_sptr at location: (%d,%d)" % (x_keys[key_idx], y_keys[key_idx])
            dsm_batch.init_process("dsmExtractIntensityFeatureProcess")
            dsm_batch.set_input_from_db(0,img_view_sptr);
            dsm_batch.set_input_unsigned(1,x_keys[key_idx]);
            dsm_batch.set_input_unsigned(2,y_keys[key_idx]);
            dsm_batch.set_input_unsigned(3, time_step);
            dsm_batch.run_process();
            (id,type)=dsm_batch.commit_output(0);
            dsm_feature_sptr=dbvalue(id,type);
            
            print "Inserting feature into dsm_pixel_time_series_map"
            dsm_batch.init_process("dsmPixelTimeSeriesMapInsertFeatureProcess");
            dsm_batch.set_input_from_db(0,pixel_time_series_map_sptr);
            dsm_batch.set_input_unsigned(1,x_keys[key_idx]);
            dsm_batch.set_input_unsigned(2,y_keys[key_idx]);
            dsm_batch.set_input_unsigned(3,time_step);
            dsm_batch.set_input_from_db(4, dsm_feature_sptr);
            
        dsm_batch.remove_data(img_view_sptr.id);
        
        return pixel_time_series_map_sptr; 
    
# pixels indicating image coordinates of the form:
# pixels = [ [x1, x2, x3, ... , xn],
#          [ [y1, y2, y3, ... , yn] ]
def extract_sift_time_series(pixels, image_sequence_glob):    
    x_keys = pixels[0];
    y_keys = pixels[1];
       
    print("Creating dsm_pixel_time_series_map_sptr")
    dsm_batch.init_process("dsmCreatePixelTimeSeriesMapSptrProcess");
    dsm_batch.set_input_unsigned(0,128)
    dsm_batch.run_process();
    (id,type)=dsm_batch.commit_output(0);
    pixel_time_series_map_sptr = dbvalue(id,type);
    
    time_step=0;
    for frame in glob.glob(image_sequence_glob):
        frame = os.path.abspath(frame);
        print "----- Processing Frame %s -----" % time_step
                   
        print "\t Loading Image %s" % os.path.abspath(frame);
        dsm_batch.init_process("vilLoadImageViewProcess")
        dsm_batch.set_input_string(0,frame);
        dsm_batch.run_process();
        (id,type)=dsm_batch.commit_output(0);
        img_view_sptr=dbvalue(id,type);
        
        print("Creating bapl_dsift_sptr")
        dsm_batch.init_process("baplDsiftSptrProcess");
        dsm_batch.set_input_from_db(0,img_view_sptr);
        dsm_batch.run_process();
        (id,type)=dsm_batch.commit_output(0);
        bapl_dsift_sptr=dbvalue(id,type);
        
        
        for key_idx in range(len(x_keys)):
            print "Extracting dsift_feature_sptr at location: (%d,%d)" % (x_keys[key_idx],y_keys[key_idx])
            dsm_batch.init_process("dsmExtractBaplDsiftFeatureProcess");
            dsm_batch.set_input_from_db(0,bapl_dsift_sptr);
            dsm_batch.set_input_unsigned(1,x_keys[key_idx]);
            dsm_batch.set_input_unsigned(2,y_keys[key_idx]);
            dsm_batch.set_input_unsigned(3,time_step);
            dsm_batch.run_process();
            (id,type)=dsm_batch.commit_output(0);
            dsm_feature_sptr=dbvalue(id,type);
        
            print("Inserting feature into dsm_pixel_time_series_map")
            dsm_batch.init_process("dsmPixelTimeSeriesMapInsertFeatureProcess")
            dsm_batch.set_input_from_db(0,pixel_time_series_map_sptr);
            dsm_batch.set_input_unsigned(1,x_keys[key_idx]);
            dsm_batch.set_input_unsigned(2,y_keys[key_idx]);
            dsm_batch.set_input_unsigned(3,time_step);
            dsm_batch.set_input_from_db(4,dsm_feature_sptr);
            dsm_batch.run_process();
        
        dsm_batch.remove_data(img_view_sptr.id);
        dsm_batch.remove_data(bapl_dsift_sptr.id);
        time_step = time_step + 1;
        
    return pixel_time_series_map_sptr;

def extract_sift_time_series_box(xmin,xmax,ymin,ymax,image_glob):
    print "Extracting Sift Time Series From Box:"
    #print "\t [xmin,xmax,ymin,ymax]: [%d,%d,%d,%d]" % (xmin,xmax,ymin,ymax)
    
    dsm_batch.init_process("dsmExtractBaplDsiftTimeSeriesBoxProcess")
    dsm_batch.set_input_unsigned(0,xmin);
    dsm_batch.set_input_unsigned(1,xmax);
    dsm_batch.set_input_unsigned(2,ymin);
    dsm_batch.set_input_unsigned(3,ymax);
    dsm_batch.set_input_string(4,image_glob);
    dsm_batch.run_process()
    (id,type)=dsm_batch.commit_output(0);
    pixel_time_series_map_sptr=dbvalue(id,type)
    return pixel_time_series_map_sptr;
#    print "Creating dsm_pixel_time_series_map_sptr"
#    dsm_batch.init_process("dsmCreatePixelTimeSeriesMapSptrProcess");
#    dsm_batch.run_process();
#    (id,type)=dsm_batch.commit_output(0);
#    pixel_time_series_map_sptr=dbvalue(id,type);
#    
#    time_step = 0;
#    for frame in glob.glob(image_glob):
#        frame = os.path.abspath(frame);
#        
#        print"---- Processing Frame %d ----" % time_step;
#        
#        print"\t Loading Image %s " % os.path.abspath(frame);
#        dsm_batch.init_process("vilLoadImageViewProcess")
#        dsm_batch.set_input_string(0,frame);
#        dsm_batch.run_process();
#        (id,type)=dsm_batch.commit_output(0);
#        img_view_sptr=dbvalue(id,type);
#        
#        print("Creating bapl_dsift_sptr")
#        dsm_batch.init_process("baplDsiftSptrProcess");
#        dsm_batch.set_input_from_db(0,img_view_sptr);
#        dsm_batch.run_process();
#        (id,type)=dsm_batch.commit_output(0);
#        bapl_dsift_sptr=dbvalue(id,type);
#        
#        for x in range(xmin,xmax+1):
#            for y in range(ymin,ymax+1):
#                print "Extracting dsift_feature_sptr at location: (%d,%d)" % (x,y)
#                dsm_batch.init_process("dsmExtractBaplDsiftFeatureProcess");
#                dsm_batch.set_input_from_db(0,bapl_dsift_sptr);
#                dsm_batch.set_input_unsigned(1,x);
#                dsm_batch.set_input_unsigned(2,y);
#                dsm_batch.set_input_unsigned(3,time_step);
#                dsm_batch.run_process();
#                (id,type)=dsm_batch.commit_output(0);
#                dsm_feature_sptr=dbvalue(id,type);
#                
#                print("Inserting Feature into dsm_pixel_time_series_map");
#                dsm_batch.init_process("dsmPixelTimeSeriesMapInsertFeatureProcess");
#                dsm_batch.set_input_from_db(0,pixel_time_series_map_sptr);
#                dsm_batch.set_input_unsigned(1,x);
#                dsm_batch.set_input_unsigned(2,y);
#                dsm_batch.set_input_unsigned(3,time_step);
#                dsm_batch.set_input_from_db(4,dsm_feature_sptr);
#                dsm_batch.run_process();
#                
#        #end pixel iterations
#
#        dsm_batch.remove_data(img_view_sptr.id);
#        dsm_batch.remove_data(bapl_dsift_sptr.id);
#                
#        time_step=time_step+1;
#        
#        #end image iterations
#    
#    return pixel_time_series_map_sptr;
                
        
          
def reduce_pixel_time_series_map_dimension(pixel_time_series_map_sptr, ndims_to_keep=2):
    print "Reducing Pixel Time Series Map Feature Dimension to %d" % ndims_to_keep;
    dsm_batch.init_process("dsmReducePixelTimeSeriesMapDimensionProcess");
    dsm_batch.set_input_from_db(0,pixel_time_series_map_sptr);
    dsm_batch.set_input_unsigned(1,ndims_to_keep);
    dsm_batch.run_process();
    (id,type)=dsm_batch.commit_output(0);
    reduced_pixel_time_series_map_sptr = dbvalue(id,type);
    return reduced_pixel_time_series_map_sptr;
    
    

def write_pixel_time_series_map_sptr_dat(pixel_time_series_map_sptr, filename):
    print "Writing Pixel Time Series Map to %s" % filename
    filename = os.path.abspath(filename);
    dsm_batch.init_process("dsmWritePixelTimeSeriesMapDatProcess")
    dsm_batch.set_input_string(0,filename);
    dsm_batch.set_input_from_db(1,pixel_time_series_map_sptr);
    dsm_batch.run_process();
    
def write_pixel_time_series_map_sptr_bin(pixel_time_series_map_sptr, filename):
    print "Writing Pixel Time Series Map to %s" % filename
    filename = os.path.abspath(filename);
    dsm_batch.init_process("dsmWritePixelTimeSeriesMapBinProcess")
    dsm_batch.set_input_string(0,filename);
    dsm_batch.set_input_from_db(1,pixel_time_series_map_sptr);
    dsm_batch.run_process();    
    
def read_pixel_time_series_map_bin(filename):
    print "Reading Pixel Time Series Map from %s" % filename
    filename = os.path.abspath(filename);
    dsm_batch.init_process("dsmReadPixelTimeSeriesMapBinProcess");
    dsm_batch.set_input_string(0,filename);
    dsm_batch.run_process();
    (id,type) = dsm_batch.commit_output(0);
    pixel_time_series_map_sptr = dbvalue(id,type);
    return pixel_time_series_map_sptr;

#this function will return a dsm_ncn_sptr 
# pixels indicating image coordinates of the form:
# pixels = [ [x1, x2, x3, ... , xn],
#          [ [y1, y2, y3, ... , yn] ] 
def dsm_ncn_sptr(pixels, image_sequence_glob, num_neighbors=10,num_pivot_pixels=2000, num_particles=10000):
    print "Creating dsm_ncn_sptr"
    dsm_batch.init_process("dsmNcnSptrProcess");
    dsm_batch.run_process();
    (id,type)=dsm_batch.commit_output(0);
    dsm_ncn_sptr=dbvalue(id,type);
    
    dsm_batch.init_process("dsmNcnSetVideoGlobProcess");
    dsm_batch.set_input_from_db(0,dsm_ncn_sptr);
    dsm_batch.set_input_string(1,image_sequence_glob);
    dsm_batch.run_process();
    
    dsm_batch.init_process("dsmNcnSetNumNeighborsProcess");
    dsm_batch.set_input_from_db(0,dsm_ncn_sptr);
    dsm_batch.set_input_unsigned(1,num_neighbors);
    dsm_batch.run_process();
    
    dsm_batch.init_process("dsmNcnSetNumPivotPixelsProcess");
    dsm_batch.set_input_from_db(0,dsm_ncn_sptr);
    dsm_batch.set_input_unsigned(1,num_pivot_pixels);
    dsm_batch.run_process();
    
    dsm_batch.init_process("dsmNcnSetNumParticlesProcess")
    dsm_batch.set_input_from_db(0,dsm_ncn_sptr);
    dsm_batch.set_input_unsigned(1,num_particles);
    dsm_batch.run_process();
    
    key_x = pixels[0];
    key_y = pixels[1];
    
    for key_idx in range(len(key_x)):
        dsm_batch.init_process("dsmNcnAddTargetProcess");
        dsm_batch.set_input_from_db(0,dsm_ncn_sptr);
        dsm_batch.set_input_unsigned(1,key_x[key_idx]);
        dsm_batch.set_input_unsigned(2,key_y[key_idx]);
        dsm_batch.run_process();
        
    return dsm_ncn_sptr;

def write_ncn_sptr_bin(dsm_ncn_sptr, filename):
    dsm_batch.init_process("dsmWriteNcnSptrBinProcess");
    dsm_batch.set_input_string(0,filename);
    dsm_batch.set_input_from_db(1,dsm_ncn_sptr);
    dsm_batch.run_process();
    
def read_ncn_sptr_bin(filename):
    dsm_batch.init_process("dsmReadNcnSptrBinProcess");
    dsm_batch.set_input_string(0,filename);
    dsm_batch.run_process();
    (id,type)=dsm_batch.commit_output(0);
    ncn_sptr=dbvalue(id,type);
    return ncn_sptr;  

def ncn_write_neighborhood_dat(ncn_sptr, filename):
    dsm_batch.init_process("dsmNcnWriteNeighborhoodDatProcess");
    dsm_batch.set_input_from_db(0,ncn_sptr);
    dsm_batch.set_input_string(1,filename);
    dsm_batch.run_process();
    
def build_neighborhood(ncn_sptr):
    dsm_batch.init_process("dsmBuildNcnProcess");
    dsm_batch.set_input_from_db(0,ncn_sptr);
    dsm_batch.run_process();
    
def ncn_calculate_temporal_entropy(ncn_sptr, nbins=16):
    dsm_batch.init_process("dsmNcnCalculateTemporalEntropyProcess");
    dsm_batch.set_input_from_db(0,ncn_sptr);
    dsm_batch.set_input_unsigned(1,nbins);
    dsm_batch.run_process();

def ncn_write_temporal_entropy_bin(ncn_sptr,filename):
    dsm_batch.init_process("dsmWriteTemporalEntropyBinProcess");
    dsm_batch.set_input_string(0,filename);
    dsm_batch.set_input_from_db(1,ncn_sptr);
    dsm_batch.run_process();
    
def ncn_read_temporal_entropy_bin(ncn_sptr,filename):
    dsm_batch.init_process("dsmReadTemporalEntropyBinProcess");
    dsm_batch.set_input_string(0,filename);
    dsm_batch.set_input_from_db(1,ncn_sptr);
    dsm_batch.run_process();
    
def ncn_write_temporal_entropy_dat(ncn_sptr,filename):
    dsm_batch.init_process("dsmWriteTemporalEntropyDatProcess");
    dsm_batch.set_input_string(0,filename);
    dsm_batch.set_input_from_db(1,ncn_sptr);
    dsm_batch.run_process();    
    
def ncn_write_pivot_pixel_candidates_dat(ncn_sptr, filename):
    dsm_batch.init_process("dsmNcnWritePivotPixelCandidatesDatProcess");
    dsm_batch.set_input_from_db(0,ncn_sptr);
    dsm_batch.set_input_string(1,filename);
    dsm_batch.run_process(); 
    
    
def target_neighborhood_map_sptr_from_ncn(ncn_sptr):
    dsm_batch.init_process("dsmTargetNeighborhoodMapFromNcnSptrProcess")
    dsm_batch.set_input_from_db(0,ncn_sptr);
    dsm_batch.run_process();
    (id,type)=dsm_batch.commit_output(0);
    target_neighborhood_map_sptr=dbvalue(id,type);
    return target_neighborhood_map_sptr;  

def target_neighborhood_map_write_dat(target_neighborhood_map_sptr, filename):
    dsm_batch.init_process("dsmTargetNeighborhoodMapWriteDatProcess");
    dsm_batch.set_input_string(0,filename);
    dsm_batch.set_input_from_db(1,target_neighborhood_map_sptr);
    dsm_batch.run_process();

def target_neighborhood_map_write_bin(target_neighborhood_map_sptr, filename):    
    dsm_batch.init_process("dsmTargetNeighborhoodMapWriteBinProcess");
    dsm_batch.set_input_string(0,filename);
    dsm_batch.set_input_from_db(1,target_neighborhood_map_sptr);
    dsm_batch.run_process();
    
def target_neighborhood_map_read_bin(filename):
    dsm_batch.init_process("dsmTargetNeighborhoodMapReadBinProcess");
    dsm_batch.set_input_string(0,filename);
    dsm_batch.run_process();
    (id,type)=dsm_batch.commit_output(0);
    target_neighborhood_map_sptr=dbvalue(id,type);
    return target_neighborhood_map_sptr;  

def dsm_extract_intensity_ratio_feature(dsm_target_neighborhood_map_sptr, target_x, target_y, time, vil_image_view_sptr):
    dsm_batch.init_process("dsmExtractIntensityRatioFeatureProcess");
    dsm_batch.set_input_from_db(0,dsm_target_neighborhood_map_sptr);
    dsm_batch.set_input_unsigned(1,target_x);
    dsm_batch.set_input_unsigned(2,target_y);
    dsm_batch.set_input_unsigned(3,time);
    dsm_batch.set_input_from_db(4,vil_image_view_sptr);
    dsm_batch.run_process();
    (id,type)=dsm_batch.commpit_output(0);
    feature_sptr=dbvalue(id,type);
    return feature_sptr; 

def extract_intensity_ratio_pixel_time_series_map(target_neighborhood_map_sptr, image_sequence_glob):
    print "Extracting Intensity Ratio Time Series"
    
    dsm_batch.init_process("dsmExtractIntensityRatioPixelTimeSeriesMapProcess");
    dsm_batch.set_input_from_db(0,target_neighborhood_map_sptr);
    dsm_batch.set_input_string(1,image_sequence_glob);
    dsm_batch.run_process();
    (id,type)=dsm_batch.commit_output(0);
    pixel_time_series_map=dbvalue(id,type);
    return pixel_time_series_map;

def classify_pixel_time_series_map(result_directory, pixel_time_series_map, mahalan_factor=2.0, ndims=2, t_forget=1000, prob_thresh = 0.0002, init_covar = 1.0, min_covar = 0.00001,verbose=True ):
    if verbose:
        dsm_batch.init_process("dsmStateMachineClassifyPixelTimeSeriesMapProcess");
        dsm_batch.set_input_string(0,result_directory);
        dsm_batch.set_input_from_db(1,pixel_time_series_map);
        dsm_batch.set_input_unsigned(2,ndims);
        dsm_batch.set_input_unsigned(3,t_forget);
        dsm_batch.set_input_double(4, prob_thresh);
        dsm_batch.set_input_double(5, mahalan_factor);
        dsm_batch.set_input_double(6, init_covar);
        dsm_batch.set_input_double(7, min_covar);
        dsm_batch.run_process();
        (id,type)=dsm_batch.commit_output(0);
    else:
        dsm_batch.init_process("dsmStateMachineClassifyPixelTimeSeriesMapNoOutputProcess")
        dsm_batch.set_input_from_db(0,pixel_time_series_map);
        dsm_batch.set_input_unsigned(1,ndims);
        dsm_batch.set_input_unsigned(2,t_forget);
        dsm_batch.set_input_double(3, prob_thresh);
        dsm_batch.set_input_double(4, mahalan_factor);
        dsm_batch.set_input_double(5, init_covar);
        dsm_batch.set_input_double(6, min_covar);
        dsm_batch.run_process();
        (id,type)=dsm_batch.commit_output(0);
    
    state_machine_manager_sptr = dbvalue(id,type);
    return state_machine_manager_sptr;



def write_change_maps(dsm_manager_sptr, original_img_glob, change_map_directory):
    dsm_batch.init_process("dsmManagerWriteChangeMapsProcess");
    dsm_batch.set_input_from_db(0,dsm_manager_sptr);
    dsm_batch.set_input_string(1,original_img_glob);
    dsm_batch.set_input_string(2,change_map_directory);
    dsm_batch.run_process();

    


def write_manager_bin(dsm_manager_sptr,filename):
    dsm_batch.init_process("dsmManagerWriteBinProcess");
    dsm_batch.set_input_from_db(0,dsm_manager_sptr);
    dsm_batch.set_input_string(1,filename);
    dsm_batch.run_process();
    
def read_manager_bin(filename):
    dsm_batch.init_process("dsmManagerReadBinProcess")
    dsm_batch.set_input_string(0,filename)
    dsm_batch.run_process();
    (id,type)=dsm_batch.commit_output(0)
    manager_sptr=dbvalue(id,type)
    return manager_sptr;    

def write_manager_output(dsm_manager_sptr, dsm_pixel_time_series_sptr, x, y, output_directory):
    dsm_batch.init_process("dsmManagerWriteOutputProcess")
    dsm_batch.set_input_from_db(0,dsm_manager_sptr);
    dsm_batch.set_input_from_db(1,dsm_pixel_time_series_sptr);
    dsm_batch.set_input_unsigned(2,x);
    dsm_batch.set_input_unsigned(3,y);
    dsm_batch.set_input_string(4,output_directory);
    dsm_batch.run_process();
    
def manager_write_average_num_states_per_frame(dsm_manager_sptr, filename):
    dsm_batch.init_process("dsmManagerWriteAverageNumStatesPerFrameProcess")
    dsm_batch.set_input_from_db(0,dsm_manager_sptr);
    dsm_batch.set_input_string(1,filename);
    dsm_batch.run_process();   
    
def dsm_manager_write_prediction_maps(dsm_manager_base_sptr, ni, nj, result_dir, format="tiff"):
    dsm_batch.init_process("dsmManagerWritePredictionMapProcess")
    dsm_batch.set_input_from_db(0,dsm_manager_base_sptr);
    dsm_batch.set_input_unsigned(1,ni);
    dsm_batch.set_input_unsigned(2,nj);
    dsm_batch.set_input_string(3,result_dir);
    dsm_batch.set_input_string(4,format);
    dsm_batch.run_process();    
    
def pixel_time_series_map_extract_sift_box_and_reduce(img_glob, xmin, xmax, ymin, ymax, ndims2keep):
    dsm_batch.init_process("dsmPixelTimeSeriesMapSptrExtractSiftAndReduceProcess")
    dsm_batch.set_input_string(0,img_glob);
    dsm_batch.set_input_unsigned(1,xmin);
    dsm_batch.set_input_unsigned(2,xmax);
    dsm_batch.set_input_unsigned(3,ymin);
    dsm_batch.set_input_unsigned(4,ymax);
    dsm_batch.set_input_unsigned(5,ndims2keep);
    dsm_batch.run_process();
    (id,type)=dsm_batch.commit_output(0);
    pixel_time_series_map_sptr=dbvalue(id,type);
    return pixel_time_series_map_sptr;  

def pixel_time_series_map_extract_intensity_box(video_glob, xmin, xmax, ymin, ymax, normalize = False): 
    dsm_batch.init_process("dsmTimeSeriesExtractIntensityBoxProcess");
    dsm_batch.set_input_string(0,video_glob);
    dsm_batch.set_input_unsigned(1,xmin);
    dsm_batch.set_input_unsigned(2,xmax);
    dsm_batch.set_input_unsigned(3,ymin);
    dsm_batch.set_input_unsigned(4,ymax);
    dsm_batch.set_input_bool(5,normalize)
    dsm_batch.run_process();
    (id,type)=dsm_batch.commit_output(0);
    pixel_time_series_map_sptr=dbvalue(id,type);
    return pixel_time_series_map_sptr;      
    
def write_pixel_dat_file(dsm_pixel_time_series_map_sptr, x, y, filename):
    dsm_batch.init_process("dsmWritePixelDatProcess")
    dsm_batch.set_input_from_db(0,dsm_pixel_time_series_map_sptr);
    dsm_batch.set_input_unsigned(1,x);
    dsm_batch.set_input_unsigned(2,y);
    dsm_batch.set_input_string(3,filename);
    dsm_batch.run_process();    

def dsm_ground_truth_read_bwm_bin( bwm_gt_filename ):
    dsm_batch.init_process("dsmGroundTruthReadBwmBinProcess");
    dsm_batch.set_input_string(0,bwm_gt_filename);
    dsm_batch.run_process();
    (id,type)=dsm_batch.commit_output(0);
    ground_truth_sptr=dbvalue(id,type);
    return ground_truth_sptr;

def dsm_ground_truth_build_change_maps( dsm_ground_truth_sptr, ni, nj ):
    dsm_batch.init_process("dsmGroundTruthBuildChangeMapsProcess");
    dsm_batch.set_input_from_db(0,dsm_ground_truth_sptr);
    dsm_batch.set_input_unsigned(1,ni);
    dsm_batch.set_input_unsigned(2,nj);
    dsm_batch.run_process();

def dsm_ground_truth_save_change_map_tiff( dsm_ground_truth_sptr, result_dir ): 
    dsm_batch.init_process("dsmGroundTruthWriteChangeMapsProcess");
    dsm_batch.set_input_from_db(0,dsm_ground_truth_sptr);
    dsm_batch.set_input_string(1,result_dir);
    dsm_batch.run_process();  
    
def dsm_ground_truth_save_change_map_classical_tiff( dsm_ground_truth_sptr, ni, nj, result_dir ):
    dsm_batch.init_process("dsmGroundTruthWriteChangeMapsClassicalProcess");
    dsm_batch.set_input_from_db(0,dsm_ground_truth_sptr);
    dsm_batch.set_input_unsigned(1,ni);
    dsm_batch.set_input_unsigned(2,nj);
    dsm_batch.set_input_string(3,result_dir);     
    dsm_batch.run_process();

def dsm_confusion_matrix_from_globs(gt_video_glob, prediction_video_glob, result_filename_txt, bias=5):
    dsm_batch.init_process("dsmUtilityComputeConfusionMatrixFromChangeMapsProcess")
    dsm_batch.set_input_string(0,gt_video_glob);
    dsm_batch.set_input_string(1,prediction_video_glob);
    dsm_batch.set_input_string(2,result_filename_txt);
    dsm_batch.set_input_unsigned(3,bias);
    dsm_batch.run_process();
    
    
    
        
        
    
             
    
  
    

