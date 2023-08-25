function v=myreadv(fname)

    v=myread(fname);
    v=reshape(v,3,size(v,1)/3);
    v=v';
