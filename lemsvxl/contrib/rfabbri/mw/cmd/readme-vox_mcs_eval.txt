Evaluating multiview curve stereo
---------------------------------

use the script mcs_eval:

1- create an input.xml (examples inside 'inputs' directory),
and edit it with the appropriate directories.

2 - mcs_eval <input.xml>

3 - Plot the curves in matlab:

3.A)
  - edit the file read_pr_mcs to point to your eval_results.xml

  - read_pr_dtheta

3.B)
  - cd to eval directory
  - read_all_pr
  - plot_pr

To speed up:
  - edit read_save_pr
  - run read_save_pr
    - this will read all xml files,
    generate precision/recall structures,
    and write this to a quickly loadable pr.mat file
    - then load pr.mat every thime you want to plot using plot_pr


Practicalities
--------------

- seems the use of first_to_second_ratio without lonely threshold has worked
  best, and this is what I report in CVPR'10.


