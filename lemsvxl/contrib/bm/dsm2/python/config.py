# this is /contrib/bm/dsm2/python/config.py
# A basic module to keep configuration settings
import os

#print 'config imported'

class Config(object):
    def __init__(self,computer):
        if (computer == 'lems'):
            #'C:/vxl/src/contrib/brl/lemsvxl/contrib/bm/dsm/gt_bin'
            self.dsm_gt = os.path.abspath(
                                os.path.join('C:\\','vxl','scr','contrib',
                                       'brl','lemsvxl','contrib',
                                       'bm','dsm','gt_bin')) 
            
            self.results_dir = os.path.abspath(
                                    os.path.join('E:\\','research',
                                            'changeDetection',
                                            'results'))
            
            self.datasets_dir = os.path.abspath(os.path.join('Y:','video'))
            
            self.university_of_nebraska_60frames_grey = os.path.join(
            self.datasets_dir,'universityOfNebraska','60frames','grey','*.tif')
        else:
            print 'Unknown Computer'
            
