%This Code is computes the second derivatives of 2-D Gaussian Function using the concept
%of finite differences using Forward Difference.

%Create Gaussian Function of size as per requirement 
gaussfunc=fspecial('gaussian',[512 512],170);
%gaussfunc=gaussfunc+0.2*rand(size(gaussfunc));   %Adding Random Noise if needed
[a, b]=size(gaussfunc);
ddx=zeros(size(gaussfunc));
ddy=zeros(size(gaussfunc));
dxy=zeros(size(gaussfunc));
surf(gaussfunc,'LineStyle','none');
title('Curve of Gaussian Function');
figure;
imagesc(gaussfunc);
title('Colormap of Gaussian');

%Second Derivative in X
for i=1:a
    for j=1:b-1
        ddx(i,j) = gaussfunc(i,j+1) - gaussfunc(i,j);
    end
end
for n=1:a
    j=b;
    ddx(n ,j)= gaussfunc(i ,j) - gaussfunc(i ,j-1);
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
surf(ddx,'LineStyle','none');
title('Curve for 2nd Derivative in X');
figure;
imagesc(ddx);
title('Colormap of 2nd X Derivative');

%Second Derivative in Y
for i=1:a-1
    for j=1:b
        ddy(i,j)=gaussfunc(i+1,j)-gaussfunc(i,j);
    end
end
for n=1:b
    i=a;
    ddy(i,n)=gaussfunc(i,n)-gaussfunc(i-1,n);
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
surf(ddy,'LineStyle','none');
title('Curve for 2nd Derivative in Y');
figure;
imagesc(ddy);
title('Colormap of 2nd Y Derivative');

%Partial Derivative wrt x and y(del x del y)
for i=1:a
    for j=1:b-1
        dxy(i,j) = gaussfunc(i,j+1) - gaussfunc(i,j);
    end
end
for n=1:a
    j=b;
    dxy(n ,j)= gaussfunc(i ,j) - gaussfunc(i ,j-1);
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
surf(dxy,'LineStyle','none');
title('Curve for 2nd Derivative wrt x and y');
figure;
imagesc(dxy);
title('Colormap of 2nd Derivative wrt x and y');
