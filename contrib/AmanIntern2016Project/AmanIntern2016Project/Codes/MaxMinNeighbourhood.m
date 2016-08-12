%Calculate Maxima and Minima points by analysing a neighbourhood around a point in a 1-D signal
%If the point in consideration has highest value in the given neighbourhood, it's a maxima 
%If the point in consideration has least value in the given neighbourhood, it's a minima 

x=0:0.1:8*pi;
sig=sin(x);
l=length(x);
sig = sig + .01 * randn(1, length(sig));            %Adding random noise to signal
sigsmooth=smooth(sig);
figure 
plot(x,sigsmooth);
title('Smooth Signal');
figure 
plot(x,sig);
title('Signal with Random Noise');
y=awgn(sin(x),200,'measured','linear');
figure 
plot(x,y);
title('Signal with Gaussian Noise');

maxima=[];
minima=[];
j=0;

%%Taking a 5 neighbourhood to the left and right of a point
for i=5:l-4
    if (sig(i)>sig(i-4) && sig(i)>sig(i-3) && sig(i)>sig(i-2) && sig(i)>sig(i-1) && sig(i)>sig(i+1) && sig(i)>sig(i+2) && sig(i)>sig(i+3) && sig(i)>sig(i+4))
        j=j+1;
        maxima(j,1) = i;
        maxima(j,2) = 0.01*i;
        
    end
end

k=0;
%%Taking a 5 neighbourhood to the left and right of a point
for i=5:l-4
    if (sig(i)<sig(i-4) && sig(i)<sig(i-3) && sig(i)<sig(i-2) && sig(i)<sig(i-1) && sig(i)<sig(i+1) && sig(i)<sig(i+2) && sig(i)<sig(i+3) && sig(i)<sig(i+4))
        k=k+1;
        minima(k,1) = i;
        minima(k,2) = 0.01*i;
    end
end




