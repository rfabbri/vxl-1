#ifndef aerial_vehicle_observation_dll
#define aerial_vehicle_observation_dll

#ifdef WIN32
#ifdef aerial_vehicle_observation_api_EXPORTS
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
// erosionrad (2): morphological erosion of the blobs so as to get rid of the noise
// postdilationerosionrad(2): morphological erosion of the blobs to bring them back to their original size
// dilationrad(7): connects the disconnected components
// polyfile : output file for getting the polygons.


//: returns the pointer to observation of ith polygon.
extern "C" VEHICLE_API unsigned int getobservation_i(unsigned char * input_img, int ni, int nj,int i,unsigned int polyhandle,int frame);

extern "C" VEHICLE_API int getlabel_i(char * name, 
         int motion_orientation_bin,
         int view_angle_bin,
         int shadow_angle_bin,
              int shadow_length);

#endif
