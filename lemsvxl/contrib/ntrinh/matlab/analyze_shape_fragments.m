% This script analyzes the shape fragment from a shape class
% (c) Nhon Trinh
% Date: Sep 22, 2008

close all;
clear all;

%% input
input_folder = 'D:\vision\data\symseg\ETHZShapeClasses\training';
data_filename = 'giraffes-eid_8-negative.txt';

%% process data

% parse input data
data_file = fullfile(input_folder, data_filename);
data = dlmread(data_file, ' ', 1, 0);


% x0 = data(:, 1);
% y0 = data(:, 2);
% r0 = data(:, 3);
% psi0 = data(:, 4);
% phi0 = data(:, 5);
% 
% x1 = data(:, 6);
% y1 = data(:, 7);
% r1 = data(:, 8);
% psi1 = data(:, 9);
% phi1 = data(:, 10);
% 
% scale = data(:, 11);

start = 1;
nbins = 9;

lhog1 = data(:, start : start+nbins-1);
start = start + nbins;

lhog2 = data(:, start : start+nbins-1);
start = start + nbins;

lhog3 = data(:, start : start+nbins-1);
start = start + nbins;

lhog4 = data(:, start : start+nbins-1);
start = start + nbins;

rhog1 = data(:, start : start+nbins-1);
start = start + nbins;

rhog2 = data(:, start : start+nbins-1);
start = start + nbins;

rhog3 = data(:, start : start+nbins-1);
start = start + nbins;

rhog4 = data(:, start : start+nbins-1);
start = start + nbins;

% % vars derived from raw data
% len = sqrt((x1-x0).^2 + (y1-y0).^2);
% chord_psi = atan2(y1-y0, x1-x0);
% dpsi0 = asin(sin(psi0 - chord_psi));
% dpsi1 = asin(sin(psi1 - chord_psi));

plot_xdesc = 0;
if (plot_xdesc)
 
% plot results
figure(1);
hist(len ./ scale);
xlabel('len / scale');
title('hist of len / scale');

figure(2);
hist(chord_psi);
xlabel('chord-psi');
title('hist of chordpsi');

figure(3);
hist(dpsi0);
xlabel('psi0 - chordpsi')
title('hist of psi0-chordpsi');

figure(4);
hist(dpsi1);
xlabel('psi1 - chordpsi');
title('hist of psi1-chordpsi');

figure(5);
hist(r0 ./ scale);
xlabel('r0 / scale');
title('hist of r0 / scale');

figure(6);
hist(r1 ./ scale);
xlabel('r1 / scale');
title('hist of r1 / scale');

figure(7);
hist(phi0);
xlabel('phi0');
title('hist of phi0');

figure(8);
hist(phi1);
xlabel('phi1');
title('hist of phi1');

end;

%  frag = [len ./ scale, dpsi0, dpsi1, r0 ./ scale, r1 ./scale, phi0, phi1];



%% plot hog
if (0)
lmag1 = sqrt(sum(lhog1 .* lhog1, 2));
lhog1 = lhog1 ./ repmat(lmag1, [1, 9]);

lmag2 = sqrt(sum(lhog2 .* lhog2, 2));
lhog2 = lhog2 ./ repmat(lmag2, [1, 9]);

lmag3 = sqrt(sum(lhog3 .* lhog3, 2));
lhog3 = lhog3 ./ repmat(lmag3, [1, 9]);

lmag4 = sqrt(sum(lhog4 .* lhog4, 2));
lhog4 = lhog4 ./ repmat(lmag4, [1, 9]);

rmag1 = sqrt(sum(rhog1 .* rhog1, 2));
rhog1 = rhog1 ./ repmat(rmag1, [1, 9]);

rmag2 = sqrt(sum(rhog2 .* rhog2, 2));
rhog2 = rhog2 ./ repmat(rmag2, [1, 9]);

rmag3 = sqrt(sum(rhog3 .* rhog3, 2));
rhog3 = rhog3 ./ repmat(rmag3, [1, 9]);

rmag4 = sqrt(sum(rhog4 .* rhog4, 2));
rhog4 = rhog4 ./ repmat(rmag4, [1, 9]);

end;

figure(1);
hold on;
plot(mean(lhog1), 'b-')
plot(std(lhog1), 'b-*')
plot(mean(lhog1(13:25:end, :)), 'r-');
plot(std(lhog1(13:25:end, :)), 'r-*')
hold off;
title('lhog1');
xlabel('orientation bin');
ylabel('normalized magnitude');
legend('mean of hog, 8x8 around groundtruth fragments', ...
  'std of hog, 8x8 around groundtruth fragments', ...
  'mean of hog, only groundtruth fragments', ...
  'std of hog, only groundtruth fragments');
