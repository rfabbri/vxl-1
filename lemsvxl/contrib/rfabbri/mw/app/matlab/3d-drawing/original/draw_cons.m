function [cons]=draw_cons(filename,img_filename,lengthThreshold,ID)

addpath export_fig
base_dir='/vision/scratch/maruthi/DataSets/cats_dogs/trainval_bp/';
[path,name,str]=fileparts(filename);
cons=read_cem_file(filename);

%Calculate the lengths of each contour

numCons = size(cons,2);
conLengths = zeros(numCons,1);

if(lengthThreshold > 0)
    for c=1:numCons
        curCon = cons{1,c};
        numSamples = size(curCon,1);
        curLength = 0;

        for s=2:numSamples
            xPrev = curCon(s-1,1);
            yPrev = curCon(s-1,2);

            xCur = curCon(s,1);
            yCur = curCon(s,2);

            curDist = sqrt((xCur-xPrev)^2 + (yCur - yPrev)^2);
            curLength = curLength + curDist;
        end

        conLengths(c,1) = curLength;
    end
end

index=strfind(name,'_04');
category=name(1:(index-1));


%imshow(img_filename);
%draw_contours(cons,0,1,[1 1 0.99],0,[1 0 1],conLengths,lengthThreshold,ID);
draw_contours(cons,0,0,[1 1 0.99],0,[1 0 1],conLengths,lengthThreshold,ID);

%eval(['export_fig -native ' name '_con_map.png'])
