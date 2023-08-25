clear all;
close all;

numIM = 27;
all_recs = cell(numIM,1);
all_nR = zeros(numIM,1);
all_links_3d = cell(numIM,1);
all_offsets_3d = cell(numIM,1);

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

first_anchor = 8;
cons = read_cons(['./curves_yuliang/',num2str(first_anchor,'%08d'),'.cemv'],[num2str(first_anchor,'%08d'),'.jpg'],0,-1);
num_im_contours = size(cons,2);
[links_3d, offsets_3d, edge_support_3d] = read_association_attributes(first_anchor,num_im_contours,all_nR(first_anchor+1,1),numIM);
all_num_im_contours(first_anchor+1,1) = num_im_contours;

other_views = [10 13 6 7];
disp('READING DATA');
for v=1:size(other_views,2);
%for v=1:1
    other_views(1,v)
    cons = read_cons(['./curves_yuliang/',num2str(other_views(1,v),'%08d'),'.cemv'],[num2str(other_views(1,v),'%08d'),'.jpg'],0,-1);
    num_im_contours = size(cons,2);
    [all_links_3d{other_views(1,v)+1,1}, all_offsets_3d{other_views(1,v)+1,1}, dummy] = read_association_attributes(other_views(1,v),num_im_contours,all_nR(other_views(1,v)+1,1),numIM);
    all_num_im_contours(other_views(1,v)+1,1) = num_im_contours;
end

colors = distinguishable_colors(numIM);
queryID = 7;
n = 9;
    
fid = fopen(['./calibration/',num2str(n-1,'%08d'),'.projmatrix']);
curP = (fscanf(fid,'%f',[4 3]))';

curIM = imread(['./images/',num2str(n-1,'%08d'),'.jpg']);
figure;imshow(curIM);
set(figure(1),'Units','Normalized','OuterPosition',[0 0 1 1]);
hold on;

queryCurve = all_recs{first_anchor+1,1}{1,queryID}; 
querySupport = edge_support_3d{queryID,1};
numSamples = size(queryCurve,1);
reprojCurve = zeros(numSamples,2);

for s=1:numSamples

    curSample = [(queryCurve(s,:))';1];
    imSample = curP*curSample;
    imSample = imSample./imSample(3,1);

    reprojCurve(s,1) = imSample(1,1);
    reprojCurve(s,2) = imSample(2,1);

end

disp('DRAWING ASSOCIATIONS');
allEdges = [];
for v=1:size(other_views,2);
%for v=2:2    
    other_views(1,v)
    plotted_curves = zeros(all_num_im_contours(other_views(1,v)+1,1),1);
    edge_support = querySupport{other_views(1,v)+1,1};
    cur_links_3d = all_links_3d{other_views(1,v)+1,1};
    cur_offsets_3d = all_offsets_3d{other_views(1,v)+1,1};
    [edg edgmap] = load_edg(['./edges/',num2str(other_views(1,v),'%08d'),'.edg']);

    for s=1:numSamples
        
        edges = edge_support{s,1};

        for e=1:size(edges,2)
            edge = edges(1,e);
            if(edge < size(curveIndices{other_views(1,v)+1,1},1))
                curve = curveIndices{other_views(1,v)+1,1}(edge+1,1);
                ds = curveOffsets{other_views(1,v)+1,1}(edge+1,1);
                if(ds>-1)
                    if(~isempty(cur_links_3d{curve,1}))
                        curves_3d = cur_links_3d{curve,1};
                        offsets_3d = cur_offsets_3d{curve,1};

                        [sorted_offsets, sorting] = sort(offsets_3d);
                        sorted_curves = curves_3d(sorting);

                        num_sorted_curves = size(sorted_offsets,2);

                        %If it gets associated with no curve, then it must be
                        %associated with the last curve
                        curve_index_to_link = num_sorted_curves;
                        final_ds = ds - sorted_offsets(1,num_sorted_curves);

                        if(ds<sorted_offsets(1,1))
                            disp('WARNING: Query sample associates to a non-existant sample!');
                            continue;
                        end

                        for sc=2:num_sorted_curves
                           if(ds<sorted_offsets(1,sc))
                               curve_index_to_link = sc-1;
                               final_ds = ds - sorted_offsets(1,sc-1);
                               break;
                           end
                        end
                        curve_to_link = sorted_curves(1,curve_index_to_link);
                        plotCurve = all_recs{other_views(1,v)+1,1}{1,curve_to_link+1}; 
                        numPlotSamples = size(plotCurve,1);

                        reprojPlotCurve = zeros(numPlotSamples,2);

                        for ps=1:numPlotSamples

                            curSample = [(plotCurve(ps,:))';1];
                            imSample = curP*curSample;
                            imSample = imSample./imSample(3,1);

                            reprojPlotCurve(ps,1) = imSample(1,1);
                            reprojPlotCurve(ps,2) = imSample(2,1);

                        end

                        %If the associated curve is not shown yet, draw it
                        if(plotted_curves(curve_to_link+1,1)==0 && final_ds+1 <= numPlotSamples)
                            plot(reprojPlotCurve(:,1)+1,reprojPlotCurve(:,2)+1,'Color',colors(other_views(1,v)+1,:),'LineWidth',2);
                            plotted_curves(curve_to_link+1,1) = 1;
                        end

                        if(final_ds+1 <= numPlotSamples)
                            %Draw a line between the corresponding samples
                            xx = [reprojCurve(s,1) reprojPlotCurve(final_ds+1,1)];
                            yy = [reprojCurve(s,2) reprojPlotCurve(final_ds+1,2)];
                            plot(xx+1,yy+1,'Color',colors(other_views(1,v)+1,:),'Linewidth',1);
                        end

                    end
                end
            else
                disp('WARNING: Weird edge ID!!!');
            end
        end
    end
end

plot(reprojCurve(:,1)+1,reprojCurve(:,2)+1,'Color',[1 0.99 0.99],'LineWidth',2);
hold off;

        