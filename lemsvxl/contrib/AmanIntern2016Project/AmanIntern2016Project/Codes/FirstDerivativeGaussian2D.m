%This Code is computes the maxima and minima points of 2-D Gaussian Function using the concept
%of finite differences using Forward Difference.

%Create Gaussian Function of size as per requirement 
gaussfunc=fspecial('gaussian',[5 5],2);
%gaussfunc=gaussfunc+0.2*rand(size(gaussfunc));   %Adding Random Noise if needed
[a, b]=size(gaussfunc);
dx=[];
dy=[];
surf(gaussfunc);
title('Curve of Gaussian Function');
figure;
imagesc(gaussfunc);
title('Colormap of Gaussian');

%First Derivative along X Direction 
for i=1:a
    for j=1:b-1
        dx(i,j) = gaussfunc(i,j+1) - gaussfunc(i,j);
    end
end
for n=1:a
    j=b;
    dx(n ,j)= gaussfunc(n ,j) - gaussfunc(n ,j-1);
end
figure;
surf(dx);
title('Curve for Derivative in X');
figure;
imagesc(dx);
title('Colormap of X Derivative');

%First Derivative along Y Direction
for i=1:a-1
    for j=1:b
        dy(i,j)=gaussfunc(i+1,j)-gaussfunc(i,j);
    end
end
for n=1:b
    i=a;
    dy(i,n)=gaussfunc(i,n)-gaussfunc(i-1,n);
end
figure;
surf(dy);
title('Curve for Derivative in Y');
figure;
imagesc(dy);
title('Colormap of Y Derivative');
