#ifndef dbrl_superresolution_edgemap_dll
#define dbrl_superresolution_edgemap_dll

#ifdef WIN32
#ifdef dbrl_superresolution_edgemap_api_EXPORTS
    #define EDGEMAP_API __declspec(dllexport) 
#else 
    #define EDGEMAP_API __declspec(dllimport) 
#endif 

#else 
    #define EDGEMAP_API 
#endif // now declare a function...


//: returns the pointer to observation of ith polygon.
extern "C" EDGEMAP_API unsigned int postprocessobservations(unsigned char * input_img, int ni, int nj,int i,unsigned int polyhandle,int frame);
//: returns the pointer to label of ith polygon.

extern "C" EDGEMAP_API int getlabel_i(char * name, 
         int motion_orientation_bin,
         int view_angle_bin,
         int shadow_angle_bin,
              int shadow_length);

#endif
