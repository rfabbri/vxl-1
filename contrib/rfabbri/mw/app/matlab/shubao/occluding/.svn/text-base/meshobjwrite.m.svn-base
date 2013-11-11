function [] = meshobjwrite(fid,pts3d,tri)

fprintf(1,'Saving the mesh as OBJ ...\n');
npts3d = size(pts3d,2);
for i = 1:npts3d
    fprintf(fid,'v %d %d %d\n', pts3d(1,i),pts3d(3,i),pts3d(2,i));
end
fprintf(fid,'# %d vertices\n', npts3d);
ntri = size(tri,2);
for i = 1:ntri
    fprintf(fid,'f %d %d %d\n',tri(1,i)+1,tri(2,i)+1,tri(3,i)+1);
% Note the obj file format is little different from vrml and ply. It counts the vertex from 0 instead of 1.
end

