curve_drawing_main.m is average_curves_final_12 modularized or broken into different files

First, you need to define which dataset to use. Record how many views it has. You will need this.

open curve_drawing_main.m and go to the definitions_12.m module. inside it, make changes to the variables numIM and all_views. read the comments.

inside load_pattern_12.m module put the path that accesses the .dat files below before crvs. you can to see this in load_curve_sketch_without_associations.m module and it is called inside this module. 

now go to the load_edge_and_curve_files.m module. there you will need to enter the paths where the cemv, edg, projmatrix, txt and png (or jpg) files are located.

in curve_Drawing_main it will be need enter paths the projmatrix files are
located in line 158 and 183

in read_association_attributes_12 you need to puth the path where attributes
from mcs_e are loacted

create a folder named ply inside dataset and put the path in
wrtite_curve_graph_to_vrml_12.m module line 90 and 124




obs:create a folder called export_fig inside the dataset which is called by the read_cons function.
indispensable functions: distinguishable_colors.m, read_cons.m, read_cem_file.m, read_association_attributes_9.m, load_edg.m, get_length.m, find_closest_point_on_curve.m, write_curve_graph_to_vrml_9.m
