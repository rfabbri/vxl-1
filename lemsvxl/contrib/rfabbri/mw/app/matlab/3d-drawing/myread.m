function a = myread(fname)

fid = fopen(fname,'r');
a = fread(fid,'double');
fclose(fid);
