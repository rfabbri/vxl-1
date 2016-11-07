function v=myreadv(fname)

    v=myread(fname);
    v=reshape(v,2,size(v,1)/2);
    v=v';
