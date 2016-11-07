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

#ifndef _VISUALIZER_CMDLINE_
//============= VISUALIZER ==============
#include "basegui_basewindow.h"
extern BaseWindow*  basePluginWnd;
#define COUT basePluginWnd->Message
#define MSGOUT(x) basePluginWnd->MessageOut(x);
#define STATUSOUT basePluginWnd->StatusOut();
#define CLEARMSG(x) basePluginWnd->ClearMessage(x);
extern MESSAGE_OPTION MessageOption;
//=======================================
#else
//========= VISUALIZER CMDLINE ==========
#include <vcl_iostream.h>
#define COUT cout
#define MSGOUT(x) cout.flush();
//=======================================
#endif



#endif
