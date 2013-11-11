function [] = pcvrmlwrite(fid, pts3d)
%PCVRMLWRITE save the point cloud into a vrml file
%
% @author: Shubao Liu (shubao.liu@gmail.com)
% @date: May, 2006

fprintf(1,'Saving the point cloud as VRML ...\n');
fprintf(fid, '#VRML V2.0 utf8\n');
fprintf(fid,'Shape{\n');
fprintf(fid,'  appearance Appearance{\n');
fprintf(fid,'    material Material{\n');
fprintf(fid,'      diffuseColor 1 1 1\n');
fprintf(fid,'    }\n');
fprintf(fid,'  }\n\n');
fprintf(fid,'  geometry PointSet{\n');
fprintf(fid,'    solid TRUE\n');
fprintf(fid,'    coord Coordinate{\n');
fprintf(fid,'      point[\n');
N = size(pts3d,2);
for i = 1:N
    fprintf(fid,'    %d\t%d\t%d,\n', pts3d(1,i), pts3d(2,i), pts3d(3,i));
end
fprintf(fid,'      ]\n');
fprintf(fid,'    }\n');
fprintf(fid,'  }\n');
fprintf(fid,'}');
