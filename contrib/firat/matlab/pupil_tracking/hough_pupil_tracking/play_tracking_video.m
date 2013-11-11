% This is play_tracking_video.m
% \file
% \author Firat Kalaycilar (firat_kalaycilar@brown.edu)
% \date Apr 26, 2012

% Copyright (c) 2012 
% Firat Kalaycilar
% Benjamin Kimia
% The Laboratory for Engineering Man/Machine Systems (LEMS)
% School of Engineering
% Brown University
% http://vision.lems.brown.edu

function smooth_centers = play_tracking_video(mov, centers, radii, sigma)
	angles = ((0:8:352)/180*pi)';
	R = [cos(angles) sin(angles)];
	num_frames = size(mov,3);
	smooth_centers = zeros(size(centers));
	if sigma > 0
		L = round(6*sigma);
		if mod(L,2) == 0
			L = L + 1;
		end
		smooth_centers(:,1) = imfilter(centers(:,1), fspecial('gaussian', [L,1], sigma),'replicate');
		smooth_centers(:,2) = imfilter(centers(:,2), fspecial('gaussian', [L,1], sigma),'replicate');
	else
		smooth_centers = centers;
	end
	for f_index = 1:num_frames        
    	circ_points = radii(f_index) * R + repmat(smooth_centers(f_index,:), length(angles), 1);
        imagesc(mov(:,:,f_index)); colormap gray; axis image; axis off; hold on;        
        plot(circ_points([1:end 1],1), circ_points([1:end 1],2), 'r'); hold off
        drawnow
    end
    
end
