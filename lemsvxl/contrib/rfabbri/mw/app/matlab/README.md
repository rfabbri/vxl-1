# How to run Matlab MEX requisites

- compile enabling matlab mex in ccmake
- the lib will be in lemsvxl-bin/lib
- put that folder in matlab PATH j
- rename the mex library file to extension, eg: libsynthetic_data.so to synthetic_data.mexa64
- run a simple test synthetic_data('22,37'), for instance;
- if it complains of some library, do as follows
  - for libstdc\* simply go to /usr/local/MATLAB/R2022a/sys/os/glnxa64 and rm or
    rename the libstdc\* libs to some backup name
- now **restart matlab** and simply rerun synthetic_data('22,37');
