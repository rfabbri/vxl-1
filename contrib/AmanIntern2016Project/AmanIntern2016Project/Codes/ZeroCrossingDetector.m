%Calculate Zero Crossing Points of a signal using linear interpolation 
%A zero-crossing is a point where the sign of a mathematical function changes (e.g. from positive to negative),
%represented by a crossing of the axis (zero value) in the graph of the function

x=0:0.01:8*pi;
x1=sin(x);
y1=cos(x).^2;
y1(1:157)=0;      %so that the effect of y1 on x1 starts from pi/2 radians, may or may not be needed
l=length(x);
sig=x1 + y1;
dcx=zeros(1,l);
plot(x,sig);
title('original signal');
% figure;
% imagesc(sig);
% title('Colormap of Sinusoid');

%First Derivative using Central Difference
for n=2:l-1
    dcx(n)=(sig(n+1)-sig(n-1))/(2*0.01);
end
dcx(1)=(sig(2)-sig(1))/0.01;
dcx(l)=(sig(l)-sig(l-1))/0.01;
figure
plot(x,dcx)
title('Central difference Derivative');
% figure;
% imagesc(dcx);
% title('Colormap of Central Difference');

max=[];
min=[];
i=0;
j=0;
for n=2:l
    %Iteratively checks if signs of successive points differ or are similar
    if((dcx(n) > 0) && (dcx(n-1) < 0))      %Signifies a Minima -ve to +ve
        i=i+1;
        min(i,1)=(0.01)*(n-1);
        min(i,2)=(0.01)*n;
        min(i,3)=dcx((n-1));
        min(i,4)=dcx(n);
        
    elseif((dcx(n) < 0) && (dcx(n-1) > 0))  %Signifies a Maxima +ve to -ve
        j=j+1;
        max(j,1)=(0.01)*(n-1);
        max(j,2)=(0.01)*n;
        max(j,3)=dcx((n-1));
        max(j,4)=dcx(n);
        
    end
end

zcpmax=zeros(1,j);                  %Zero Crossing Points of Maxima
zcpmin=zeros(1,i);                  %Zero Crossing Points of Minima

for n=1:j
    mn = (max(n,3)/abs(max(n,4)))*(max(n,2)-max(n,1));
    md = (max(n,3)/abs(max(n,4)))+1;
    m=mn/md;
    zcpmax(n)=max(n,1)+m;
end

for n=1:i
    mn = (min(n,3)/abs(min(n,4)))*(min(n,2)-min(n,1));
    md = (min(n,3)/abs(min(n,4)))+1;
    m=mn/md;
    zcpmin(n)=min(n,1)+m;
end


        




