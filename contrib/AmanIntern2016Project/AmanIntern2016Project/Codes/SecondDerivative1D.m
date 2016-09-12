%This Code computes the second derivative of a signal using the concept of finite differences using
%Backward, Forward and Central Differences.

x=0:0.2:2*pi;
sig=sin(x);
sig=sig+0.2*rand(1,length(sig));                    %Adding Noise
dfx=zeros(1,length(sig)); 
dbx=zeros(1,length(sig));
dcx=zeros(1,length(sig));
plot(x,sig);
title('sinusoidal signal');
figure;
imagesc(sig);
title('Colormap of Sinusoid');

%Forward Difference 2nd Derivative
for n=1:31
    dfx(n)=sig(n+1)-sig(n);
end
dfx(32)=sig(32)-sig(31);
for n=1:31
    dfx(n)=dfx(n+1)-dfx(n);
end
dfx(32)=dfx(32)-dfx(31);
figure
plot(x,dfx)
title('forward difference 2nd Derivative');
figure;
imagesc(dfx);
title('Colormap of Forward Difference');

%Backward Difference 2nd Derivative
for n=2:32
    dbx(n)=sig(n)-sig(n-1);
end
dbx(1)=sig(2)-sig(1);
for n=2:32
    dbx(n)=dbx(n)-dbx(n-1);
end
dbx(1)=dbx(2)-dbx(1);
figure
plot(x,dbx)
title('backward difference 2nd Derivative');
figure;
imagesc(dbx);
title('Colormap of Backward Difference');

%2nd Derivative using Central Difference 
for n=2:31
    dcx(n)=(sig(n+1)-sig(n-1))/2;
end
dcx(1)=sig(2)-sig(1);
dcx(32)=sig(32)-sig(31);
for n=2:31
    dcx(n)=(dcx(n+1)-dcx(n-1))/2;
end
dcx(1)=dcx(2)-dcx(1);
dcx(32)=dcx(32)-dcx(31);
figure
plot(x,dcx)
title('central difference 2nd Derivative');
figure;
imagesc(dcx);
title('Colormap of Central Difference');

