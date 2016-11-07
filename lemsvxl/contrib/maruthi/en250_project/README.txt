EN250 project 
Shock Based Models for Knee-Cartilage Segmentation using Appearance
--------------------------------------------------------------------

This folder holds all the code to build the top down shock model
using appearance costs. After you build this contrib folder an 
executable will come called xdetect. Grab the parameters file
and change parameters of the xml file accordingly to run the executable.

Type below ...

./xdetect -print-def-xml

Edit the parameters file with where the model is, input files, etc

Other changes that can be made to make computation faster
change the file 
brcv/seg/dbsks/dbsks_xshock_dp.cxx

Specifically lines 55 to 57

original
--------
   this->num_samples_c_ = 40; // degree-2 node: number of samples for child
   this->num_samples_c1_ = 40; // degree-3 node: number of samples for child1
   this->num_samples_c2_ = 40; // degree-3 node: number of samples for child2

faster
----------
   this->num_samples_c_ = 20; // degree-2 node: number of samples for child
   this->num_samples_c1_ = 20; // degree-3 node: number of samples for child1
   this->num_samples_c2_ = 20; // degree-3 node: number of samples for child2

Change the sampling to 20 to make it run faster. That was how the experiments
were run with it. 

Also in terms of the gui in ...

/lemsvxl/build/contrib/ntrinh/dbsksp_gui/

On linux some of the controls dont work so you might have to change the key
bindings, to create the model


change dbsksp_xshock_design_tool.cxx

example of key bindings used from cvs diff

< this->gesture_increase_graph_x_ = vgui_event_condition(vgui_key('1'),vgui_MODIFIER_NULL,true);
<   this->gesture_decrease_graph_x_ = vgui_event_condition(vgui_key('1'), vgui_SHIFT, true);
---
>   this->gesture_increase_graph_x_ = vgui_event_condition(vgui_key('v'),vgui_MODIFIER_NULL,true);
>   this->gesture_decrease_graph_x_ = vgui_event_condition(vgui_key('v'), vgui_SHIFT, true);
63,64c63,64
<   this->gesture_increase_graph_y_ = vgui_event_condition(vgui_key('2'),vgui_MODIFIER_NULL,true);
<   this->gesture_decrease_graph_y_ = vgui_event_condition(vgui_key('2'), vgui_SHIFT, true);
---
>   this->gesture_increase_graph_y_ = vgui_event_condition(vgui_key('b'),vgui_MODIFIER_NULL,true);
>   this->gesture_decrease_graph_y_ = vgui_event_condition(vgui_key('b'), vgui_SHIFT, true);
67,68c67,68
<   this->gesture_rotate_graph_cw_ = vgui_event_condition(vgui_key('3'),vgui_MODIFIER_NULL,true);
<   this->gesture_rotate_graph_ccw_ = vgui_event_condition(vgui_key('3'), vgui_SHIFT, true);
---
>   this->gesture_rotate_graph_cw_ = vgui_event_condition(vgui_key('l'),vgui_MODIFIER_NULL,true);
>   this->gesture_rotate_graph_ccw_ = vgui_event_condition(vgui_key('l'), vgui_SHIFT, true);
71,72c71,72
<   this->gesture_scale_up_ = vgui_event_condition(vgui_key('4'),vgui_MODIFIER_NULL,true);
<   this->gesture_scale_down_ = vgui_event_condition(vgui_key('4'), vgui_SHIFT, true);
---
>   this->gesture_scale_up_ = vgui_event_condition(vgui_key('m'),vgui_MODIFIER_NULL,true);
>   this->gesture_scale_down_ = vgui_event_condition(vgui_key('m'), vgui_SHIFT, true);


// Parameters used
<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
<vox_detect_xgraph><io detection_window_height="512" 
               detection_window_width="512" 
               input_image_extention=".png" 
               input_image_folder="/home/maruthi/Desktop/en250proj" 
               input_model_scales="63" 
               input_object_name="masktopCartilage_cropped" 
               input_xgraph_extention=".xml" 
               input_xgraph_folder="/home/maruthi/Desktop/en250proj"
               input_xgraph_geom_filename="xgraph_geom_model-knee_cartilage-root_vid-2.xml" 
               input_xgraph_name="cartilageModel" 
               output_object_folder="/home/maruthi/Desktop/en250proj/Results" 
               save_to_object_folder="off">
</io>
<web_directive> </web_directive>
</vox_detect_xgraph>


