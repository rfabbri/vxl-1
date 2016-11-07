#include "basegui_baseplugin.h"
#include "basegui_pluginmanager.h"

#include <vcl_iostream.h>

#include <vcl_vector.h>
#include <vcl_string.h>




/**
 * Global Plugin Manager.
 *
 * */

static PluginManager *GlobalPluginManager;

//
//
//Plugin Manager
//
//

PluginManager *PluginManager::GetGlobalPluginManager()
  {
    if (!GlobalPluginManager)
      {
        //
        //This is freed only after the program exits.
        //
        GlobalPluginManager = new PluginManager();
       }
    return GlobalPluginManager;
   }


int PluginManager::RegisterPlugin(BasePlugin *plugin)
  {
    if (!plugin)
      {
        vcl_cout<<" Warning: <RegisterPlugin> Null Args. {"<<__FILE__<<", "<<__LINE__<<"} "<<vcl_endl;
        return -1;
       }
    PluginManager *mgr = GetGlobalPluginManager();
    vcl_vector<BasePlugin *> plugin_list =mgr->_plugin_list; 

    for (unsigned int i =0;i<plugin_list.size();i++)
      {
        if ((plugin_list[i]->GetPluginName())==(plugin->GetPluginName()))
          {
            vcl_cout<<" Error: Plugin Cannot be registered as another plugin with same name has already been registered. {"<<__FILE__<<", "<<__LINE__<<"} "<<vcl_endl;
           }
  
//        if ((plugin_list.at(i)->GetPluginID())==(plugin->GetPluginID()))
//          {
//            vcl_cout<<" Error: Plugin Cannot be registered as another plugin with same ID has already been registered. {"<<__FILE__<<", "<<__LINE__<<"} "<<vcl_endl;
//           }
//
       
       }  
    
    mgr->_plugin_list.push_back(plugin);
    plugin->SetPluginID(mgr->_plugin_list.size()); // assign a global id to each plugin
    return 0;
   }

 

int PluginManager::UnRegisterPlugin (BasePlugin *plugin)
  {
            
    if (!plugin)
      {
        vcl_cout<<" Error: <PluginManager::UnRegisterPlugin> Null Args. {"__FILE__<<", "<<__LINE__<<"} "<<vcl_endl;
        return -1;
       }

    PluginManager *mgr = GetGlobalPluginManager();

    vcl_vector<BasePlugin *>::iterator it = mgr->_plugin_list.begin();
    for (;it!=mgr->_plugin_list.end();++it)
      {
   
//TODO :
//Comapre pointers or names?
//
        if ((*it)==plugin)
          {
            mgr->_plugin_list.erase(it);
            return 0;
           }
       }

    vcl_cout<<" Error: <PluginManager::UnRegisterPlugin> Unable to find plugin with name = "<<plugin->GetPluginName()<<" {"<<__FILE__<<", "<<__LINE__<<"} "<<vcl_endl;
    return -1;
   }


vcl_vector<BasePlugin *>& PluginManager::GetPluginList(void)
 {
    PluginManager *mgr = GetGlobalPluginManager();
    return mgr->_plugin_list;
   }





BasePlugin* PluginManager::GetPluginFromService(const vcl_string& service) 
  {
    PluginManager *mgr = GetGlobalPluginManager();
    vcl_vector<BasePlugin *> plugin_list = mgr->_plugin_list;
    for (unsigned int i=0;i<plugin_list.size();i++)
      {
        vcl_vector<vcl_string> temp;
        
        temp = plugin_list[i]->GetPluginServices();
        if (temp.size()==0)
          {
            vcl_cout<<" Error: <GetPluginFromService> Unable to get Plugin Services. {"<<__FILE__<<", "<<__LINE__<<"} "<<vcl_endl;
            return 0;
           }
        
        for (unsigned int j=0;j<temp.size();j++)
          {
            if (temp[j]==service)
              {
                return plugin_list[i];
               }
           }
       }

    vcl_cout<<" Error : <PluginManager::GetPluginFromService> Unable to get Plugin with service = "<<service<<" {"<<__FILE__<<", "<<__LINE__<<"} "<<vcl_endl;
    return 0;
   }

BasePlugin* PluginManager::GetPluginFromName   (const vcl_string& name)   
  {
    PluginManager *mgr = GetGlobalPluginManager();

    vcl_vector<BasePlugin *> plugin_list = mgr->_plugin_list;
    for (unsigned int i =0;i<plugin_list.size();i++)
      {
        if (plugin_list[i]->GetPluginName()==name)
          {
            return plugin_list[i];
           }
       }

    vcl_cout<<" Error : <PluginManager::GetPluginName> Unable to get Plugin with name = "<<name<<" {"<<__FILE__<<", "<<__LINE__<<"} "<<vcl_endl;
    return 0;
   }
  

BasePlugin* PluginManager::GetPluginFromID   (int id) 
  {
    PluginManager *mgr = GetGlobalPluginManager();
    vcl_vector<BasePlugin *> plugin_list = mgr->_plugin_list;
    
    for (unsigned int i =0;i<plugin_list.size();i++)
      {
        if (plugin_list[i]->GetPluginID()==id)
          {
            return plugin_list[i];
           }
       }

    vcl_cout<<" Error : <PluginManager::GetPluginFromID> Unable to get Plugin with ID = "<<id<<" {"__FILE__<<", "<<__LINE__<<" } "<<vcl_endl;
    return 0;
   }



int PluginManager::GetPluginCount() 
  {
    PluginManager *mgr = GetGlobalPluginManager();
    assert(mgr!=0);
    return mgr->_plugin_list.size();
   }

/**
 * If there is only one plugin, get it's name
 * else
 * return "";
 * */

vcl_string PluginManager::GetUniquePluginName() 
  {
    PluginManager *mgr = GetGlobalPluginManager();
    vcl_vector<BasePlugin *> plugin_list = mgr->_plugin_list;
    if (plugin_list.size()==1)
        return (plugin_list[0]->GetPluginName());
    else
        return "";
   }
