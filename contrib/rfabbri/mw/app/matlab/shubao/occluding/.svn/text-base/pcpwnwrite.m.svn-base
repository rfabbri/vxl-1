function [] = pcpwnwrite(fid,pts3d,normal)

fprintf(1,'Saving the point cloud in PWN format ...\n');
N = size(pts3d,2);
fprintf(fid,'%d\n',N);
for i = 1:N
    fprintf(fid, '%d\t%d\t%d\n', pts3d(1,i), pts3d(2,i), pts3d(3,i));
end
for i = 1:N
    fprintf(fid, '%d\t%d\t%d\n', normal(1,i), normal(2,i), normal(3,i));
end