This code works on commandline in windows. After building this go to the bin folder and open 
D:\LEMSVXL\bin\contrib\Anant\HindSiteReloaded\web_system_executables

Open the HindSiteReloaded_Combined_Processes vcc project and build it. 

Get the fragments you want to run the code on in a folder organized as:
Fragments(Main Folder)
	Frag01
		frag01_front.con
		frag01_back.con
		frag01_back.jpg
		frag01_front.jpg
	Frag02
	Frag03
	...
Create an empty "Output" folder with a folder "111" and a folder "user" in it. This will have the results of the iters. It would be a goood idea for someone to modify the code such that these folders get created themselves.

Now open the params.xml file and edit paths for the frags and the ouptut folder.

Now run the code as HindSiteReloaded_Combined_Process.exe -x params.xml
Note: You can change parameters in the params file like no. of iterations, no. of top states to save etc.

Now the code produces folders like iter0, iter1 etc... and a status file and a frag pairs xml file. The status file is to check the status of the current job. The xml file stores all frag pairs.
All these folders have 4 files .sest,.mtc, .puso, .frg and an xml.
I have figured out the following(Could be wrong!)
	1) .sest: Gives current status of state including frags it includes, their relative poise, and final curve 		drawn at that point in puzzle. Remember a state is a partially solved puzzle.
	2) .mtc: Stores all matches. Here intmap is printed out which is basically vector of pair of indices of 	level 3 curve points of the 2 frags. 
	3) .puso: Similar to .frg files.
	4) .frg: tells the current points of all frgaments with transforms
	5) output.xml: This is the file to be used in the visualizer to view the partially solved puzzle. It has 		info about the various files paths etc.

Using the Visualizer:
1) Build the HindSite Reloaded_vis project
2) Click on load. In first XML file enter path of one output.xml file depending on which state of puzzle u want to see. In second input path of params.xml file.
A complete description of how the prog runs and what happens at each stage(to the best of my understanding!) may be found in another txt file. Other useful resources are the masters thesis of Can Aras and a User Guide. Though I have tried my best to explain fully major aspects of program to save you time which i wasted trying to debug everything...