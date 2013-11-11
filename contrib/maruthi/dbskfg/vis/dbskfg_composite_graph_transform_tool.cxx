// This is shp/dbsksp/dbskfg_composite_graph_transform_tool.cxx
//:
// \file

// dbskfg headers
#include <dbskfg/vis/dbskfg_composite_graph_transform_tool.h>
#include <dbskfg/algo/dbskfg_transform_descriptor_sptr.h>
#include <dbskfg/algo/dbskfg_detect_transforms.h>
#include <dbskfg/dbskfg_composite_graph_sptr.h>
#include <dbskfg/dbskfg_composite_graph.h>
#include <dbskfg/algo/dbskfg_transformer.h>
#include <dbskfg/algo/dbskfg_region_growing_transforms.h>
#include <dbskfg/algo/dbskfg_cgraph_directed_tree.h>
#include <dbskfg/algo/dbskfg_composite_graph_fileio.h>
#include <dbskfg/vis/dbskfg_soview_rag_node.h>
#include <dbskfg/vis/dbskfg_soview_transform.h>
#include <dbskfg/dbskfg_rag_node.h>
#include <dbskfg/dbskfg_rag_graph.h>

// vgui headers
#include <vgui/vgui_dialog.h>
#include <vgui/vgui.h>
// bvis1 headers
#include <bvis1/bvis1_manager.h>

#include <vul/vul_file.h>
#include <dbxml/dbxml_algos.h>

//: Constructor
dbskfg_composite_graph_transform_tool::
dbskfg_composite_graph_transform_tool() : 
  tableau_(0), 
  storage_(0),
  current_(0),
  training_mode_(false),
  training_filename_(),
  root_xml_(0)
{
  // define the keyboard condition for getting information
  apply_transform_ = vgui_event_condition(vgui_LEFT, vgui_MODIFIER_NULL, true);

  root_xml_     = new bxml_element("transform_training");

}


// -----------------------------------------------------------------------------
//: Destructor
dbskfg_composite_graph_transform_tool::
~dbskfg_composite_graph_transform_tool()
{
}

  
// -----------------------------------------------------------------------------
//: Set the tableau to work with
bool dbskfg_composite_graph_transform_tool::
set_tableau ( const vgui_tableau_sptr& tableau )
{
   
  if(!tableau)
  {
    return false;
  }

  if( tableau->type_name() == "dbskfg_composite_graph_tableau" )
  {
      if( !this->set_storage(bvis1_manager::instance()
                             ->storage_from_tableau(tableau)) )
      {
          return false;
      }
      this->tableau_.vertical_cast(tableau);
      return true;
  }

  tableau_ = 0;
  return false;
}


// -----------------------------------------------------------------------------
//: Set the storage class for the active tableau
bool dbskfg_composite_graph_transform_tool::
set_storage ( const bpro1_storage_sptr& storage_sptr)
{
  if (!storage_sptr)
  {
      return false;
  }

  //make sure its a vsol storage class
  if (storage_sptr->type() == "composite_graph")
  {
      this->storage_.vertical_cast(storage_sptr);
      return true;
  }

  this->storage_ = 0;
  return false;

}


// ----------------------------------------------------------------------------
//: Return the name of this tool
vcl_string dbskfg_composite_graph_transform_tool::
name() const 
{
    return "Composite Graph Transformer";
}


// ----------------------------------------------------------------------------
//: Allow the tool to add to the popup menu as a tableau would
void dbskfg_composite_graph_transform_tool::
get_popup(const vgui_popup_params& params, vgui_menu &menu)
{
    // Fill in later with stuff
    vcl_string on = "[x] ", off = "[ ] ";

    menu.add( "Training Mode",
              bvis1_tool_toggle, (void*)(&training_mode_));
    
    menu.add( "Training File Name",
              bvis1_tool_set_string, (void*)(&training_filename_));
              

}

// ----------------------------------------------------------------------------
//: Handle events
bool dbskfg_composite_graph_transform_tool::
handle( const vgui_event & e, const bvis1_view_tableau_sptr& view )
{

    // Display information of selected object
    if (apply_transform_(e))
    {

        return handle_apply_transform();
    }

    // rechange current
    if ( tableau_->get_highlighted_soview() )
    {

        current_=tableau_->get_highlighted_soview();
       
    }
   
    return false;
}


// -----------------------------------------------------------------------------
//: Display info of the selected object (node, edge)
bool dbskfg_composite_graph_transform_tool::
handle_apply_transform()
{
    if ( current_)
    {
        if ( current_->type_name() == "dbskfg_soview_transform" )
        {
            current_->print(vcl_cout);
          
            dbskfg_soview_transform* trans = 
                dynamic_cast<dbskfg_soview_transform*>(current_);
        
            // get coarse shock graph_process
            dbskfg_detect_transforms_sptr transformer=
                storage_->get_transforms();
         
            if ( training_mode_ )
            {
                int classify(0);
                vgui_dialog params("Classification");
                params.field("Enter 1 for Good Gap, 0 for Bad",classify );
                if (!params.ask())
                {
                    return false; // cancelled
                }

                bool flag(false);

                if ( classify == 1 )
                {
                    flag = true;
                }

                trans->transform()->write_transform(root_xml_,
                                                    training_filename_+".xml",
                                                    flag);
                trans->transform()->hidden_ = true;

                vcl_stringstream id;
                id<<trans->transform()->id_;
                
                dbskfg_composite_graph_fileio file;
                file.write_out_transform(trans->transform(),
                                         storage_->get_image(),
                                         training_filename_+"_id_"+id.str()+
                                         "_local_context.ps");
            }
            else
            { 
                
                transformer->apply_transforms(trans->transform()->id_,
                                              storage_->get_rag_graph());
          

            }
            update_composite_graph_display();

        }
        else if ( current_->type_name() == "dbskfg_soview_rag_node" )
        {
            current_->print(vcl_cout);
            dbskfg_soview_rag_node* rag = dynamic_cast<dbskfg_soview_rag_node*>
            (current_);

            dbskfg_composite_graph_fileio file;
            file.write_out(storage_->get_rag_graph(),storage_->get_image(),
                           rag->rag_node_id(),
                           1.0,
                           "manual",
                           vul_file::get_cwd());

        }
        else
        {
            current_->print(vcl_cout);
      }
    }

    return true;
}


//------------------------------------------------------------------------------
//: Update display of the xgraph
void dbskfg_composite_graph_transform_tool::
update_composite_graph_display()
{
  bvis1_manager::instance()->update_tableau(tableau_, storage_);
  bvis1_manager::instance()->post_redraw();
  return;
}

