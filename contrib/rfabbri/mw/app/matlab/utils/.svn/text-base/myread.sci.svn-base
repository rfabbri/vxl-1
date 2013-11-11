function a = myread(fname)

fid=mopen(fname,'rb');

// determine filesize

mseek(0,fid,'end');
sz=mtell(fid);
mseek(0,fid,'set');

// 8 bytes for double
nrows=sz/8;

a = mget(nrows,'d',fid);

mclose(fid);
endfunction

// -----------------------------------------------
function a = myreadv(fname)

fid=mopen(fname,'rb');

// determine filesize

mseek(0,fid,'end');
sz=mtell(fid);
mseek(0,fid,'set');

// 16  = 8 bytes for double, times 2
nrows=sz/16;

a = zeros(nrows,2);
for i=1:nrows
  a(i,1) = mget(1,'d',fid);
  a(i,2) = mget(1,'d',fid);
end

mclose();
endfunction
