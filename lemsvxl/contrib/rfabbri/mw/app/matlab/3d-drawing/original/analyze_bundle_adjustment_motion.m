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

confVec = [2 9 10 45];
anchVec = [7 12];

%Replace the reserved colors that are used for image curves
colors = distinguishable_colors(9);
% colors(5,:) = colors(nR+1,:);
% colors(6,:) = colors(nR+2,:);
% colors(46,:) = colors(nR+3,:);
% colors(65,:) = colors(nR+4,:);
% colors(78,:) = colors(nR+5,:);
% 
% selected_curves = cell(48,1);
% selected_curves{1,1} = [100 389 446];
% selected_curves{2,1} = [423 434 442 493];
% selected_curves{3,1} = [162 168 461]; 
% selected_curves{4,1} = [89 728 739]; 
% selected_curves{5,1} = [74 137 645]; 
% selected_curves{6,1} = [58 103 762]; 
% selected_curves{7,1} = [323 440]; 
% selected_curves{8,1} = [256 396]; 
% selected_curves{9,1} = [225 327]; 
% selected_curves{10,1} = [179 246]; 
% selected_curves{11,1} = [322]; 
% selected_curves{12,1} = [315]; 
% selected_curves{13,1} = [405]; 
% selected_curves{14,1} = [243 364]; 
% selected_curves{15,1} = [384 389]; 
% selected_curves{16,1} = [299 337]; 
% selected_curves{17,1} = [228 295 544]; 
% selected_curves{18,1} = [286 323 374]; 
% selected_curves{19,1} = [226 263 471]; 
% selected_curves{20,1} = [283 306 317];
% selected_curves{21,1} = [40 54 288 306 564]; 
% selected_curves{22,1} = [355 463]; 
% selected_curves{23,1} = [374];
% selected_curves{24,1} = [330 345 396 551]; 
% selected_curves{25,1} = [435 436 472]; 
% selected_curves{26,1} = [539 542]; 
% selected_curves{27,1} = [198 216 228 231 242]; 
% selected_curves{28,1} = [228 229 242 251 275 403]; 
% selected_curves{29,1} = [198 257 314]; 
% selected_curves{30,1} = [17 295 309]; 
% selected_curves{31,1} = [260]; 
% selected_curves{32,1} = [205 402]; 
% selected_curves{33,1} = [220 410]; 
% selected_curves{34,1} = [587 619]; 
% selected_curves{35,1} = [759]; 
% selected_curves{36,1} = [130 764]; 
% selected_curves{37,1} = [586 668]; 
% selected_curves{38,1} = [260 278 300 310]; 
% selected_curves{39,1} = [271 285 352]; 
% selected_curves{40,1} = [229 243]; 
% selected_curves{41,1} = [301 309]; 
% selected_curves{42,1} = [233 237]; 
% selected_curves{43,1} = [147 183]; 
% selected_curves{44,1} = [83 97 136 137]; 
% selected_curves{45,1} = [52 53 270 281]; 
% selected_curves{46,1} = [263 269 271 287]; 
% selected_curves{47,1} = [40 221 245]; 
% selected_curves{48,1} = [74 224 231 232 239]; 
% idx = 1;
%curve_overlap1
%for n=22:30

%curve_overlap2
%for n=1:13

%curve_overlap3
% for nn = 1:7
%     
%     n = viewVec(1,nn);

%otherwise
% seed = 11;
% secAnc = 45;

% load modifiedCurve-10-1.mat;
% plotCurve10 = modifiedCurve;
% clear modifiedCurve;

plotCurves13 = cell(3,1);
load modifiedCurve-13-177im-plane.mat;
plotCurves13{1,1} = modifiedCurve;
%plotCurves13{1,1} = perpModifiedCurve;
clear modifiedCurve;
load modifiedCurve-13-178im-plane.mat;
plotCurves13{2,1} = modifiedCurve;
%plotCurves13{2,1} = perpModifiedCurve;
clear modifiedCurve;
load modifiedCurve-13-179im-plane.mat;
plotCurves13{3,1} = modifiedCurve;
%plotCurves13{3,1} = perpModifiedCurve;
clear modifiedCurve;

plotCurves6 = cell(2,1);
load modifiedCurve-6-33im-plane.mat;
plotCurves6{1,1} = modifiedCurve;
%plotCurves6{1,1} = perpModifiedCurve;
clear modifiedCurve;
load modifiedCurve-6-176im-plane.mat;
plotCurves6{2,1} = modifiedCurve;
%plotCurves6{2,1} = perpModifiedCurve;
clear modifiedCurve;

