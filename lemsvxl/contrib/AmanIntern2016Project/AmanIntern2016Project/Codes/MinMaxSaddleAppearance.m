%Script to look at minima and maxima
%Add paths
addpath(genpath('/home/krishna/Research/TAG/code'));

%Create Gaussian signal
x = [-10:.1:10];
mu = 0;
sigma = 3;
signal = normpdf(x,mu,sigma);
figure(1);
plot(x,signal)
%axis equal
%Get extrema
[xmax,imax,xmin,imin] = extrema(signal);
hold on

%Create small Gaussian noise
x = [-10:.1:10];
mu = 5;
sigma = 0.1;
noise = normpdf(x,mu,sigma);
plot(x,noise)

%Add noise to signal
noisySignal = signal + 0.1 * noise;
%plot
figure(2)
plot(x,noisySignal)
[xmax,imax,xmin,imin] = extrema(noisySignal);

