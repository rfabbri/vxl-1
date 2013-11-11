% This is /lemsvxl/contrib/firat/matlab/fast_curve_matching/report_related.m.

% \file
% \author Firat Kalaycilar (firat_kalaycilar@brown.edu)
% \date Apr 5, 2011

clear
close all
load ../known.mat
addpath ~/lemsvxl/src/contrib/firat/matlab
addpath ~/lemsvxl/src/contrib/firat/matlab/fast_curve_matching
if 0
SG1 = sample_curve(G1, 81, 1);
figure; subplot(1,2,1);
plot(G1(:,1),G1(:,2), '.-');
title(sprintf('Original curve with %d points', size(G1,1)));
axis square
subplot(1,2,2);
plot(SG1(:,1),SG1(:,2), '.-');
title(sprintf('Sampled curve with %d points', size(SG1,1)));
axis square
end

SG1 = sample_curve(G1, 81, 1);
P = SG1(60:69, :);
figure;
plot(P(:,1),P(:,2), '-', 'LineWidth', 4); hold on;
plot(P(:,1),P(:,2), 'r.', 'MarkerSize', 30);
for i = 1:size(P,1)
    text(P(i,1),P(i,2)+.01,num2str(i),'FontSize', 16);
end
hold off;
axis off;
axis equal;
figure;
for k = 1:4
    subplot(2,2,k);
    plot(P(:,1),P(:,2), '-', 'LineWidth', 4); hold on;
    plot(P(:,1),P(:,2), 'r.', 'MarkerSize', 30);
    for i = 1:size(P,1)
        line([P(3*k-2,1) P(i,1)],[P(3*k-2,2) P(i,2)],'Color','g', 'LineWidth', 3);
        %draw_arrow(P(3*k-2,:),P(i,:),.1,3);
    end
    hold off;
    axis equal;
    axis off;
end
