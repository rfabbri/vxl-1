#ifndef XPDEFS_H_INCLUDED
#define XPDEFS_H_INCLUDED

#ifndef DISABLE_GL_FONTS
#if __WXMSW__
#define DISABLE_GL_FONTS 1 //Ming: 0
#else 
#define DISABLE_GL_FONTS 0
#endif
#endif

#if !(DISABLE_GL_FONTS)
#if __linux
// linux seems has to have a problem with many small glDrawPixels-es
#define ANTIALIAS_GL_FONTS 0
#else
#define ANTIALIAS_GL_FONTS 1
#endif
#endif

#ifndef USE_RESOURCE_BITMAPS
#ifdef __WXMSW__
#define USE_RESOURCE_BITMAPS 1
#else
#define USE_RESOURCE_BITMAPS 0
#endif
#endif

#ifndef DEFAULT_FACE_PATH
#if __WXMSW__
#define DEFAULT_FACE_PATH "..\\base\\include\\xp\\arial.ttf" //"c:\\windows\\fonts\\arial.ttf"
#else
#define DEFAULT_FACE_PATH "/vision/packages/ftgl/share/default-face"
#endif
#endif

#endif
