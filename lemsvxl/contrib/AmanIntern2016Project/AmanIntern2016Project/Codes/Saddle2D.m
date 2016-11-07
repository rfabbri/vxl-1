x=-5:0.1:7;
y=-5:0.1:7;
a=length(x);
b=length(y);

[X,Y]=meshgrid(x,y);

% for i=1:a
%     for j=1:b
%         %func(i,j)=x(i)^2.*y(j)^2 - x(i)^2 - y(j)^2;
%         %func(i,j)=3*x(i).*y(j)+x(i)^3 + y(j)^3 +3;
%         func(i,j)=x(i)^3+x(i)^2.*y(j)-y(j)^2-4*y(j);
%     end
% end

func=X.^3 + (X.^2).*Y - Y.^2 -4*Y;

fig=surf(func);
set(fig,'LineStyle','none')
               
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


%del x del y
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

fl=zeros(3,3);
k=0;

for i=2:a-1
    for j=2:b-1
        fl = func(i-1:i+1,j-1:j+1);
        if ( fl(2,2) == max(max(fl)))  
            k=k+1;
        end
    end
end

k=0;
sp=[];

%sp is matrix of saddle points
for i=1:a
    for j=1:b
        hess=[ddx(i,j) dxy(i,j);dxy(i,j) ddy(i,j)];
        d=det(hess);
        e=eig(hess)';
        if(dx(i,j)==0 && dy(i,j)==0)
            k=k+1;
            sp(k,1) = -5 + 0.1*i;
            sp(k,2) = -5 + 0.1*j;
        end
     
    end
end





