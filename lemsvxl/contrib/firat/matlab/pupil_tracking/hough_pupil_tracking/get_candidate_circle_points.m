% This is get_candidate_circle_points.m.

% \file
% \author Firat Kalaycilar (firat_kalaycilar@brown.edu)
% \date Mar 10, 2011
% \update Feb 20, 2012

% Copyright (c) 2011 
% Firat Kalaycilar
% Benjamin Kimia
% The Laboratory for Engineering Man/Machine Systems (LEMS)
% School of Engineering
% Brown University
% http://vision.lems.brown.edu

% crop a patch of size (2*s+1 x 2*s+1) whose center is at (x,y)
% increase the contrast of the patch
% detect edges using canny edge detector


%function points = get_candidate_circle_points(I0, x, y, s, search_mask)    
%    J = uint8(I0(y-s:y+s,x-s:x+s));
%    J = imadjust(J,stretchlim(J),[]);       
%    edg = mex_third_order_edge_detector(J,1,10,1);	 
%    mask_val = interp2(search_mask, edg(:,1), edg(:,2), '*linear');
%	edg(mask_val == 0, :) = []; 
%	points = [edg(:,1)-s+x-1 edg(:,2)-s+y-1];   
%end

function points = get_candidate_circle_points(I0, x, y, s, search_mask)    
    J = uint8(I0(y-s:y+s,x-s:x+s));
    J = imadjust(J,stretchlim(J),[]);
    I = double(J);    
    C = edge(I, 'canny');	 
    if ~isempty(search_mask)
		C = C .* search_mask;
    end	
    [row,col] = find(C > 0);
    points = [col-s+x-1 row-s+y-1];   
end
