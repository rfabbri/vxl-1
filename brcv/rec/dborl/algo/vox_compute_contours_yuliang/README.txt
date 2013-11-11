Directions for stand-alone execution (processing of one image)

1. Build brcv/rec/dborl/algo/vox_compute_contours

2. After that is built you will find an executable there called dborl_compute_contours

3. Run ./dborl_compute_contours -print-def-xml. This will give you the xml file
with all the parameters. In the case of running a vox process through the
website this is what is generated automatically and sent to each of your
executables.

4. After running step 3, you will get a file called input_defaults.xml. Now all
you have to do is fill that in with your parameters. The algorithm parameters
are simple, the io set of xml flags is what you should concentrate on.

- You should change input_object_dir to where the image lives
- Change input_extension to whatever the image is
- By default the algorithm will save the result wherever the input_object_dir
  is, you can change that by turning save_to_object_folder on and specifying an
  ouput_edge_link_folder
- Output Extension can be .cem or .cemv
- I would turn prune contours off in your case, that is more used by me for object recognition
- Keep trace contours off as that is for black/white images
- By default contours will be saved, but you can save edges by turning on save edges

- Save curvelets is broken !!!!!!!!!!!

<io edge_extension=".edg" input_extention=".jpg"
input_object_dir="/home/mn/images" input_object_name="test"
output_edge_link_folder="/vision/projects/kimia/categorization/output"
output_extension=".cemv" prune_contours="on" save_curvelets="off"
save_edges="off" save_to_object_folder="off" trace_contours="off"> 
</io>

5. Run the algorithm, ./dborl_compute_contours -x input_defaults.xml

In terms of filling out the algorithm parameters, when you run step 3 you will
also get a params.xml file. This is the file vox uses to generate the webpage
when you fill in the parameters online. You should look at that file, when  you
are trying to figure what number to put in

<param attribute_label="Edge_Linking*Edge_Linking-linking_algo"
description="[Edge_Linking] Extract Image Contours , 0: Do not Link, 1: From the Link Graph, 2: Regular Contours" type="int" default="0" value="0"/>

For example, you should fill out the respective field in input_defaults.xml with
the exact number for extract regular contours.

