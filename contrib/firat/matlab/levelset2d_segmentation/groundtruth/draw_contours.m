% This is draw_contours.m.

% \file
% \author Firat Kalaycilar (firat_kalaycilar@brown.edu)
% \date Aug 31, 2011

% parameters:
% vtk_file: full path to the vtk file containing the slices. (required parameter)
% output_folder: full path to the folder to store the contours. (required parameter)
% start_index: index of the first slice (optional parameter) (default: 1)
% end_index: index of the last slice (optional parameter) (default: last slice)

% Example usage:

% draw_contours('/vision/images/medical/elbow-dog/data/TransverseCT_rightSide.vtk', '/home/firat/contour_output');
% or
% draw_contours('/vision/images/medical/elbow-dog/data/TransverseCT_rightSide.vtk', '/home/firat/contour_output', 10, 15);

% This program treats a contour as a polygon. You need to click on the image to create vertices of the polygon.
% Left click your mouse to add vertices. The final vertex must be added by a right-click. 


function draw_contours(vtk_file, output_folder, start_index, end_index)
	%%read volume
	V = double(readVTK(vtk_file));
	Max = max(V(:));
	Min = min(V(:));
	V = 255 * (V - Min)/Max;
	
	%%create output folder if it doesn't exist
	if ~exist(output_folder, 'dir')
		mkdir(output_folder);
	end
	
	%%set default values if necessary
	[nrows, ncols, nbands] = size(V);
	if ~exist('start_index', 'var')
		start_index = 1;
	end
	if ~exist('end_index', 'var')
		end_index = nbands;
	end
	
	num_slices = end_index - start_index + 1;
		
	%%loop over selected slices
	i = start_index;
	while i <= end_index
		Slice = V(:,:,i);
		fid = figure;		
		k = 0;
		contours = {};
		while 1
			clf(fid);
			hold on;
			axis ij;
			imagesc(Slice, [0 255]); colormap gray; axis image; axis off
			title(sprintf('Slice index: %d', i))
			show_accepted_contours(contours);			
			answer = questdlg('Do you want to draw a new contour on this slice?', '', 'Yes', 'No', 'Yes');
			if strcmp(answer, 'No')
				break
			end
			k = k + 1;
			c = draw_single_contour;			
			answer = questdlg('Accept or reject the contour', '', 'Accept', 'Reject', 'Accept');
			if strcmp(answer, 'Accept')	
				contours{k} = c;
			else
				k = k - 1;
			end			
		end
		clf(fid);
		hold on;
		axis ij;
		imagesc(Slice, [0 255]); colormap gray; axis image; axis off
		show_accepted_contours(contours);		
		answer = questdlg('Choose one', '', 'Save contours', 'Retry drawing contours', 'Exit', 'Save contours');
		if strcmp(answer, 'Save contours')			
			fprintf('Saving contours...\n');
			out_file = fullfile(output_folder, sprintf('%d.txt', i));
			save_contours(contours, out_file);
			i = i + 1;
		elseif strcmp(answer, 'Exit')
			close all
			return			
		end
		close(fid);
	end		
end

function c = draw_single_contour
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
	c = [x' y'];
end

function show_accepted_contours(contours)
	for i = 1:length(contours)
		plot([contours{i}(:,1); contours{i}(1,1)] , [contours{i}(:,2); contours{i}(1,2)],'g-');
	end
end

function save_contours(contours, out_file)
	num_poly = length(contours);
	in_cell = cell(2*num_poly,1);
	for i = 1:num_poly
		contour1 = contours{i};
		l = size(contour1, 1);
		in_cell{2*i-1} = [l 0];
		in_cell{2*i} = contour1;
	end
	in_mat = cell2mat(in_cell);
	dlmwrite(out_file, in_mat, 'delimiter', ' ');
end

function V = readVTK(vtkfile)
	%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	% Usage: V = readVTK(vtkfile)
	%
	%   V:       The matrix to be stored
	%   vtkfile: The filename
	%   notes:   Only reads binary STRUCTURED_POINTS
	%
	% Erik Vidholm 2006
	%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	V = 0;

	% open file (OBS! big endian format)
	fid = fopen(vtkfile,'r','b');

	if( fid == -1 )
	  return
	end

	fgetl(fid); % # vtk DataFile Version x.x
	fgetl(fid); % comments
	fgetl(fid); % BINARY
	fgetl(fid); % DATASET STRUCTURED_POINTS

	s = fgetl(fid); % DIMENSIONS NX NY NZ
	sz = sscanf(s, '%*s%d%d%d').'

	fgetl(fid); % ORIGIN OX OY OZ
	fgetl(fid); % SPACING SX SY SZ
	fgetl(fid); % POINT_DATA NXNYNZ

	s = fgetl(fid); % SCALARS/VECTORS name data_type (ex: SCALARS imagedata unsigned_char)
	svstr = sscanf(s, '%s', 1)
	dtstr = sscanf(s, '%*s%*s%s')

	if( strcmp(svstr,'SCALARS') > 0 )
	  fgetl(fid); % the lookup table
	  if( strcmp(dtstr,'unsigned_char') > 0 ) 
		% read data
		V = fread(fid,prod(sz),'*uint8');
		V = reshape(V,sz);
	  elseif( strcmp(dtstr,'char') > 0 )
		% read data
		V = fread(fid,prod(sz),'*int8');
		V = reshape(V,sz);
	  elseif( strcmp(dtstr,'unsigned_short') > 0 )
		% read data
		V = fread(fid,prod(sz),'*uint16');
		V = reshape(V,sz);
	  elseif( strcmp(dtstr,'short') > 0 )
		% read data
		V = fread(fid,prod(sz),'*int16');
		V = reshape(V,sz);
	  elseif( strcmp(dtstr,'unsigned_int') > 0 )
		% read data
		V = fread(fid,prod(sz),'*uint32');
		V = reshape(V,sz);
	  elseif( strcmp(dtstr,'int') > 0 )
		% read data
		V = fread(fid,prod(sz),'*int32');
		V = reshape(V,sz);
	  elseif( strcmp(dtstr,'float') > 0 )
		% read data
		V = fread(fid,prod(sz),'*single');
		V = reshape(V,sz);
	  elseif( strcmp(dtstr,'double') > 0 )
		% read data
		V = fread(fid,prod(sz),'*double');
		V = reshape(V,sz);
	  end
	  
	elseif( strcmp(svstr,'VECTORS') > 0 )
	  if( strcmp(dtstr,'float') > 0 ) 
		% read data
		V = fread(fid,3*prod(sz),'*single');
		V = reshape(V,[3 sz]);
		V = permute(V,[2 3 4 1]);
	  end
	end

	fclose(fid);
end
