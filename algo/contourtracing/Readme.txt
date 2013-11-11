*******************************************************************
****** THIS CODE MOVED TO lemsvxlsrc/brcv/seg/dbdet/tracer ********
*******************************************************************

This file describes some components of the code and some tasks to be done in
order to put the code under VXL standards



examples/contourtracing_command

Input: 
  - binary image containing many connected components; 
    - white=1, black=0
    - grayscale or color images are thresholded in the middle range (127).
      Anything above threshold is white; the rest is black.

  - smoothing sigma s
      - passed as parameter to the actual tracer, but not used!
        - In fact: by changing s and using cnt = 0 iterations, I didn't notice any
          difference.
      - actually used only by the post-processing smoothing iterations (done after
        tracing).

  - number of smoothing iterations cnt
      - this is used only as a final smoothing of the traced contour. 
      only the .con file will reflect this

Output:
  - vcl_vector of vsol_point_2d_sptr 
    - points_ private member of ContourTracing is only 1st contour??

  - eps and con files


Code design
===========

Main class  is ContourTracing
  - usage sequence:
    contourTracing->setSigma (s);
    contourTracing->setImage (loaded_image);
    contourTracing->setImageFileName (filename);

    contourTracing->detectContour(); // MAIN ROUTINE

    vcl_vector<vsol_point_2d_sptr>& points = contourTracing->getResult();

  - private members
    - points_ : this is what is returned by getResult. However, the
      contour_tracing function currently does not alter this vector.

  - There are remainings of an unfinished attempt to make this class hold many
    contours. There is a PointsListHead_ memeber and class ContourPoint_, which
    are simply NOT being used by the code.
    
-------

contourTracing::detectContour()
  - Calls C function contour_tracing, which is local to ContourTracing.cpp

-------
contour_tracing (ContourTracing.cpp)
  - Core function
  - Call sequence
    - signed_distance_transform
      output: (f)surface_array (double image)
    - curvature_smoothing
      executed on surface_array (DT)
    - trace_eno_zero_xings
      input:  surface_array
      output: Tracer data structure
      
------
ENO Tracer


trace_eno_zero_xings
  input:  surface_array
  output: Tracer data structure
  - Call sequence
    - enoimage_from_data
    - eno_label_zerox
    - subpixel_contour_tracer_all
  
DATA STRUCTURES

Zero Crossings are stored in 2 distinct data structures:

  Xings data structure (translate to EnoImage)
    struct {
      int size; // not used?
      int type; // not used?
      XingsLoc  *vert, *horiz; // like vert and horiz in eno_zerox_image
    } Xings;

    struct{
      int cnt; // # zeros, 0, 1 or 2
      double dist; // not used?
      double loc[2];
      int label[2];
    } XingsLoc;


Tracer data structure 

  typedef struct {
    int size;  // total number of contour points; not very well maintained and
               // not actually used. I'd rather re-count from scratch.
    int type;  // unused
    int *length; // length[i] == #pixels in contour i (1000 = max #of contours)
    double *vert, *horiz; // y,x coords (?)
    int *label; // unused by the working code
    int *id; // unused by the working code
  } Tracer;

  - all the pointers are arrays with "size" elements, where size =nrows*ncols,
  i.e., all these arrays map to (max) the pixels in the image.

  - horiz and vert seem to be lists of pixels. The element marking the end of
    list has value ENDOFLIST.
    ENDOFCONTOUR marks the end of each contour.

  - length[i = length[contour i]



subpixel_contour_tracer_all
  - Call sequence
    - some processing
    - subpixel_contour_tracer
      calls:
      - find_initial_tracer_direction (direction.cpp)
      - next_xing_location (next_xing_location.cpp)
    - some more processing 



-------
eno_image_from_data
  ENO_Interval_Image_t   datastructure

struct ENO_Interval_Image_S
{ 
    int height, width;
    ENO_Interval_t *horiz, *vert;
};

So horiz/vert are 2D images of eno intervals, stored in contiguous memory.
  - label info are stored in zero crossing structure (one for each interval)
  - an analogous in my dbnl framework seems to be a dbnl_eno_zerox_image
    - why not manage everything as lists of vectors, two sets of list for each
      row/col direction:
       - list of dbnl_eno_zerox_vectors
       - list of dbnl_eno_1d
       - list of dbnl_eno_shock_1d

  - 
   





Misc. TODO
==========

- 1st step would be make code work with _my_ eno; compare to original code.
 Only then try to create a tracer class and stuff


- change name to contour_tracing and other names to use underscores, and
  fix other internal variable names
    - underscores should be after, not before
- Make the code use dbnl_eno
  - Understand tag stuff

- change filenames to .cxx

- Internal Ansi-C code is OK, as long as interface is VXL-standardized. We just
  have to replace the following parts of the internal code, as to make it more
  maintainable:
    - use distance transform, ENO, and other utils from lemsvxl/vxl

- in the end, compare times of old implementation to mine.

- declare algorithm parameters as static consts in the toplevel class.

Questions
=========
  - Who are current users of the contour tracer, and what do they require?
    - SCDM?
  - What's the concepts behind the computation of signed DT from two signed ones
    i.e., explain/improve the process, it might be wrong!
  - What's the advantage of outputting contours into polygon instead of vector
    of vsol_point_sptr?
