% This is /lemsvxl/contrib/firat/matlab/fast_curve_matching/test.m.

% \file
% \author Firat Kalaycilar (firat@lems.brown.edu)
% \date Feb 1, 2011

clear
close all
load known.mat

query_curve = G3;
test_curve = G2;

query_curve = sample_curve(query_curve, 3, 50);
test_curve = sample_curve(test_curve, 3, 50);


qsize = size(query_curve, 1);
frag_size = round(qsize / 8) + unidrnd(qsize / 2);
spt = unidrnd(qsize - frag_size);
ept = spt + frag_size;

F = query_curve(spt:ept,:);

figure;
plot(query_curve(:,1),query_curve(:,2));
hold on; plot(F(:,1),F(:,2), 'r', 'LineWidth', 3)
hold off;

tic;
cf_center = match(test_curve, F, size(test_curve,1), size(F, 1), 0, 0);
toc

if length(cf_center) > 7
    stop1 = 7;
else
    stop1 = length(cf_center);
end
for i = 1:stop1
    j = cf_center(i);
    l = size(F,1);
    MF = mod((round(j-l/2):round(j+l/2))-1,   size(test_curve, 1)) + 1;
    figure;
    plot(test_curve(:,1),test_curve(:,2));
    hold on; plot(test_curve(MF,1),test_curve(MF,2), 'g', 'LineWidth', 3)
    hold off;
end


