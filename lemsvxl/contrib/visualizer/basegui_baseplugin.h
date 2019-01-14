#ifndef PLUGIN_BASE
#define PLUGIN_BASE
#include "basegui_BaseWindow.h"

#include <iostream>

#include <vector>
#include <string>

class BaseWindow;
class MainGUIWindow;

class BasePlugin
  {
    public:
        BasePlugin()
          {
            _plugin_id    = 0; 
            _priority     = 0;
            _name         = "BasePlugin";
            _description  = "The Base class which all other plugins must inherit\n";
            _window_count = 0;
           };

       virtual ~BasePlugin(){};

        const std::string& GetPluginName()     
          {
            return _name; 
           }

        const std::string& GetPluginMenuName()     
          {
            return _pretty_name; 
           }

        const std::vector<std::string>& GetPluginServices()     
          {
            return _services; 
           }

        const std::string& GetPluginDescription()
          {
            return _description; 
           }

        int SetPluginName(const std::string& name)     
          {
            _name = name;
            return 0;
           }

        int SetPluginMenuName(const std::string& name)     
          {
            _pretty_name = name;

            return 0;
           }
        
        int SetPluginServices(const std::vector<std::string> &services)
          {
            _services = services;

            return 0;
           }
       
        int AddToPluginServices(const std::string & service)
          {
            _services.push_back(service);

            return 0;
           }
        
        int SetPluginDescription(const std::string &desc)
          {
            _description = desc;
            return 0;
           }


        
        int   GetPluginID(void)  { return _plugin_id;};
        int   SetPluginID(int id){ _plugin_id=id; return 0;};
        
        
        int   GetPriority(){ return _priority;};
        int   SetPriority(int prio){_priority = prio; return _priority;};


        int IncrementWindowCount(int num=1)
          {
            _window_count +=num;
            return 0;
           }

        int DecrementWindowCount(int num=1)
          {
            _window_count -=num;
            if (_window_count<0)
                return -1;
            else
                return 0;
           }

        int GetWindowCount(void)
          {
            return  _window_count;
           }


  static BaseWindow *DefaultPluginWindow(MainGUIWindow *win)
    {
      BaseWindow *subframe = new BaseWindow
        (win,"Canvas Frame",NULL);
            return subframe;      
    }

        
        
        //The Abstract function that child classes must implement.
        virtual BaseWindow *GetPluginWindow(MainGUIWindow *)=0;


    private:
        int _plugin_id;  // Used in the plugins menu.
        int _priority;
        std::string _name;
        std::string _pretty_name;
        std::string _description;
        std::vector<std::string> _services;
        int _window_count;
   };



#endif   /* PLUGIN_BASE */
