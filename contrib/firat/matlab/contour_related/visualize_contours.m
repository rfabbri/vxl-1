% This is /lemsvxl/contrib/firat/matlab/visualize_contours.m.

% \file
% \author Firat Kalaycilar (firat_kalaycilar@brown.edu)
% \date Jul 29, 2011

function visualize_contours(contours, color1)
	%hold on; 
	colors = [1 0 0; 0 0 1];
	for i = 1:length(contours)
		C = contours{i};
		if 0
			plot(C(1,1),C(1,2), '.', 'MarkerSize', 15);
			plot(C([1:end ],1),C([1:end ],2),'.-','Color', color1)
		else			
			plot(C([1:end 1],1),C([1:end 1],2),'-','Color', color1, 'LineWidth', 2)
		end
		
	end
	%hold off;
	axis image
	%grid on;
	%set(gca,'XTick',[0:1:300])
	%set(gca,'YTick',[0:1:300])
	axis ij
	%axis([7 48 13 37])
end
