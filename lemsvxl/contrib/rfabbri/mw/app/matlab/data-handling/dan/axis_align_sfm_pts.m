function [new_pts, newRs, newTs] = axis_align_sfm_pts(pts, Rs, Ts)

% assume the ground plane is dominant
[plane, plane_inliers] = fit_plane_ransac(pts);
gnd_pts_idx = find(plane_inliers);

rz = plane(1:3)';
rx = [1 0 0];
ry = cross(rz,rx);
ry = ry / norm(ry);
rx = cross(ry,rz);

R = [rx; ry; rz];

ncams = length(Rs);
new_pts = R * pts;
newRs = cell(1,ncams);
newTs = cell(1,ncams);

gnd_centroid = mean(new_pts(:,gnd_pts_idx),2);
cloud_centroid = mean(new_pts,2);
if (gnd_centroid(3) > cloud_centroid(3))
    % z is pointing down - flip
    R = rotx(pi)*R;
    new_pts = R * pts;
    gnd_centroid = mean(new_pts(:,gnd_pts_idx),2);
end

T = -gnd_centroid;
new_pts = [R T] * [pts; ones(1,length(pts))];

%plot3_vector(new_pts,'k.');
%hold on


for i=1:ncams
    RTnew = [Rs{i} Ts{i}; 0 0 0 1] * [R' -R'*T; 0 0 0 1];
    newRs{i} = RTnew(1:3,1:3);
    newTs{i} = RTnew(1:3,4);
    
    %plot_camera(eye(3),newRs{i},newTs{i},'g');
end

axis equal

return
