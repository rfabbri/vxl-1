// \file
// \brief Contains class which reads all the parameters and the file paths required for cali
// \date   09/03/2006


#ifndef cali_param_h
#define cali_param_h

#include <vcl_string.h>
#include <vcl_vector.h>
#include <vcl_iostream.h>
#include <vcl_cstdio.h>
#include <vcl_cstddef.h>
#include <vcl_cstdlib.h>
#include <vcl_utility.h>
#include <vcl_fstream.h>

#include <vnl/vnl_vector.h>

class cali_param

{
    public:
        //  path for the txt file containing all the necessary parameters & paths for cali
    cali_param(vcl_string pathname); 

     cali_param(void);
    cali_param(cali_param const& par);

    //  path for the text file which stores the centers of the conics projected into the images from
    //  the artifact model
     vcl_string CENTERS;

     // path for the text file which stores the difference vectors corresponding to the ball centers,widths & the 
     // heights
     vcl_string DIFF;

     // there are currently two methods,method 1 in which the user decides how many parameters are to be varied & method 2 
     // in which 9 parameters (more details in cali_calibrator.h) are varied in the first stage of iterations ,followed by 
     // varying 18 parameters in the second stage

     // path for the text file which stores the converged values of the parameters obtained after the iterations 
     // (corresponding to method 1 )

     vcl_string CONVERGEDVALUES;
    
     // path for the text file which stores the converged values of the parameters obtained after the first stage in 
     // calibration (corresponding to method 2 )
     vcl_string CONVERGEDVALUES_STAGE1;
   
     // path for the text file which stores the converged values of the parameters obtained after the second stage in 
     // calibration (corresponding to method 2 )

     vcl_string CONVERGEDVALUES_STAGE2;

     // path for the text file which contains the coordinates of the balls attached to the artifact (measured by CMM machine)
     vcl_string CMMINFO;

     // path for the log file of the data set being used for the calibration
     vcl_string LOGFILE;
     
     // path for the text file which contains the information about the conics fitted to the image data set
     vcl_string CONICS_BIN_FILE_BASE;
      
     // path for the text file which contains the correspondences ( the centers of the balls in the images & the centers of 
     // the projected spheres  ( used only in case of manual calibration...)
     vcl_string CORRESPONDENCEINFO;
     
     // path for the scan file which would be generated after the calibration process is finished
     vcl_string OUTPUT_SCANFILE;
    
 // the number of balls attached to the cylinder ( the calibration artifact )
  int BALL_NUMBER;

 // the radius of the big ball 
  double BALL_RADIUS_BIG;   

// the ball density 
  double BALL_DENSITY;

// the height of the cylinder artifact
  double HEIGHT;

// the radius of the cylinder artifact
  double RADIUS; 

// PARAMETERS USED FOR THE CONIC FITTING

// the threshold used for the conic fitting
  double THRESHOLD;

// the gaussian sigma which determines the extent of smoothing
  double GAUSSIAN_SIGMA;

// the minimum fit length for the conics being fitted to the data set 
  int MIN_FIT_LENGTH;

  double RMS_DISTANCE;

// the aspect ratio is the ratio of the width to height of the conics (essentially used to filter out the ellipses
//  with higher ellipticity )
  double ASPECT_RATIO;

// t
  double WIDTH_HEIGHT_DIFF_THRESHOLD;

  double WIDTH_THRESHOLD;

  double X_COORD_DIFF_RANGE;

  double Y_COORD_DIFF_RANGE;

// the maximum number of parameters that can be used in the iterations
  int SIZE_OF_X;

// the number of correspondences that should be used from each image 
  unsigned int NUM_OF_CORRESP;

// the number of conics from each image (should be same as the number of correspondences..so no need for this entity
//  actually !!!)
  unsigned int NUM_OF_CONICS;

// the number of parameters to be varied in the iterations (note that for method 2,this is fixed..since 9 parameters are being 
//used in stage 1 & 18 parameters are being used in stage 2
  unsigned int NUM_OF_PARAMETERS;

// the starting frame number of the data set images to be used for the calibration
  int START;

// the ending frame number of the data set images to be used for the calibration 
  int END;

// the interval between the frames

  int INTERVAL;

// the error margin factor is a parameter which determines the threshold distance between the conics  below which they
// would be combined together
  unsigned int ERROR_MARGIN_FACTOR;

// this vector is used to activate (a value of 1 )  & freeze ( a value of 0 ) the parameters in the iterations
  vnl_vector<int>SETPARAM;

// vector containing the initial values of the parameters 
  vnl_vector<double>PARAM;

// vector which activates & freezes the balls of the cylinder artifact ( to keep out the balls with 
// spurious co-ordinates )
  vnl_vector<int>SETMASK;
 
// vector containing the center correspondences read out from the path specified by CORRESPONDENCEINFO (used only for 
// manual calibration )
  vcl_vector<vcl_pair<double,double> > COORDS;

// vector containing the height,width correspondences read out from the path specified by CORRESPONDENCEINFO (used only for 
// manual calibration )
  vcl_vector<vcl_pair<double,double> >HEIGHT_RADII;
 
private:
    vcl_string pathname_;


    
};

#endif
