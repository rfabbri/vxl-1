% This is /lemsvxl/contrib/firat/matlab/elbow/segment_bones.m.

% \file
% \author Firat Kalaycilar (firat_kalaycilar@brown.edu)
% \date Jun 30, 2011

function S = segment_bones_just_levelset(I,bw)
	I = double(I);
	if ~exist('bw','var')	
		fid = figure;
		imagesc(I);colormap gray; axis image; axis off;
		hold on;    
		[x{1},y{1},b] = ginput(1);
		plot(x{1},y{1},'rd');    
		[x{2},y{2},b] = ginput(1);        
		plot([x{1} x{2}], [y{1} y{2}], 'rd-')
		t = 3;
		while b == 1
		    [x{t},y{t},b] = ginput(1);
		    plot([x{t-1} x{t}], [y{t-1} y{t}], 'rd-');
		    t = t + 1;    
		end  
		plot([x{t-1} x{1}], [y{t-1} y{1}], 'rd-');  
		hold off;  
		pause(.1)  
		bw = poly2mask(cell2mat(x), cell2mat(y), size(I,1), size(I,2));
	end
	S = levelset(I, logical(bw));
	figure; imagesc(I); colormap gray; axis image; axis off;
	hold on;
	contour(S, [0,0], 'r', 'LineWidth', 3);
	hold off;
end

