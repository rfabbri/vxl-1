clear all;
close all;

numIM = 27;
colors = distinguishable_colors(numIM);

all_recs = cell(numIM,1);
all_nR = zeros(numIM,1);
all_links_3d = cell(numIM,1);
all_offsets_3d = cell(numIM,1);
all_edge_support_3d = cell(numIM,1);

all_num_im_contours = zeros(numIM,1);

read_curve_sketch4;
all_recs{11,1} = recs;
all_nR(11,1) = size(recs,2);
clear recs;
read_curve_sketch4a;
all_recs{14,1} = recs;
all_nR(14,1) = size(recs,2);
clear recs;
read_curve_sketch4b;
all_recs{7,1} = recs;
all_nR(7,1) = size(recs,2);
clear recs;
read_curve_sketch4c;
all_recs{8,1} = recs;
all_nR(8,1) = size(recs,2);
clear recs;
read_curve_sketch4d;
all_recs{9,1} = recs;
all_nR(9,1) = size(recs,2);
clear recs;

load edge-curve-index_yuliang.mat;
load edge-curve-offset_yuliang.mat;
load clusters-close-edges.mat;

%All the views that will be used in the clustering.
%The first view is where the initial seed/query curve is located.
all_views = [10 7];
numViews = size(all_views,2);

% recs = all_recs{11,1};
% plot_all_recs;
% hold on;
% recs = all_recs{8,1};
% plot_all_recs_overlay;
% hold on;

%FIX THE ORDERING AND MERGE THE CURVES

numSamples = size(curve1,1);

for s=50:82
   
    curSample = curve1(s,:);
    [closest_pt, outside_range,prev_pt,next_pt] = find_closest_point_on_curve(curve2,curSample);
    
    order_curve(s,:) = (order_curve(s,:) + closest_pt)./2;
%     if(~outside_range)
%          plot3([closest_pt(1,1) curSample(1,1)],[closest_pt(1,2) curSample(1,2)],[closest_pt(1,3) curSample(1,3)],'-m');
%     end
end

numSamples = size(curve2,1);

for s=1:numSamples
   
    curSample = curve2(s,:);
    [closest_pt, outside_range,prev_pt,next_pt] = find_closest_point_on_curve(curve1,curSample);
    
    oc_size = size(order_curve,1);
    prev_ind = index_curve(prev_pt,1);
    next_ind = index_curve(next_pt,1);
    order_curve = [order_curve(1:(next_ind-1),:); (closest_pt+curSample)./2; order_curve(next_ind:oc_size,:)];
    index_curve(next_pt:size1,1)=index_curve(next_pt:size1,1)+1;
%     if(~outside_range)
%          plot3([closest_pt(1,1) curSample(1,1)],[closest_pt(1,2) curSample(1,2)],[closest_pt(1,3) curSample(1,3)],'-g');
%     end
end

for n=16:5:16
    
    fid = fopen(['./calibration/',num2str(n-1,'%08d'),'.projmatrix']);
    curP = (fscanf(fid,'%f',[4 3]))';

    curIM = imread(['./images/',num2str(n-1,'%08d'),'.jpg']);
    h=figure;imshow(curIM);
    set(figure(1),'Units','Normalized','OuterPosition',[0 0 1 1]);
    hold on;
    draw_cons(['./curves_yuliang/',num2str(n-1,'%08d'),'.cemv'],[num2str(n-1,'%08d'),'.jpg'],0,-1);

    curCurve = curve1;
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
    plot(reprojCurve(:,1)+1,reprojCurve(:,2)+1,'Color','r','LineWidth',2);
    
    curCurve = curve2;
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
    plot(reprojCurve(:,1)+1,reprojCurve(:,2)+1,'Color','g','LineWidth',2);
    
    curCurve = order_curve;
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
    plot(reprojCurve(:,1)+1,reprojCurve(:,2)+1,'xb','LineWidth',1);
    
%     hold off;
%     print_pdf([num2str(n-1,'%08d'),'.pdf']);
%     close all;
    
end
