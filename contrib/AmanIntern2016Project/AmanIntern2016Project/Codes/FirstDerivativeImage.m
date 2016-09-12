%This Code computes the maxima and minima points of a grayscale image or a 2-D Matrix using the
%concept of finite differences using Backward, Forward and Central Differences.

img=imread('cameraman.tif');
[a, b]=size(img);
dx=zeros(size(img));
dy=zeros(size(img));
imshow(img);

%First Derivative along X Direction
for i=1:a
    for j=1:b-1
        dx(i,j) = img(i,j+1) - img(i,j);
    end
end
for n=1:a
    j=b;
    dx(n ,j)= img(i ,j) - img(i ,j-1);
end
figure;
imshow(dx);
title('X Derivative');

%First Derivative Along Y Direction

for i=1:a-1
    for j=1:b
        dy(i,j)=img(i+1,j)-img(i,j);
    end
end
for n=1:b
    i=a;
    dy(i,n)=img(i,n)-img(i-1,n);
end
figure;
imshow(dy);
title('Y Derivative');

%Matrix m contains the gradient magnitude at each point of the image
mag=sqrt(dx.^2 + dy.^2);
figure;
imshow(mag);
title('gradient magnitude');