grid on;
axis([1, 9, 0, 0.8]);

figure(2);
hold on;
plot(mean(lhog2), 'b-')
plot(std(lhog2), 'b-*')
plot(mean(lhog2(13:25:end, :)), 'r-');
plot(std(lhog2(13:25:end, :)), 'r-*')
hold off;
title('lhog2');
xlabel('orientation bin');
ylabel('normalized magnitude');
legend('mean of hog, 8x8 around groundtruth fragments', ...
  'std of hog, 8x8 around groundtruth fragments', ...
  'mean of hog, only groundtruth fragments', ...
  'std of hog, only groundtruth fragments');
grid on;
axis([1, 9, 0, 0.8]);


figure(3);
hold on;
plot(mean(lhog3), 'b-')
plot(std(lhog3), 'b-*')
plot(mean(lhog3(13:25:end, :)), 'r-');
plot(std(lhog3(13:25:end, :)), 'r-*')
hold off;
title('lhog3');
xlabel('orientation bin');
ylabel('normalized magnitude');
legend('mean of hog, 8x8 around groundtruth fragments', ...
  'std of hog, 8x8 around groundtruth fragments', ...
  'mean of hog, only groundtruth fragments', ...
  'std of hog, only groundtruth fragments');
grid on;
axis([1, 9, 0, 0.8]);



figure(4);
hold on;
plot(mean(lhog4), 'b-')
plot(std(lhog4), 'b-*')
plot(mean(lhog4(13:25:end, :)), 'r-');
plot(std(lhog4(13:25:end, :)), 'r-*')
hold off;
title('lhog4');
xlabel('orientation bin');
ylabel('normalized magnitude');
legend('mean of hog, 8x8 around groundtruth fragments', ...
  'std of hog, 8x8 around groundtruth fragments', ...
  'mean of hog, only groundtruth fragments', ...
  'std of hog, only groundtruth fragments');
grid on;
axis([1, 9, 0, 0.8]);

%% right hog
figure(5);
hold on;
plot(mean(rhog1), 'b-')
plot(std(rhog1), 'b-*')
plot(mean(rhog1(13:25:end, :)), 'r-');
plot(std(rhog1(13:25:end, :)), 'r-*')
hold off;
title('rhog1');
xlabel('orientation bin');
ylabel('normalized magnitude');
legend('mean of hog, 8x8 around groundtruth fragments', ...
  'std of hog, 8x8 around groundtruth fragments', ...
  'mean of hog, only groundtruth fragments', ...
  'std of hog, only groundtruth fragments');
grid on;
axis([1, 9, 0, 0.8]);

figure(6);
hold on;
plot(mean(rhog2), 'b-')
plot(std(rhog2), 'b-*')
plot(mean(rhog2(13:25:end, :)), 'r-');
plot(std(rhog2(13:25:end, :)), 'r-*')
hold off;
title('rhog2');
xlabel('orientation bin');
ylabel('normalized magnitude');
legend('mean of hog, 8x8 around groundtruth fragments', ...
  'std of hog, 8x8 around groundtruth fragments', ...
  'mean of hog, only groundtruth fragments', ...
  'std of hog, only groundtruth fragments');
grid on;
axis([1, 9, 0, 0.8]);

figure(7);
hold on;
plot(mean(rhog3), 'b-')
plot(std(rhog3), 'b-*')
plot(mean(rhog3(13:25:end, :)), 'r-');
plot(std(rhog3(13:25:end, :)), 'r-*')
hold off;
title('rhog3');
xlabel('orientation bin');
ylabel('normalized magnitude');
legend('mean of hog, 8x8 around groundtruth fragments', ...
  'std of hog, 8x8 around groundtruth fragments', ...
  'mean of hog, only groundtruth fragments', ...
  'std of hog, only groundtruth fragments');
grid on;
axis([1, 9, 0, 0.8]);


figure(8);
hold on;
plot(mean(rhog4), 'b-')
plot(std(rhog4), 'b-*')
plot(mean(rhog4(13:25:end, :)), 'r-');
plot(std(rhog4(13:25:end, :)), 'r-*')
hold off;
title('rhog4');
xlabel('orientation bin');
ylabel('normalized magnitude');
legend('mean of hog, 8x8 around groundtruth fragments', ...
  'std of hog, 8x8 around groundtruth fragments', ...
  'mean of hog, only groundtruth fragments', ...
  'std of hog, only groundtruth fragments');
grid on;
axis([1, 9, 0, 0.8]);


disp('done');

