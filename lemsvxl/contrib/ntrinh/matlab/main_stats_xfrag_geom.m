% This script analyzes the geometry statistics of a shape fragments from an
% object class
% (c) Nhon Trinh
% Date: Oct 10, 2008

close all;
clear all;

%% input
input_folder = 'D:\vision\projects\symseg\xshock\ETHZ-shapes\training\giraffes-eid_8-nrm';
data_filename = 'xfrag_geom-giraffes-eid_8.txt';

%% process data

% parse input data
data_file = fullfile(input_folder, data_filename);
data = dlmread(data_file, ' ', 0, 0);


x0 = data(:, 1);
y0 = data(:, 2);
psi0 = data(:, 3);
phi0 = data(:, 4);
r0 = data(:, 5);
 
x1 = data(:, 6);
y1 = data(:, 7);
psi1 = data(:, 8);
phi1 = data(:, 9);
r1 = data(:, 10);
 
area = data(:, 11);
scale = sqrt(area);

% vars derived from raw data
len = sqrt((x1-x0).^2 + (y1-y0).^2);
chord_psi = atan2(y1-y0, x1-x0);
dpsi0 = asin(sin(psi0 - chord_psi));
dpsi1 = asin(sin(psi1 - chord_psi));

plot_xdesc = 1;
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

figure(9);
hist(scale);
xlabel('scale of the shock graph (sqrt of area)');
title('hist of scale');
grid on;

end;

%  frag = [len ./ scale, dpsi0, dpsi1, r0 ./ scale, r1 ./scale, phi0, phi1];
disp('done');

