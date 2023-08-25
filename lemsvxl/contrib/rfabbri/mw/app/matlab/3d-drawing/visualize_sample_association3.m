clear all;
close all;

numIM = 27;
all_recs = cell(numIM,1);
all_recs_next = cell(numIM,1);

all_nR = zeros(numIM,1);
all_links_3d = cell(numIM,1);
all_offsets_3d = cell(numIM,1);

all_curves_to_process = cell(numIM,1);

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
all_recs_fixed = all_recs;

for it=1:3
    
    all_recs_next = all_recs;

    first_anchor = 10;
    cons = read_cons(['./curves_yuliang/',num2str(first_anchor,'%08d'),'.cemv'],[num2str(first_anchor,'%08d'),'.jpg'],0,-1);
    num_im_contours = size(cons,2);
    [links_3d, offsets_3d, edge_support_3d] = read_association_attributes(first_anchor,num_im_contours,all_nR(first_anchor+1,1),numIM);
    all_num_im_contours(first_anchor+1,1) = num_im_contours;

    other_views = [13 6 7 8];
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
    queryID = 1;
    n = 5;

    fid = fopen(['./calibration/',num2str(n-1,'%08d'),'.projmatrix']);
    curP = (fscanf(fid,'%f',[4 3]))';

    %[edg edgmap] = load_edg(['./edges/',num2str(n-1,'%08d'),'.edg']);

    if(it==3)
        curIM = imread(['./images/',num2str(n-1,'%08d'),'.jpg']);
        figure;imshow(curIM);
        set(figure(1),'Units','Normalized','OuterPosition',[0 0 1 1]);
        hold on;
        draw_cons(['./curves_yuliang/',num2str(n-1,'%08d'),'.cemv'],[num2str(n-1,'%08d'),'.jpg'],0,-1);
    end


    queryCurve = all_recs{first_anchor+1,1}{1,queryID};
    queryCurveNext = queryCurve;

    querySupport = edge_support_3d{queryID,1};

    numSamples = size(queryCurve,1);
    reprojCurve = zeros(numSamples,2);
    cumulativeCurve = cell(numSamples,1);

    for s=1:numSamples

        curSample = [(queryCurve(s,:))';1];
        imSample = curP*curSample;
        imSample = imSample./imSample(3,1);

        reprojCurve(s,1) = imSample(1,1);
        reprojCurve(s,2) = imSample(2,1);

        cumulativeCurve{s,1} = queryCurve(s,:);

    end

    disp('AVERAGING QUERY CURVE');
    for v=1:size(other_views,2);
    %for v=2:2    
        other_views(1,v)
        plotted_curves = zeros(all_num_im_contours(other_views(1,v)+1,1),1);
        edge_support = querySupport{other_views(1,v)+1,1};
        cur_links_3d = all_links_3d{other_views(1,v)+1,1};
        cur_offsets_3d = all_offsets_3d{other_views(1,v)+1,1};

        for s=1:numSamples

            if(mod(s,5)==0 && it==3)
                plot(reprojCurve(s,1)+1,reprojCurve(s,2)+1,'Color','r','Marker','x');
            end
            edges = edge_support{s,1};
            associated_points = cell(0,0);

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

                            if(final_ds+1 <= numPlotSamples)
                                associated_points = [associated_points; plotCurve(final_ds+1,:)];
                            end

                            reprojPlotCurve = zeros(numPlotSamples,2);

                            for ps=1:numPlotSamples

                                curSample = [(plotCurve(ps,:))';1];
                                imSample = curP*curSample;
                                imSample = imSample./imSample(3,1);

                                reprojPlotCurve(ps,1) = imSample(1,1);
                                reprojPlotCurve(ps,2) = imSample(2,1);

                            end

    %                         %If the associated curve is not shown yet, draw it
    %                         if(plotted_curves(curve_to_link+1,1)==0 && final_ds+1 <= numPlotSamples)
    %                             plot(reprojPlotCurve(:,1)+1,reprojPlotCurve(:,2)+1,'Color',colors(other_views(1,v)+1,:),'LineWidth',2);
    %                             plotted_curves(curve_to_link+1,1) = 1;
    %                         end

                            if(final_ds+1 <= numPlotSamples)

                                all_curves_to_process{other_views(1,v)+1,1} = [all_curves_to_process{other_views(1,v)+1,1} curve_to_link+1];
                                % Draw a line between the corresponding samples
                                if(mod(s,5)==0 && it==3)
                                    plot(reprojPlotCurve(final_ds+1,1)+1,reprojPlotCurve(final_ds+1,2)+1,'Color',colors(other_views(1,v)+1,:),'Marker','x');
                                    xx = [reprojCurve(s,1) reprojPlotCurve(final_ds+1,1)];
                                    yy = [reprojCurve(s,2) reprojPlotCurve(final_ds+1,2)];
                                    plot(xx+1,yy+1,'Color',colors(other_views(1,v)+1,:),'Linewidth',1);
                                end
                            end
                        end
                    end
                else
                    disp('WARNING: Weird edge ID!!!');
                end
            end
            if(~isempty(associated_points))
                pointSum = [0 0 0];
                for ap=1:size(associated_points,1)
                    pointSum = pointSum + associated_points{ap,1};
                end
                pointAverage = pointSum./size(associated_points,1);
                cumulativeCurve{s,1} = [cumulativeCurve{s,1}; pointAverage];
            end
        end
    end

    averageCurve = zeros(numSamples,3);
    for s=1:numSamples
       cumulativeSum = [0 0 0];
       size(cumulativeCurve{s,1},1)
       for ccs=1:size(cumulativeCurve{s,1},1)
          cumulativeSum = cumulativeSum + cumulativeCurve{s,1}(ccs,:); 
       end
       cumulativeAverage = cumulativeSum./size(cumulativeCurve{s,1},1);
       averageCurve(s,:) = cumulativeAverage;
    end

    all_recs_next{first_anchor+1,1}{1,queryID} = averageCurve;

    for v=1:size(other_views,2);
        all_curves_to_process{other_views(1,v)+1,1} = unique(all_curves_to_process{other_views(1,v)+1,1});
    end


    for s=1:numSamples
        if(mod(s,5)==0 && it==3)
            curSample = [(averageCurve(s,:))';1];
            imSample = curP*curSample;
            imSample = imSample./imSample(3,1);

            plot(imSample(1,1)+1,imSample(2,1)+1,'Color','g','Marker','x');
            xx = [reprojCurve(s,1) imSample(1,1)];
            yy = [reprojCurve(s,2) imSample(2,1)];
            plot(xx+1,yy+1,'Color','g','Linewidth',1);
        end
    end

    if(it<3)
        disp('AVERAGING RELATED CURVE');
        for vv=1:size(other_views,2);
        %for vv=1:1
            cur_first_anchor = other_views(1,vv)
            cons = read_cons(['./curves_yuliang/',num2str(cur_first_anchor,'%08d'),'.cemv'],[num2str(cur_first_anchor,'%08d'),'.jpg'],0,-1);
            num_im_contours = size(cons,2);
            [links_3d, offsets_3d, edge_support_3d] = read_association_attributes(cur_first_anchor,num_im_contours,all_nR(cur_first_anchor+1,1),numIM);

            cur_other_views = first_anchor;
            for v=1:size(other_views,2)
                if(other_views(1,v)~=cur_first_anchor)
                    cur_other_views = [cur_other_views other_views(1,v)];
                end
            end

            for v=1:size(cur_other_views,2)
            %for v=1:1
                cons = read_cons(['./curves_yuliang/',num2str(cur_other_views(1,v),'%08d'),'.cemv'],[num2str(cur_other_views(1,v),'%08d'),'.jpg'],0,-1);
                num_im_contours = size(cons,2);
                [all_links_3d{cur_other_views(1,v)+1,1}, all_offsets_3d{cur_other_views(1,v)+1,1}, dummy] = read_association_attributes(cur_other_views(1,v),num_im_contours,all_nR(cur_other_views(1,v)+1,1),numIM);
                all_num_im_contours(cur_other_views(1,v)+1,1) = num_im_contours;
            end

            colors = distinguishable_colors(numIM);

            curves_to_process = all_curves_to_process{cur_first_anchor+1,1};

            for ctp=1:size(curves_to_process,2)
            %for ctp=1:1

                queryID = curves_to_process(1,ctp);

                fid = fopen(['./calibration/',num2str(n-1,'%08d'),'.projmatrix']);
                curP = (fscanf(fid,'%f',[4 3]))';

                %[edg edgmap] = load_edg(['./edges/',num2str(n-1,'%08d'),'.edg']);

        %         curIM = imread(['./images/',num2str(n-1,'%08d'),'.jpg']);
        %         figure;imshow(curIM);
        %         set(figure(1),'Units','Normalized','OuterPosition',[0 0 1 1]);
        %         hold on;

                queryCurve = all_recs{cur_first_anchor+1,1}{1,queryID};
                queryCurve(1,:)
                queryCurveNext = queryCurve;

                querySupport = edge_support_3d{queryID,1};

                numSamples = size(queryCurve,1);
                reprojCurve = zeros(numSamples,2);
                cumulativeCurve = cell(numSamples,1);

                for s=1:numSamples

                    curSample = [(queryCurve(s,:))';1];
                    imSample = curP*curSample;
                    imSample = imSample./imSample(3,1);

                    reprojCurve(s,1) = imSample(1,1);
                    reprojCurve(s,2) = imSample(2,1);

                    cumulativeCurve{s,1} = queryCurve(s,:);

                end

                for v=1:size(cur_other_views,2);
                %for v=2:2    
                    plotted_curves = zeros(all_num_im_contours(cur_other_views(1,v)+1,1),1);
                    edge_support = querySupport{cur_other_views(1,v)+1,1};
                    cur_links_3d = all_links_3d{cur_other_views(1,v)+1,1};
                    cur_offsets_3d = all_offsets_3d{cur_other_views(1,v)+1,1};

                    for s=1:numSamples

                        %plot(reprojCurve(s,1)+1,reprojCurve(s,2)+1,'Color','r','Marker','x');
                        edges = edge_support{s,1};
                        associated_points = cell(0,0);

                        for e=1:size(edges,2)
                            edge = edges(1,e);
                            if(edge < size(curveIndices{cur_other_views(1,v)+1,1},1))
                                curve = curveIndices{cur_other_views(1,v)+1,1}(edge+1,1);
                                ds = curveOffsets{cur_other_views(1,v)+1,1}(edge+1,1);
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
                                        plotCurve = all_recs{cur_other_views(1,v)+1,1}{1,curve_to_link+1}; 
                                        numPlotSamples = size(plotCurve,1);

                                        if(final_ds+1 <= numPlotSamples)
                                            associated_points = [associated_points; plotCurve(final_ds+1,:)];
                                        end

                                        reprojPlotCurve = zeros(numPlotSamples,2);

                                        for ps=1:numPlotSamples

                                            curSample = [(plotCurve(ps,:))';1];
                                            imSample = curP*curSample;
                                            imSample = imSample./imSample(3,1);

                                            reprojPlotCurve(ps,1) = imSample(1,1);
                                            reprojPlotCurve(ps,2) = imSample(2,1);

                                        end

                %                         %If the associated curve is not shown yet, draw it
                %                         if(plotted_curves(curve_to_link+1,1)==0 && final_ds+1 <= numPlotSamples)
                %                             plot(reprojPlotCurve(:,1)+1,reprojPlotCurve(:,2)+1,'Color',colors(other_views(1,v)+1,:),'LineWidth',2);
                %                             plotted_curves(curve_to_link+1,1) = 1;
                %                         end

                                        if(final_ds+1 <= numPlotSamples)
                                            all_curves_to_process{cur_other_views(1,v)+1,1} = [all_curves_to_process{cur_other_views(1,v)+1,1} curve_to_link+1];
        %                                     %Draw a line between the corresponding samples
        %                                     plot(reprojPlotCurve(final_ds+1,1)+1,reprojPlotCurve(final_ds+1,2)+1,'Color',colors(cur_other_views(1,v)+1,:),'Marker','x');
        %                                     xx = [reprojCurve(s,1) reprojPlotCurve(final_ds+1,1)];
        %                                     yy = [reprojCurve(s,2) reprojPlotCurve(final_ds+1,2)];
        %                                     plot(xx+1,yy+1,'Color',colors(cur_other_views(1,v)+1,:),'Linewidth',1);
                                        end


                                    end
                                end
                            else
                                disp('WARNING: Weird edge ID!!!');
                            end
                        end
                        if(~isempty(associated_points))
                            pointSum = [0 0 0];
                            for ap=1:size(associated_points,1)
                                pointSum = pointSum + associated_points{ap,1};
                            end
                            pointAverage = pointSum./size(associated_points,1);
                            cumulativeCurve{s,1} = [cumulativeCurve{s,1}; pointAverage];
                        end
                    end
                end

                averageCurve = zeros(numSamples,3);
                for s=1:numSamples
                   cumulativeSum = [0 0 0];
                   for ccs=1:size(cumulativeCurve{s,1},1)
                      cumulativeSum = cumulativeSum + cumulativeCurve{s,1}(ccs,:); 
                   end
                   cumulativeAverage = cumulativeSum./size(cumulativeCurve{s,1},1);
                   averageCurve(s,:) = cumulativeAverage;
                end
                averageCurve(1,:)
                all_recs_next{cur_first_anchor+1,1}{1,queryID} = averageCurve;

                for v=1:size(cur_other_views,2);
                    all_curves_to_process{cur_other_views(1,v)+1,1} = unique(all_curves_to_process{cur_other_views(1,v)+1,1});
                end
            end
        end

        all_recs = all_recs_next; 
    end
end
