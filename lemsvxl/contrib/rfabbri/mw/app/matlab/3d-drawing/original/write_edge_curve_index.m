% clear all;
% close all;
% 
% load edge-curve-index_yuliang_fountain.mat;
% load edge-curve-offset_yuliang_fountain.mat;

for n=1:N
    fileName = fileNames(n,1).name;
    tloc = findstr(fileName,'.txt');
    viewName = fileName(1,1:(tloc-1));
    
    fid = fopen(['./edge-curve/',viewName,'.txt'],'w');
    fid_off = fopen(['./edge-curve/',viewName,'_offset.txt'],'w');
    
    curIndices = curveIndices{n,1};
    curOffsets = curveOffsets{n,1};
    
    numEdges = size(curIndices,1);
    fprintf(fid,'%d',numEdges);
    fprintf(fid,' ');
    fprintf(fid_off,'%d',numEdges);
    fprintf(fid_off,' ');
    
    for e=1:numEdges
        fprintf(fid,'%d',curIndices(e,1)-1);
        fprintf(fid,' ');
        fprintf(fid_off,'%d',curOffsets(e,1));
        fprintf(fid_off,' ');
    end
end