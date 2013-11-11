function [V, origin, spacing] = readVTK(vtkfile)
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% Usage: V = readVTK(vtkfile)
%
%   V:       The matrix to be stored
%   vtkfile: The filename
%   notes:   Only reads binary STRUCTURED_POINTS
%
% Erik Vidholm 2006
% Modified by Firat Kalaycilar 2012
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

	spacing_line = fgetl(fid) % SPACING SX SY SZ
	spacing = sscanf(spacing_line, '%*s%f%f%f').'
	origin_line = fgetl(fid) % ORIGIN OX OY OZ
	origin = sscanf(origin_line, '%*s%f%f%f').'

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
	  V = permute(V, [2 1 3]);
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
