% This is /lemsvxl/contrib/firat/matlab/levelset2d_segmentation/tools/view_zero_level_curve.m.

% \file
% \author Firat Kalaycilar (firat_kalaycilar@brown.edu)
% \date Aug 15, 2011

function contours = view_zero_level_curve(phi, hx, hy)
	debug_mode = 0;
	hold on;
	contours = compute_zero_level_curve(phi, hx, hy);
	for i = 1:length(contours)
		C = contours{i};
		if debug_mode
			plot(C(1,1),C(1,2), '.', 'MarkerSize', 15);				
			plot(C([1:end ],1),C([1:end ],2),'.-','Color', [1 0 0])
		else							
			plot(C([1:end ],1),C([1:end ],2),'-','Color', [1 0 0])
		end
						
	end
	hold off;
	axis ij
	axis image
	grid on
	set(gca,'XTick',[0:1:300])
	set(gca,'YTick',[0:1:300])
end
