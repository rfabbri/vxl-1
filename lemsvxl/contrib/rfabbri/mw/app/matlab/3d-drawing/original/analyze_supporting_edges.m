clear all;
close all;

% numIM = 27;
% colors = distinguishable_colors(numIM);
% 
% all_recs = cell(numIM,1);
% all_nR = zeros(numIM,1);
% all_links_3d = cell(numIM,1);
% all_offsets_3d = cell(numIM,1);
% all_edge_support_3d = cell(numIM,1);
% all_flags = cell(numIM,1);
% 
% all_num_im_contours = zeros(numIM,1);
% 
% read_curve_sketch4;
% all_recs{11,1} = recs;
% all_nR(11,1) = size(recs,2);
% all_flags{11,1} = zeros(size(recs,2),1);
% clear recs;
% read_curve_sketch4a;
% all_recs{14,1} = recs;
% all_nR(14,1) = size(recs,2);
% all_flags{14,1} = zeros(size(recs,2),1);
% clear recs;
% read_curve_sketch4b;
% all_recs{7,1} = recs;
% all_nR(7,1) = size(recs,2);
% all_flags{7,1} = zeros(size(recs,2),1);
% clear recs;
% read_curve_sketch4c;
% all_recs{8,1} = recs;
% all_nR(8,1) = size(recs,2);
% all_flags{8,1} = zeros(size(recs,2),1);
% clear recs;
% read_curve_sketch4d;
% all_recs{9,1} = recs;
% all_nR(9,1) = size(recs,2);
% all_flags{9,1} = zeros(size(recs,2),1);
% clear recs;
% 
% %All the views that will be used in the clustering.
% %The first view is where the initial seed/query curve is located.
% all_views = [10 13 6 7 8];
% numViews = size(all_views,2);
% 
% %Read all the edge support evidence
% disp('READING DATA');
% for v=1:numViews
% %for v=1:1
%     all_views(1,v)
%     cons = read_cons(['./curves_yuliang/',num2str(all_views(1,v),'%08d'),'.cemv'],[num2str(all_views(1,v),'%08d'),'.jpg'],0,-1);
%     num_im_contours = size(cons,2);
%     [all_links_3d{all_views(1,v)+1,1}, all_offsets_3d{all_views(1,v)+1,1}, all_edge_support_3d{all_views(1,v)+1,1}] = read_association_attributes(all_views(1,v),num_im_contours,all_nR(all_views(1,v)+1,1),numIM);
%     all_num_im_contours(all_views(1,v)+1,1) = num_im_contours;
%     
% end

load analysis-intermediate.mat;

read_curve_sketch4a;
analysis_view_id = 13;
analysis_curve_id = 42;

for n=1:numIM;

    fid = fopen(['./calibration/',num2str(n-1,'%08d'),'.projmatrix']);
    curP = (fscanf(fid,'%f',[4 3]))';

    [edg edgmap] = load_edg(['./edges/',num2str(n-1,'%08d'),'.edg']);
    supporting_edg = [];

    curIM = imread(['./images/',num2str(n-1,'%08d'),'.jpg']);
    h=figure;imshow(curIM);
    set(figure(1),'Units','Normalized','OuterPosition',[0 0 1 1]);
    hold on;

    %draw_cons(['./curves_yuliang/',num2str(n-1,'%08d'),'.cemv'],[num2str(n-1,'%08d'),'.jpg'],0,495);

    curCurve = recs{1,analysis_curve_id};
    numSamples = size(curCurve,1);

    for s=1:numSamples
        if(isempty(all_edge_support_3d{analysis_view_id+1,1}{analysis_curve_id,1}{n,1}))
            continue;
        end
        edges = all_edge_support_3d{analysis_view_id+1,1}{analysis_curve_id,1}{n,1}{s,1};
        for e=1:size(edges,2)
            edge = edges(1,e);
            
            cur_sample = [curCurve(s,:)';1];
            imSample = curP*cur_sample;
            imSample = imSample./imSample(3,1);
            im_pixel = imSample(1:2,1)+1;
            cur_edge = edg(edge+1,1:2)';
            edge_dist = norm(cur_edge - im_pixel);
                    
            if(edge_dist<=1)
                edge_data = edg(edge+1,:);
                supporting_edg = unique([supporting_edg; edge_data],'rows');
            end
        end
    end

    disp_edg(supporting_edg,0);

    reprojCurve = zeros(numSamples,2);

    for s=1:numSamples

        curSample = [(curCurve(s,:))';1];
        imSample = curP*curSample;
        imSample = imSample./imSample(3,1);

        reprojCurve(s,1) = imSample(1,1);
        reprojCurve(s,2) = imSample(2,1);

    end

    %plot(reprojCurve(:,1)+1,reprojCurve(:,2)+1,'Color','r','LineWidth',1);
    hold off;
    
    print_pdf(['./ply/',num2str(n-1,'%08d'),'.pdf']);
    close all;
    
end