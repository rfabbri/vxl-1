% This is /lemsvxl/contrib/firat/matlab/third_order_edge_detector_3d/project_edge_to_plane.m.

% \file
% \author Firat Kalaycilar (firat_kalaycilar@brown.edu)
% \date Aug 31, 2011

% pe: Nx3
% ne: Nx3
% ps: 1x3
% ns: 1x3

%plane-plane intersection : definition from wikipedia
% http://en.wikipedia.org/wiki/Plane_(geometry)#Line_of_intersection_between_two_planes

function p = project_edge_to_plane(pe, ne, ps, ns)	
	N = size(ne,1);
	ne = ne./mynorm(ne);	 %Nx3
	ns = ns./mynorm(ns);	 %1x3
	he = sum(ne.*pe, 2); %Nx1
	hs = sum(ns.*ps, 2); %1x1
	T = (ne*ns'); %Nx1 
	Q = 1 - T.^2;
	ce = (he - hs*T) ./ Q; %Nx1
	cs = (ones(size(N,1))*hs - he.*T) ./ Q; %Nx1
	p1 = repmat(ce,1,3).*ne + repmat(cs,1,3).*repmat(ns,N,1); %Nx3
	p2 = cross(ne, repmat(ns,N,1), 2); %Nx3
	lambda = sum(p2 .* (pe - p1),2) ./ (sum(p2.*p2,2)); %Nx1
	p = p1 + repmat(lambda,1,3).*p2; %Nx3	
end

function n = mynorm(a)
	n = repmat(sqrt(sum(a.^2,2)),1,size(a,2));
end
