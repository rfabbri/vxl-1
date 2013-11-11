#ifndef aerial_background_foreground_detection_dll
#define aerial_background_foreground_detection_dll

#ifdef WIN32
#ifdef aerial_bg_modeler_api_EXPORTS
    #define VEHICLE_API __declspec(dllexport) 
#else 
    #define VEHICLE_API __declspec(dllimport) 
#endif 
#else 
    #define VEHICLE_API 
#endif // now declare a function...

//: input_vid : path of the folder which contains images
//: maxComp (3) : no of components for building mixture. Generally 3 is good but if there are lot of dynamic things in the scene, it should be increased.
//: initialstd (15): This paramter is used to initialize the std deviation of gaussian component in the mixture. 
//               If the camera has large noise, then it should be increased otherwise 15 is good.
// initialweight(0.1) : parameter used to initialize the weight of each gaussian component.
// sigmathresh (2.5) : threshold for detecting a mode for a given sample and it would be detected if it lies within the sigmathresh*sigma of the component.
// minweightthresh(0.3) : To detect only significant modes(0.2 is good).
// min_no_of_pixels (75): this selects the objects with a minimum size which want to be segmented from the video.
// max_no_of_pixels (200): this selects the objects with a maximum size.
// erosionrad#include <vsol/vsol_point_2d_sptr.h> (2): morphological erosion of the blobs so as to get rid of the noise
// postdilationerosionrad(2): morphological erosion of the blobs to bring them back to their original size
// dilationrad(7): connects the disconnected components
// polyfile : output file for getting the polygons.


extern "C" VEHICLE_API  unsigned int intializemodel(int ni,int nj);
extern "C" VEHICLE_API void savemodel(unsigned int handle,char *filename);
extern "C" VEHICLE_API unsigned int loadmodel(char *filename);

extern "C" VEHICLE_API  void updatemodel(unsigned int handle, unsigned char * 
input_img,unsigned int maxComp,float initialstd,float intialweight,int 
ni,int nj);
extern "C" VEHICLE_API  unsigned int getpolygon(unsigned int handle,unsigned char* img_data,float sigmathresh,float minweightthresh,
                                       int min_no_of_pixels,int max_no_of_pixels,float erosionrad,
                                       float postdilationerosionrad,float dilationrad,int ni,int nj,float sigma,int rad_of_uncert);

extern "C" VEHICLE_API int numvertices_i(int i,unsigned int polyhandle);
extern "C" VEHICLE_API void returnvertices(int i, double * x, double *y, unsigned int polyhandle);
extern "C" VEHICLE_API void returnmean(int i, double *pt, unsigned int polyhandle);
extern "C" VEHICLE_API int num_of_poly(unsigned int handle);
extern "C" VEHICLE_API int getnumpointsconvexhull(int i,  unsigned int polyhandle);
extern "C" VEHICLE_API void getconvexhullpoints(int i, double * x, double *y, unsigned int polyhandle);
extern "C" VEHICLE_API unsigned int getconvexhulls(unsigned int polyhandle);
#endif
