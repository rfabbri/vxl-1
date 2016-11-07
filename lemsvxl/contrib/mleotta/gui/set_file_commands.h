// This is contrib/mleotta/gui/set_file_commands.h
#ifndef set_file_commands_h_
#define set_file_commands_h_

//:
// \file
// \brief vgui commands to set I/O filenames
// \author Matt Leotta (mleotta@lems.brown.edu)
// \date 10/18/06
//
// \verbatim
//  Modifications
// \endverbatim


#include <vgui/vgui_command.h>
#include <vgui/vgui_dialog.h>
#include <dbvidl2/pro/dbvidl2_source.h>
#include <dbvidl2/pro/dbvidl2_sink.h>
#include <dbpro/xio/dbpro_xio_sink.h>
#include <dbpro/xio/dbpro_xio_source.h>
#include <dbpro/dbpro_vsl_processes.h>
#include <dbpro/dbpro_ios_processes.h>

class set_istream_command : public vgui_command
{
  public:
    set_istream_command(const dbpro_process_sptr& pro) : process_(pro) {}

    void execute()
    {
      dbvidl2_source* source = dynamic_cast<dbvidl2_source*>(process_.ptr());
      if(source){
        vidl_istream* istream = vidl_gui_open_istream_dialog();
        if(istream){
          source->set_stream(istream);
        }
      }
    }

    dbpro_process_sptr process_;
};


class set_ostream_command : public vgui_command
{
  public:
    set_ostream_command(const dbpro_process_sptr& pro) : process_(pro) {}

    void execute()
    {
      dbvidl2_sink* sink = dynamic_cast<dbvidl2_sink*>(process_.ptr());
      if(sink){
        vidl_ostream* ostream = vidl_gui_open_ostream_dialog();
        if(ostream){
          sink->set_stream(ostream);
        }
      }
    }

    dbpro_process_sptr process_;
};


class set_b_istream_command : public vgui_command
{
  public:
    set_b_istream_command(const dbpro_process_sptr& pro) : process_(pro) {}
    
    void execute()
    {
      dbpro_b_istream_source_base* source = 
        dynamic_cast<dbpro_b_istream_source_base*>(process_.ptr());
      if(source){
        vgui_dialog file_dlg("Select Binary Input File");
        vcl_string regexp("*.*"), filename;
        file_dlg.inline_file("Binary File",regexp,filename);
        if(!file_dlg.ask())
          return;
        
        if(filename != "")
          source->open(filename);
      }
    }
    
    dbpro_process_sptr process_;
};


class set_b_ostream_command : public vgui_command
{
  public:
    set_b_ostream_command(const dbpro_process_sptr& pro) : process_(pro) {}
    
    void execute()
    {
      dbpro_b_ostream_sink_base* sink = 
      dynamic_cast<dbpro_b_ostream_sink_base*>(process_.ptr());
      if(sink){
        vgui_dialog file_dlg("Select Binary Output File");
        vcl_string regexp("*.*"), filename;
        file_dlg.inline_file("Binary File",regexp,filename);
        if(!file_dlg.ask())
          return;
        
        if(filename != "")
          sink->open(filename);
      }
    }
    
    dbpro_process_sptr process_;
};


class set_ifstream_command : public vgui_command
{
  public:
    set_ifstream_command(const dbpro_process_sptr& pro) : process_(pro) {}
    
    void execute()
    {
      dbpro_ifstream_source_base* source = 
        dynamic_cast<dbpro_ifstream_source_base*>(process_.ptr());
      if(source){
        vgui_dialog file_dlg("Select Input File");
        vcl_string regexp("*.*"), filename;
        file_dlg.inline_file("Input File",regexp,filename);
        if(!file_dlg.ask())
          return;
        
        if(filename != "")
          source->open(filename);
      }
    }
    
    dbpro_process_sptr process_;
};


class set_xml_infile_command : public vgui_command
{
  public:
    set_xml_infile_command(const dbpro_process_sptr& pro) : process_(pro) {}

    void execute()
    {
      dbpro_xio_source* source = dynamic_cast<dbpro_xio_source*>(process_.ptr());
      if(source){
        vgui_dialog file_dlg("Select XML Input File");
        vcl_string regexp("*.xml"), filename;
        file_dlg.inline_file("XML File",regexp,filename);
        if(!file_dlg.ask())
          return;

        if(filename != "")
          source->open(filename);
      }
    }

    dbpro_process_sptr process_;
};


class set_xml_outfile_command : public vgui_command
{
  public:
    set_xml_outfile_command(const dbpro_process_sptr& pro) : process_(pro) {}

    void execute()
    {
      dbpro_xio_sink* sink = dynamic_cast<dbpro_xio_sink*>(process_.ptr());
      if(sink){
        vgui_dialog file_dlg("Select XML Output File");
        vcl_string regexp("*.xml"), filename;
        file_dlg.inline_file("XML File",regexp,filename);
        if(!file_dlg.ask())
          return;

        if(filename != "")
          sink->open(filename);
      }
    }

    dbpro_process_sptr process_;
};

#endif // set_file_commands_h_
