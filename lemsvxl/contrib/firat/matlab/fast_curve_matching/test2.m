% This is /lemsvxl/contrib/firat/matlab/fast_curve_matching/test2.m.

% \file
% \author Firat Kalaycilar (firat@lems.brown.edu)
% \date Feb 1, 2011

clear
close all
load known.mat

test_curve = G3;
query_curve = G1;


max_num_samples = 81;

qsize = size(query_curve, 1);
frag_size =  round(qsize / 8) + unidrnd(qsize / 2);
spt = unidrnd(qsize - frag_size);
ept = spt + frag_size;

F = query_curve(spt:ept,:);

tic;
%[cf_center, cf_len] = scale_invariant_match(test_curve, F, max_num_samples, 3);
%[cf_center, cf_len] = scale_invariant_match2(test_curve, F, max_num_samples, 3, 30);
[cf_center, cf_len, match_reverse, match_scores] = match_model_query_multi_scale(test_curve, F, max_num_samples, 0, 15, 80);
toc

figure; subplot(9,9,1)
 plot(F(:,1),F(:,2), 'r', 'LineWidth', 3)
axis off
for i = 1:length(cf_center)
    j = cf_center(i)/max_num_samples*size(test_curve,1);
    l = cf_len(i)/max_num_samples*size(test_curve,1);
    MF = mod((round(j-l/2):round(j+l/2))-1,   size(test_curve, 1)) + 1;
    subplot(9,9,i+1)
    plot(test_curve(:,1),test_curve(:,2)); axis off;
    hold on; plot(test_curve(MF,1),test_curve(MF,2), 'g', 'LineWidth', 3)
    hold off;
    title(num2str(match_scores(i)))
end


