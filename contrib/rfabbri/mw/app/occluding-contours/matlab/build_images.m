% read images and build 3D matrices with them

nimages=5;

mypath='ct-spheres/';

tmp=imread([mypath 'artifact_2_000' int2str(0) '_8bpp_600x.tif']);
nx = size(tmp,2);
ny = size(tmp,1);
img=zeros(ny,nx,nimages);
seg=zeros(ny,nx,nimages);
seg2=zeros(ny,nx,nimages);

se = strel('disk',2);

for i=0:nimages
  grayname=[mypath 'artifact_2_000' int2str(i) '_8bpp_600x.tif'];
  segname =[mypath 'artifact_2_000' int2str(i) '_600x_seg.png'];
  
  img(:,:,i+1) = imread(grayname);
  seg(:,:,i+1) = imread(segname);
  seg2(:,:,i+1) = imerode(seg(:,:,i+1),se);
end
img = double(img);
seg = double(seg);
seg2 = double(seg2);
