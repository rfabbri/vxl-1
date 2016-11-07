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
            print "Extracting dsift_feature_sptr at location: (%d,%d)" % (x_keys[key_idx],y_keys[key_idx])
            
            print("Creating bapl_dsift_sptr")
            dsm_batch.init_process("baplDsiftSptrProcess");
            dsm_batch.set_input_from_db(0,img_view_sptr);
            dsm_batch.run_process();
            (id,type)=dsm_batch.commit_output(0);
            bapl_dsift_sptr=dbvalue(id,type);
        
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
    
def write_pixel_time_series_map_sptr_bin(pixel_time_series_map_stpr, filename):
    print "Writing Pixel Time Series Map to %s" % filename
    filename = os.path.abspath(filename);
    dsm_batch.init_process("dsmWritePixelTimeSeriesMapBinProcess")
    dsm_batch.set_input_string(0,filename);
    dsm_batch.set_input_from_db(1,pixel_time_series_map_stpr);
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
    
            
    
  
    

