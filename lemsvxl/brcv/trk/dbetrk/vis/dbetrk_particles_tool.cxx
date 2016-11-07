#include <dbetrk/vis/dbetrk_particles_tool.h>
#include <bvis1/bvis1_manager.h>
#include <bvis1/bvis1_view_tableau.h>
#include <vgui/vgui_style.h>
#include <dbetrk/dbetrk_edge.h>
#include <vgui/vgui.h>
#include <vgui/vgui_dialog.h>
#include <vgui/vgui_projection_inspector.h>
#include <bgui/bgui_vsol_soview2D.h>
#include <vsol/vsol_point_2d.h>
#include <dbctrk/dbctrk_utils.h>

dbetrk_particles_tool::dbetrk_particles_tool()
{
   /* if (!(ep = engOpen("\0"))) {
         fprintf(stderr, "\nCan't start MATLAB engine\n");
         
     }
    */
    neighbor_style_ = vgui_style::new_style(1.0, 0.0, 0.0, 3.0, 3.0);
}

dbetrk_particles_tool::~dbetrk_particles_tool()
{
  //  engEvalString(ep, "close;");

}

bool
dbetrk_particles_tool::set_tableau( const vgui_tableau_sptr& tableau )
{
  bgui_vsol2D_tableau_sptr temp_tab;
  temp_tab.vertical_cast(tableau);
  if (!temp_tab.ptr())
    return false;
    
  if (this->set_storage(bvis1_manager::instance()->storage_from_tableau(tableau))){
    tableau_ = temp_tab;
    return true;
  }
  return false;
}

bool
dbetrk_particles_tool::set_storage ( const bpro1_storage_sptr& storage)
{
  if (!storage.ptr())
    return false;
  //make sure its a vsol storage class
  if (storage->type() == "edgetrk"){
    storage_.vertical_cast(storage);
    return true;
  }
  return false;
}

bgui_vsol2D_tableau_sptr
dbetrk_particles_tool::tableau()
{
  return tableau_;
}

dbetrk_storage_sptr
dbetrk_particles_tool::storage()
{
  dbetrk_storage_sptr edgetrk_storage;
  edgetrk_storage.vertical_cast(storage_);
  return edgetrk_storage;
}



bool
dbetrk_particles_tool::handle( const vgui_event & e, const bvis1_view_tableau_sptr& view )
{
  float ix, iy;
  vgui_projection_inspector().window_to_image_coordinates(e.wx, e.wy, ix, iy);

  
  if( e.type == vgui_MOTION){
  tableau_->motion(e.wx, e.wy);
    vgui_soview2D* curr_obj =  (vgui_soview2D*)tableau_->get_highlighted_soview();
 
      object_ = (dbetrk_soview2D*)curr_obj;
      curr_edge_node_ = NULL;
      if( curr_obj && curr_obj->type_name() == "dbetrk_soview2D"){   
        curr_edge_node_ = object_->edgetrk_sptr();
      }
      bvis1_manager::instance()->post_overlay_redraw();
    
  }
   int frame = view->frame();

   if( e.type == vgui_DRAW_OVERLAY){
    if(!curr_edge_node_)
        return false;

   for(unsigned int i=0;i<curr_edge_node_->particles.size() && i<6;i++)
   {    
   
       if(curr_edge_node_->frame_!=frame)
       {
       float r,g,b;
       utils::set_rank_colors(i,r,g,b);
       for(unsigned int j=0;j<curr_edge_node_->particles[i].size();j++)
       {
           
        if(curr_edge_node_->particles[i][j]->frame_==frame) 
        {
            neighbor_style_->rgba[0] =r  ;
            neighbor_style_->rgba[1] =g  ;
           neighbor_style_->rgba[2] =b   ;
       
          neighbor_style_->point_size=4.0;
         neighbor_style_->apply_all();  
         dbetrk_soview2D(curr_edge_node_->particles[i][j]).draw();
       }
       }
      
     }
    
   }
   }
    if ( e.type == vgui_KEY_PRESS && e.key == 's' &&  vgui_SHIFT  ) {

       static int number=0;

       vgui_dialog inputfile("Enter sample number");
       inputfile.field("Sample number",number);
       if(!inputfile.ask())
            return true;
  
       
       vcl_vector<vgui_soview*> selcted_objects;
    selcted_objects = tableau_->get_selected_soviews();

    for (unsigned int i = 0; i<selcted_objects.size() && i<1; i++) {
   
    if(((dbetrk_soview2D*)selcted_objects[i])->type_name()=="dbetrk_soview2D")
    {
     nodes_to_draw.clear();
        dbetrk_edge_sptr currnode=((dbetrk_soview2D*)selcted_objects[i])->edgetrk_sptr();
     if((int)currnode->particles.size()>number)
     {
        for(unsigned int i=0;i<currnode->particles[number].size();i++)
        {
             nodes_to_draw.push_back(currnode->particles[number][i]);
        }
     }
    }
    tableau_->deselect_all();
    tableau_->post_redraw();
    
  }
  }


     if ( e.type == vgui_KEY_PRESS && e.key == 'l' &&  vgui_SHIFT  ) {

       
       
       vcl_vector<vgui_soview*> selcted_objects;
    selcted_objects = tableau_->get_selected_soviews();

    for (int i = 0; i<(int)selcted_objects.size() && i<1; i++) {
//   engEvalString(ep, "figure;"); 
    if(((dbetrk_soview2D*)selcted_objects[i])->type_name()=="dbetrk_soview2D")
    {
        dbetrk_edge_sptr currnode=((dbetrk_soview2D*)selcted_objects[i])->edgetrk_sptr();
        double * costarray=new double[currnode->particles.size()];
        for(i=0;i<(int)currnode->particles.size();i++)
        {
            costarray[i]=currnode->prob_particles[i];
        }

     /*   prepare_vector("costarray",costarray,currnode->particles.size(),ep);
        engEvalString(ep, "plot(costarray./sum(costarray));");
        printf("Hit return to exit\n\n");
                fgetc(stdin); */

    }

    }

     }
  return false;
}



//: Return the name of this tool
vcl_string 
dbetrk_particles_tool::name() const
{

  return "Particles Inspector";

}

/*void dbetrk_particles_tool::prepare_vector(char matlab_name[], double data[], int dim1,Engine *ep) {
  mxArray *matlab_data = NULL;
  matlab_data = mxCreateDoubleMatrix(1, dim1, mxREAL);
  // Give the variable a name in Matlab 
  memcpy((void *)mxGetPr(matlab_data), (void *)data, dim1*sizeof(*data));

  engPutVariable(ep,matlab_name,matlab_data);
 // Free the memory 
  mxDestroyArray(matlab_data);
}
*/
