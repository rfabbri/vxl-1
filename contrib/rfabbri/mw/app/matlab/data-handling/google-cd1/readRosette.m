% Returns a calibration information for the rossette
function cameras = readRosette(fname);

% this works for reperojected images where all intrinsics are the
% same for all cameras

% UTM coordinates North ~ Y, East ~ X and yaw is around Z

fp = fopen(fname);
k = 0;
if fp == -1,
    error('file not found');
else
    % Skip the first two lines
    str = fgetl(fp);
    str = fgetl(fp);
    if ~ischar(str),
        error('did not expect empty file')
        return;
    end
    k = 1;
    while 1
        str = fgetl(fp);
        if ~ischar(str); % done reading
            return;
        else
 	  if strfind(str,'intrinsics <') % read the extrinsics
            str = fgetl(fp);
            [s,fx] = strread(str, '%s%f');
	                str = fgetl(fp);
            [s,fy] = strread(str, '%s%f');
	                str = fgetl(fp);
            [s,cx] = strread(str, '%s%f');
	                str = fgetl(fp);
            [s,cy] = strread(str, '%s%f');
	    cameras(k).calib = [fx 0 cx; 0 fy cy; 0 0 1];
	  end;
	  if strfind(str,'extrinsics <') % read the extrinsics
            str = fgetl(fp);
            [s,q(1)] = strread(str, '%s%f');
	                str = fgetl(fp);
            [s,q(2)] = strread(str, '%s%f');
	                str = fgetl(fp);
            [s,q(3)] = strread(str, '%s%f');
	                str = fgetl(fp);
            [s,q(4)] = strread(str, '%s%f');
	                str = fgetl(fp);
            [s,tr(1)] = strread(str, '%s%f');
	                str = fgetl(fp);
            [s,tr(2)] = strread(str, '%s%f');
	                str = fgetl(fp);
	    [s,tr(3)] = strread(str, '%s%f');
	    rot = q2rot(q);
            cameras(k).pose = [rot tr'; 0 0 0 1];
	    k = k+1;
	  end;
        end;
        % pause
    end;
end;
fclose(fp);
