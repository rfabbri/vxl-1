//this is contrib/bm/dsm2/Templates/dsm2_manager_pixels+uint.dsm2_node_gaussian_full.d.2.dsm2_transition_table.uint.uint.double-.cxx
#include<dsm2/dsm2_manager_pixels.txx>
#include<dsm2/dsm2_node_gaussian.h>
#include<dsm2/dsm2_transition_map.h>
#include<dsm2/dsm2_state_machine.h>
DSM2_MANAGER_PIXELS_INSTANTIATE(unsigned, dsm2_state_machine<dsm2_node_gaussian<bsta_gaussian_full<double,2> >, dsm2_transition_map<unsigned,unsigned,double> >);