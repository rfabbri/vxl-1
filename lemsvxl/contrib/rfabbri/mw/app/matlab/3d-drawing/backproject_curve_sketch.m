clear all;
close all;

%addpath ~/lemsvxl/src/contrib/ntrinh/matlab/utils/
%addpath ./amsterdam-house-quarter-size-small-subset-mcs-work/cemv/
%addpath ../../Curve-Sketch-Datasets/amsterdam-house-quarter-size-small-subset-mcs-work/

% read_curve_sketch3;
% recs_old = recs;

%read_supporting_edges;

read_curve_sketch4;
recs10 = recs;
read_curve_sketch4a;
recs13 = recs;
read_curve_sketch4b;
recs6 = recs;
read_curve_sketch4c;
recs7 = recs;
read_curve_sketch4d;
recs8 = recs;

numIM = 27;
%nRoo = size(recs_oldest,2);
% nRo = size(recs_old,2);
nR10 = size(recs10,2);
nR13 = size(recs13,2);
nR6 = size(recs6,2);
nR7 = size(recs7,2);
nR8 = size(recs8,2);

%Replace the reserved colors that are used for image curves
colors = distinguishable_colors(numIM);

load clusters-close-edges.mat;

for n=7:5:7
    
    %if(mod(n,5)==0)
    if(1)
        fid = fopen(['./calibration/',num2str(n-1,'%08d'),'.projmatrix']);
        curP = (fscanf(fid,'%f',[4 3]))';

        [edg edgmap] = load_edg(['./edges/',num2str(n-1,'%08d'),'.edg']);
        
        curIM = imread(['./images/',num2str(n-1,'%08d'),'.jpg']);
        h=figure;imshow(curIM);
        set(figure(1),'Units','Normalized','OuterPosition',[0 0 1 1]);
        hold on;

        %draw_cons(['./curves_yuliang/',num2str(n-1,'%08d'),'.cemv'],[num2str(n-1,'%08d'),'.jpg'],0,495);
        
        ncc = size(clusters{11,1},2);
        
        %VIEW 10
        for rr=1:ncc
            r = clusters{11,1}(1,rr);
            curCurve = recs10{1,r};
            numSamples = size(curCurve,1);

            reprojCurve = zeros(numSamples,2);

            for s=1:numSamples

                curSample = [(curCurve(s,:))';1];
                imSample = curP*curSample;
                imSample = imSample./imSample(3,1);

                reprojCurve(s,1) = imSample(1,1);
                reprojCurve(s,2) = imSample(2,1);

            end

            %plot(reprojCurve(:,1)+1,reprojCurve(:,2)+1,'Color',colors(r,:),'LineWidth',1);
            plot(reprojCurve(:,1)+1,reprojCurve(:,2)+1,'Color',colors(11,:),'LineWidth',2);

        end
        
%         ncc = size(clusters{14,1},2);
%         
%         %VIEW 13
%         for rr=1:ncc
%             r = clusters{14,1}(1,rr);
%             curCurve = recs13{1,r};
%             numSamples = size(curCurve,1);
% 
%             reprojCurve = zeros(numSamples,2);
% 
%             for s=1:numSamples
% 
%                 curSample = [(curCurve(s,:))';1];
%                 imSample = curP*curSample;
%                 imSample = imSample./imSample(3,1);
% 
%                 reprojCurve(s,1) = imSample(1,1);
%                 reprojCurve(s,2) = imSample(2,1);
% 
%             end
% 
%             %plot(reprojCurve(:,1)+1,reprojCurve(:,2)+1,'Color',colors(r,:),'LineWidth',1);
%             plot(reprojCurve(:,1)+1,reprojCurve(:,2)+1,'Color',colors(14,:),'LineWidth',2);
% 
%         end
%         
%         ncc = size(clusters{7,1},2);
        
%         %VIEW 6
%         for rr=1:ncc
%         %for r=1:nR6
%             r = clusters{7,1}(1,rr);
%             curCurve = recs6{1,r};
%             numSamples = size(curCurve,1);
% 
%             reprojCurve = zeros(numSamples,2);
% 
%             for s=1:numSamples
% 
%                 curSample = [(curCurve(s,:))';1];
%                 imSample = curP*curSample;
%                 imSample = imSample./imSample(3,1);
% 
%                 reprojCurve(s,1) = imSample(1,1);
%                 reprojCurve(s,2) = imSample(2,1);
% 
%             end
% 
%             %plot(reprojCurve(:,1)+1,reprojCurve(:,2)+1,'Color',colors(r,:),'LineWidth',1);
%             plot(reprojCurve(:,1)+1,reprojCurve(:,2)+1,'Color',colors(7,:),'LineWidth',1);
% 
%         end
%         
        ncc = size(clusters{8,1},2);
        
        %VIEW 7
        for rr=1:ncc
            r = clusters{8,1}(1,rr);
            curCurve = recs7{1,r};
            numSamples = size(curCurve,1);

            reprojCurve = zeros(numSamples,2);

            for s=1:numSamples

                curSample = [(curCurve(s,:))';1];
                imSample = curP*curSample;
                imSample = imSample./imSample(3,1);

                reprojCurve(s,1) = imSample(1,1);
                reprojCurve(s,2) = imSample(2,1);

            end

            %plot(reprojCurve(:,1)+1,reprojCurve(:,2)+1,'Color',colors(r,:),'LineWidth',1);
            plot(reprojCurve(:,1)+1,reprojCurve(:,2)+1,'Color',colors(8,:),'LineWidth',2);

        end
        
%         ncc = size(clusters{9,1},2);
%         
%         %VIEW 8
%         for rr=1:ncc
%             r = clusters{9,1}(1,rr);
%             curCurve = recs8{1,r};
%             numSamples = size(curCurve,1);
% 
%             reprojCurve = zeros(numSamples,2);
% 
%             for s=1:numSamples
% 
%                 curSample = [(curCurve(s,:))';1];
%                 imSample = curP*curSample;
%                 imSample = imSample./imSample(3,1);
% 
%                 reprojCurve(s,1) = imSample(1,1);
%                 reprojCurve(s,2) = imSample(2,1);
% 
%             end
% 
%             %plot(reprojCurve(:,1)+1,reprojCurve(:,2)+1,'Color',colors(r,:),'LineWidth',1);
%             plot(reprojCurve(:,1)+1,reprojCurve(:,2)+1,'Color',colors(9,:),'LineWidth',2);
% 
%         end
        

        hold off;
%         print_pdf([num2str(n-1,'%08d'),'.pdf']);
%         close all;
        
    end

%     if(n~=11 && n~=14)
%         idx = idx+1;
%     end
end