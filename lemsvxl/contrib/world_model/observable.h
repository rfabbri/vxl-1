#ifndef observable_h_
#define observable_h_

#include <vgui/vgui_observable.h>
#include <vgui/vgui_message.h>
#include <string>


class observable : public vgui_observable
{
public:

  virtual std::string type_name()=0; 

  //: constructer
  observable(){}
  virtual ~observable(){};
};

#endif
