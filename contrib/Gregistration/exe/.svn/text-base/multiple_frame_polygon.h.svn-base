#ifndef multiple_frame_polygon_dll
#define multiple_frame_polygon_dll

#ifdef WIN32
#ifdef multiple_frame_polygon_api_EXPORTS
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


extern "C" VEHICLE_API unsigned int intializemodel(int ni,int nj);
extern "C" VEHICLE_API double get_mean(unsigned int handle,int i,int j,int component_num);
extern "C" VEHICLE_API double get_var(unsigned int handle,int i,int j,int component_num);
extern "C" VEHICLE_API double get_weight(unsigned int handle,int i,int j,int component_num);

 extern "C" VEHICLE_API void updatemodel(unsigned int handle, unsigned char * 
input_img,unsigned int maxComp,float initialstd,float intialweight,int 
ni,int nj);

extern "C" VEHICLE_API void savemodel(unsigned int handle,char *filename);
extern "C" VEHICLE_API unsigned int loadmodel(char *filename);

extern "C" VEHICLE_API  void updateedgemodel(unsigned int handle, unsigned char * input_img,unsigned int maxComp,
                 float initialstd,float intialweight,float ethresh,int ni,int nj);


extern "C" VEHICLE_API  unsigned int getpolygon(unsigned int handle,unsigned char* img_data,float sigmathresh,float minweightthresh,
                                       int min_no_of_pixels,int max_no_of_pixels,float erosionrad,
                                       float postdilationerosionrad,float dilationrad,int ni,int nj,float sigma,int rad_of_uncert);
extern "C" VEHICLE_API  unsigned int initialize_tracker(int n_samples ,
                                float search_radius ,
                                float sigma ,
                                float scorethresh ,
                                unsigned int intensity_hist_bins ,
                                unsigned int gradient_dir_hist_bins 
                                );

extern "C" VEHICLE_API  unsigned int return_i_multiple_instance(unsigned int mobs, int index);
extern "C" VEHICLE_API  unsigned int size_of_multiple_instance_vector(unsigned int mobs);
extern "C" VEHICLE_API  unsigned int return_polys_from_multiple_instance(unsigned int mobs);

extern "C" VEHICLE_API  void track_polygons(unsigned int trkhndno,unsigned char *img_data, int ni,int nj,unsigned int polyhandle, float expmotion, int frameno);


extern "C" VEHICLE_API   unsigned int superimpose_edges(unsigned int trkhandle, unsigned char *img_data,float Tinit,
                               float Tfinal,float annealrate,float lambdainit, float mconvg, 
                               float moutlier, int ni,int nj, int frame_no,
                               int winsize, float std, float thresh);



extern "C" VEHICLE_API int numvertices_i(int i,unsigned int polyhandle);
extern "C" VEHICLE_API void returnvertices(int i, double * x, double *y, unsigned int polyhandle);
extern "C" VEHICLE_API int num_of_poly(unsigned int handle);
extern "C" VEHICLE_API void save_tracks(unsigned int trkhandle, char * filename);
extern "C" VEHICLE_API unsigned int load_tracks(char * filename);
extern "C" VEHICLE_API void compute_clips(unsigned int trkhandle, unsigned int edgehandle,int frameno,unsigned char *img_data,int ni,int nj,
                   float bgdist,float bgminweightthresh,int bgrad,float bgthresh, float polydilate);

#endif
