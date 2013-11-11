#ifndef PLUGIN_MANAGER_H
#define PLUGIN_MANAGER_H

#include <vcl_iostream.h>
#include <vcl_vector.h>
#include <vcl_string.h>


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
   
        static BasePlugin *GetPluginFromService(const vcl_string& service);
        static BasePlugin *GetPluginFromName   (const vcl_string& name)   ;
        static BasePlugin *GetPluginFromID     (int id)               ;
        
        static PluginManager* GetGlobalPluginManager(void);
        static int RegisterPlugin   (BasePlugin  *win);
        static int UnRegisterPlugin (BasePlugin  *win);
        static vcl_vector<BasePlugin *>& GetPluginList(void);
        static int GetPluginCount();
        static vcl_string GetUniquePluginName();
    
    private:
        vcl_vector<BasePlugin *> _plugin_list;
        
   };







#endif                /* PLUGIN_MANAGER_H */
