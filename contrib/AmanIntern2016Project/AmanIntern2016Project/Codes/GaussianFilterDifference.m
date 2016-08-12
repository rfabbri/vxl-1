%This code aims to compute difference between successive Gaussian Filters in order to see the
%effect of increasing sigma and/or kernel size
%The sigma and kernel size can be set as per requirement

i=0.1:0.1:20;
dif=zeros(1,length(i));
k=1;

for i=0.1:0.1:20
    f=fspecial('gaussian',[7 7],i);
    if i>0.1
        dif(k)=abs(max(max(f-pre)));
        k=k+1;
    end
    pre=f;
    
end

i=0.1:0.1:20;
plot(i,dif);
xlabel('Value of Sigma');
ylabel('Absolute Maximum of difference between consecutive filters (differ by sigma=0.5)');
title('3x3 Gaussian Filter');
grid on;