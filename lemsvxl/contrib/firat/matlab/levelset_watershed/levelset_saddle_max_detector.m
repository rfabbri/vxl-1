% This is /lemsvxl/contrib/firat/matlab/levelset_watershed/levelset_saddle_max_detector.

% \file
% \author Firat Kalaycilar (firat_kalaycilar@brown.edu)
% \date Mar 7, 2013


function [saddles, maxs] = levelset_saddle_max_detector(I, sigma, do_plot)
    [saddles, mins, J, K, H, W] = levelset_saddle_min_detector(-I, sigma, false);
    maxs = mins;
    J = -J;
    if do_plot
        figure;imagesc(J); colormap gray; hold on; 
        plot(maxs(:,1)+K, maxs(:,2)+K, '^', 'MarkerSize', 5, 'MarkerEdgeColor',[0, 127, 255]/255, 'LineWidth', 2);
	    plot(saddles(:,1)+K, saddles(:,2)+K, '+', 'MarkerSize', 5, 'MarkerEdgeColor',[0, 127, 255]/255, 'LineWidth', 2);	    
        hold off;
        axis([K+1 W-K K+1 H-K]);
    end
end
