#ifndef PLUGIN_MANAGER_H
#define PLUGIN_MANAGER_H

#include <iostream>
#include <vector>
#include <string>


class BasePlugin;
class PluginManager
  {
    public :
        PluginManager()
          {
           }
        ~PluginManager()
          {
           };
   
        static BasePlugin *GetPluginFromService(const std::string& service);
        static BasePlugin *GetPluginFromName   (const std::string& name)   ;
        static BasePlugin *GetPluginFromID     (int id)               ;
        
        static PluginManager* GetGlobalPluginManager(void);
        static int RegisterPlugin   (BasePlugin  *win);
        static int UnRegisterPlugin (BasePlugin  *win);
        static std::vector<BasePlugin *>& GetPluginList(void);
        static int GetPluginCount();
        static std::string GetUniquePluginName();
    
    private:
        std::vector<BasePlugin *> _plugin_list;
        
   };







#endif                /* PLUGIN_MANAGER_H */
