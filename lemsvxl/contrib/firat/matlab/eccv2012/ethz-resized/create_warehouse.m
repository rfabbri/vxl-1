%create_warehouse.m

% Created on: Feb 2, 2012
%     Author: firat

function create_warehouse(mask_folder, out_folder)
	files = dir([mask_folder '/*.bmp']);
	for i = 1:length(files)
		I = imread([mask_folder '/' files(i).name]);
		C = extract_contour_from_phi(I > 0, 0.5);
		save_contours(C, [out_folder '/exemplar_' num2str(i) '.txt']);		
	end	
end

function polygons = extract_contour_from_phi(phi, a)
        C = contour(phi, [a,a]);
	close(gcf);
	polygons = {};
	L = size(C,2);		
	i = 1;
	while i < L		
		vertices = C(:,i+1:i+C(2,i)-1)';
		vertices = sample_curve(vertices, 401, 1);
		polygons = [polygons; vertices];
		i = i + C(2,i) + 1;			
	end
		
end
