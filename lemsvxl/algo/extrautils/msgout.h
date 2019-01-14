#ifndef _MSGOUT_H
#define _MSGOUT_H

//////////////////////////////////////////////////////
// MESSAGEOUT for GUI (MessageOut) and Non-GUI (cout)
//////////////////////////////////////////////////////

enum MESSAGE_OPTION { 
  MSG_BOGUS_MESSAGE_OPTION=0,
  MSG_NONE=1, 
  MSG_TERSE=2, 
  MSG_NORMAL=3, 
  MSG_VERBOSE=4,
};

#include <iostream>
#define COUT std::cout
#define MSGOUT(x) std::cout.flush();
//=======================================

//#endif

#endif
