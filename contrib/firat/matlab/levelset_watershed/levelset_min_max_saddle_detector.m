% This is /lemsvxl/contrib/firat/matlab/levelset_watershed/levelset_min_max_saddle_detector.m.

% \file
% \author Firat Kalaycilar (firat_kalaycilar@brown.edu)
% \date Mar 7, 2013

function [mins, maxs, saddles, J] = levelset_min_max_saddle_detector(I, sigma, do_plot)
    [saddles1, mins, J, K, H, W] = levelset_saddle_min_detector(I, sigma, false);
    [saddles2, maxs] = levelset_saddle_max_detector(I, sigma, false);
    %hungarian matching
    addpath /home/firat/lemsvxl/src/contrib/firat/matlab/surface_networks/keypoint_evaluation
    D = pdist2(saddles1,saddles2);
    D(D > sqrt(2)) = Inf;
    Matches1 = assignmentoptimal(D);
    Matches1(Matches1 == 0) = [];
    unique_set2 = setdiff(1:size(saddles2,1), Matches1');
    saddles = [saddles1; saddles2(unique_set2,:)];
    J = J(K+1:end-K,K+1:end-K);
    if do_plot        
        figure;imagesc(J); colormap gray; hold on; 
        plot(maxs(:,1), maxs(:,2), '^', 'MarkerSize', 5, 'MarkerEdgeColor',[0, 127, 255]/255, 'LineWidth', 2);
        plot(mins(:,1), mins(:,2), 'o', 'MarkerSize', 5, 'MarkerEdgeColor',[0, 127, 255]/255, 'LineWidth', 2);
        %plot(saddles(:,1), saddles(:,2), '+', 'MarkerSize', 5, 'MarkerEdgeColor',[0, 127, 255]/255, 'LineWidth', 2);
        plot(saddles1(:,1), saddles1(:,2), 'r+', 'MarkerSize', 5, 'LineWidth', 2);
        plot(saddles2(:,1), saddles2(:,2), 'g+', 'MarkerSize', 5, 'LineWidth', 2);		       
        hold off;
        axis image
        axis off
               
    end
end
