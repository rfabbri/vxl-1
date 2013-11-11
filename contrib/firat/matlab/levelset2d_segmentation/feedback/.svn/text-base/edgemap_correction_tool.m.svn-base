% This is /lemsvxl/contrib/firat/matlab/levelset2d_segmentation/feedback/edgemap_correction_tool.m.

% \file
% \author Firat Kalaycilar (firat_kalaycilar@brown.edu)
% \date Aug 23, 2011

function edg = edgemap_correction_tool(I, edge_thresh, edge_sigma)
	I = double(I);
	I_orig = I;
	I_orig = [I_orig(1,:);I_orig];
	I_orig = [I_orig;I_orig(end,:)];
	I_orig = [I_orig(:,1), I_orig];
	I_orig = [I_orig, I_orig(:,end)];
	I = 255*bfilter2(I/255);
	I = [I(1,:);I];
	I = [I;I(end,:)];
	I = [I(:,1), I];
	I = [I, I(:,end)];
	[edg, edgemap] = third_order_edge_detector_wrapper(I, edge_sigma, edge_thresh, 1);
	edg = lengthen_edges(edg, 1);
	figure; imagesc(I_orig); colormap gray; axis off; axis image;
	disp_edg(edg, 'g', 0);
	option1 = input('Choose [1] edge removal [2] edge insert [3] exit: ');
	close all
	while option1 == 1 || option1 == 2
		if option1 == 1
			figure; imagesc(I_orig); colormap gray; axis off; axis image;
			disp_edg(edg, 'g', 0);
			hold on
			[x(1),y(1),b] = ginput(1);
			plot(x(1),y(1),'rd');    
			[x(2),y(2),b] = ginput(1);        
			plot([x(1) x(2)], [y(1) y(2)], 'rd-')
			t = 3;
			while b == 1
				[x(t),y(t),b] = ginput(1);
				plot([x(t-1) x(t)], [y(t-1) y(t)], 'rd-');
				t = t + 1;    
			end  
			plot([x(t-1) x(1)], [y(t-1) y(1)], 'rd-');  
			hold off;
			mask_removal = inpolygon(edg(:,1), edg(:,2), x, y);
			edg(logical(mask_removal), :) = [];
			close all
			clear x
			clear y
		else
			figure; imagesc(I_orig); colormap gray; axis off; axis image;
			disp_edg(edg, 'g', 0);
			hold on
			[x(1),y(1),b] = ginput(1);
			plot(x(1),y(1),'r.');    
			t = 2;
			while b == 1
				[x(t),y(t),b] = ginput(1);
				plot([x(t-1) x(t)], [y(t-1) y(t)], 'r.');
				t = t + 1;    
			end	
			edg = [edg; x' y' zeros(length(x), 3)];	
			close all
			clear x
			clear y
		end
		figure; imagesc(I_orig); colormap gray; axis off; axis image;
		disp_edg(edg, 'g', 0);
		option1 = input('Choose [1] edge removal [2] edge insert [3] exit: ');
		close all
	end
		
end
