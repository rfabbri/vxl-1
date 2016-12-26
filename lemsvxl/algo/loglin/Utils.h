#ifndef UTILS_H
#define UTILS_H

#include <string>
#include <sstream>

/**---------------------Compile-time Enforcement --------------*/

//
//NOTE: Do NOT specialize this for "false". It will defeat the very purpose of this function!.
//
template<bool condition>
inline void Enforce()
  {
    //
    //The size of the following array MUST be <0.
    //This is used to detect error's at compile Time.
    //
    char dummy[-1];
   };

template<>
void Enforce<true>(){};

#ifndef NDEBUG
#define COMPILE_TIME_ENFORCE(cond)        Enforce<cond>()
#else
#define COMPILE_TIME_ENFORCE(cond)
#endif

/*--------------------------------*/



#ifndef NDEBUG
#define COMPILE_TIME_CHECK(cond)    cond
#else
#define COMPILE_TIME_CHECK(cond)
#endif




template<typename ValueType>
string toString(const ValueType& val)
  {
    ostringstream ostrm;

    ostrm<<val;
    return ostrm.str();
   };






#endif     /* UTILS_H */
