#ifndef boulder_load_dll
#define boulder_load_dll

#ifdef WIN32
#ifdef boulder_codec_api_EXPORTS
    #define BOULDER_CODEC_API __declspec(dllexport) 
#else 
    #define BOULDER_CODEC_API __declspec(dllimport) 
#endif 
#else 
    #define BOULDER_CODEC_API 
#endif // now declare a function...

  
extern "C" BOULDER_CODEC_API void load_video_roi(unsigned short * roi,char * inputvid, int frameno,int x0,int y0,int ni,int nj);




#endif
