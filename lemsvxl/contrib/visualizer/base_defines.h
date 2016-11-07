#ifndef _DEFINES_H
#define _DEFINES_H
 
////////////////////////////////////////////////
// Math definition...
////////////////////////////////////////////////

#ifdef NULL
#undef NULL
#endif
#ifndef NULL
#define NULL    0
#endif

#ifdef NUL
#undef NUL
#endif
#ifndef NUL
#define NUL      0
#endif

#ifdef HUGE
#undef HUGE
#endif
#ifndef HUGE
#define HUGE    1E28
#endif

#ifdef M_PI
#undef M_PI
#endif
#ifndef M_PI
#define M_PI    3.1415926535897932384626433832795
#endif

#ifdef M_PI_2
#undef M_PI_2
#endif
#ifndef M_PI_2
#define M_PI_2    1.5707963267948966192313216916398
#endif

#ifdef min
#undef min
#endif
#ifndef min
#define min(x,y)   ((x)>(y)?(y):(x))
#endif

#ifdef max
#undef max
#endif
#ifndef max
#define max(x,y)   ((x)>(y)?(x):(y))
#endif

////////////////////////////////////////////////
// Math Functions
#define round(x) (int((x)+0.5))

//sign of x: +1 or -1
#define sign(x)  (((x)>0)?1:-1)

#define isBetween(z,x,y) ((((z)<=(x) && (z)>=(y))||((z)>=(x) && (z)<=(y)))?(1):(0))

#endif
