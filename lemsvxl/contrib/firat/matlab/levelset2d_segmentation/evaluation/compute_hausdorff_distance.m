% This is /lemsvxl/contrib/firat/matlab/levelset2d_segmentation/evaluation/compute_hausdorff_distance.m.

% \file
% \author Firat Kalaycilar (firat_kalaycilar@brown.edu)
% \date Sep 7, 2011

function hd = compute_hausdorff_distance(seg_folder, gt_folder, curve_samples, start_index, end_index, hx, hy)
	num_slices = end_index - start_index + 1;
	seg_points = cell(num_slices,1);
	for k = start_index:end_index
		load([seg_folder '/' num2str(k) '.mat']);
		if ~exist('height','var')
			[t1, t2] = size(phi);
			height = (t1-1)*hy+1;
			width = (t2-1)*hx+1;
			points = cell(end_index-start_index+1,1);
			[gridx, gridy] = meshgrid(1:hx:width, 1:hy:height);
		end
		C = contour(gridx, gridy, phi, [0,0]);
		close all;
		temp_seg_points = [];		
		L = size(C,2);		
		i = 1;
		while i < L		
			vertices = C(:,i+1:i+C(2,i)-1)';
			sampled_curve_coords = sample_curve(vertices, curve_samples, 0);			
			%sampled_curve_coords = vertices;
			temp_seg_points = [temp_seg_points; sampled_curve_coords];
			i = i + C(2,i) + 1;			
		end	
		if ~isempty(temp_seg_points)
			seg_points{k-start_index+1} = [temp_seg_points repmat(k,size(temp_seg_points,1),1)];
		end		
	end
	seg_points = cell2mat(seg_points);

	%load gt
	gt_points = cell(num_slices,1);
	for i = start_index:end_index
		contours = load_contours1([gt_folder '/' num2str(i) '.txt']);		
		temp = cell2mat(contours);
		gt_points{i-start_index+1} = [temp repmat(i,size(temp,1),1)];
	end
	gt_points = cell2mat(gt_points);
	hd = hausdorff(seg_points,gt_points);

end

function contours = load_contours1(filename)
    [x,y] = textread(filename,'%f%f', 'delimiter',' ');
    contours = {};
    while length(x) > 0
        l = x(1);
        cx = x(2:l+1);
        cy = y(2:l+1);
        x(1:l+1) = [];
        y(1:l+1) = [];
        contours = [contours; [cx cy]];           
    end     
end

