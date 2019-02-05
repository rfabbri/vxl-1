mydir="~/data/rfabbri/out-tmp/";
fname = dir(mydir + '*.extrinsic');
nviews = length(fname);
extrinsics = zeros(4,3,nviews);

for i=1:nviews
  extrinsics(:,:,i) = load(mydir + fname(i).name);
end
