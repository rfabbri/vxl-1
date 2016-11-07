function mywrite(fname,v)
  mopen(fname,'wb');
  v=v';
  for i=1:size(v,'*')
    mput(v(i),'d');
  end
  mclose();
endfunction
