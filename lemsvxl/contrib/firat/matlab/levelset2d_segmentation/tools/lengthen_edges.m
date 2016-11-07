% This is /lemsvxl/contrib/firat/matlab/levelset2d_segmentation/tools/lengthen_edges.m.

% \file
% \author Firat Kalaycilar (firat_kalaycilar@brown.edu)
% \date Aug 19, 2011

function edg2 = lengthen_edges(edg, l)
	N = size(edg,1);
	inc = .25;
	f = l/inc + 1;
	edg2 = zeros(N*f, size(edg,2));
	for i = 1:N
		edg1 = edg(i,:);
		p1 = [edg1(1)-(l/2)*cos(edg1(3)) edg1(2)-(l/2)*sin(edg1(3))];
		p2 = [edg1(1)+(l/2)*cos(edg1(3)) edg1(2)+(l/2)*sin(edg1(3))];
		for lambda_index = 1:f
			lambda = inc*(lambda_index-1);
			p = p1*lambda + p2*(1-lambda);
			edg2((i-1)*f+lambda_index, 1:2) = p;
			edg2((i-1)*f+lambda_index, 3:end) = edg1(3:end); 
		end
	end
end

