%This is older function to do the mate curve calculation on the matlab side
%visualize_mate_curves is the up-to-date version

clear all;
close all;

addpath ~/lemsvxl/src/contrib/ntrinh/matlab/utils/
%addpath ./amsterdam-house-quarter-size-small-subset-mcs-work/cemv/
%addpath ../../Curve-Sketch-Datasets/amsterdam-house-quarter-size-small-subset-mcs-work/

read_curve_sketch1;
read_supporting_edges;
load edge-curve-index.mat;

% read_curve_sketch2;
% recs_old = recs;
% 
% read_curve_sketch3;

numIM = 50;
% nRoo = size(recs_oldest,2);
% nRo = size(recs_old,2);
nR = size(recs,2);

%Replace the reserved colors that are used for image curves
colors = distinguishable_colors(nR+5);
colors(5,:) = colors(nR+1,:);
colors(6,:) = colors(nR+2,:);
colors(46,:) = colors(nR+3,:);
colors(65,:) = colors(nR+4,:);
colors(78,:) = colors(nR+5,:);

d_threshold = 1;
%views = [10 15 11 12 13 16 17 9 8 19 6 20];

for n=1:numIM
    %n = views(1,v)+1;
    
    fid = fopen(['./calibration/',num2str(n-1,'%08d'),'.projmatrix']);
    curP = (fscanf(fid,'%f',[4 3]))';

    curEdgeCurveIndex = curveIndices{n,1};
    
    curIM = imread(['./images/',num2str(n-1,'%08d'),'.jpg']);
    imshow(curIM);
    set(figure(1),'Units','Normalized','OuterPosition',[0 0 1 1]);
    hold on;
    
    contours = read_cem_file(['./curves/',num2str(n-1,'%08d'),'.cemv']);
    draw_cons(['./curves/',num2str(n-1,'%08d'),'.cemv'],[num2str(n-1,'%08d'),'.jpg'],0);
    
    for r=32:32
        
        curCurve = recs{1,r};
        numSamples = size(curCurve,1);

        reprojCurve = zeros(numSamples,2);

        for s=1:numSamples

            curSample = [(curCurve(s,:))';1];
            imSample = curP*curSample;
            imSample = imSample./imSample(3,1);

            reprojCurve(s,1) = imSample(1,1) + 1;
            reprojCurve(s,2) = imSample(2,1) + 1;

        end
        
        curSupportViews = supportingViews{r,1};
        curEdgels = supportingEdgels{r,1};
        
        viewIDX = find(curSupportViews==(n-1));
        mate_curves = [];
        
        if(size(viewIDX,2)>0)
            edgels = curEdgels{1,viewIDX};
            numEdgels = size(edgels,2);
            
            for e=1:numEdgels
                edgelID = edgels(1,e) + 1;
                curveID = curEdgeCurveIndex(edgelID,1);
                
                loc = find(mate_curves==curveID);
                
                if(size(loc,2)==0)
                    mate_curves = [mate_curves curveID];                
                end
                
            end
            
            numMates = size(mate_curves,2);
            
            for mc=1:numMates
                curMateID = mate_curves(1,mc);
                if(curMateID>-1)
                    curMate = contours{1,curMateID};
                    plot(curMate(:,1),curMate(:,2),'-g','LineWidth',1);
                end
            end
            
        end
        plot(reprojCurve(:,1),reprojCurve(:,2),'-r','LineWidth',1);  
    end
    
    hold off;
    print_pdf([num2str(n-1,'%08d'),'.pdf']);
    close all;
end
