# How to run Matlab MEX requisites
- See lemsvxl/contrib/rfabbri/mw/app/matlab/README.md

# Synthetic data

- Curves, sphere, occoluding contours and cameras are all generated and projected In mw/mex/synthetic_data.cxx 


# Matlab problems & solutions

## Ubuntu 22.04.1 LTS

If you cannot plot anything, first test:
plot(1:10)
If no window shows up, then quit matlab and start using:
MESA_LOADER_DRIVER_OVERRIDE=i965 matlab

If it works, update your ~/.bash_profile accordingly

