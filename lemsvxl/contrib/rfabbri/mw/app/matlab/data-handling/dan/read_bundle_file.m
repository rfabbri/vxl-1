function [Ks, Rs, Ts, pts] = read_bundle_file(fname, npix_x, npix_y)

fd = fopen(fname,'r');
if (fd < 0)
    disp(['error opening file ' fname ' for read.']);
    return;
end

pp_x = npix_x/2;
pp_y = npix_y/2;

header = fgetl(fd);
ncams = fscanf(fd,'%d',1);
npts = fscanf(fd,'%d',1);

Ks = cell(1,ncams);
Rs = cell(1,ncams);
Ts = cell(1,ncams);

for c=1:ncams
    fc = fscanf(fd,'%f',1);
    dist_coeffs = fscanf(fd,'%f',2);
    R = fscanf(fd,'%f',[3 3]);
    % matlab reads "column-wise"
    R = R';
    T = fscanf(fd,'%f',[3 1]);
    % we want viewing direction along positive z axis, not negative
    R(3,:) = -R(3,:);
    R(2,:) = -R(2,:);
    T(3) = -T(3);
    T(2) = -T(2);
    K = [fc 0 pp_x; 0 fc pp_y; 0 0 1];
    Ks{c} = K;
    Rs{c} = R;
    Ts{c} = T;
end

pts = zeros(3,npts);

for p=1:npts
    pts(:,p) = fscanf(fd,'%f',[3 1]);
    dummy = fgetl(fd);
    color = fgetl(fd);
    img_pts = fgetl(fd);
end

fclose(fd);

% axis-align points and cameras
[pts, Rs, Ts] = axis_align_sfm_pts(pts,Rs,Ts);
pts = filter_point_cloud(pts);

return


    
