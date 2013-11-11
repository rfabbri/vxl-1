function [] = pcp3dwrite(fid,pts3d)

fprintf(1,'Saving the point cloud in P3D format ...\n');
N = size(pts3d,2);
fprintf(fid,'3\n');
fprintf(fid,'%d\n',N);
for i = 1:N
    fprintf(fid, '%d\t%d\t%d\n', pts3d(1,i), pts3d(2,i), pts3d(3,i));
end