% plotCurves7 = cell(2,1);
% load modifiedCurve-7-68.mat;
% plotCurves7{1,1} = modifiedCurve;
% %plotCurves7{1,1} = perpModifiedCurve;
% clear modifiedCurve;
% load modifiedCurve-7-165im-plane.mat;
% plotCurves7{2,1} = modifiedCurve;
% clear modifiedCurve;

% plotCurves8 = cell(2,1);
% load modifiedCurve-8-3.mat;
% plotCurves8{1,1} = modifiedCurve;
% %plotCurves8{1,1} = perpModifiedCurve;
% clear modifiedCurve;
% load modifiedCurve-8-4.mat;
% plotCurves8{2,1} = modifiedCurve;
% %plotCurves8{2,1} = perpModifiedCurve;
% clear modifiedCurve;

%STRAND 0
allPlotCurvesVec{1,1} = 1;
allPlotCurvesVec{2,1} = [177 178 179];
allPlotCurvesVec{3,1} = [33 176];
allPlotCurvesVec{4,1} = [1 132];
allPlotCurvesVec{5,1} = [3 4];

% %STRAND 1
% allPlotCurvesVec{1,1} = [57 58 59];
% allPlotCurvesVec{2,1} = [];
% allPlotCurvesVec{3,1} = 69;
% allPlotCurvesVec{4,1} = [46 47];
% allPlotCurvesVec{5,1} = 49;

% %STRAND 2
% allPlotCurvesVec{1,1} = [];
% allPlotCurvesVec{2,1} = [];
% allPlotCurvesVec{3,1} = 163;
% allPlotCurvesVec{4,1} = 165;
% allPlotCurvesVec{5,1} = 136;

% %STRAND 3
% allPlotCurvesVec{1,1} = 211;
% allPlotCurvesVec{2,1} = 183;
% allPlotCurvesVec{3,1} = [31 32];
% allPlotCurvesVec{4,1} = [130 131];
% allPlotCurvesVec{5,1} = [1 2];

% %STRAND 4
% allPlotCurvesVec{1,1} = [];
% allPlotCurvesVec{2,1} = 59;
% allPlotCurvesVec{3,1} = 29;
% allPlotCurvesVec{4,1} = 68;
% allPlotCurvesVec{5,1} = 50;

for n=1:5:numIM
    
    %if(mod(n,5)==0)
    if(1)
        fid = fopen(['./calibration/',num2str(n-1,'%08d'),'.projmatrix']);
        curP = (fscanf(fid,'%f',[4 3]))';

        [edg edgmap] = load_edg(['./edges/',num2str(n-1,'%08d'),'.edg']);
        
        curIM = imread(['./images/',num2str(n-1,'%08d'),'.jpg']);
        h=figure;imshow(curIM);
        set(figure(1),'Units','Normalized','OuterPosition',[0 0 1 1]);
        hold on;

        draw_cons(['./curves_yuliang/',num2str(n-1,'%08d'),'.cemv'],[num2str(n-1,'%08d'),'.jpg'],0,-1);

