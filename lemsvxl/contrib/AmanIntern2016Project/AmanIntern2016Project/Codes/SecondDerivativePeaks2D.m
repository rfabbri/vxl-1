%{
This Code Computes following parameters for a Peaks Function of custom size
1)First Derivative along X Direction
2)First Direction along Y Direction
3)Gradient Magnitude at each point of the image
4)Second Derivative along X Direction
5)Second Derivative along Y Direction
6)Partial Derivative with respect to x and y (df/dxdy)
These parameters are computed using the concept of Central Finite Difference.
%}

func=peaks(20);
subplot(3,3,1);
imagesc(func);
axis equal;
title('colormap of function');
[a, b]=size(func);
dx=zeros(a);
dy=zeros(a);

%First Derivative X
for i=1:a
    for j=2:b-1
        dx(i,j) = (func(i,j+1) - func(i,j-1))/2;
    end
end
for n=1:a
    j=1;
    dx(n ,j)= func(n ,j+1) - func(n ,j);
end
for n=1:a
    j=b;
    dx(n ,j)= func(n ,j) - func(n ,j-1);
end
subplot(3,3,2);
imagesc(dx);
axis equal;
title('X Derivative');

%First Derivative in Y

for i=2:a-1
    for j=1:b
        dy(i,j)=(func(i+1,j)-func(i-1,j))/2;
    end
end
for n=1:b
    i=a;
    dy(i,n)=func(i,n)-func(i-1,n);
end
for n=1:b
    i=1;
    dy(i,n)=func(i+1,n)-func(i,n);
end
subplot(3,3,3);
imagesc(dy);
axis equal;
title('Y Derivative');

%Gradient Magnitude
mag=sqrt(dx.^2 + dy.^2);
subplot(3,3,4);
imagesc(mag);
axis equal;
title('gradient magnitude');

ddx=zeros(a);
ddy=zeros(a);
dxy=zeros(a);
%2nd Derivative in X
for i=1:a
    for j=2:b-1
        ddx(i,j) = (dx(i,j+1) - dx(i,j-1))/2;
    end
end

for n=1:a
    j=1;
    ddx(n ,j)= dx(n ,j+1) - dx(n ,j);
end
for n=1:a
    j=b;
    ddx(n ,j)= dx(n ,j) - dx(n ,j-1);
end
subplot(3,3,5);
imagesc(ddx);
axis equal;
title('2nd Derivative in X');

%2nd Derivative in Y
for i=2:a-1
    for j=1:b
        ddy(i,j)=(dy(i+1,j)-dy(i-1,j))/2;
    end
end
for n=1:b
    i=1;
    ddy(i,n)=dy(i+1,n)-dy(i,n);
end
for n=1:b
    i=a;
    ddy(i,n)=dy(i,n)-dy(i-1,n);
end
subplot(3,3,6);
imagesc(ddy);
axis equal;
title('2nd Derivative in Y');

%Partial Derivative wrt x and y (del x del y)
for i=1:a
    for j=2:b-1
        dxy(i,j) = (dx(i,j+1) - dx(i,j-1))/2;
    end
end
for n=1:a
    j=1;
    dxy(n ,j)= dx(i ,j+1) - dx(i ,j);
end
for n=1:a
    j=b;
    dxy(n ,j)= dx(i ,j) - dx(i ,j-1);
end
for i=2:a-1
    for j=1:b
        dxy(i,j)=(dxy(i+1,j)-dxy(i-1,j))/2;
    end
end
for n=1:b
    i=1;
    dxy(i,n)=dxy(i+1,n)-dxy(i,n);
end
for n=1:b
    i=a;
    dxy(i,n)=dxy(i,n)-dxy(i-1,n);
end
subplot(3,3,7);
imagesc(dxy);
axis equal;
title('2nd Derivative wrt x and y');

%The sum and product of eigenvalues of matrix at each point is computed by first constructing the
%Hessian matrix
clearvars sumeig prodeig sum prod 
sumeig=zeros(a);
prodeig=zeros(a);
for i=1:a
    for j=1:b
        hess=[ddx(i,j) dxy(i,j);dxy(i,j) ddy(i,j)];
        e=eig(hess)';
        sumeig(i,j)=sum(e);
        prodeig(i,j)=prod(e);
    end
end

subplot(3,3,8);
imagesc(sumeig);
axis equal;
title('sum of eigenvalues');
subplot(3,3,9);
imagesc(prodeig);
axis equal;
title('product of eigenvalues');



