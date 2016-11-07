% This is /lemsvxl/contrib/firat/matlab/fast_curve_matching/test9.m.

% \file
% \author Firat Kalaycilar (firat_kalaycilar@brown.edu)
% \date Apr 1, 2011

clear
close all

load known.mat

Model = K1;
Query = K2;

K = 3;
N = 81;
L_min = round(N/3);
L_increment = round(N/20);

SModel = sample_curve(Model, N, 1);
SQuery = sample_curve(Query, N, 1);

%[M_match_center, Q_match_center, match_length, match_reverse, match_scores] = match_model_model_single_scale(Model, Query, N, 3, L_min, 5);
tic;[M_match_center, Q_match_center, match_length, match_reverse, match_scores] = get_model_model_correspondences(Model, Query, N, K, L_min, L_increment, 0.11);
toc
colors = 'rbmgk';
for i = 1:min(510,length(match_length))
    figure('Name',sprintf('Matching score: %f', match_scores(i)),'NumberTitle','off');
    
    subplot(1,2,1)
    plot(SQuery(:,1),SQuery(:,2));
    j = Q_match_center(i);
    l = match_length(i);    
    MF = mod(((j-(l-1)/2):(j+(l-1)/2))-1,   N) + 1;
    F = SQuery(MF,:);
    hold on; plot(F(1:end,1),F(1:end,2), 'c', 'LineWidth', 3)
    for b = 1:size(F,1)
        plot(F(b,1),F(b,2),[colors(mod(b-1,length(colors))+1) '.'],'MarkerSize', 20);    
    end    
    hold off;
    axis square
    
    subplot(1,2,2)
    if match_reverse(i)
        SModel = SModel(end:-1:1,:);
    end
    plot(SModel(:,1),SModel(:,2));
    j = M_match_center(i);
    l = match_length(i);    
    MF = mod(((j-(l-1)/2):(j+(l-1)/2))-1,   N) + 1;
    F = SModel(MF,:);
    hold on; plot(F(1:end,1),F(1:end,2), 'c', 'LineWidth', 3)
    for b = 1:size(F,1)
        plot(F(b,1),F(b,2),[colors(mod(b-1,length(colors))+1) '.'],'MarkerSize', 20);    
    end       
    hold off;    
    axis square
    
end
