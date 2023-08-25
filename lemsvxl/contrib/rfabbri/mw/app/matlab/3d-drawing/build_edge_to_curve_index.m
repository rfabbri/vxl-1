clear all;
close all;

fileNames = dir('./curve-edge/*.txt');
N = size(fileNames,1);

curveIndices = cell(N,1);
curveOffsets = cell(N,1);

for nn=1:N
    
    nn
    
    fileName = fileNames(nn,1).name;
    tloc = findstr(fileName,'.txt');
    viewName = fileName(1,1:(tloc-1));
   
    [edg edgmap] = load_edg(['./edges/',viewName,'.edg']);
    numEdges = size(edg,1);
    
    clear edg;
    clear edgmap;
    
    curveIndex = (-1).*ones(numEdges,1);
    curveOffset = (-1).*ones(numEdges,1);
    
    contours = read_cem_file(['./curves/',viewName,'.cemv']);
    numContours = size(contours,2);
    clear contours
    
    fid = fopen(['./curve-edge/',viewName,'.txt']);
    
    for c=1:numContours
       numCurEdges = fscanf(fid,'%d',[1 1]);
       edgeIDs = fscanf(fid,'%d',[1 numCurEdges]);
       
       for e=1:numCurEdges
           %MATLAB indices for curves, edgeIDs are 0-indices
           curveIndex(edgeIDs(1,e)+1,1) = c;
           curveOffset(edgeIDs(1,e)+1,1) = e-1;
       end
       
    end
    curveIndices{nn,1} = curveIndex;
    curveOffsets{nn,1} = curveOffset;
    
end

save('edge-curve-index_yuliang_pavilion-mixed.mat','curveIndices');
save('edge-curve-offset_yuliang_pavilion-mixed.mat','curveOffsets');