%         %VIEW 10
%          for r=1:3
% %             curCurve = recs10{1,r};
% %             numSamples = size(curCurve,1);
% % 
% %             reprojCurve = zeros(numSamples,2);
% % 
% %             for s=1:numSamples
% % 
% %                 curSample = [(curCurve(s,:))';1];
% %                 imSample = curP*curSample;
% %                 imSample = imSample./imSample(3,1);
% % 
% %                 reprojCurve(s,1) = imSample(1,1);
% %                 reprojCurve(s,2) = imSample(2,1);
% % 
% %             end
% % 
% %             %plot(reprojCurve(:,1)+1,reprojCurve(:,2)+1,'Color',colors(r,:),'LineWidth',1);
% %             plot(reprojCurve(:,1)+1,reprojCurve(:,2)+1,'Color',colors(9,:),'LineWidth',1);
%                 
%             modifiedCurve = plotCurve10;
%             numSamples = size(modifiedCurve,1);
% 
%             reprojCurve = zeros(numSamples,2);
% 
%             for s=1:numSamples
% 
%                 curSample = [(modifiedCurve(s,:))';1];
%                 imSample = curP*curSample;
%                 imSample = imSample./imSample(3,1);
% 
%                 reprojCurve(s,1) = imSample(1,1);
%                 reprojCurve(s,2) = imSample(2,1);
% 
%             end
% 
%             plot(reprojCurve(:,1)+1,reprojCurve(:,2)+1,'Color',colors(9,:),'LineWidth',1);
%             
%         end
        
        %VIEW 13
         for rr=1:3
            r = allPlotCurvesVec{2,1}(1,rr);
            curCurve = recs13{1,r};
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
            %plot(reprojCurve(:,1)+1,reprojCurve(:,2)+1,'Color',colors(5,:),'LineWidth',1);
            plot(reprojCurve(:,1)+1,reprojCurve(:,2)+1,'Color','m','LineWidth',1);
                
            modifiedCurve = plotCurves13{rr,1};
            numSamples = size(modifiedCurve,1);

            reprojCurve = zeros(numSamples,2);

            for s=1:numSamples

                curSample = [(modifiedCurve(s,:))';1];
                imSample = curP*curSample;
                imSample = imSample./imSample(3,1);

                reprojCurve(s,1) = imSample(1,1);
                reprojCurve(s,2) = imSample(2,1);

            end

            %plot(reprojCurve(:,1)+1,reprojCurve(:,2)+1,'Color',colors(5,:),'LineWidth',1);
            plot(reprojCurve(:,1)+1,reprojCurve(:,2)+1,'Color','k','LineWidth',1);
            
        end
        
        %VIEW 6
         for rr=1:2
            r = allPlotCurvesVec{3,1}(1,rr);
            curCurve = recs6{1,r};
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
            %plot(reprojCurve(:,1)+1,reprojCurve(:,2)+1,'Color',colors(6,:),'LineWidth',1);
            plot(reprojCurve(:,1)+1,reprojCurve(:,2)+1,'Color','r','LineWidth',1);
                
            modifiedCurve = plotCurves6{rr,1};
            numSamples = size(modifiedCurve,1);

            reprojCurve = zeros(numSamples,2);

            for s=1:numSamples

                curSample = [(modifiedCurve(s,:))';1];
                imSample = curP*curSample;
                imSample = imSample./imSample(3,1);

                reprojCurve(s,1) = imSample(1,1);
                reprojCurve(s,2) = imSample(2,1);

            end

            %plot(reprojCurve(:,1)+1,reprojCurve(:,2)+1,'Color',colors(6,:),'LineWidth',1);
            plot(reprojCurve(:,1)+1,reprojCurve(:,2)+1,'Color','g','LineWidth',1);
            
        end
        
%         %VIEW 7
%         for rr=1:1
%             r = allPlotCurvesVec{4,1}(1,rr);
%             curCurve = recs7{1,r};
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
%             %plot(reprojCurve(:,1)+1,reprojCurve(:,2)+1,'Color',colors(7,:),'LineWidth',1);
%             plot(reprojCurve(:,1)+1,reprojCurve(:,2)+1,'Color','m','LineWidth',1);
%                 
%             modifiedCurve = plotCurves7{rr,1};
%             numSamples = size(modifiedCurve,1);
% 
%             reprojCurve = zeros(numSamples,2);
% 
%             for s=1:numSamples
% 
%                 curSample = [(modifiedCurve(s,:))';1];
%                 imSample = curP*curSample;
%                 imSample = imSample./imSample(3,1);
% 
%                 reprojCurve(s,1) = imSample(1,1);
%                 reprojCurve(s,2) = imSample(2,1);
% 
%             end
% 
%             %plot(reprojCurve(:,1)+1,reprojCurve(:,2)+1,'Color',colors(7,:),'LineWidth',1);
%             plot(reprojCurve(:,1)+1,reprojCurve(:,2)+1,'Color','k','LineWidth',1);
%             
%         end
        
%         %VIEW 8
%         for rr=1:2
%             r = allPlotCurvesVec{5,1}(1,rr);
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
%             %(reprojCurve(:,1)+1,reprojCurve(:,2)+1,'Color',colors(8,:),'LineWidth',1);
%             plot(reprojCurve(:,1)+1,reprojCurve(:,2)+1,'Color','m','LineWidth',1);
%                 
%             modifiedCurve = plotCurves8{rr,1};
%             numSamples = size(modifiedCurve,1);
% 
%             reprojCurve = zeros(numSamples,2);
% 
%             for s=1:numSamples
% 
%                 curSample = [(modifiedCurve(s,:))';1];
%                 imSample = curP*curSample;
%                 imSample = imSample./imSample(3,1);
% 
%                 reprojCurve(s,1) = imSample(1,1);
%                 reprojCurve(s,2) = imSample(2,1);
% 
%             end
% 
%             %plot(reprojCurve(:,1)+1,reprojCurve(:,2)+1,'Color',colors(8,:),'LineWidth',1);
%             plot(reprojCurve(:,1)+1,reprojCurve(:,2)+1,'Color','k','LineWidth',1);
%             
%         end

        hold off;
        %export_fig([num2str(n-1,'%08d'),'.pdf']);
        print_pdf([num2str(n-1,'%08d'),'.pdf']);
        close all;
        
    end

%     if(n~=11 && n~=14)
%         idx = idx+1;
%     end
end