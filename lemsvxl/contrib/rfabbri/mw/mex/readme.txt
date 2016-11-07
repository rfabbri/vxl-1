Here is how to compile this directory:


In cmake, toggle BUILD_MATLAB_MEX to on and build shared libs to on as well.
Note that the official vxl has to be built as shared as well. This is no
problem at all on Linux. In fact, it makes things run faster.
Also, toggle advanced and set these variables to something like this:

 MATLAB_ENG_LIBRARY               /usr/local/matlab/bin/glnx86/libeng.so
 MATLAB_INCLUDE_DIR               /usr/local/matlab/extern/include
 MATLAB_MEX_LIBRARY               /usr/local/matlab/bin/glnx86/libmex.so
 MATLAB_MX_LIBRARY                /usr/local/matlab/bin/glnx86/libmx.so

To compile p2*dist

compile wildmagic first. Make sure the path to it is in the CMakeLists.txt.
Then copy the wild magic libs over to lemsvxl's lib directory.

Then ln -s the generated lib to have mexglx suffix and make sure it is in
matlab's path.


Matlab tips


In Linux, to debug your C++ code, do: 

  matlab -Dddd core

Inside ddd do 

  run -nojvm

To debug after a coredump within matlab, do

  matlab -D"ddd /home/matlab/bin/glnx86/MATLAB core"  
