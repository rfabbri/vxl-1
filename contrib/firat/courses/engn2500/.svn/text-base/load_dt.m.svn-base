% This is /lemsvxl/contrib/firat/courses/engn2500/load_dt.m.

% \file
% \author Firat Kalaycilar (firat_kalaycilar@brown.edu)
% \date May 20, 2011

function DT = load_dt(dt_file, swap)
	fid = fopen(dt_file, 'rb');
	type = fscanf(fid, '%s', 1)
	siz = fscanf(fid, '%d', 3)
	dummy = fscanf(fid, '%d\n', 9)
	DT = reshape(fread(fid, inf, '*single'), siz(3), siz(2), siz(1));
	if swap
		DT = swapbytes(DT);
	end
	fclose(fid);
end

