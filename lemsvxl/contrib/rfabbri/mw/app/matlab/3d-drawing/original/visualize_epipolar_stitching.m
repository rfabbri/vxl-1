clear all;
close all;

addpath ~/lemsvxl/src/contrib/ntrinh/matlab/utils/
%addpath ./amsterdam-house-quarter-size-small-subset-mcs-work/cemv/
%addpath ../../Curve-Sketch-Datasets/amsterdam-house-quarter-size-small-subset-mcs-work/

path_to_out='./amsterdam-house-quarter-size-original-order-small-subset-mcs-work/minlength-10_mininliersview-8_minviews-4_48-conf_edgel-support/anch-10-13_pruning-fix/';
%Using 0-index for views, not the MATLAB index
views_to_project = [10 13];

read_curve_sketch2;
numIM = 50;
nR = size(recs,2);
use_first_anchor=1;

attr_file = [path_to_out, 'attributes.txt'];
fid = fopen(attr_file);
numCurves = fscanf(fid,'%d',[1 1]);

if(numCurves~=nR)
    disp('ERROR: Number of curves is not the same in 3D curve sketch files and attributes file');
    disp('The results will not be reliable');
end

%Total edge support each curve receives
totalSupport = zeros(1,numCurves);
%Length of the image curve in 1st anchor view that seeded the 3D curve
curveLength = zeros(1,numCurves);
%ID of the image curve in the 1st anchor view that seeded the 3D curve
%After epipolar breaking
curvesV0 = zeros(1,numCurves);
%ID of the image curve in the 2nd anchor view that seeded the 3D curve
%After epipolar breaking
curvesV1 = zeros(1,numCurves);
%ID of the image curve in the 1st anchor view that seeded the 3D curve
%Before epipolar breaking
origID_V0 = zeros(1,numCurves);
%ID of the image curve in the 2nd anchor view that seeded the 3D curve
%Before epipolar breaking
origID_V1 = zeros(1,numCurves);

for r=1:numCurves
   curAttr = fscanf(fid,'%d',[1 8]);
   
   totalSupport(1,r) = curAttr(1,1);
   curveLength(1,r) = curAttr(1,2);
   curvesV0(1,r) = curAttr(1,3);
   curvesV1(1,r) = curAttr(1,4);
   origID_V0(1,r) = curAttr(1,5);
   origID_V1(1,r) = curAttr(1,6);

end

%A value of 1 in this vector means there were initial cues for stitching
stitchingFlags = zeros(1,numCurves);
%Same numbers in this vector means the respective curves should be stitched
%-1 is a flag for do not stitch
stitchingLabels = (-1).*ones(1,numCurves);

if(use_first_anchor)
    maxOrigID = max(origID_V0);      
    maxCurvesID = max(curvesV0);

    for i=0:maxOrigID
        %Find all the indices that could potentially be stitched
        siblingCurvesIDX = find(origID_V0==i);
        sc = size(siblingCurvesIDX,2);
        %If there's more than 1 such curve, mark them as potentially associated
        if(sc>1)
            stitchingFlags(siblingCurvesIDX) = 1;
        end
    end

    %Find the curves that have the same underlying original curve ID
    %If their post-breakup versions are adjacent, then the gap could not be due
    %to occlusion so they should be marked to be linked.

    curIDX = 1;

    %Go over all the broken curve IDs
    for c=0:maxCurvesID
        %Find the current broken curve in the array
        curLoc = find(curvesV0==c);

        if(size(curLoc,2)>1)
            disp('WARNING: More than 1 curve ID detected in the broken curves');
            disp('The results will not be reliable');
        elseif(size(curLoc,2)>0 && stitchingFlags(1,curLoc))
            if(c==1)
                stitchingLabels(1,curLoc) = curIDX;
            else
                prevLoc = find(curvesV0==c-1);
                if(stitchingLabels(1,prevLoc) > -1)
                    stitchingLabels(1,curLoc) = curIDX; 
                else
                    curIDX = curIDX+1;
                    stitchingLabels(1,curLoc) = curIDX;
                end
            end
        end

    end
    
else
    maxOrigID = max(origID_V1);      
    maxCurvesID = max(curvesV1);

    %A value of 1 in this vector means there were initial cues for stitching
    stitchingFlags = zeros(1,numCurves);
    %Same numbers in this vector means the respective curves should be stitched
    %-1 is a flag for do not stitch
    stitchingLabels = (-1).*ones(1,numCurves);

    for i=0:maxOrigID
        %Find all the indices that could potentially be stitched
        siblingCurvesIDX = find(origID_V1==i);
        sc = size(siblingCurvesIDX,2);
        %If there's more than 1 such curve, mark them as potentially associated
        if(sc>1)
            stitchingFlags(siblingCurvesIDX) = 1;
        end
    end

    %Find the curves that have the same underlying original curve ID
    %If their post-breakup versions are adjacent, then the gap could not be due
    %to occlusion so they should be marked to be linked.

    curIDX = 1;

    %Go over all the broken curve IDs
    for c=0:maxCurvesID
        %Find the current broken curve in the array
        curLoc = find(curvesV1==c);

        if(size(curLoc,2)>1)
            disp('WARNING: More than 1 curve ID detected in the broken curves');
            disp('The results will not be reliable');
        elseif(size(curLoc,2)>0 && stitchingFlags(1,curLoc))
            if(c==1)
                stitchingLabels(1,curLoc) = curIDX;
            else
                prevLoc = find(curvesV1==c-1);
                if(stitchingLabels(1,prevLoc) > -1)
                    stitchingLabels(1,curLoc) = curIDX; 
                else
                    curIDX = curIDX+1;
                    stitchingLabels(1,curLoc) = curIDX;
                end
            end
        end

    end
end

for sl=1:numCurves
    if(size(find(stitchingLabels==stitchingLabels(1,sl)),2)==1)
        stitchingLabels(1,sl) = -1;
    end
end

%  curvesV0
%  origID_V0
%  stitchingFlags
%  stitchingLabels

colors = distinguishable_colors(max(stitchingLabels));

for n=1:numIM
    
    if(size(find(views_to_project==(n-1)),2)>0)

        fid = fopen(['./calibration/',num2str(n-1,'%08d'),'.projmatrix']);
        curP = (fscanf(fid,'%f',[4 3]))';

        [edg edgmap] = load_edg(['./edges/',num2str(n-1,'%08d'),'.edg']);

        curIM = imread(['./images/',num2str(n-1,'%08d'),'.jpg']);
        figure;imshow(curIM);
        hold on;

        draw_cons(['./curves/',num2str(n-1,'%08d'),'.cemv'],[num2str(n-1,'%08d'),'.jpg'],0);

        for r=1:nR

            if(stitchingLabels(1,r)>-1)

                curCurve = recs{1,r};
                numSamples = size(curCurve,1);

                reprojCurve = zeros(numSamples,2);

                for s=1:numSamples

                    curSample = [(curCurve(s,:))';1];
                    imSample = curP*curSample;
                    imSample = imSample./imSample(3,1);

                    reprojCurve(s,1) = imSample(1,1);
                    reprojCurve(s,2) = imSample(2,1);

                end

                plot(reprojCurve(:,1),reprojCurve(:,2),'-','LineWidth',1,'Color',colors(stitchingLabels(1,r),:));

            end
        end


        hold off;
        print_pdf([num2str(n-1,'%08d'),'.pdf']);
        close all;
    end
    
end