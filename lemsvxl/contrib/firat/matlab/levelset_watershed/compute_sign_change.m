% This is /lemsvxl/contrib/firat/matlab/levelset_watershed/compute_sign_change.m.

% \file
% \author Firat Kalaycilar (firat_kalaycilar@brown.edu)
% \date Mar 20, 2013

function [mins, maxs, saddles, J] = compute_sign_change(I, sigma, mins, maxs, saddles, J)
    addpath ~/lemsvxl/src/contrib/firat/matlab/surface_networks
    [Gx, Gy] = gaussian_derivative_filter(sigma, 1, 1);
	Ix = imfilter(I, Gx, 'replicate', 'conv');
	Iy = imfilter(I, Gy, 'replicate', 'conv');
	if ~exist('mins','var')
	    [mins, maxs, saddles, J] = levelset_min_max_saddle_detector(I, sigma, 1);
	else
	    figure;imagesc(J); colormap gray; hold on; 
        plot(maxs(:,1), maxs(:,2), '^', 'MarkerSize', 5, 'MarkerEdgeColor',[0, 127, 255]/255, 'LineWidth', 2);
        plot(mins(:,1), mins(:,2), 'o', 'MarkerSize', 5, 'MarkerEdgeColor',[0, 127, 255]/255, 'LineWidth', 2);
	    plot(saddles(:,1), saddles(:,2), '+', 'MarkerSize', 5, 'MarkerEdgeColor',[0, 127, 255]/255, 'LineWidth', 2);   
        hold off;
        axis image
        axis off
	end
    BW = roipoly;
    D = bwdist(BW);
    [Dx,Dy] = gradient(D);
    hold on
    C = contour(D, [1,1], 'r');
    quiver(Ix,Iy)
    %quiver(Dx,Dy,'b')
    hold off
    f = zeros(1, C(2,1));
    for i = 1:C(2,1)
        u = [Dx(C(2,i+1), C(1,i+1)) Dy(C(2,i+1), C(1,i+1))];
        v = [Ix(C(2,i+1), C(1,i+1)) Iy(C(2,i+1), C(1,i+1))];
        f(i) = dot(u,v);
    end
    figure; bar(sign(f));
end
