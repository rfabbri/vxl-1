%This Code computes the Second Derivative of a grayscale image or a 2-D Matrix using the
%concept of finite differences using Forward Differences.

img=imread('cameraman.tif');
%img=imnoise(img,'gaussian');                  %Adding noise to an image
[a, b]=size(img);
ddx=zeros(size(img));
ddy=zeros(size(img));
dxy=zeros(size(img));
imshow(img);

%2nd Derivative in X
for i=1:a
    for j=1:b-1
        ddx(i,j) = img(i,j+1) - img(i,j);
    end
end
for n=1:a
    j=b;
    ddx(n ,j)= img(i ,j) - img(i ,j-1);
end
for i=1:a
    for j=1:b-1
        ddx(i,j) = ddx(i,j+1) - ddx(i,j);
    end
end
for n=1:a
    j=b;
    ddx(n ,j)= ddx(i ,j) - ddx(i ,j-1);
end
figure;
imshow(ddx);
title('2nd Derivative in X');

%2nd Derivative in Y
for i=1:a-1
    for j=1:b
        ddy(i,j)=img(i+1,j)-img(i,j);
    end
end
for n=1:b
    i=a;
    ddy(i,n)=img(i,n)-img(i-1,n);
end
for i=1:a-1
    for j=1:b
        ddy(i,j)=ddy(i+1,j)-ddy(i,j);
    end
end
for n=1:b
    i=a;
    ddy(i,n)=ddy(i,n)-ddy(i-1,n);
end
figure;
imshow(ddy);
title('2nd Derivative in Y');

%Partial Derivative wrt x and y(del x del y)
for i=1:a
    for j=1:b-1
        dxy(i,j) = img(i,j+1) - img(i,j);
    end
end
for n=1:a
    j=b;
    dxy(n ,j)= img(i ,j) - img(i ,j-1);
end
for i=1:a-1
    for j=1:b
        dxy(i,j)=dxy(i+1,j)-dxy(i,j);
    end
end
for n=1:b
    i=a;
    dxy(i,n)=dxy(i,n)-dxy(i-1,n);
end
figure;
imshow(dxy);
title('2nd Derivative wrt x and y');

%The sum and product of eigenvalues of matrix at each point is computed by first constructing the
%Hessian matrix
clearvars sumeig prodeig sum prod 
sumeig=zeros(256);
prodeig=zeros(256);
for i=1:256
    for j=1:256
        hess=[ddx(i,j) dxy(i,j);dxy(i,j) ddy(i,j)];
        e=eig(hess)';
        sumeig(i,j)=sum(e);
        prodeig(i,j)=prod(e);
    end
end

figure;
imshow(sumeig);
title('sum of eigenvalues');
figure;
imshow(prodeig);
title('product of eigenvalues');


