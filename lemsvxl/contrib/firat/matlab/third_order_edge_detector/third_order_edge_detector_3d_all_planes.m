% This is /lemsvxl/contrib/firat/matlab/third_order_edge_detector/third_order_edge_detector_3d_all_planes.m.

% \file
% \author Firat Kalaycilar (firat_kalaycilar@brown.edu)
% \date Aug 25, 2011

function edg3d = third_order_edge_detector_3d_all_planes(V, sigma, grad_thresh, N, bfilter2_flag, maxV)
	xyz = V;
	zxy = shiftdim(V,1);
	yzx = shiftdim(V,2);
	edg_cell = cell(sum(size(V)),1);
	k = 1;
	for i = 1:size(xyz,3)
		I = xyz(:,:,i);
		if bfilter2_flag
			I = maxV*bfilter2(I/maxV);
		end
		edg_temp = third_order_edge_detector_wrapper(I, sigma(1), grad_thresh(1), N(1));
		edg_cell{k} = [edg_temp(:,1:2) repmat(i, size(edg_temp,1), 1) edg_temp(:,3:end)];
		k = k+1;
	end
	for i = 1:size(zxy,3)
		I = zxy(:,:,i);
		if bfilter2_flag
			I = maxV*bfilter2(I/maxV);
		end
		edg_temp = third_order_edge_detector_wrapper(I, sigma(2), grad_thresh(2), N(2));
		edg_cell{k} = [edg_temp(:,2) repmat(i, size(edg_temp,1), 1) edg_temp(:,[1, 3:end])];
		k = k+1;
	end
	for i = 1:size(yzx,3)
		I = yzx(:,:,i);
		if bfilter2_flag
			I = maxV*bfilter2(I/maxV);
		end
		edg_temp = third_order_edge_detector_wrapper(I, sigma(3), grad_thresh(3), N(3));
		edg_cell{k} = [repmat(i, size(edg_temp,1), 1) edg_temp];
		k = k+1;
	end
	edg3d = cell2mat(edg_cell);
	[r,c] = find(any(edg3d(:,1:3) < 1, 2) | edg3d(:,1) > size(V,2) | edg3d(:,2) > size(V,1) | edg3d(:,3) > size(V,3));	
	edg3d(r,:) = [];	
end
