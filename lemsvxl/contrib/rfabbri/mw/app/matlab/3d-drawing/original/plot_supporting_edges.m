clear all;
close all;

addpath ~/lemsvxl/src/contrib/ntrinh/matlab/utils/
%addpath ./amsterdam-house-quarter-size-small-subset-mcs-work/cemv/
%addpath ../../Curve-Sketch-Datasets/amsterdam-house-quarter-size-small-subset-mcs-work/

read_curve_sketch4;
recs_oldest = recs;

read_supporting_edges;

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
views = [10 15 11 12 13 16 17 9 8 19 6 20];

for n=10:10
    
    %n = views(1,v)+1;
    
    fid = fopen(['./calibration/',num2str(n-1,'%08d'),'.projmatrix']);
    curP = (fscanf(fid,'%f',[4 3]))';
    
    [edg edgmap] = load_edg(['./edges/',num2str(n-1,'%08d'),'.edg']);
    
    curIM = imread(['./images/',num2str(n-1,'%08d'),'.jpg']);
    imshow(curIM);
    set(figure(1),'Units','Normalized','OuterPosition',[0 0 1 1]);
    hold on;
    
    disp_edg(edg,0);
    
    for r=1:nR
        
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
        
        plot(reprojCurve(:,1),reprojCurve(:,2),'-y','LineWidth',1);
        
        %close_edg = get_close_edges(edg, reprojCurve, d_threshold);
        %disp_edg(close_edg,1);
        
        curSupportViews = supportingViews{r,1};
        curEdgels = supportingEdgels{r,1};
        
        viewIDX = find(curSupportViews==(n-1));
        supporting_edg = [];
        
        if(size(viewIDX,2)>0)
            edgels = curEdgels{1,viewIDX};
            numEdgels = size(edgels,2)
            
            for e=1:numEdgels
%                 edgelID = edgels(1,e);
%                 edgeX = edg(edgelID+1,1);
%                 edgeY = edg(edgelID+1,2);
%                 
%                 plot(edgeX, edgeY, '.g');
                edgelID = edgels(1,e) + 1;
                edgel = edg(edgelID,:);
                supporting_edg = [supporting_edg; edgel];
            end
            
        end
        disp_edg(supporting_edg,2);    
    end
    
    hold off;
    print_pdf([num2str(n-1,'%08d'),'.pdf']);
    close all;
end