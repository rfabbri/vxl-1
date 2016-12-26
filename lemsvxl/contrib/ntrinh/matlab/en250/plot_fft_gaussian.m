% Plot Fourier Transform of gaussian
% (c) Nhon Trinh
% Date: 2/12/2008

clear all;
close all;

sigma = 0.1;

% Gaussian
x = [-2:0.01:2];
y = exp(-(x/sigma).^2/2);
figure(1); 

subplot(1, 2, 1);
plot(x, y);
axis([-0.5, 0.5, 0, 1]);
title('Gaussian function');
grid on;

% Fourier Transform of a Gaussian
Y = fft(y, 1024);
Pyy = Y.* conj(Y) / 1024;
f = 100*(0:512)/1024;

subplot(1, 2, 2); plot(f(1:128),Pyy(1:128))
title('Fourier Tranform of a Gaussian')
xlabel('frequency (Hz)');
grid on;

% super Gaussian
y2 = exp(-(x/sigma).^4/2);
figure(2); 
subplot(1, 2, 1); plot(x, y2);
axis([-0.5, 0.5, 0, 1]);
grid on;
title('Super-Gaussian function (^4)');

% Fourier Transform of a Gaussian
Y2 = fft(y2, 1024);
Pyy2 = Y2.* conj(Y2) / 1024;
f2 = 100*(0:512)/1024;

subplot(1, 2, 2); plot(f2(1:128), Pyy2(1:128));
title('Fourier Tranform of a super-Gaussian (^4)')
xlabel('frequency (Hz)');
grid on;



% super Gaussian ^6
y3 = exp(-(x/sigma).^6/2);
figure(3); 
subplot(1, 2, 1); plot(x, y3);
axis([-0.5, 0.5, 0, 1]);
title('Super-Gaussian function (^6)');
grid on;

% Fourier Transform of a Gaussian
Y3 = fft(y3, 1024);
Pyy3 = Y3.* conj(Y3) / 1024;
f3 = 100*(0:512)/1024;

subplot(1, 2, 2); plot(f3(1:128), Pyy3(1:128))
title('Fourier Tranform of a super-Gaussian (^6)')
xlabel('frequency (Hz)');
grid on;


