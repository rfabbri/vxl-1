% This is save_performance_surfaces_as_gif.m.

% Created on: Apr 21, 2013
%     Author: firat

clear
close all
addpath ../..
prefix = 'combined_tp3';
% load cp + sift mixture performance charts
load(['~/Desktop/data/_old_code/' prefix '.mat']);
[x,y] = meshgrid(p, p);
%ref -> y, x, z
%com_tp_min -> min, max, saddle
for i = 1:length(p)
    surf(x, y, com_tp_min(:,:,i)/225); 
    xlabel('max'); ylabel('min'); zlabel('TPR');
    axis([0 1 0 1 0 1]);
    title(sprintf('%d%% saddle', uint8(100*p(i))));
    save_gif(['~/Desktop/data/_old_code/' prefix '_saddle.gif'], 1, false, true);
end
com_tp_min = shiftdim(com_tp_min, 1);
%com_tp_min -> max, saddle, min
for i = 1:length(p)
    surf(x, y, com_tp_min(:,:,i)/225); 
    xlabel('saddle'); ylabel('max'); zlabel('TPR');
    axis([0 1 0 1 0 1]);
    title(sprintf('%d%% min', uint8(100*p(i))));
    save_gif(['~/Desktop/data/_old_code/' prefix '_min.gif'], 1, false, true);
end
com_tp_min = shiftdim(com_tp_min, 1);
%com_tp_min -> saddle, min, max
for i = 1:length(p)
    surf(x, y, com_tp_min(:,:,i)/225); 
    xlabel('min'); ylabel('saddle'); zlabel('TPR');
    axis([0 1 0 1 0 1]);
    title(sprintf('%d%% max', uint8(100*p(i))));
    save_gif(['~/Desktop/data/_old_code/' prefix '_max.gif'], 1, false, true);
end
close all