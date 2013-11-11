
#include <math.h>
#include <stdio.h>
#include <iiointerface.h> 

#include <utils.h>

#include "read_initial_parameters.h"


void read_initial_parameters(Pars *pars, int argc, char **argv) 
     
{
  int err;
  extern PICINFO thePInfo;
  pars->intensity_intervals = (int *) calloc(100,sizeof(int));

  /* default values */
  pars->seeds = "random initialization is used";
  pars->stopimage = NULL;
  pars->alpha = -0.3;
  pars->beta = 0.0;
  pars->steps = 1;
  pars->start_from = 0;
  pars->stop_function = 1.5; 
  pars->how_often_save = 1;
  pars->stdev = 3.0;
  pars->circle_size = 3.0;
  pars->band_size = 300.0;
  pars->type = 11;
  pars->save_surface = 0;
  pars->n_smoothing = 0.0;
  pars->g_smoothing = 0.0;
  pars->initialize_min = 0;
  pars->initialize_max = 9999;
  pars->height = 128;
  pars->width  = 128;
  pars->userinit  = 0;
  pars->dam = 0;
 
  if (argc < 2) {   /* at least two arguments have to be inputed */ 
    usage();
    exit(11);
  }
  pars->inputimage = argv[1];
  pars->outputimage = argv[2]; 
  
  for (--argc, ++argv; argc; --argc, ++argv ) {
    
    if ( **argv == '-' ) {  
      ++*argv;
      --argc;
      
      if ( keymatch( *argv, "imagetype", 9))
        pars->type = atoi( *++argv);
      else if ( keymatch ( *argv, "stopimage", 9))
        pars->stopimage = *++argv;
      else if ( keymatch ( *argv, "alpha", 5))
        pars->alpha = -(atof( *++argv));
      else if ( keymatch ( *argv, "beta", 4))
        pars->beta = atof( *++argv);
      else if ( keymatch ( *argv, "steps", 5))
        pars->steps = atoi( *++argv);    
      else if ( keymatch ( *argv, "start_from", 10))
        pars->start_from = atoi( *++argv);
      else if ( keymatch ( *argv, "how_often_save", 14))
        pars->how_often_save = atoi( *++argv);
      else if ( keymatch ( *argv, "save_surface", 12))
          pars->save_surface = atoi( *++argv);
      else if ( keymatch ( *argv, "stop_function",13))
        pars->stop_function = atof( *++argv);
      else if ( keymatch(*argv, "stdev",5))
        pars->stdev = atof( *++argv);
      else if ( keymatch ( *argv, "circle_size",11))
        pars->circle_size = atof( *++argv);
      else if ( keymatch ( *argv, "g_smoothing",11))
        pars->g_smoothing = atoi(*++argv);
      else if ( keymatch ( *argv, "n_smoothing", 11))
        pars->n_smoothing = atoi(*++argv);
      else if ( keymatch ( *argv, "intensity_intervals",19)) {
        ii=0;
        while () {
          intensity_intervals[ii] =  atoi(*++argv);
          intensity_intervals[ii+1] =  atoi(*++argv);
          --argc; --argc; 
          if ( **argv != '-' )
            break;
        }
      else if ( keymatch ( *argv, "initialize_max", 14))
        pars->initialize_max = atoi(*++argv);
      else if ( keymatch ( *argv, "userinit", 7))
        pars->userinit = atoi(*++argv);
      else if  ( keymatch ( *argv, "height", 6))
        pars->height = atoi(*++argv);
      else if  ( keymatch ( *argv, "width", 5))
        pars->width = atoi(*++argv);
      else if ( keymatch ( *argv, "seeds", 5))
          pars->seeds = *++argv;
      else if  ( keymatch ( *argv, "nband_size", 10))
        pars->band_size = atof(*++argv);
      else if  ( keymatch ( *argv, "dam", 3))
        pars->dam = atof(*++argv);
      else {
        printf("%s is not a paramater for bubbles\n", *argv);
        usage();
        exit(1);
      }
      
    } /* end if else */
    
  } /* end for loop */
  

  if ((pars->inputimage == NULL) || (pars->outputimage == NULL))  {
    usage();
    exit(11);
  }
  
  if (pars->type == 11) {
    err = ImageIORead(pars->inputimage,&thePInfo);
    if (err == readFailure){
      fprintf(stderr,"could not read file %s. Try another one..\n",pars->inputimage);
      exit(1);
    }
    pars->height = thePInfo.h;  
    pars->width  = thePInfo.w;
  }
  

}
  
void usage ()
{

 printf("Bubbles Usage
Mandatory Inputs:        
        -inputimage <filename>
        -outputimage <filename>
Optional Inputs:
        -seeds [null]
        -userinit [0] 
            options [0 1 2]: 
               0 = random initialization 
               1 = random initialization and stop
               2 = get bubbles from user 
        -imagetype [raster]
             options are: 1byte, 2byte, int32
        -stopimage [null]
        -alpha [-0.3]
        -beta  [0.0]
        -steps [1]  total number of iterations
        -start_from [0] 
            !=0 then it will read the surface from 
                ouputimage-<alpha>-<beta>-<start_from>.raw
        -how_often_save [1]
        -save_surface [0]
        -stop_function  [1.5] 
             options are [1.0, 2.0]
        -dam [0]
        -nband_size [100.0]
        -initialize_min [0]
        -initialize_max [255]
        -stdev [5]
        -circle_size [3]
        -g_smoothing [0.0]
        -n_smoothing [0.0]
        -height [height of inputimage]
        -width [width of inputimage] 
            supply height and width if type is 1 or 2 \n");

}

void print_input_values(Pars *pars) 

{

  printf("-inputimage         %s\n", pars->inputimage);
  printf("-outputimage        %s\n", pars->outputimage);
  printf("-seeds              %s\n", pars->seeds);
  printf("-type               %i\n", pars->type);
  printf("-alpha              %f\n", pars->alpha);
  printf("-beta               %f\n", pars->beta);
  printf("-steps              %i\n", pars->steps);
  printf("-start_from         %i\n", pars->start_from);
  printf("-how_often_save     %i\n", pars->how_often_save);
  printf("-save_surface       %i\n", pars->save_surface);
  printf("-stop_function      %f\n", pars->stop_function);
  printf("-dam                %i\n", pars->dam);
  printf("-stdev              %f\n", pars->stdev);
  printf("-circle_size        %f\n", pars->circle_size);
  printf("-band_size          %f\n", pars->band_size);
  printf("-initialize_min     %i\n", pars->initialize_min);
  printf("-initialize_max     %i\n", pars->initialize_max);
  printf("-g_smoothing        %f\n", pars->g_smoothing);
  printf("-n_smoothing        %f\n", pars->n_smoothing);
  printf("-height             %i\n", pars->height);
  printf("-width              %i\n", pars->width);

  if (pars->stopimage != NULL) 
    printf("stopimage = %s\n", pars->stopimage);
 
}

















