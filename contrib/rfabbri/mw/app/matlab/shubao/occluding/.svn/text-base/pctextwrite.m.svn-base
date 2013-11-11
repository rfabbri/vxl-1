function [] = pctextwrite(fid, pts3d)
% save the point cloud into a raw text file
%
% @author: Shubao Liu (shubao.liu@gmail.com)
% @date: May, 2006

fprintf(1,'Saving the point cloud as text file ...\n');
N = size(pts3d,2);
for i = 1:N
    fprintf(fid, '%d\t%d\t%d\n', pts3d(1,i), pts3d(2,i), pts3d(3,i));
end