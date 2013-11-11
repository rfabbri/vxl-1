% This is show_contours.m.

% \file
% \author Firat Kalaycilar (firat_kalaycilar@brown.edu)
% \date Aug 31, 2011

% This can be run after running draw_contours.m to view the ground truth contours drawn for slice #slice_index.

% example usage:
% show_contours('/home/firat/contour_output', '/vision/images/medical/elbow-dog/data/TransverseCT_rightSide.vtk', 15);
% Shows the ground truth contours for slice 15.


function show_contours(contour_folder, vtk_file)
	%%read volume
	V = double(readVTK(vtk_file));
	Max = max(V(:));
	Min = min(V(:));
	V = 255 * (V - Min)/(Max - Min);
	files = dir([contour_folder '/*.txt']);
	N = length(files);
	r = 5;
	c = 5;
	num_pages = floor((N-1)/(r*c))+1;
	exit_loop = false;
	
	
	for  p = 1:num_pages
	figure;
		for ri = 1:r
			for ci = 1:c
				i = (p-1)*r*c + (ri-1)*c + ci;
				if i > N
					exit_loop = true;
					break
				end
				Q = load_contours1(fullfile(contour_folder, files(i).name));
				dotpos = strfind(files(i).name, '.');
				slice_index = str2num(files(i).name(1:dotpos(1)-1));
				subplot(r,c,mod(i-1,r*c)+1);;imagesc(V(:,:,slice_index), [0 255]); colormap gray; axis image; axis off
				hold on;
				show_accepted_contours(Q);
				hold off;
				title(files(i).name);
			end
			if exit_loop
				break
			end			
		end
		if exit_loop
			break
		end
	end	
end

function show_accepted_contours(contours)
	for i = 1:length(contours)
		plot([contours{i}(:,1); contours{i}(1,1)] , [contours{i}(:,2); contours{i}(1,2)],'g-');
	end
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
