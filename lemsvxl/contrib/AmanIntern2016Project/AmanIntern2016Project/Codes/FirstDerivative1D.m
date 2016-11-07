%This Code computes the maxima and minima points of a signal using the concept of finite
%differences using Backward, Forward and Central Differences.

x=0:0.2:2*pi;
sig=sin(x);
sig=sig + 0.2*randn(1,length(sig));         %Adding Noise
dfx=zeros(1,length(sig));
dbx=zeros(1,length(sig));
dcx=zeros(1,length(sig));
plot(x,sig);
title('sinusoidal signal');
figure;
imagesc(sig);
title('Colormap of Sinusoid');

%First Derivative using Forward Difference
for n=1:31
    dfx(n)=sig(n+1)-sig(n);
end
dfx(32)=sig(32)-sig(31);
figure
plot(x,dfx)
title('forward difference');
figure;
imagesc(dfx);
title('Colormap of Forward Difference');

%First Derivative using  Backward Difference
for n=2:32
    dbx(n)=sig(n)-sig(n-1);
end
dbx(1)=sig(2)-sig(1);
figure
plot(x,dbx)
title('backward difference');
figure;
imagesc(dbx);
title('Colormap of Backward Difference');

%First Derivative using  Central Difference
for n=2:31
    dcx(n)=(sig(n+1)-sig(n-1))/2;
end
dcx(1)=sig(2)-sig(1);
dcx(32)=sig(32)-sig(31);
figure
plot(x,dcx)
title('central difference');
figure;
imagesc(dcx);
title('Colormap of Central Difference');

