% This is /lemsvxl/contrib/firat/matlab/fast_curve_matching/test8.m.

% \file
% \author Firat Kalaycilar (firat_kalaycilar@brown.edu)
% \date Mar 28, 2011

clear
close all

load known.mat

Model = J1;
Query = J3;

N = 81;
L_min = N-2;round(N/3);
L_increment = round(N/20);

S = N;

%[M_match_center, Q_match_center, match_length, match_reverse, match_scores] = match_model_model_single_scale(Model, Query, N, 3, L_min, 5);
tic;[M_match_center, Q_match_center, match_length, match_reverse, match_scores] = get_model_model_correspondences(Model, Query, N, 3, L_min, L_increment, 0.1);
toc
colors = 'rbmk';
for i = 1:min(510,length(match_length))
    figure('Name',sprintf('Matching score: %f', match_scores(i)),'NumberTitle','off');
    
    subplot(1,2,1)
    plot(Query(:,1),Query(:,2));
    j = Q_match_center(i)/N*size(Query,1);
    l = match_length(i)/N*size(Query,1);    
    MF = mod((round(j-l/2):round(j+l/2))-1,   size(Query, 1)) + 1;
    F = Query(MF,:);
    hold on; plot(F(1:end,1),F(1:end,2), 'g', 'LineWidth', 3)
    SL = size(F,1)/(S-1);
    P = 0:SL:size(F,1);
    P(1) = 1;
    P = round(P);
    for b = 1:S
        a = P(b);
        plot(F(a,1),F(a,2),[colors(mod(b-1,length(colors))+1) '.'],'MarkerSize', 20);    
    end    
    hold off;
    
    subplot(1,2,2)
    if match_reverse(i)
        Model = Model(end:-1:1,:);
    end
    plot(Model(:,1),Model(:,2));
    j = M_match_center(i)/N*size(Model,1);
    l = match_length(i)/N*size(Model,1);    
    MF = mod((round(j-l/2):round(j+l/2))-1,   size(Model, 1)) + 1;
    F = Model(MF,:);
    hold on; plot(F(1:end,1),F(1:end,2), 'g', 'LineWidth', 3)
    SL = size(F,1)/(S-1);
    P = 0:SL:size(F,1);
    P(1) = 1;
    P = round(P);
    for b = 1:S
        a = P(b);
        plot(F(a,1),F(a,2),[colors(mod(b-1,length(colors))+1) '.'],'MarkerSize', 20);    
    end       
    hold off;    
    
end
