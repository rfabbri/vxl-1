% clear all;
% close all;
% 
% numIM = 27;
% colors = distinguishable_colors(numIM);
% 
% all_recs = cell(numIM,1);
% all_nR = zeros(numIM,1);
% all_links_3d = cell(numIM,1);
% all_offsets_3d = cell(numIM,1);
% all_edge_support_3d = cell(numIM,1);
% all_reg_curves = cell(numIM,1);
% 
% all_num_im_contours = zeros(numIM,1);
% 
% read_curve_sketch4;
% all_recs{11,1} = recs;
% all_nR(11,1) = size(recs,2);
% all_reg_curves{11,1} = cell(size(recs,2),1);
% clear recs;
% read_curve_sketch4a;
% all_recs{14,1} = recs;
% all_nR(14,1) = size(recs,2);
% all_reg_curves{14,1} = cell(size(recs,2),1);
% clear recs;
% read_curve_sketch4b;
% all_recs{7,1} = recs;
% all_nR(7,1) = size(recs,2);
% all_reg_curves{7,1} = cell(size(recs,2),1);
% clear recs;
% read_curve_sketch4c;
% all_recs{8,1} = recs;
% all_nR(8,1) = size(recs,2);
% all_reg_curves{8,1} = cell(size(recs,2),1);
% clear recs;
% read_curve_sketch4d;
% all_recs{9,1} = recs;
% all_nR(9,1) = size(recs,2);
% all_reg_curves{9,1} = cell(size(recs,2),1);
% clear recs;
% 
% load edge-curve-index_yuliang.mat;
% load edge-curve-offset_yuliang.mat;
% load clusters_view-10_10-edges.mat;
% 
% %All the views that will be used in the clustering.
% %The last view is where the initial seed/query curve should be located.
% all_views = [13 6 7 8 10];
% numViews = size(all_views,2);
% 
% %DETERMINE OVERLAP USING SAMPLE ASSOCIATIONS
% 
% disp('READING DATA');
% for v=1:numViews
%     all_views(1,v)
%     cons = read_cons(['./curves_yuliang/',num2str(all_views(1,v),'%08d'),'.cemv'],[num2str(all_views(1,v),'%08d'),'.jpg'],0,-1);
%     num_im_contours = size(cons,2);
%     [all_links_3d{all_views(1,v)+1,1}, all_offsets_3d{all_views(1,v)+1,1}, all_edge_support_3d{all_views(1,v)+1,1}] = read_association_attributes(all_views(1,v),num_im_contours,all_nR(all_views(1,v)+1,1),numIM);
%     all_num_im_contours(all_views(1,v)+1,1) = num_im_contours;
% end

num_clusters = size(all_clusters,1);
for cl=66:66%1:num_clusters
    
    cl

    clusters = all_clusters{cl,1};
    cluster_done = 0;
    
    cluster_flags = cell(numIM,1);
    for v=1:numViews
        cluster_flags{all_views(1,v)+1,1} = zeros(size(clusters{all_views(1,v)+1,1}));
    end
    
    %Check to see if there are more than 1 curve in the cluster
    first_curve_found = 0;
    multiple_curves_found = 0;

    for v=1:numViews
        curveIDs = clusters{all_views(1,v)+1,1};
        num_curves = size(curveIDs,2);

        if(num_curves > 1)
           multiple_curves_found = 1; 
           break; 
        elseif(num_curves > 0)
           if(first_curve_found)
               multiple_curves_found = 1;
               break;
           else
               first_curve_found = 1;
           end
        end

    end
    
    if(~multiple_curves_found)
        continue;
    end

    %INITIALIZE THE SEED CURVE THAT WILL KEEP GROWING, ALONG WITH ALL ITS
    %ATTRIBUTES

    %Views included in the growing seed curve at a given iteration
    seedViews = all_views(1,numViews);
    firstView = seedViews(1,1);
    %Curve IDs included in the growing seed curve at a given iteration
    seedIDs = cell(numIM,1);
    seedIDs{firstView+1,1} = clusters{firstView+1,1}(1,1);
    firstSeedID = seedIDs{firstView+1,1}(1,1);
    %Seed curve is initialized with the first curve to be processed.
    seed_curve = all_recs{firstView+1,1}{1,firstSeedID};
    %Number of samples averaged for each sample of the seed curve
    %Initialized with 1, will get increased as more samples get averaged
    seed_count = ones(size(seed_curve,1),1);
    %Supporting edges on each view, for the seed curve
    seed_support_all = all_edge_support_3d{firstView+1,1}{firstSeedID,1};
    %Mark the initial seed curve as processed
    cluster_flags{firstView+1,1}(1,1) = 1;
    %Sample and curve ID info for each sample of the seed curve
    seed_reg_curve = [(1:size(seed_curve,1))' firstSeedID.*ones(size(seed_curve,1),1) firstView.*ones(size(seed_curve,1),1)];
    %As multiple branches grow, more than one seed curve (and their related
    %attributes) will need to be stored
    seed_curve_graph = cell(1,1);
    seed_curve_graph{1,1} = seed_curve;
    seedViews_graph = cell(1,1);
    seedViews_graph{1,1} = seedViews;
    seedIDs_graph = cell(1,1);
    seedIDs_graph{1,1} = seedIDs;
    seed_support_graph = cell(1,1);
    seed_support_graph{1,1} = seed_support_all;
    seed_reg_graph = cell(1,1);
    seed_reg_graph{1,1} = seed_reg_curve;
    seed_count_graph = cell(1,1);
    seed_count_graph{1,1} = seed_count;
    %Information on how to connect the branches to the long curve
    is_init_graph = cell(1,1);
    stitch_point_graph = cell(1,1);

    %Delete this after debugging
    seed_stitch_samples = zeros(size(seed_curve,1),1);

    %KEEP RUNNING UNTIL ALL THE CURVES IN THE CLUSTER ARE STITCHED TOGETHER
    while(~cluster_done)

        %During each iteration, go over each view one by one
        for v=1:numViews
            queryView = all_views(1,v);

            %Links from image curves to 3d curves in the queryView
            query_links_3d = all_links_3d{queryView+1,1};
            %Offset between image and 3d curves
            query_offsets_3d = all_offsets_3d{queryView+1,1};

            %Check if all the curves in the cluster from that view are used
            if(any(~cluster_flags{queryView+1,1}))

                csize = size(clusters{queryView+1,1},2);

                if(v==5)
                    csize=1;
                end

                for cc=1:csize
                    %CHECK TO SEE IF THERE IS OVERLAP BETWEEN SEED-QUERY

                    %We will keep checking different branches of the graph
                    %until overlap is found
                    for br=1:size(seed_curve_graph,1)

                        seed_curve = seed_curve_graph{br,1};
                        seedViews = seedViews_graph{br,1};
                        seedIDs = seedIDs_graph{br,1};
                        seed_support_all = seed_support_graph{br,1};
                        seed_reg_curve = seed_reg_graph{br,1};
                        seed_count = seed_count_graph{br,1};

                        seed_curve1 = seed_curve;

                        %CHECK TO SEE IF SEED CURVE WRAPS AROUND ITSELF
                        seed_size = size(seed_curve,1);

                        %Search space if the first 1/3 portion
                        seed_search_space = seed_curve(1:round(seed_size/3),:); 

                        %Starting point is the end of the first 2/3 portion
                        search_start = round(2*seed_size/3);

                        %Process a few samples beforehand to obtain sample distance
                        %average

                        average_dist = 0;
                        average_count = 0;

                        for s=(search_start-4):(search_start-1)
                            if(s<2)
                                continue;
                            end
                            curDist = norm(seed_curve(s-1,:) - seed_curve(s,:));
                            average_dist = (average_dist*average_count + curDist)/(average_count+1);
                            average_count = average_count+1;
                        end

                        for s=search_start:seed_size

                            curSample = seed_curve(s,:);
                            [closest_pt,outside_init_range,outside_final_range,prev_pt,next_pt] = find_closest_point_on_curve(seed_search_space,curSample);

                            %We need to have some tolerance when checking for
                            %distances
                            if(norm(closest_pt - curSample) < average_dist*3)

                                truncated_seed_reg_curve = seed_reg_curve(1:s,:);
                                %Go over the truncated portion to remove views
                                %and IDs no longer present in the seed curve

                                for st=seed_size:-1:(s+1)
                                    curveID = seed_reg_curve(st,2);
                                    viewID = seed_reg_curve(st,3);
                                    search_space = truncated_seed_reg_curve;

                                    if(isempty(find(search_space(:,2)==curveID,1)))
                                        seedIDVec = seedIDs{viewID+1,1};
                                        rem_ind = find(seedIDVec==curveID,1);
                                        seedIDVec(1,rem_ind) = [];
                                        seedIDs{viewID+1,1} = seedIDVec;
                                    end

                                    if(isempty(find(search_space(:,3)==viewID,1)))
                                        rem_ind = find(seedViews==viewID,1);
                                        seedViews(1,rem_ind) = [];
                                    end
                                end


                                %Truncate the seed curve at the found point
                                seed_curve = seed_curve(1:s,:);
                                seed_reg_curve = seed_reg_curve(1:s,:);

                                for i=1:numIM
                                    if(~isempty(seed_support_all{i,1}))
                                        seed_support_all{i,1} = seed_support_all{i,1}(1:s,1);
                                    end
                                end

                                seed_count = seed_count(1:s,1);

                                break;

                            else
                                curDist = norm(seed_curve(s-1,:) - seed_curve(s,:));
                                average_dist = (average_dist*average_count + curDist)/(average_count+1);
                                average_count = average_count+1;
                            end

                        end



                        seed_curve2 = seed_curve;


                        %Check to see if this curve is already processed
                        if(~cluster_flags{queryView+1,1}(1,cc))

                            %Support for the seed curve on the query view
                            seed_support = seed_support_all{queryView+1,1};

                            if(~isempty(seed_support))
                                %Initialize
                                queryID = clusters{queryView+1,1}(1,cc);
                                query_curve = all_recs{queryView+1,1}{1,queryID};
                                %Supporting edges on each view, for the query curve
                                query_support_all = all_edge_support_3d{queryView+1,1}{queryID,1};



                                %CHECK TO SEE IF QUERY CURVE WRAPS AROUND ITSELF
                                query_size = size(query_curve,1);

                                %Search space if the first 1/3 portion
                                query_search_space = query_curve(1:round(query_size/3),:); 

                                %Starting point is the end of the first 2/3 portion
                                search_start = round(2*query_size/3);

                                %Process a few samples beforehand to obtain sample distance
                                %average

                                average_dist = 0;
                                average_count = 0;

                                for s=(search_start-4):(search_start-1)
                                    if(s<2)
                                        continue;
                                    end
                                    curDist = norm(query_curve(s-1,:) - query_curve(s,:));
                                    average_dist = (average_dist*average_count + curDist)/(average_count+1);
                                    average_count = average_count+1;
                                end

                                for s=search_start:query_size

                                    curSample = query_curve(s,:);
                                    [closest_pt,outside_init_range,outside_final_range,prev_pt,next_pt] = find_closest_point_on_curve(query_search_space,curSample);

                                    %We need to have some tolerance when checking for
                                    %distances
                                    if(norm(closest_pt - curSample) < average_dist*3)

                                        %Truncate the seed curve at the found point
                                        query_curve = query_curve(1:s,:);

                                        for i=1:numIM
                                            if(~isempty(query_support_all{i,1}))
                                                query_support_all{i,1} = query_support_all{i,1}(1:s,1);
                                            end
                                        end

                                        break;

                                    else
                                        curDist = norm(query_curve(s-1,:) - query_curve(s,:));
                                        average_dist = (average_dist*average_count + curDist)/(average_count+1);
                                        average_count = average_count+1;
                                    end

                                end


                                %Flags and markers
                                init_corresp_found = 0;
                                final_corresp_found = 0;
                                is_init_junction = 0;
                                is_final_junction = 0;
                                is_query_longer_init = 0;
                                is_query_longer_final = 0;
                                seed_init_index = 0;
                                seed_final_index = 0;
                                query_init_index = 0;
                                query_final_index = 0;
                                s = 0;

                                %-----------------------------------------------------

                                %Detect whether seed curve order needs to reversed

                                %Find the init correspondence for seed curve
                                while(~init_corresp_found)
                                    %Go to the next sample on both curves
                                    s=s+1;
                                    if(s>size(seed_curve,1))
                                        break;
                                    end
                                    %Look for correspondence on the seed curve by
                                    %analyzing supporting edges
                                    edges = seed_support{s,1};

                                    for e=1:size(edges,2)
                                        edge = edges(1,e);
                                        if(edge < size(curveIndices{queryView+1,1},1))
                                            %Get the curve ID that contains this edge
                                            curve = curveIndices{queryView+1,1}(edge+1,1);
                                            %Get the offset of this edge in that curve
                                            ds = curveOffsets{queryView+1,1}(edge+1,1);

                                            if(ds>-1 && (~isempty(query_links_3d{curve,1})))
                                                %Get the linked 3d curves and their
                                                %offsets
                                                curves_3d = query_links_3d{curve,1}+1;
                                                offsets_3d = query_offsets_3d{curve,1};

                                                queryIND = find(curves_3d==queryID,1);

                                                if(~isempty(queryIND))

                                                    cur_curve = curves_3d(1,queryIND);
                                                    cur_offset = offsets_3d(1,queryIND);
                                                    cur_size = size(all_recs{queryView+1,1}{1,cur_curve},1);



                                                    %Sample s corresponds to sample
                                                    %final_ds
                                                    final_ds = ds - cur_offset + 1;

                                                    if(ds>=cur_offset && (ds<cur_offset+cur_size) && final_ds <= size(query_curve,1))
                                                        init_corresp_found = 1;
                                                        if(s>1)
                                                            is_init_junction = 1;
                                                        else
                                                            is_query_longer_init = 1;
                                                        end
                                                        seed_init_index = s;
                                                        query_init_index = final_ds;
                                                        break;
                                                    end
                                                end
                                            end
                                        else
                                            disp('ERROR: Edge index is larger than the number of edges!!');
                                            continue;
                                        end
                                    end
                                end

                                if(~init_corresp_found)
                                    continue;
                                end

                                s=0;

                                %Find the final correspondence for seed curve
                                while(~final_corresp_found)

                                    %Go to the next sample on both curves
                                    s_seed = size(seed_curve,1)-s;

                                    if(s_seed < 1)
                                        break;
                                    end

                                    %Look for correspondence on the seed curve by
                                    %analyzing supporting edges
                                    edges = seed_support{s_seed,1};
                                    for e=1:size(edges,2)
                                        edge = edges(1,e);
                                        if(edge < size(curveIndices{queryView+1,1},1))
                                            %Get the curve ID that contains this edge
                                            curve = curveIndices{queryView+1,1}(edge+1,1);
                                            %Get the offset of this edge in that curve
                                            ds = curveOffsets{queryView+1,1}(edge+1,1);
                                            if(ds>-1 && (~isempty(query_links_3d{curve,1})))
                                                %Get the linked 3d curves and their
                                                %offsets
                                                curves_3d = query_links_3d{curve,1}+1;
                                                offsets_3d = query_offsets_3d{curve,1};

                                                queryIND = find(curves_3d==queryID,1);

                                                if(~isempty(queryIND))

                                                    cur_curve = curves_3d(1,queryIND);
                                                    cur_offset = offsets_3d(1,queryIND);
                                                    cur_size = size(all_recs{queryView+1,1}{1,cur_curve},1);

                                                    %Sample s corresponds to sample
                                                    %final_ds
                                                    final_ds = ds - cur_offset + 1;

                                                    if(ds>=cur_offset && (ds<cur_offset+cur_size) && final_ds <= size(query_curve,1))
                                                        final_corresp_found = 1;
                                                        if(s>0)
                                                            is_final_junction = 1;
                                                        else
                                                            is_query_longer_final = 1;
                                                        end
                                                        seed_final_index = s_seed;
                                                        query_final_index = final_ds;
                                                        break;
                                                    end
                                                end
                                            end
                                        else
                                            disp('ERROR: Edge index is larger than the number of edges!!');
                                            continue;
                                        end
                                    end
                                    s=s+1;    
                                end

                                if(~final_corresp_found)
                                    continue;
                                end

                                %Check to see if seed curve and its attributes need to
                                %be reversed
                                if(query_init_index > query_final_index)
                                    seed_curve = flipud(seed_curve);
                                    seed_count = flipud(seed_count);
                                    seed_support = flipud(seed_support);
                                    for i=1:numIM
                                        seed_support_all{i,1} = flipud(seed_support_all{i,1});
                                    end
                                    seed_reg_curve = flipud(seed_reg_curve);   

                                    %Fix the initial and final correspondence
                                    %samples markers
                                    seed_size = size(seed_curve,1);
                                    temp = seed_init_index;
                                    seed_init_index = seed_size - seed_final_index + 1;
                                    seed_final_index = seed_size - temp + 1;

                                    temp = query_init_index;
                                    query_init_index = query_final_index;
                                    query_final_index = temp;

                                end

                                %Flags and markers
                                init_next_corresp_found = 0;
                                final_next_corresp_found = 0;
                                is_closed_curve_merging = 0;

                                %-----------------------------------------------------
                                %Now we need to see if we have a closed-curve
                                %situation

                                %We move 0.1*size(seed_curve,1) samples ahead
                                %of the first correspondence found
                                seed_size = size(seed_curve,1);
                                %Sample ID where search for another
                                %correspondence will start
                                
                                %scan_start = round(0.1*seed_size) + seed_init_index;
                                
                                scan_start = round(0.1*(seed_final_index - seed_init_index + 1)) + seed_init_index;


                                %Go backwards in samples until correspondence
                                %is found
                                for s=scan_start:-1:seed_init_index
                                    %Look for correspondence on the seed curve by
                                    %analyzing supporting edges
                                    edges = seed_support{s,1};

                                    for e=1:size(edges,2)
                                        edge = edges(1,e);
                                        if(edge < size(curveIndices{queryView+1,1},1))
                                            %Get the curve ID that contains this edge
                                            curve = curveIndices{queryView+1,1}(edge+1,1);
                                            %Get the offset of this edge in that curve
                                            ds = curveOffsets{queryView+1,1}(edge+1,1);

                                            if(ds>-1 && (~isempty(query_links_3d{curve,1})))
                                                %Get the linked 3d curves and their
                                                %offsets
                                                curves_3d = query_links_3d{curve,1}+1;
                                                offsets_3d = query_offsets_3d{curve,1};

                                                queryIND = find(curves_3d==queryID,1);

                                                if(~isempty(queryIND))

                                                    cur_curve = curves_3d(1,queryIND);
                                                    cur_offset = offsets_3d(1,queryIND);
                                                    cur_size = size(all_recs{queryView+1,1}{1,cur_curve},1);

                                                    %Sample s corresponds to sample
                                                    %final_ds
                                                    final_ds = ds - cur_offset + 1;

                                                    if(ds>=cur_offset && (ds<cur_offset+cur_size) && final_ds <= size(query_curve,1))
                                                        init_next_corresp_found = 1;

                                                        %If the motion direction on the query
                                                        %curve is opposite, mark this as a
                                                        %closed-curve merging case
                                                        if(final_ds < query_init_index)
                                                            is_closed_curve_merging = 1;
                                                        end

                                                        break;
                                                    end
                                                end
                                            end
                                        else
                                            disp('ERROR: Edge index is larger than the number of edges!!');
                                            continue;
                                        end
                                    end


                                    if(init_next_corresp_found)
                                        break;
                                    end
                                end

                                %If no next best sample is found at the
                                %beginning
                                if(~init_next_corresp_found)

                                    %We move 0.1*size(seed_curve,1) samples
                                    %behind the last correspondence found
                                    %Sample ID where search for another
                                    %correspondence will start
                                    scan_start = seed_final_index - round(0.1*(seed_final_index - seed_init_index + 1));

                                    for s=scan_start:seed_final_index

                                        %Look for correspondence on the seed curve by
                                        %analyzing supporting edges
                                        edges = seed_support{s,1};
                                        for e=1:size(edges,2)
                                            edge = edges(1,e);
                                            if(edge < size(curveIndices{queryView+1,1},1))
                                                %Get the curve ID that contains this edge
                                                curve = curveIndices{queryView+1,1}(edge+1,1);
                                                %Get the offset of this edge in that curve
                                                ds = curveOffsets{queryView+1,1}(edge+1,1);
                                                if(ds>-1 && (~isempty(query_links_3d{curve,1})))
                                                    %Get the linked 3d curves and their
                                                    %offsets
                                                    curves_3d = query_links_3d{curve,1}+1;
                                                    offsets_3d = query_offsets_3d{curve,1};

                                                    queryIND = find(curves_3d==queryID,1);

                                                    if(~isempty(queryIND))

                                                        cur_curve = curves_3d(1,queryIND);
                                                        cur_offset = offsets_3d(1,queryIND);
                                                        cur_size = size(all_recs{queryView+1,1}{1,cur_curve},1);

                                                        %Sample s corresponds to sample
                                                        %final_ds
                                                        final_ds = ds - cur_offset + 1;

                                                        if(ds>=cur_offset && (ds<cur_offset+cur_size) && final_ds <= size(query_curve,1))
                                                            final_next_corresp_found = 1;

                                                            %If the motion direction on the query
                                                            %curve is opposite, mark this as a
                                                            %closed-curve merging case
                                                            if(final_ds > query_final_index)
                                                                disp('WARNING: Closed curve case not found for init, but found for final!');
                                                                disp('The results will likely be wrong');
                                                                is_closed_curve_merging = 1;
                                                            end

                                                            break;
                                                        end
                                                    end
                                                end
                                            else
                                                disp('ERROR: Edge index is larger than the number of edges!!');
                                                continue;
                                            end
                                        end
                                        if(final_next_corresp_found)
                                            break;
                                        end
                                    end

                                end

                                if(~is_closed_curve_merging)

                                    %Flags and markers
                                    init_corresp_found = 0;
                                    final_corresp_found = 0;
                                    is_init_junction = 0;
                                    is_final_junction = 0;
                                    is_query_longer_init = 0;
                                    is_query_longer_final = 0;
                                    seed_init_index = 0;
                                    seed_final_index = 0;
                                    query_init_index = 0;
                                    query_final_index = 0;
                                    s = 0;

                                    %-----------------------------------------------------

                                    %Find the first correspondence at the beginning of both
                                    %curves by going over each sample until a correspondence is
                                    %found

                                    while(~init_corresp_found)
                                        %Go to the next sample on both curves
                                        s=s+1;
                                        if(s>size(seed_curve,1) || s>size(query_curve,1))
                                            break;
                                        end

                                        %Look for correspondence on the seed curve by
                                        %analyzing supporting edges
                                        edges = seed_support{s,1};
                                        for e=1:size(edges,2)
                                            edge = edges(1,e);
                                            if(edge < size(curveIndices{queryView+1,1},1))
                                                %Get the curve ID that contains this edge
                                                curve = curveIndices{queryView+1,1}(edge+1,1);
                                                %Get the offset of this edge in that curve
                                                ds = curveOffsets{queryView+1,1}(edge+1,1);
                                                if(ds>-1 && (~isempty(query_links_3d{curve,1})))
                                                    %Get the linked 3d curves and their
                                                    %offsets
                                                    curves_3d = query_links_3d{curve,1}+1;
                                                    offsets_3d = query_offsets_3d{curve,1};

                                                    queryIND = find(curves_3d==queryID,1);

                                                    if(~isempty(queryIND))

                                                        cur_curve = curves_3d(1,queryIND);
                                                        cur_offset = offsets_3d(1,queryIND);
                                                        cur_size = size(all_recs{queryView+1,1}{1,cur_curve},1);

                                                        %Sample s corresponds to sample
                                                        %final_ds
                                                        final_ds = ds - cur_offset + 1;

                                                        if(ds>=cur_offset && (ds<cur_offset+cur_size) && final_ds <= size(query_curve,1))
                                                            init_corresp_found = 1;
                                                            if(s>1)
                                                                is_init_junction = 1;
                                                            else
                                                                is_query_longer_init = 1;
                                                            end
                                                            seed_init_index = s;
                                                            query_init_index = final_ds;
                                                            break;
                                                        end
                                                    end
                                                end
                                            else
                                                disp('ERROR: Edge index is larger than the number of edges!!');
                                                continue;
                                            end
                                        end

                                        %If no correspondence is found using the seed curve, 
                                        %Look for correspondence on the query curve by
                                        %analyzing supporting edges
                                        if(~init_corresp_found)
                                            seed_view_size = size(seedViews,2);
                                            for sv=1:seed_view_size
                                                cur_seed_view = seedViews(1,sv); 
                                                query_support = query_support_all{cur_seed_view+1,1};
                                                cur_seed_IDs = seedIDs{cur_seed_view+1,1};
                                                %Links from image curves to 3d curves in the queryView
                                                seed_links_3d = all_links_3d{cur_seed_view+1,1};
                                                %Offset between image and 3d curves
                                                seed_offsets_3d = all_offsets_3d{cur_seed_view+1,1};

                                                if(~isempty(query_support))

                                                    edges = query_support{s,1};

                                                    for e=1:size(edges,2)
                                                        edge = edges(1,e);
                                                        if(edge < size(curveIndices{cur_seed_view+1,1},1))
                                                            %Get the curve ID that contains this edge
                                                            curve = curveIndices{cur_seed_view+1,1}(edge+1,1);
                                                            %Get the offset of this edge in that curve
                                                            ds = curveOffsets{cur_seed_view+1,1}(edge+1,1);

                                                            if(ds>-1 && (~isempty(seed_links_3d{curve,1})))
                                                                %Get the linked 3d curves and their
                                                                %offsets
                                                                curves_3d = seed_links_3d{curve,1}+1;
                                                                offsets_3d = seed_offsets_3d{curve,1};

                                                                for csi=1:size(cur_seed_IDs,2)
                                                                    seedIND = find(curves_3d==cur_seed_IDs(1,csi),1);
                                                                    if(~isempty(seedIND))
                                                                        cur_curve = curves_3d(1,seedIND);
                                                                        cur_offset = offsets_3d(1,seedIND);
                                                                        cur_size = size(all_recs{cur_seed_view+1,1}{1,cur_curve},1);

                                                                        %Sample s corresponds to sample
                                                                        %final_ds, 1-index
                                                                        final_ds = ds - cur_offset + 1;

                                                                        if(ds>=cur_offset && (ds<cur_offset+cur_size) && final_ds <= size(seed_curve,1))

                                                                            %Find which sample final_ds corresponds to
                                                                            reg_index = find(seed_reg_curve(:,1)==final_ds & seed_reg_curve(:,2)==cur_seed_IDs(1,csi) & seed_reg_curve(:,3)==cur_seed_view);

                                                                            if(~isempty(reg_index))
                                                                                seed_init_index = reg_index(1,1);
                                                                                query_init_index = s;

                                                                                init_corresp_found = 1;
                                                                                if(s>1)
                                                                                    is_init_junction = 1;
                                                                                else
                                                                                    is_query_longer_init = 0;
                                                                                end

                                                                                break;
                                                                            else
                                                                                disp('WARNING: curve index could not be converted for merging!');
                                                                            end
                                                                        end
                                                                    end
                                                                end

                                                                %If found, break out of the
                                                                %edge loop
                                                                if(init_corresp_found)
                                                                    break;
                                                                end

                                                            end
                                                        else
                                                            disp('ERROR: Edge index is larger than the number of edges!!');
                                                            continue;
                                                        end
                                                    end

                                                    %If found, break out of the seedView loop
                                                    if(init_corresp_found)
                                                        break;
                                                    end

                                                end                              

                                            end
                                        end

                                    end

                                    %------------------------------------------------------

                                    %Find the first correspondence at the end of both
                                    %curves by going over each sample until a correspondence is
                                    %found

                                    s=0;

                                    if(init_corresp_found)

                                        while(~final_corresp_found)
                                            %Go to the next sample on both curves
                                            s_seed = size(seed_curve,1)-s;
                                            s_query = size(query_curve,1)-s;

                                            if(s_seed < 1 || s_query < 1)
                                                break;
                                            end

                                            %Look for correspondence on the seed curve by
                                            %analyzing supporting edges
                                            edges = seed_support{s_seed,1};
                                            for e=1:size(edges,2)
                                                edge = edges(1,e);
                                                if(edge < size(curveIndices{queryView+1,1},1))
                                                    %Get the curve ID that contains this edge
                                                    curve = curveIndices{queryView+1,1}(edge+1,1);
                                                    %Get the offset of this edge in that curve
                                                    ds = curveOffsets{queryView+1,1}(edge+1,1);
                                                    if(ds>-1 && (~isempty(query_links_3d{curve,1})))
                                                        %Get the linked 3d curves and their
                                                        %offsets
                                                        curves_3d = query_links_3d{curve,1}+1;
                                                        offsets_3d = query_offsets_3d{curve,1};

                                                        queryIND = find(curves_3d==queryID,1);

                                                        if(~isempty(queryIND))

                                                            cur_curve = curves_3d(1,queryIND);
                                                            cur_offset = offsets_3d(1,queryIND);
                                                            cur_size = size(all_recs{queryView+1,1}{1,cur_curve},1);

                                                            %Sample s corresponds to sample
                                                            %final_ds
                                                            final_ds = ds - cur_offset + 1;

                                                            if(ds>=cur_offset && (ds<cur_offset+cur_size) && final_ds <= size(query_curve,1))
                                                                final_corresp_found = 1;
                                                                if(s>0)
                                                                    is_final_junction = 1;
                                                                else
                                                                    is_query_longer_final = 1;
                                                                end
                                                                seed_final_index = s_seed;
                                                                query_final_index = final_ds;
                                                                break;
                                                            end
                                                        end
                                                    end
                                                else
                                                    disp('ERROR: Edge index is larger than the number of edges!!');
                                                    continue;
                                                end
                                            end

                                            %If no correspondence is found using the seed curve, 
                                            %Look for correspondence on the query curve by
                                            %analyzing supporting edges
                                            if(~final_corresp_found)
                                                seed_view_size = size(seedViews,2);
                                                for sv=1:seed_view_size
                                                    cur_seed_view = seedViews(1,sv); 
                                                    query_support = query_support_all{cur_seed_view+1,1};
                                                    cur_seed_IDs = seedIDs{cur_seed_view+1,1};
                                                    %Links from image curves to 3d curves in the queryView
                                                    seed_links_3d = all_links_3d{cur_seed_view+1,1};
                                                    %Offset between image and 3d curves
                                                    seed_offsets_3d = all_offsets_3d{cur_seed_view+1,1};

                                                    if(~isempty(query_support))

                                                        edges = query_support{s_query,1};

                                                        for e=1:size(edges,2)
                                                            edge = edges(1,e);
                                                            if(edge < size(curveIndices{cur_seed_view+1,1},1))
                                                                %Get the curve ID that contains this edge
                                                                curve = curveIndices{cur_seed_view+1,1}(edge+1,1);
                                                                %Get the offset of this edge in that curve
                                                                ds = curveOffsets{cur_seed_view+1,1}(edge+1,1);
                                                                if(ds>-1 && (~isempty(seed_links_3d{curve,1})))
                                                                    %Get the linked 3d curves and their
                                                                    %offsets
                                                                    curves_3d = seed_links_3d{curve,1}+1;
                                                                    offsets_3d = seed_offsets_3d{curve,1};

                                                                    for csi=1:size(cur_seed_IDs,2)
                                                                        seedIND = find(curves_3d==cur_seed_IDs(1,csi),1);
                                                                        if(~isempty(seedIND))
                                                                            cur_curve = curves_3d(1,seedIND);
                                                                            cur_offset = offsets_3d(1,seedIND);
                                                                            cur_size = size(all_recs{cur_seed_view+1,1}{1,cur_curve},1);

                                                                            %Sample s corresponds to sample
                                                                            %final_ds, 1-index
                                                                            final_ds = ds - cur_offset + 1;

                                                                            if(ds>=cur_offset && (ds<cur_offset+cur_size) && final_ds <= size(seed_curve,1))

                                                                                %Find which sample final_ds corresponds to
                                                                                reg_index = find(seed_reg_curve(:,1)==final_ds & seed_reg_curve(:,2)==cur_seed_IDs(1,csi) & seed_reg_curve(:,3)==cur_seed_view);

                                                                                if(~isempty(reg_index))
                                                                                    seed_final_index = reg_index(1,1);
                                                                                    query_final_index = s_query;

                                                                                    final_corresp_found = 1;
                                                                                    if(s>0)
                                                                                        is_final_junction = 1;
                                                                                    else
                                                                                        is_query_longer_final = 0;
                                                                                    end

                                                                                    break;
                                                                                else
                                                                                    disp('WARNING: curve index could not be converted for merging!');
                                                                                end

                                                                            end
                                                                        end
                                                                    end

                                                                    if(final_corresp_found)
                                                                        break;
                                                                    end

                                                                end
                                                            else
                                                                disp('ERROR: Edge index is larger than the number of edges!!');
                                                                continue;
                                                            end
                                                        end

                                                        %If found, break out of the seedView loop
                                                        if(init_corresp_found)
                                                            break;
                                                        end

                                                    end

                                                end
                                            end
                                            s=s+1;
                                        end

                                        %If seed_init_index and
                                        %seed_final_index are too close, the
                                        %results are unreliable. Repeat the
                                        %procedure but only use query curve
                                        %correspondences


                                        if(abs(query_init_index - query_final_index)<3)

                                            disp('TEMP: Seed curve not reliable!');

                                            %Flags and markers
                                            init_corresp_found = 0;
                                            final_corresp_found = 0;
                                            is_init_junction = 0;
                                            is_final_junction = 0;
                                            is_query_longer_init = 0;
                                            is_query_longer_final = 0;
                                            seed_init_index = 0;
                                            seed_final_index = 0;
                                            query_init_index = 0;
                                            query_final_index = 0;
                                            s = 0;

                                            %Find the init correspondence for seed curve
                                            while(~init_corresp_found)
                                                %Go to the next sample on both curves
                                                s=s+1;
                                                if(s>size(query_curve,1))
                                                    break;
                                                end

                                                seed_view_size = size(seedViews,2);
                                                for sv=1:seed_view_size
                                                    cur_seed_view = seedViews(1,sv); 
                                                    query_support = query_support_all{cur_seed_view+1,1};
                                                    cur_seed_IDs = seedIDs{cur_seed_view+1,1};
                                                    %Links from image curves to 3d curves in the queryView
                                                    seed_links_3d = all_links_3d{cur_seed_view+1,1};
                                                    %Offset between image and 3d curves
                                                    seed_offsets_3d = all_offsets_3d{cur_seed_view+1,1};

                                                    if(~isempty(query_support))

                                                        %Look for correspondence on the seed curve by
                                                        %analyzing supporting edges
                                                        edges = query_support{s,1};

                                                        for e=1:size(edges,2)
                                                            edge = edges(1,e);
                                                            if(edge < size(curveIndices{cur_seed_view+1,1},1))
                                                                %Get the curve ID that contains this edge
                                                                curve = curveIndices{cur_seed_view+1,1}(edge+1,1);
                                                                %Get the offset of this edge in that curve
                                                                ds = curveOffsets{cur_seed_view+1,1}(edge+1,1);

                                                                if(ds>-1 && (~isempty(seed_links_3d{curve,1})))
                                                                    %Get the linked 3d curves and their
                                                                    %offsets
                                                                    curves_3d = seed_links_3d{curve,1}+1;
                                                                    offsets_3d = seed_offsets_3d{curve,1};

                                                                    for csi=1:size(cur_seed_IDs,2)
                                                                        seedIND = find(curves_3d==cur_seed_IDs(1,csi),1);
                                                                        if(~isempty(seedIND))
                                                                            cur_curve = curves_3d(1,seedIND);
                                                                            cur_offset = offsets_3d(1,seedIND);
                                                                            cur_size = size(all_recs{cur_seed_view+1,1}{1,cur_curve},1);

                                                                            %Sample s corresponds to sample
                                                                            %final_ds, 1-index
                                                                            final_ds = ds - cur_offset + 1;

                                                                            if(ds>=cur_offset && (ds<cur_offset+cur_size) && final_ds <= size(seed_curve,1))


                                                                                %Find which sample final_ds corresponds to
                                                                                reg_index = find(seed_reg_curve(:,1)==final_ds & seed_reg_curve(:,2)==cur_seed_IDs(1,csi) & seed_reg_curve(:,3)==cur_seed_view);

                                                                                if(~isempty(reg_index))
                                                                                    seed_init_index = reg_index(1,1);
                                                                                    query_init_index = s;

                                                                                    init_corresp_found = 1;
                                                                                    if(s>1)
                                                                                        is_init_junction = 1;
                                                                                    else
                                                                                        is_query_longer_init = 0;
                                                                                    end

                                                                                    break;
                                                                                else
                                                                                    disp('WARNING: curve index could not be converted for merging!');
                                                                                end
                                                                            end
                                                                        end
                                                                    end

                                                                    %If found, break out of the
                                                                    %edge loop
                                                                    if(init_corresp_found)
                                                                        break;
                                                                    end

                                                                end
                                                            else
                                                                disp('ERROR: Edge index is larger than the number of edges!!');
                                                                continue;
                                                            end
                                                        end
                                                    end

                                                    %If found, break out of the
                                                    %seedViews loop
                                                    if(init_corresp_found)
                                                        break;
                                                    end

                                                end
                                            end

                                            if(~init_corresp_found)
                                                continue;
                                            end

                                            s=0;

                                            %Find the final correspondence for seed curve
                                            while(~final_corresp_found)

                                                %Go to the next sample on both curves
                                                s_query = size(query_curve,1)-s;

                                                if(s_query < 1)
                                                    break;
                                                end

                                                seed_view_size = size(seedViews,2);
                                                for sv=1:seed_view_size
                                                    cur_seed_view = seedViews(1,sv); 
                                                    query_support = query_support_all{cur_seed_view+1,1};
                                                    cur_seed_IDs = seedIDs{cur_seed_view+1,1};
                                                    %Links from image curves to 3d curves in the queryView
                                                    seed_links_3d = all_links_3d{cur_seed_view+1,1};
                                                    %Offset between image and 3d curves
                                                    seed_offsets_3d = all_offsets_3d{cur_seed_view+1,1};

                                                    if(~isempty(query_support))

                                                        %Look for correspondence on the seed curve by
                                                        %analyzing supporting edges
                                                        edges = query_support{s_query,1};
                                                        for e=1:size(edges,2)
                                                            edge = edges(1,e);
                                                            if(edge < size(curveIndices{cur_seed_view+1,1},1))
                                                                %Get the curve ID that contains this edge
                                                                curve = curveIndices{cur_seed_view+1,1}(edge+1,1);
                                                                %Get the offset of this edge in that curve
                                                                ds = curveOffsets{cur_seed_view+1,1}(edge+1,1);
                                                                if(ds>-1 && (~isempty(seed_links_3d{curve,1})))
                                                                    %Get the linked 3d curves and their
                                                                    %offsets
                                                                    curves_3d = seed_links_3d{curve,1}+1;
                                                                    offsets_3d = seed_offsets_3d{curve,1};

                                                                    for csi=1:size(cur_seed_IDs,2)
                                                                        seedIND = find(curves_3d==cur_seed_IDs(1,csi),1);
                                                                        if(~isempty(seedIND))
                                                                            cur_curve = curves_3d(1,seedIND);
                                                                            cur_offset = offsets_3d(1,seedIND);
                                                                            cur_size = size(all_recs{cur_seed_view+1,1}{1,cur_curve},1);

                                                                            %Sample s corresponds to sample
                                                                            %final_ds, 1-index
                                                                            final_ds = ds - cur_offset + 1;

                                                                            if(ds>=cur_offset && (ds<cur_offset+cur_size) && final_ds <= size(seed_curve,1))


                                                                                %Find which sample final_ds corresponds to
                                                                                reg_index = find(seed_reg_curve(:,1)==final_ds & seed_reg_curve(:,2)==cur_seed_IDs(1,csi) & seed_reg_curve(:,3)==cur_seed_view);

                                                                                if(~isempty(reg_index))
                                                                                    seed_final_index = reg_index(1,1);
                                                                                    query_final_index = s_query;

                                                                                    final_corresp_found = 1;
                                                                                    if(s>1)
                                                                                        is_final_junction = 1;
                                                                                    else
                                                                                        is_query_longer_final = 0;
                                                                                    end

                                                                                    break;
                                                                                else
                                                                                    disp('WARNING: curve index could not be converted for merging!');
                                                                                end
                                                                            end
                                                                        end
                                                                    end

                                                                    %If found, break out of the
                                                                    %edge loop
                                                                    if(final_corresp_found)
                                                                        break;
                                                                    end

                                                                end
                                                            else
                                                                disp('ERROR: Edge index is larger than the number of edges!!');
                                                                continue;
                                                            end
                                                        end

                                                    end

                                                    %If found, break out of the
                                                    %seedViews loop
                                                    if(final_corresp_found)
                                                        break;
                                                    end

                                                end
                                                s=s+1;
                                            end

                                            if(~final_corresp_found)
                                                continue;
                                            end
                                            
                                        end



                                    end
                                    
                                    if(abs(seed_init_index - seed_final_index)<3 || abs(query_init_index - query_final_index)<3 || seed_init_index >= seed_final_index || query_init_index >= query_final_index)
                                        init_corresp_found = 0;
                                        final_corresp_found = 0;
                                    end
                                    
                %                     init_corresp_found
                %                     final_corresp_found
                %                     is_init_junction
                %                     is_final_junction
                %                     is_query_longer_init
                %                     is_query_longer_final
                %                     seed_init_index
                %                     seed_final_index
                %                     query_init_index
                %                     query_final_index

                                    %------------------------------------------------------

                                    %Merge the related curves
                                    %Mark merged curves, add views and IDs present in the
                                    %merged curve
                                    %Fix edge support data for the merged curve
                                    %Fix the pointers to the merged query curve
                                    if(init_corresp_found && final_corresp_found)

                                        cluster_flags{queryView+1,1}(1,cc) = 1;
                                        seedViews = unique([seedViews queryView]);
                                        seedIDs{queryView+1,1} = [seedIDs{queryView+1,1} clusters{queryView+1,1}(1,cc)];

                                        %Find the averaged samples for the existing seed curve
                                        num_seed_overlap_samples = seed_final_index - seed_init_index + 1;
                                        seed_average_curve = zeros(num_seed_overlap_samples,3);
                                        seed_average_count = zeros(num_seed_overlap_samples,1); 

                                        seed_subset = seed_curve(seed_init_index:seed_final_index,:);
                                        seed_closest = zeros(num_seed_overlap_samples,3);

                                        seed_average_support_all = cell(numIM,1);
                                        for i=1:numIM
                                            seed_average_support_all{i,1} = cell(num_seed_overlap_samples,1);
                                        end

                                        for ms=seed_init_index:seed_final_index

                                            curSample = seed_curve(ms,:);
                                            [closest_pt,outside_init_range,outside_final_range,prev_pt,next_pt] = find_closest_point_on_curve(query_curve,curSample);

                                            seed_closest(ms-seed_init_index+1,:) = closest_pt;

                                            seed_average_curve(ms-seed_init_index+1,:) = (seed_curve(ms,:).*seed_count(ms,:) + closest_pt)./(seed_count(ms,:)+1);
                                            seed_average_count(ms-seed_init_index+1,1) = (seed_count(ms,:)+1);

                                            for i=1:numIM
                                                if(~isempty(seed_support_all{i,1}))
                                                    seed_average_support_all{i,1}{ms-seed_init_index+1,1} = seed_support_all{i,1}{ms,1};
                                                end
                                            end
                                        end

                                        %Find the averaged samples for the existing query curve
                                        num_query_overlap_samples = query_final_index - query_init_index + 1;
                                        query_average_curve = zeros(num_query_overlap_samples,3);
                                        query_average_count = zeros(num_query_overlap_samples,1);

                                        query_subset = query_curve(query_init_index:query_final_index,:);
                                        query_closest = zeros(num_query_overlap_samples,3);

                                        query_average_support_all = cell(numIM,1);
                                        for i=1:numIM
                                            query_average_support_all{i,1} = cell(num_query_overlap_samples,1);
                                        end

                                        for ms=query_init_index:query_final_index

                                            curSample = query_curve(ms,:);
                                            [closest_pt,outside_init_range,outside_final_range,prev_pt,next_pt] = find_closest_point_on_curve(seed_curve,curSample);

                                            weight = max(seed_count(prev_pt,1),seed_count(next_pt,1));

                                            query_closest(ms-query_init_index+1,:) = closest_pt;                                

                                            query_average_curve(ms-query_init_index+1,:) = (query_curve(ms,:) + closest_pt.*weight)./(weight+1);
                                            query_average_count(ms-query_init_index+1,1) = weight+1;

                                            for i=1:numIM
                                                if(~isempty(query_support_all{i,1}))
                                                    query_average_support_all{i,1}{ms-query_init_index+1,1} = query_support_all{i,1}{ms,1};
                                                end
                                            end
                                        end

                                        if(0)
                                            merged_overlap_curve = seed_average_curve;

                                        else

                                            %Merge the two overlap curves by inserting the samples
                                            %of the query average curve into the seed average curve

                                            merged_overlap_curve = seed_average_curve;
                                            merged_overlap_count  = seed_average_count;
                                            merged_overlap_support_all = seed_average_support_all;
                                            index_overlap_curve = (1:num_seed_overlap_samples)';
                                            %First column is the sample ID, second is curve ID
                                            %and third is the view ID
                                            reg_curve = seed_reg_curve(seed_init_index:seed_final_index,:);

                                            %Parts of the query_average_curve that fall outside the
                                            %range of seed_average_curve
                                            prefix_curve = [];
                                            prefix_count = [];
                                            prefix_support_all = cell(numIM,1);
                                            for i=1:numIM
                                                prefix_support_all{i,1} = cell(0,0);
                                            end
                                            prefix_reg = [];

                                            suffix_curve = [];
                                            suffix_count = [];
                                            suffix_support_all = cell(numIM,1);
                                            for i=1:numIM
                                                suffix_support_all{i,1} = cell(0,0);
                                            end
                                            suffix_reg = [];

                                            for ms=1:num_query_overlap_samples
                                            %for ms=1:4   

                                                curSample = query_average_curve(ms,:);
                                                [closest_pt,outside_init_range,outside_final_range,prev_pt,next_pt] = find_closest_point_on_curve(seed_average_curve,curSample);

                                                if(prev_pt==1 && outside_init_range)
                                                    prefix_curve = [prefix_curve; curSample];
                                                    prefix_count = [prefix_count; query_average_count(ms,:)];
                                                    prefix_reg = [prefix_reg; [(ms+query_init_index-1) queryID queryView]];

                                                    for i=1:numIM
                                                        if(~isempty(query_average_support_all{i,1}))
                                                            prefix_support_all{i,1} = [prefix_support_all{i,1}; query_average_support_all{i,1}(ms,1)];
                                                        else
                                                            prefix_support_all{i,1} = [prefix_support_all{i,1}; cell(0,0)];
                                                        end
                                                    end
                                                    continue;

                                                elseif(next_pt==num_seed_overlap_samples && outside_final_range)
                                                    suffix_curve = [suffix_curve; curSample];
                                                    suffix_count = [suffix_count; query_average_count(ms,:)];
                                                    suffix_reg = [suffix_reg; [(ms+query_init_index-1) queryID queryView]];

                                                    for i=1:numIM
                                                        if(~isempty(query_average_support_all{i,1}))
                                                            suffix_support_all{i,1} = [suffix_support_all{i,1}; query_average_support_all{i,1}(ms,1)];
                                                        else
                                                            suffix_support_all{i,1} = [suffix_support_all{i,1}; cell(0,0)];
                                                        end
                                                    end
                                                    continue;

                                                end

                                                oc_size = size(merged_overlap_curve,1);
                                                next_ind = index_overlap_curve(next_pt,1);
                                                merged_overlap_curve = [merged_overlap_curve(1:(next_ind-1),:); curSample; merged_overlap_curve(next_ind:oc_size,:)];
                                                merged_overlap_count = [merged_overlap_count(1:(next_ind-1),1); query_average_count(ms,1); merged_overlap_count(next_ind:oc_size,:)];
                                                reg_curve = [reg_curve(1:(next_ind-1),:); [(ms+query_init_index-1) queryID queryView]; reg_curve(next_ind:oc_size,:)];

                                                for i=1:numIM
                                                    if(~isempty(query_average_support_all{i,1}) && ~isempty(merged_overlap_support_all{i,1}))
                                                        merged_overlap_support_all{i,1} = [merged_overlap_support_all{i,1}(1:(next_ind-1),1); query_average_support_all{i,1}(ms,1); merged_overlap_support_all{i,1}(next_ind:oc_size,:)];
                                                    else
                                                        merged_overlap_support_all{i,1} = [merged_overlap_support_all{i,1}(1:(next_ind-1),1); cell(0,0); merged_overlap_support_all{i,1}(next_ind:oc_size,:)];
                                                    end
                                                end
                                                index_overlap_curve(next_pt:num_seed_overlap_samples,1)=index_overlap_curve(next_pt:num_seed_overlap_samples,1)+1;
                                            end

                                            %Append the prefix and suffix curve
                                            merged_overlap_curve = [prefix_curve; merged_overlap_curve; suffix_curve];
                                            merged_overlap_count = [prefix_count; merged_overlap_count; suffix_count];
                                            reg_curve = [prefix_reg; reg_curve; suffix_reg];

                                            for i=1:numIM
                                                merged_overlap_support_all{i,1} = [prefix_support_all{i,1}; merged_overlap_support_all{i,1}; suffix_support_all{i,1}];
                                            end

                                        end



                                        %Add the branches that do not overlap, deal with the
                                        %junction breaking here
                                        merged_final_curve = merged_overlap_curve;
                                        merged_final_count = merged_overlap_count;
                                        merged_final_support_all = merged_overlap_support_all;



                                        seed_size = size(seed_curve,1);
                                        query_size = size(query_curve,1); 

                                        %Longer branch will be appended to the seed curve
                                        %for computational reasons
                                        seed_branch_longer = 0;
                                        if(is_init_junction)
                                            if(seed_init_index > query_init_index)
                                                long_branch_init = seed_curve(1:seed_init_index,:);
                                                short_branch_init = query_curve(1:query_init_index,:);
                                                branch_support_all = seed_support_all;

                                                seed_branch_longer = 1;

                                                %If the shorter branch is long enough,
                                                %create a new branch in the graph

                                                %Threshold is chosen to be the same as
                                                %the length threshold for curves

                                                if(size(short_branch_init,1) >= 10)
                                                    disp('Initial junction branch is long enough');
                                                    %Attributes for the short branch
                                                    short_branch_init_count = ones(query_init_index,1);
                                                    short_branch_init_support_all = cell(numIM,1);
                                                    for i=1:numIM
                                                        if(~isempty(query_support_all{i,1}))
                                                            short_branch_init_support_all{i,1} = query_support_all{i,1}{1:query_init_index,1};
                                                        end
                                                    end
                                                    short_branch_init_reg = [(1:query_init_index)' queryID.*ones(query_init_index,1) queryView.*ones(query_init_index,1)];
                                                    short_branch_init_views = queryView;
                                                    short_branch_init_IDs = cell(numIM,1);
                                                    short_branch_init_IDs{queryView+1,1} = queryID;

                                                    is_init = 1;
                                                    stitch_point = merged_final_curve(1,:);

                                                    seed_curve_graph = [seed_curve_graph; short_branch_init];
                                                    seed_count_graph = [seed_count_graph; short_branch_init_count];
                                                    seed_support_graph = [seed_support_graph; short_branch_init_support_all];
                                                    seed_reg_graph = [seed_reg_graph; short_branch_init_reg];
                                                    seedViews_graph = [seedViews_graph; short_branch_init_views];
                                                    seedIDs_graph = [seedIDs_graph; short_branch_init_IDs];
                                                    is_init_graph = [is_init_graph; is_init];
                                                    stitch_point_graph = [stitch_point_graph; stitch_point];
                                                end

                                            else
                                                long_branch_init = query_curve(1:query_init_index,:);
                                                short_branch_init = seed_curve(1:seed_init_index,:);
                                                branch_support_all = query_support_all;

                                                reg_curve_ID = queryID;
                                                reg_curve_view = queryView;

                                                %If the shorter branch is long enough,
                                                %create a new branch in the graph

                                                %Threshold is chosen to be the same as
                                                %the length threshold for curves
                                                if(size(short_branch_init,1) >= 10)
                                                    disp('Initial junction branch is long enough');
                                                    %Attributes for the short branch
                                                    short_branch_init_count = seed_count(1:seed_init_index,1);
                                                    short_branch_init_support_all = cell(numIM,1);
                                                    for i=1:numIM
                                                        if(~isempty(seed_support_all{i,1}))
                                                            short_branch_init_support_all{i,1} = seed_support_all{i,1}{1:seed_init_index,1};
                                                        end
                                                    end
                                                    short_branch_init_reg = seed_reg_curve(1:seed_init_index,:);
                                                    short_branch_init_views = [];
                                                    short_branch_init_IDs = cell(numIM,1);

                                                    [closest_pt,outside_init_range,outside_final_range,prev_pt,next_pt] = find_closest_point_on_curve(long_branch_init,merged_final_curve(1,:));

                                                    is_init = 1;
                                                    stitch_point = merged_final_curve(1,:);

                                                    %Get the attributes for the
                                                    %short branch

                                                    for sbr=1:seed_init_index
                                                        curveID = seed_reg_curve(sbr,2);
                                                        viewID = seed_reg_curve(sbr,3);

                                                        short_branch_init_views = unique([short_branch_init_views viewID]);
                                                        short_branch_init_IDs{viewID+1,1} = unique([short_branch_init_IDs{viewID+1,1} curveID]);

                                                    end
                                                    seed_curve_graph = [seed_curve_graph; short_branch_init];
                                                    seed_count_graph = [seed_count_graph; short_branch_init_count];
                                                    seed_support_graph = [seed_support_graph; short_branch_init_support_all];
                                                    seed_reg_graph = [seed_reg_graph; short_branch_init_reg];
                                                    seedViews_graph = [seedViews_graph; short_branch_init_views];
                                                    seedIDs_graph = [seedIDs_graph; short_branch_init_IDs];
                                                    is_init_graph = [is_init_graph; is_init];
                                                    stitch_point_graph = [stitch_point_graph; stitch_point];
                                                end

                                                %Loop over the short branch that's
                                                %broken from the seed curve to fix
                                                %all the attributes
                                                for sbr=1:seed_init_index
                                                    curveID = seed_reg_curve(sbr,2);
                                                    viewID = seed_reg_curve(sbr,3);
                                                    search_space = seed_reg_curve(seed_init_index:seed_size,:);

                                                    if(isempty(find(search_space(:,2)==curveID,1)))
                                                        seedIDVec = seedIDs{viewID+1,1};
                                                        rem_ind = find(seedIDVec==curveID,1);
                                                        seedIDVec(1,rem_ind) = [];
                                                        seedIDs{viewID+1,1} = seedIDVec;
                                                    end

                                                    if(isempty(find(search_space(:,3)==viewID,1)))
                                                        rem_ind = find(seedViews==viewID,1);
                                                        seedViews(1,rem_ind) = [];
                                                    end

                                                end

                                            end


                                            [closest_pt,outside_init_range,outside_final_range,prev_pt,next_pt] = find_closest_point_on_curve(long_branch_init,merged_final_curve(1,:));
                                            if(prev_pt<1)
                                                disp('WARNING: Possible junction going undetected.');
                                            else
                                                merged_final_curve = [long_branch_init(1:prev_pt,:); merged_final_curve];


                                                if(seed_branch_longer)
                                                    merged_final_count = [seed_count(1:prev_pt,1); merged_final_count];
                                                    reg_curve = [seed_reg_curve(1:prev_pt,:); reg_curve];
                                                else
                                                    merged_final_count = [ones(prev_pt,1); merged_final_count];
                                                    reg_curve = [[(1:prev_pt)' reg_curve_ID.*ones(prev_pt,1) reg_curve_view.*ones(prev_pt,1)]; reg_curve];
                                                end

                                                for i=1:numIM
                                                    if(~isempty(branch_support_all{i,1}))
                                                        merged_final_support_all{i,1} = [branch_support_all{i,1}(1:prev_pt,1); merged_final_support_all{i,1}];
                                                    else
                                                        merged_final_support_all{i,1} = [cell(prev_pt,1); merged_final_support_all{i,1}];
                                                    end
                                                end
                                            end




                                        elseif(is_query_longer_init)
                                            [closest_pt,outside_init_range,outside_final_range,prev_pt,next_pt] = find_closest_point_on_curve(query_curve,merged_final_curve(1,:));
                                            if(prev_pt<1)
                                                disp('WARNING: Possible junction going undetected.');
                                            else
                                                merged_final_curve = [query_curve(1:prev_pt,:); merged_final_curve];
                                                merged_final_count = [ones(prev_pt,1); merged_final_count];
                                                reg_curve = [[(1:prev_pt)' queryID.*ones(prev_pt,1) queryView.*ones(prev_pt,1)]; reg_curve];

                                                for i=1:numIM
                                                    if(~isempty(query_support_all{i,1}))
                                                        merged_final_support_all{i,1} = [query_support_all{i,1}(1:prev_pt,1); merged_final_support_all{i,1}];
                                                    else
                                                        merged_final_support_all{i,1} = [cell(prev_pt,1); merged_final_support_all{i,1}];
                                                    end
                                                end
                                            end
                                        else
                                            [closest_pt,outside_init_range,outside_final_range,prev_pt,next_pt] = find_closest_point_on_curve(seed_curve,merged_final_curve(1,:));
                                            if(prev_pt<1)
                                                disp('WARNING: Possible junction going undetected.');
                                            else
                                                merged_final_curve = [seed_curve(1:prev_pt,:); merged_final_curve];
                                                merged_final_count = [seed_count(1:prev_pt,1); merged_final_count];
                                                reg_curve = [seed_reg_curve(1:prev_pt,:); reg_curve];

                                                for i=1:numIM
                                                    if(~isempty(seed_support_all{i,1}))
                                                        merged_final_support_all{i,1} = [seed_support_all{i,1}(1:prev_pt,1); merged_final_support_all{i,1}];
                                                    else
                                                        merged_final_support_all{i,1} = [cell(prev_pt,1); merged_final_support_all{i,1}];
                                                    end
                                                end
                                            end
                                        end

                                        merged_final_size = size(merged_final_curve,1);
                                        seed_branch_longer = 0;

                                        if(is_final_junction)
                                            if((seed_size-seed_final_index) > (query_size-query_final_index))
                                                long_branch_final = seed_curve(seed_final_index:seed_size,:);
                                                short_branch_final = query_curve(query_final_index:query_size,:);
                                                branch_support_all = seed_support_all;

                                                seed_branch_longer = 1;

                                                %If the shorter branch is long enough,
                                                %create a new branch in the graph

                                                %Threshold is chosen to be the same as
                                                %the length threshold for curves

                                                if(size(short_branch_final,1) >= 10)
                                                    disp('Final junction branch is long enough');
                                                    %Attributes for the short branch
                                                    short_branch_final_count = ones(query_size-query_final_index+1,1);
                                                    short_branch_final_support_all = cell(numIM,1);
                                                    for i=1:numIM
                                                        if(~isempty(query_support_all{i,1}))
                                                            short_branch_final_support_all{i,1} = query_support_all{i,1}{query_final_index:query_size,1};
                                                        end
                                                    end
                                                    short_branch_final_reg = [(query_final_index:query_size)' queryID.*ones(query_size-query_final_index+1,1) queryView.*ones(query_size-query_final_index+1,1)];
                                                    short_branch_final_views = queryView;
                                                    short_branch_final_IDs = cell(numIM,1);
                                                    short_branch_final_IDs{queryView+1,1} = queryID;

                                                    is_init = 0;
                                                    stitch_point = merged_final_curve(merged_final_size,:);

                                                    seed_curve_graph = [seed_curve_graph; short_branch_final];
                                                    seed_count_graph = [seed_count_graph; short_branch_final_count];
                                                    seed_support_graph = [seed_support_graph; short_branch_final_support_all];
                                                    seed_reg_graph = [seed_reg_graph; short_branch_final_reg];
                                                    seedViews_graph = [seedViews_graph; short_branch_final_views];
                                                    seedIDs_graph = [seedIDs_graph; short_branch_final_IDs];
                                                    is_init_graph = [is_init_graph; is_init];
                                                    stitch_point_graph = [stitch_point_graph; stitch_point];
                                                end

                                            else
                                                long_branch_final = query_curve(query_final_index:query_size,:);
                                                short_branch_final = seed_curve(seed_final_index:seed_size,:);
                                                branch_support_all = query_support_all;

                                                reg_curve_ID = queryID;
                                                reg_curve_view = queryView;

                                                %If the shorter branch is long enough,
                                                %create a new branch in the graph

                                                %Threshold is chosen to be the same as
                                                %the length threshold for curves
                                                if(size(short_branch_final,1) >= 10)
                                                    disp('Final junction branch is long enough');
                                                    %Attributes for the short branch
                                                    short_branch_final_count = seed_count(seed_final_index:seed_size,1);
                                                    short_branch_final_support_all = cell(numIM,1);
                                                    for i=1:numIM
                                                        if(~isempty(seed_support_all{i,1}))
                                                            short_branch_final_support_all{i,1} = seed_support_all{i,1}{seed_final_index:seed_size,1};
                                                        end
                                                    end
                                                    short_branch_final_reg = seed_reg_curve(seed_final_index:seed_size,:);
                                                    short_branch_final_views = [];
                                                    short_branch_final_IDs = cell(numIM,1);

                                                    is_init = 0;
                                                    stitch_point = merged_final_curve(merged_final_size,:);

                                                    %Loop over the short branch that's
                                                    %broken from the seed curve to fix
                                                    %all the attributes

                                                    for sbr=seed_final_index:seed_size
                                                        curveID = seed_reg_curve(sbr,2);
                                                        viewID = seed_reg_curve(sbr,3);

                                                        short_branch_final_views = unique([short_branch_final_views viewID]);
                                                        short_branch_final_IDs{viewID+1,1} = unique([short_branch_final_IDs{viewID+1,1} curveID]);

                                                    end
                                                    seed_curve_graph = [seed_curve_graph; short_branch_init];
                                                    seed_count_graph = [seed_count_graph; short_branch_init_count];
                                                    seed_support_graph = [seed_support_graph; short_branch_init_support_all];
                                                    seed_reg_graph = [seed_reg_graph; short_branch_init_reg];
                                                    seedViews_graph = [seedViews_graph; short_branch_init_views];
                                                    seedIDs_graph = [seedIDs_graph; short_branch_init_IDs];
                                                    is_init_graph = [is_init_graph; is_init];
                                                    stitch_point_graph = [stitch_point_graph; stitch_point];
                                                end

                                                for sbr=seed_final_index:seed_size
                                                    curveID = seed_reg_curve(sbr,2);
                                                    viewID = seed_reg_curve(sbr,3);
                                                    search_space = reg_curve;

                                                    if(isempty(find(search_space(:,2)==curveID,1)))
                                                        seedIDVec = seedIDs{viewID+1,1};
                                                        rem_ind = find(seedIDVec==curveID,1);
                                                        seedIDVec(1,rem_ind) = [];
                                                        seedIDs{viewID+1,1} = seedIDVec;
                                                    end

                                                    if(isempty(find(search_space(:,3)==viewID,1)))
                                                        rem_ind = find(seedViews==viewID,1);
                                                        seedViews(1,rem_ind) = [];
                                                    end

                                                end

                                            end

                                            long_branch_size = size(long_branch_final,1);

                                            [closest_pt,outside_init_range,outside_final_range,prev_pt,next_pt] = find_closest_point_on_curve(long_branch_final,merged_final_curve(merged_final_size,:));
                                            if(next_pt>long_branch_size)
                                                disp('WARNING: Possible junction going undetected.');
                                            else
                                                merged_final_curve = [merged_final_curve; long_branch_final(next_pt:long_branch_size,:)];

                                                %Convert branch indices to curve indices                               
                                                if(seed_branch_longer)
                                                    branch_init = (next_pt-1+seed_final_index);
                                                    branch_final = (long_branch_size-1+seed_final_index);
                                                    merged_final_count = [merged_final_count; seed_count(branch_init:branch_final,1)];
                                                    reg_curve = [reg_curve; seed_reg_curve(branch_init:branch_final,:)];
                                                else
                                                    branch_init = (next_pt-1+query_final_index);
                                                    branch_final = (long_branch_size-1+query_final_index);
                                                    merged_final_count = [merged_final_count; ones(long_branch_size-next_pt+1,1)];
                                                    reg_curve = [reg_curve; [(branch_init:branch_final)' reg_curve_ID.*ones(long_branch_size-next_pt+1,1) reg_curve_view.*ones(long_branch_size-next_pt+1,1)]];
                                                end

                                                for i=1:numIM
                                                    if(~isempty(branch_support_all{i,1}))
                                                        merged_final_support_all{i,1} = [merged_final_support_all{i,1}; branch_support_all{i,1}(branch_init:branch_final,1)];
                                                    else
                                                        merged_final_support_all{i,1} = [merged_final_support_all{i,1}; cell(long_branch_size-next_pt+1,1)];
                                                    end
                                                end
                                            end

                                        elseif(is_query_longer_final)

                                            [closest_pt,outside_init_range,outside_final_range,prev_pt,next_pt] = find_closest_point_on_curve(query_curve,merged_final_curve(merged_final_size,:));
                                            if(next_pt>query_size)
                                                disp('WARNING: Possible junction going undetected.');
                                            else
                                                merged_final_curve = [merged_final_curve; query_curve(next_pt:query_size,:)];
                                                merged_final_count = [merged_final_count; ones(query_size-next_pt+1,1)];
                                                reg_curve = [reg_curve; [(next_pt:query_size)' queryID.*ones(query_size-next_pt+1,1) queryView.*ones(query_size-next_pt+1,1)]];

                                                for i=1:numIM
                                                    if(~isempty(query_support_all{i,1}))
                                                        merged_final_support_all{i,1} = [merged_final_support_all{i,1}; query_support_all{i,1}(next_pt:query_size,1)];
                                                    else
                                                        merged_final_support_all{i,1} = [merged_final_support_all{i,1}; cell(query_size-next_pt+1,1)];
                                                    end
                                                end
                                            end
                                        else
                                            [closest_pt,outside_init_range,outside_final_range,prev_pt,next_pt] = find_closest_point_on_curve(seed_curve,merged_final_curve(merged_final_size,:));
                                            if(next_pt>seed_size)
                                                disp('WARNING: Possible junction going undetected.');
                                            else
                                                merged_final_curve = [merged_final_curve; seed_curve(next_pt:seed_size,:)];
                                                merged_final_count = [merged_final_count; seed_count(next_pt:seed_size,1)];
                                                reg_curve = [reg_curve; seed_reg_curve(next_pt:seed_size,:)];

                                                for i=1:numIM
                                                    if(~isempty(seed_support_all{i,1}))
                                                        merged_final_support_all{i,1} = [merged_final_support_all{i,1}; seed_support_all{i,1}(next_pt:seed_size,1)];
                                                    else
                                                        merged_final_support_all{i,1} = [merged_final_support_all{i,1}; cell(seed_size-next_pt+1,1)];
                                                    end
                                                end
                                            end
                                        end
                                        old_seed_curve = seed_curve;
                                        old_seed_count = seed_count;
                                        old_seed_support_all = seed_support_all;
                                        old_seed_reg_curve = seed_reg_curve;

                                        seed_curve_graph{br,1} = merged_final_curve;
                                        seed_count_graph{br,1} = merged_final_count;
                                        seed_support_graph{br,1} = merged_final_support_all;
                                        seed_reg_graph{br,1} = reg_curve;
                                        seedViews_graph{br,1} = seedViews;
                                        seedIDs_graph{br,1} = seedIDs;
                                        v

                                    end
                                else
                                    %If we have a closed-curve merging situation, 
                                    %the stitching operations will be different

                                    %First, we need to find the initial
                                    %correspondence point for the query curve
                                    %(seed curve correspondences are already
                                    %computed)

                                    %Flags and markers

                                    s_ii = seed_init_index;
                                    s_ff = seed_final_index;
                                    q_fi = query_init_index;
                                    q_if = query_final_index;

                                    q_ii = 0;
                                    q_ff = 0;
                                    s_fi = 0;
                                    s_if = 0;

                                    init_corresp_found = 0;
                                    final_corresp_found = 0;
                                    s = 0;

                                    %Find the init correspondence for query curve
                                    while(~init_corresp_found)
                                        %Go to the next sample
                                        s=s+1;
                                        if(s>size(query_curve,1))
                                            break;
                                        end

                                        seed_view_size = size(seedViews,2);
                                        for sv=1:seed_view_size
                                            cur_seed_view = seedViews(1,sv); 
                                            query_support = query_support_all{cur_seed_view+1,1};
                                            cur_seed_IDs = seedIDs{cur_seed_view+1,1};
                                            %Links from image curves to 3d
                                            %curves in the cur_seed_view
                                            seed_links_3d = all_links_3d{cur_seed_view+1,1};
                                            %Offset between image and 3d curves
                                            seed_offsets_3d = all_offsets_3d{cur_seed_view+1,1};

                                            if(~isempty(query_support))

                                                %Look for correspondence on the query curve by
                                                %analyzing supporting edges
                                                edges = query_support{s,1};

                                                for e=1:size(edges,2)
                                                    edge = edges(1,e);
                                                    if(edge < size(curveIndices{cur_seed_view+1,1},1))
                                                        %Get the curve ID that contains this edge
                                                        curve = curveIndices{cur_seed_view+1,1}(edge+1,1);
                                                        %Get the offset of this edge in that curve
                                                        ds = curveOffsets{cur_seed_view+1,1}(edge+1,1);

                                                        if(ds>-1 && (~isempty(seed_links_3d{curve,1})))
                                                            %Get the linked 3d curves and their
                                                            %offsets
                                                            curves_3d = seed_links_3d{curve,1}+1;
                                                            offsets_3d = seed_offsets_3d{curve,1};

                                                            for csi=1:size(cur_seed_IDs,2)
                                                                seedIND = find(curves_3d==cur_seed_IDs(1,csi),1);
                                                                if(~isempty(seedIND))
                                                                    cur_curve = curves_3d(1,seedIND);
                                                                    cur_offset = offsets_3d(1,seedIND);
                                                                    cur_size = size(all_recs{cur_seed_view+1,1}{1,cur_curve},1);

                                                                    %Sample s corresponds to sample
                                                                    %final_ds, 1-index
                                                                    final_ds = ds - cur_offset + 1;

                                                                    if(ds>=cur_offset && (ds<cur_offset+cur_size) && final_ds <= size(seed_curve,1))

                                                                        %Find which sample final_ds corresponds to
                                                                        reg_index = find(seed_reg_curve(:,1)==final_ds & seed_reg_curve(:,2)==cur_seed_IDs(1,csi) & seed_reg_curve(:,3)==cur_seed_view);

                                                                        if(~isempty(reg_index))
                                                                            s_fi = reg_index(1,1);
                                                                            q_ii = s;
                                                                            init_corresp_found = 1;

                                                                            break;
                                                                        else
                                                                            disp('WARNING: curve index could not be converted for merging!');
                                                                        end
                                                                    end
                                                                end
                                                            end

                                                        end
                                                    else
                                                        disp('ERROR: Edge index is larger than the number of edges!!');
                                                        continue;
                                                    end

                                                    %If found, break out of the edge loop
                                                    if(init_corresp_found)
                                                        break;
                                                    end
                                                end
                                            end

                                            %If found, break out of the seedView loop
                                            if(init_corresp_found)
                                                break;
                                            end
                                        end
                                    end

                                    if(~init_corresp_found)
                                        continue;
                                    end

                                    s=0;

                                    %Find the final correspondence for seed curve
                                    while(~final_corresp_found)

                                        %Go to the next sample on both curves
                                        s_query = size(query_curve,1)-s;

                                        if(s_query < 1)
                                            break;
                                        end

                                        seed_view_size = size(seedViews,2);
                                        for sv=1:seed_view_size
                                            cur_seed_view = seedViews(1,sv); 
                                            query_support = query_support_all{cur_seed_view+1,1};
                                            cur_seed_IDs = seedIDs{cur_seed_view+1,1};
                                            %Links from image curves to 3d
                                            %curves in the cur_seed_view
                                            seed_links_3d = all_links_3d{cur_seed_view+1,1};
                                            %Offset between image and 3d curves
                                            seed_offsets_3d = all_offsets_3d{cur_seed_view+1,1};

                                            if(~isempty(query_support))

                                                %Look for correspondence on the seed curve by
                                                %analyzing supporting edges
                                                edges = query_support{s_query,1};
                                                for e=1:size(edges,2)
                                                    edge = edges(1,e);
                                                    if(edge < size(curveIndices{cur_seed_view+1,1},1))
                                                        %Get the curve ID that contains this edge
                                                        curve = curveIndices{cur_seed_view+1,1}(edge+1,1);
                                                        %Get the offset of this edge in that curve
                                                        ds = curveOffsets{cur_seed_view+1,1}(edge+1,1);
                                                        if(ds>-1 && (~isempty(seed_links_3d{curve,1})))
                                                            %Get the linked 3d curves and their
                                                            %offsets
                                                            curves_3d = seed_links_3d{curve,1}+1;
                                                            offsets_3d = seed_offsets_3d{curve,1};

                                                            for csi=1:size(cur_seed_IDs,2)
                                                                seedIND = find(curves_3d==cur_seed_IDs(1,csi),1);
                                                                if(~isempty(seedIND))
                                                                    cur_curve = curves_3d(1,seedIND);
                                                                    cur_offset = offsets_3d(1,seedIND);
                                                                    cur_size = size(all_recs{cur_seed_view+1,1}{1,cur_curve},1);

                                                                    %Sample s corresponds to sample
                                                                    %final_ds, 1-index
                                                                    final_ds = ds - cur_offset + 1;

                                                                    if(ds>=cur_offset && (ds<cur_offset+cur_size) && final_ds <= size(seed_curve,1))

                                                                        %Find which sample final_ds corresponds to
                                                                        reg_index = find(seed_reg_curve(:,1)==final_ds & seed_reg_curve(:,2)==cur_seed_IDs(1,csi) & seed_reg_curve(:,3)==cur_seed_view);

                                                                        if(~isempty(reg_index))
                                                                            s_if = reg_index(1,1);
                                                                            q_ff = s;

                                                                            final_corresp_found = 1;
                                                                            break;
                                                                        else
                                                                            disp('WARNING: curve index could not be converted for merging!');
                                                                        end
                                                                    end
                                                                end
                                                            end

                                                        end
                                                    else
                                                        disp('ERROR: Edge index is larger than the number of edges!!');
                                                        continue;
                                                    end

                                                    %If found, break out of the edge loop
                                                    if(final_corresp_found)
                                                        break;
                                                    end

                                                end
                                                s=s+1;    
                                            end

                                            %If found, break out of the seedView loop
                                            if(final_corresp_found)
                                                break;
                                            end

                                        end
                                    end

                                    if(~final_corresp_found)
                                        continue;
                                    end

                                    %Merge/Stitch the related curves
                                    if(init_corresp_found && final_corresp_found)

                                        cluster_flags{queryView+1,1}(1,cc) = 1;
                                        seedViews = unique([seedViews queryView]);
                                        seedIDs{queryView+1,1} = [seedIDs{queryView+1,1} clusters{queryView+1,1}(1,cc)];

                                        %Flip the seed curve and its attributes
                                        seed_curve = flipud(seed_curve);
                                        seed_count = flipud(seed_count);
                                        seed_support = flipud(seed_support);
                                        for i=1:numIM
                                            seed_support_all{i,1} = flipud(seed_support_all{i,1});
                                        end
                                        seed_reg_curve = flipud(seed_reg_curve);

                                        %Fix the seed indices
                                        seed_size = size(seed_curve,1);
                                        temp = s_ii;
                                        s_ii = seed_size - s_ff + 1;
                                        s_ff = seed_size - temp + 1;   
                                        temp = s_if;
                                        s_if = seed_size - s_fi + 1;
                                        s_fi = seed_size - temp + 1;

                                        %Find the averaged samples for the
                                        %ending part of the seed curve
                                        num_seed_overlap_samples = s_ff - s_if + 1;
                                        seed_average_curve = zeros(num_seed_overlap_samples,3);
                                        seed_average_count = zeros(num_seed_overlap_samples,1); 

                                        seed_subset = seed_curve(s_if:s_ff,:);
                                        seed_closest = zeros(num_seed_overlap_samples,3);

                                        seed_average_support_all = cell(numIM,1);
                                        for i=1:numIM
                                            seed_average_support_all{i,1} = cell(num_seed_overlap_samples,1);
                                        end

                                        for ms=s_if:s_ff

                                            curSample = seed_curve(ms,:);
                                            [closest_pt,outside_init_range,outside_final_range,prev_pt,next_pt] = find_closest_point_on_curve(query_curve,curSample);

                                            seed_closest(ms-s_if+1,:) = closest_pt;

                                            seed_average_curve(ms-s_if+1,:) = (seed_curve(ms,:).*seed_count(ms,:) + closest_pt)./(seed_count(ms,:)+1);
                                            seed_average_count(ms-s_if+1,1) = (seed_count(ms,:)+1);

                                            for i=1:numIM
                                                if(~isempty(seed_support_all{i,1}))
                                                    seed_average_support_all{i,1}{ms-s_if+1,1} = seed_support_all{i,1}{ms,1};
                                                end
                                            end
                                        end

                                        %Find the averaged samples for the
                                        %beginning part of the query curve
                                        num_query_overlap_samples = q_fi - q_ii + 1;
                                        query_average_curve = zeros(num_query_overlap_samples,3);
                                        query_average_count = zeros(num_query_overlap_samples,1);

                                        query_subset = query_curve(q_ii:q_fi,:);
                                        query_closest = zeros(num_query_overlap_samples,3);

                                        query_average_support_all = cell(numIM,1);
                                        for i=1:numIM
                                            query_average_support_all{i,1} = cell(num_query_overlap_samples,1);
                                        end

                                        for ms=q_ii:q_fi

                                            curSample = query_curve(ms,:);
                                            [closest_pt,outside_init_range,outside_final_range,prev_pt,next_pt] = find_closest_point_on_curve(seed_curve,curSample);

                                            weight = max(seed_count(prev_pt,1),seed_count(next_pt,1));

                                            query_closest(ms-q_ii+1,:) = closest_pt;                                

                                            query_average_curve(ms-q_ii+1,:) = (query_curve(ms,:) + closest_pt.*weight)./(weight+1);
                                            query_average_count(ms-q_ii+1,1) = weight+1;

                                            for i=1:numIM
                                                if(~isempty(query_support_all{i,1}))
                                                    query_average_support_all{i,1}{ms-q_ii+1,1} = query_support_all{i,1}{ms,1};
                                                end
                                            end
                                        end

                                        %Merge the two overlap curves by inserting the samples
                                        %of the query average curve into the seed average curve
                                        merged_overlap_curve_init = seed_average_curve;
                                        merged_overlap_count_init  = seed_average_count;
                                        merged_overlap_support_all_init = seed_average_support_all;
                                        index_overlap_curve = (1:num_seed_overlap_samples)';
                                        %First column is the sample ID, second is curve ID
                                        %and third is the view ID
                                        reg_curve_init = seed_reg_curve(s_if:s_ff,:);

                                        %Parts of the query_average_curve that fall outside the
                                        %range of seed_average_curve
                                        prefix_curve = [];
                                        prefix_count = [];
                                        prefix_support_all = cell(numIM,1);
                                        for i=1:numIM
                                            prefix_support_all{i,1} = cell(0,0);
                                        end
                                        prefix_reg = [];

                                        suffix_curve = [];
                                        suffix_count = [];
                                        suffix_support_all = cell(numIM,1);
                                        for i=1:numIM
                                            suffix_support_all{i,1} = cell(0,0);
                                        end
                                        suffix_reg = [];

                                        for ms=1:num_query_overlap_samples
                                        %for ms=1:4   

                                            curSample = query_average_curve(ms,:);
                                            [closest_pt,outside_init_range,outside_final_range,prev_pt,next_pt] = find_closest_point_on_curve(seed_average_curve,curSample);

                                            if(prev_pt==1 && outside_init_range)
                                                prefix_curve = [prefix_curve; curSample];
                                                prefix_count = [prefix_count; query_average_count(ms,:)];
                                                prefix_reg = [prefix_reg; [(ms+q_ii-1) queryID queryView]];

                                                for i=1:numIM
                                                    if(~isempty(query_average_support_all{i,1}))
                                                        prefix_support_all{i,1} = [prefix_support_all{i,1}; query_average_support_all{i,1}(ms,1)];
                                                    else
                                                        prefix_support_all{i,1} = [prefix_support_all{i,1}; cell(0,0)];
                                                    end
                                                end
                                                continue;

                                            elseif(next_pt==num_seed_overlap_samples && outside_final_range)
                                                suffix_curve = [suffix_curve; curSample];
                                                suffix_count = [suffix_count; query_average_count(ms,:)];
                                                suffix_reg = [suffix_reg; [(ms+q_ii-1) queryID queryView]];

                                                for i=1:numIM
                                                    if(~isempty(query_average_support_all{i,1}))
                                                        suffix_support_all{i,1} = [suffix_support_all{i,1}; query_average_support_all{i,1}(ms,1)];
                                                    else
                                                        suffix_support_all{i,1} = [suffix_support_all{i,1}; cell(0,0)];
                                                    end
                                                end
                                                continue;

                                            end

                                            oc_size = size(merged_overlap_curve_init,1);
                                            next_ind = index_overlap_curve(next_pt,1);
                                            merged_overlap_curve_init = [merged_overlap_curve_init(1:(next_ind-1),:); curSample; merged_overlap_curve_init(next_ind:oc_size,:)];
                                            merged_overlap_count_init = [merged_overlap_count_init(1:(next_ind-1),1); query_average_count(ms,1); merged_overlap_count_init(next_ind:oc_size,:)];
                                            reg_curve_init = [reg_curve_init(1:(next_ind-1),:); [(ms+q_ii-1) queryID queryView]; reg_curve_init(next_ind:oc_size,:)];

                                            for i=1:numIM
                                                if(~isempty(query_average_support_all{i,1}) && ~isempty(merged_overlap_support_all_init{i,1}))
                                                    merged_overlap_support_all_init{i,1} = [merged_overlap_support_all_init{i,1}(1:(next_ind-1),1); query_average_support_all{i,1}(ms,1); merged_overlap_support_all_init{i,1}(next_ind:oc_size,:)];
                                                else
                                                    merged_overlap_support_all_init{i,1} = [merged_overlap_support_all_init{i,1}(1:(next_ind-1),1); cell(0,0); merged_overlap_support_all_init{i,1}(next_ind:oc_size,:)];
                                                end
                                            end
                                            index_overlap_curve(next_pt:num_seed_overlap_samples,1)=index_overlap_curve(next_pt:num_seed_overlap_samples,1)+1;
                                        end

                                        %Append the prefix and suffix curve
                                        merged_overlap_curve_init = [prefix_curve; merged_overlap_curve_init; suffix_curve];
                                        merged_overlap_count_init = [prefix_count; merged_overlap_count_init; suffix_count];
                                        reg_curve_init = [prefix_reg; reg_curve_init; suffix_reg];

                                        for i=1:numIM
                                            merged_overlap_support_all_init{i,1} = [prefix_support_all{i,1}; merged_overlap_support_all_init{i,1}; suffix_support_all{i,1}];
                                        end

                                        %-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-

                                        %Find the averaged samples for the
                                        %beginning part of the seed curve
                                        num_seed_overlap_samples = s_fi - s_ii + 1;
                                        seed_average_curve = zeros(num_seed_overlap_samples,3);
                                        seed_average_count = zeros(num_seed_overlap_samples,1); 

                                        seed_subset = seed_curve(s_ii:s_fi,:);
                                        seed_closest = zeros(num_seed_overlap_samples,3);

                                        seed_average_support_all = cell(numIM,1);
                                        for i=1:numIM
                                            seed_average_support_all{i,1} = cell(num_seed_overlap_samples,1);
                                        end

                                        for ms=s_ii:s_fi

                                            curSample = seed_curve(ms,:);
                                            [closest_pt,outside_init_range,outside_final_range,prev_pt,next_pt] = find_closest_point_on_curve(query_curve,curSample);

                                            seed_closest(ms-s_ii+1,:) = closest_pt;

                                            seed_average_curve(ms-s_ii+1,:) = (seed_curve(ms,:).*seed_count(ms,:) + closest_pt)./(seed_count(ms,:)+1);
                                            seed_average_count(ms-s_ii+1,1) = (seed_count(ms,:)+1);

                                            for i=1:numIM
                                                if(~isempty(seed_support_all{i,1}))
                                                    seed_average_support_all{i,1}{ms-s_ii+1,1} = seed_support_all{i,1}{ms,1};
                                                end
                                            end
                                        end

                                        %Find the averaged samples for the
                                        %ending part of the query curve
                                        num_query_overlap_samples = q_ff - q_if + 1;
                                        query_average_curve = zeros(num_query_overlap_samples,3);
                                        query_average_count = zeros(num_query_overlap_samples,1);

                                        query_subset = query_curve(q_if:q_ff,:);
                                        query_closest = zeros(num_query_overlap_samples,3);

                                        query_average_support_all = cell(numIM,1);
                                        for i=1:numIM
                                            query_average_support_all{i,1} = cell(num_query_overlap_samples,1);
                                        end

                                        for ms=q_if:q_ff

                                            curSample = query_curve(ms,:);
                                            [closest_pt,outside_init_range,outside_final_range,prev_pt,next_pt] = find_closest_point_on_curve(seed_curve,curSample);

                                            weight = max(seed_count(prev_pt,1),seed_count(next_pt,1));

                                            query_closest(ms-q_if+1,:) = closest_pt;                                

                                            query_average_curve(ms-q_if+1,:) = (query_curve(ms,:) + closest_pt.*weight)./(weight+1);
                                            query_average_count(ms-q_if+1,1) = weight+1;

                                            for i=1:numIM
                                                if(~isempty(query_support_all{i,1}))
                                                    query_average_support_all{i,1}{ms-q_if+1,1} = query_support_all{i,1}{ms,1};
                                                end
                                            end
                                        end

                                        %Merge the two overlap curves by inserting the samples
                                        %of the query average curve into the seed average curve
                                        merged_overlap_curve_final = seed_average_curve;
                                        merged_overlap_count_final  = seed_average_count;
                                        merged_overlap_support_all_final = seed_average_support_all;
                                        index_overlap_curve = (1:num_seed_overlap_samples)';
                                        %First column is the sample ID, second is curve ID
                                        %and third is the view ID
                                        reg_curve_final = seed_reg_curve(s_ii:s_fi,:);

                                        %Parts of the query_average_curve that fall outside the
                                        %range of seed_average_curve
                                        prefix_curve = [];
                                        prefix_count = [];
                                        prefix_support_all = cell(numIM,1);
                                        for i=1:numIM
                                            prefix_support_all{i,1} = cell(0,0);
                                        end
                                        prefix_reg = [];

                                        suffix_curve = [];
                                        suffix_count = [];
                                        suffix_support_all = cell(numIM,1);
                                        for i=1:numIM
                                            suffix_support_all{i,1} = cell(0,0);
                                        end
                                        suffix_reg = [];

                                        for ms=1:num_query_overlap_samples
                                        %for ms=1:4   

                                            curSample = query_average_curve(ms,:);
                                            [closest_pt,outside_init_range,outside_final_range,prev_pt,next_pt] = find_closest_point_on_curve(seed_average_curve,curSample);

                                            if(prev_pt==1 && outside_init_range)
                                                prefix_curve = [prefix_curve; curSample];
                                                prefix_count = [prefix_count; query_average_count(ms,:)];
                                                prefix_reg = [prefix_reg; [(ms+q_ii-1) queryID queryView]];

                                                for i=1:numIM
                                                    if(~isempty(query_average_support_all{i,1}))
                                                        prefix_support_all{i,1} = [prefix_support_all{i,1}; query_average_support_all{i,1}(ms,1)];
                                                    else
                                                        prefix_support_all{i,1} = [prefix_support_all{i,1}; cell(0,0)];
                                                    end
                                                end
                                                continue;

                                            elseif(next_pt==num_seed_overlap_samples && outside_final_range)
                                                suffix_curve = [suffix_curve; curSample];
                                                suffix_count = [suffix_count; query_average_count(ms,:)];
                                                suffix_reg = [suffix_reg; [(ms+q_ii-1) queryID queryView]];

                                                for i=1:numIM
                                                    if(~isempty(query_average_support_all{i,1}))
                                                        suffix_support_all{i,1} = [suffix_support_all{i,1}; query_average_support_all{i,1}(ms,1)];
                                                    else
                                                        suffix_support_all{i,1} = [suffix_support_all{i,1}; cell(0,0)];
                                                    end
                                                end
                                                continue;

                                            end

                                            oc_size = size(merged_overlap_curve_final,1);
                                            next_ind = index_overlap_curve(next_pt,1);
                                            merged_overlap_curve_final = [merged_overlap_curve_final(1:(next_ind-1),:); curSample; merged_overlap_curve_final(next_ind:oc_size,:)];
                                            merged_overlap_count_final = [merged_overlap_count_final(1:(next_ind-1),1); query_average_count(ms,1); merged_overlap_count_final(next_ind:oc_size,:)];
                                            reg_curve_final = [reg_curve_final(1:(next_ind-1),:); [(ms+q_ii-1) queryID queryView]; reg_curve_final(next_ind:oc_size,:)];

                                            for i=1:numIM
                                                if(~isempty(query_average_support_all{i,1}) && ~isempty(merged_overlap_support_all_final{i,1}))
                                                    merged_overlap_support_all_final{i,1} = [merged_overlap_support_all_final{i,1}(1:(next_ind-1),1); query_average_support_all{i,1}(ms,1); merged_overlap_support_all_final{i,1}(next_ind:oc_size,:)];
                                                else
                                                    merged_overlap_support_all_final{i,1} = [merged_overlap_support_all_final{i,1}(1:(next_ind-1),1); cell(0,0); merged_overlap_support_all_final{i,1}(next_ind:oc_size,:)];
                                                end
                                            end
                                            index_overlap_curve(next_pt:num_seed_overlap_samples,1)=index_overlap_curve(next_pt:num_seed_overlap_samples,1)+1;
                                        end

                                        %Append the prefix and suffix curve
                                        merged_overlap_curve_final = [prefix_curve; merged_overlap_curve_final; suffix_curve];
                                        merged_overlap_count_final = [prefix_count; merged_overlap_count_final; suffix_count];
                                        reg_curve_final = [prefix_reg; reg_curve_final; suffix_reg];

                                        for i=1:numIM
                                            merged_overlap_support_all_final{i,1} = [prefix_support_all{i,1}; merged_overlap_support_all_final{i,1}; suffix_support_all{i,1}];
                                        end

                                    end



                                    %Find the closest points on the query curve
                                    merg_init_size = size(merged_overlap_curve_init,1);
                                    merg_final_size = size(merged_overlap_curve_final,1);
                                    [closest_pt,outside_init_range,outside_final_range,prev_pt,q_mi] = find_closest_point_on_curve(query_curve,merged_overlap_curve_init(merg_init_size,:));
                                    [closest_pt,outside_init_range,outside_final_range,q_mf,next_pt] = find_closest_point_on_curve(query_curve,merged_overlap_curve_final(1,:));
                                    [closest_pt,outside_init_range,outside_final_range,prev_pt,s_mi] = find_closest_point_on_curve(seed_curve,merged_overlap_curve_final(merg_final_size,:));
                                    [closest_pt,outside_init_range,outside_final_range,s_mf,next_pt] = find_closest_point_on_curve(seed_curve,merged_overlap_curve_init(1,:));


                                    %Stitch the middle portion of query curve
                                    %to merged overlap curves
                                    merged_final_curve = [merged_overlap_curve_init; query_curve(q_mi:q_mf,:); merged_overlap_curve_final];
                                    merged_final_count = [merged_overlap_count_init; ones(q_mf-q_mi+1,1); merged_overlap_count_final];
                                    reg_curve = [reg_curve_init; [(q_mi:q_mf)' queryID.*ones(q_mf-q_mi+1,1) queryView.*ones(q_mf-q_mi+1,1)]; reg_curve_final];

                                    merged_final_support_all = merged_overlap_support_all;

                                    for i=1:numIM
                                        if(~isempty(query_support_all{i,1}))
                                            merged_final_support_all{i,1} = [merged_overlap_support_all_init{i,1}; query_support_all{i,1}(q_mi:q_mf,1); merged_overlap_support_all_final{i,1}];
                                        else
                                            merged_final_support_all{i,1} = [merged_overlap_support_all_init{i,1}; cell(q_mf-q_mi+1,1); merged_overlap_support_all_final{i,1}];
                                        end
                                    end


                                    %Stitch the middle portion of seed 
                                    %curve to merged final overlap curve
                                    merged_final_curve = [merged_final_curve; seed_curve(s_mi:s_mf,:)];
                                    merged_final_count = [merged_final_count; seed_count(s_mi:s_mf,1)];
                                    reg_curve = [reg_curve; seed_reg_curve(s_mi:s_mf,:)];

                                    for i=1:numIM
                                        if(~isempty(seed_support_all{i,1}))
                                            merged_final_support_all{i,1} = [merged_final_support_all{i,1}; seed_support_all{i,1}(s_mi:s_mf,1)];
                                        else
                                            merged_final_support_all{i,1} = [merged_final_support_all{i,1}; cell(s_mf-s_mi+1,1)];
                                        end
                                    end


                                    old_seed_curve = seed_curve;
                                    old_seed_count = seed_count;
                                    old_seed_support_all = seed_support_all;
                                    old_seed_reg_curve = seed_reg_curve;

                                    seed_curve_graph{br,1} = merged_final_curve;
                                    seed_count_graph{br,1} = merged_final_count;
                                    seed_support_graph{br,1} = merged_final_support_all;
                                    seed_reg_graph{br,1} = reg_curve;
                                    seedViews_graph{br,1} = seedViews;
                                    seedIDs_graph{br,1} = seedIDs;
                                    v



                                end
                            end

                        end

                        if(init_corresp_found && final_corresp_found)
                            break;
                        end

                        if(is_closed_curve_merging)
                            break;
                        end

                    end

                end   
                %cluster_flags{queryView+1,1}
                %cluster_flags{queryView+1,1} = ones(size(clusters{queryView+1,1}));
            else
                cluster_done = 1; 
            end
        end
        %cluster_done = 1;
    end

    %VISUALIZE

    n=8;

    curIM = imread(['./images/',num2str(n-1,'%08d'),'.jpg']);
    figure;imshow(curIM);
    set(figure(1),'Units','Normalized','OuterPosition',[0 0 1 1]);
    hold on;
    %draw_cons(['./curves_yuliang/',num2str(n-1,'%08d'),'.cemv'],[num2str(n-1,'%08d'),'.jpg'],0,-1);

    fid = fopen(['./calibration/',num2str(n-1,'%08d'),'.projmatrix']);
    curP = (fscanf(fid,'%f',[4 3]))';

    numSamples = size(old_seed_curve,1);
    reprojCurve = zeros(numSamples,2);

    for s=1:numSamples

        curSample = [(old_seed_curve(s,:))';1];
        imSample = curP*curSample;
        imSample = imSample./imSample(3,1);

        reprojCurve(s,1) = imSample(1,1);
        reprojCurve(s,2) = imSample(2,1);

        %text(reprojCurve(s,1)+1,reprojCurve(s,2)+1,num2str(seed_count(s,1)),'Color','r');

    end

    plot(reprojCurve(:,1)+1,reprojCurve(:,2)+1,'Color','r','LineWidth',1);
    % plot(reprojCurve(418,1)+1,reprojCurve(418,2)+1,'xr');
    % plot(reprojCurve(503,1)+1,reprojCurve(503,2)+1,'xr');

    %query_curve = all_recs{8,1}{1,clusters{8,1}(1,1)};

    numSamples = size(query_curve,1);
    reprojCurve = zeros(numSamples,2);

    for s=1:numSamples

        curSample = [(query_curve(s,:))';1];
        imSample = curP*curSample;
        imSample = imSample./imSample(3,1);

        reprojCurve(s,1) = imSample(1,1);
        reprojCurve(s,2) = imSample(2,1);

    end

    plot(reprojCurve(:,1)+1,reprojCurve(:,2)+1,'Color','b','LineWidth',1);


    %-------------------------------------------------------------------------

%     for cc = 1:size(clusters{11,1},2)
%         query_curve = all_recs{11,1}{1,clusters{11,1}(1,cc)};
%         write_curve_to_vrml(query_curve,[255 0 0],['./ply/red-',num2str(cc),'.ply']);
%     end
% 
%     %**
% 
%     for cc = 1:size(clusters{14,1},2)
%         query_curve = all_recs{14,1}{1,clusters{14,1}(1,cc)};
%         write_curve_to_vrml(query_curve,[0 0 255],['./ply/blue-',num2str(cc),'.ply']); 
%     end
% 
%     %**
% 
%     for cc = 1:size(clusters{7,1},2)
%         query_curve = all_recs{7,1}{1,clusters{7,1}(1,cc)};
%         write_curve_to_vrml(query_curve,[255 255 0],['./ply/yellow-',num2str(cc),'.ply']);  
%     end
% 
%     %**
% 
%     for cc = 1:size(clusters{8,1},2)
%         query_curve = all_recs{8,1}{1,clusters{8,1}(1,cc)};
%         write_curve_to_vrml(query_curve,[0 255 255],['./ply/cyan-',num2str(cc),'.ply']);   
%     end
%     %**
% 
%     for cc = 1:size(clusters{9,1},2)
%         query_curve = all_recs{9,1}{1,clusters{9,1}(1,cc)};
%         write_curve_to_vrml(query_curve,[255 0 255],['./ply/purple-',num2str(cc),'.ply']);
%     end

    %--------------------------------------------------------------------------

%     main_curve = seed_curve_graph{1,1};
% 
%     for scg=1:size(seed_curve_graph,1)
%         seed_curve = seed_curve_graph{scg,1};
%         numSamples = size(seed_curve,1);
%         reprojCurve = zeros(numSamples,2);
% 
%         for s=1:numSamples
% 
%             curSample = [(seed_curve(s,:))';1];
%             imSample = curP*curSample;
%             imSample = imSample./imSample(3,1);
% 
%             reprojCurve(s,1) = imSample(1,1);
%             reprojCurve(s,2) = imSample(2,1);
% 
%             %text(reprojCurve(s,1)+1,reprojCurve(s,2)+1,num2str(seed_count(s,1)),'Color','w');
% 
%         end
% 
%         if(scg>1)
%             imSample = curP*([stitch_point_graph{scg,1}';1]);
%             imSample = imSample./imSample(3,1);
%             if(is_init_graph{scg,1})
%                 reprojCurve = [reprojCurve; imSample(1:2,1)'];
%                 %write_curve_to_vrml([seed_curve_graph{scg,1}; stitch_point_graph{scg,1}],[0 255 0],['./ply/green-',num2str(scg),'.ply']);
%             else
%                 reprojCurve = [imSample(1:2,1)'; reprojCurve];
%                 %write_curve_to_vrml([stitch_point_graph{scg,1}; seed_curve_graph{scg,1}],[0 255 0],['./ply/green-',num2str(scg),'.ply']);
%             end
%         else
%             %write_curve_to_vrml(main_curve,[0 255 0],['./ply/green-',num2str(scg),'.ply']);
%         end
% 
%         plot(reprojCurve(:,1)+1,reprojCurve(:,2)+1,'Color','g','LineWidth',1);
% 
%     end
    
    
    for scg=1:size(seed_curve_graph,1)
        seed_curve = seed_curve_graph{scg,1};
        write_curve_to_vrml(seed_curve,[0 255 0],['./ply/green_cl-',num2str(cl),'_br-',num2str(scg),'.ply']);
    end
%     write_curve_and_correspondence_to_vrml(old_seed_curve,[255 0 0],'./ply/red.ply',seed_subset,seed_closest);
%     write_curve_and_correspondence_to_vrml(query_curve,[0 0 255],'./ply/blue.ply',query_subset,query_closest);

    % write_curve_to_vrml(main_curve,[255 255 0],'yellow.ply');







    % figure;
    % hold on;
    % 
    % seed_curve = seed_curve_graph{1,1};
    % 
    % old_seed_size = size(old_seed_curve,1);
    % query_size = size(query_curve,1);
    % seed_average_size = size(seed_average_curve,1);
    % query_average_size = size(query_average_curve,1);
    % seed_size = size(seed_curve,1);
    % 
    % plot3(old_seed_curve(1:35,1),old_seed_curve(1:35,2),old_seed_curve(1:35,3),'Color','r');
    % plot3(query_curve(30:query_size-50,1),query_curve(30:query_size-50,2),query_curve(30:query_size-50,3),'Color','b');
    % plot3(seed_curve(35:95,1),seed_curve(35:95,2),seed_curve(35:95,3),'Color','g');
    % scatter3(seed_average_curve(1:seed_average_size-50,1),seed_average_curve(1:seed_average_size-50,2),seed_average_curve(1:seed_average_size-50,3),30,'r','d');
    % scatter3(query_average_curve(1:query_average_size-50,1),query_average_curve(1:query_average_size-50,2),query_average_curve(1:query_average_size-50,3),30,'b','d');































    % numSamples = size(seed_average_curve,1);
    % reprojCurve = zeros(numSamples,2);
    % 
    % for s=1:numSamples
    % 
    %     curSample = [(seed_average_curve(s,:))';1];
    %     imSample = curP*curSample;
    %     imSample = imSample./imSample(3,1);
    % 
    %     reprojCurve(s,1) = imSample(1,1);
    %     reprojCurve(s,2) = imSample(2,1);
    %     
    %     %text(reprojCurve(s,1)+1,reprojCurve(s,2)+1,num2str(s),'Color','m');
    % 
    % end
    % 
    % reprojCurve = [[208.2 48.14]; reprojCurve];
    % 
    % plot(reprojCurve(:,1)+1,reprojCurve(:,2)+1,'Color','y','LineWidth',1);

    %----------------------------------------------------------------------------

    % numSamples = size(query_average_curve,1);
    % reprojCurve = zeros(numSamples,2);
    % 
    % for s=1:numSamples
    % 
    %     curSample = [(query_average_curve(s,:))';1];
    %     imSample = curP*curSample;
    %     imSample = imSample./imSample(3,1);
    % 
    %     reprojCurve(s,1) = imSample(1,1);
    %     reprojCurve(s,2) = imSample(2,1);
    % 
    %     %text(reprojCurve(s,1)+1,reprojCurve(s,2)+1,num2str(s),'Color','w');
    % end
    % 
    % plot(reprojCurve(:,1)+1,reprojCurve(:,2)+1,'dm');
    % 
    % numSamples = size(seed_average_curve,1);
    % reprojCurve = zeros(numSamples,2);
    % 
    % for s=1:numSamples
    % 
    %     curSample = [(seed_average_curve(s,:))';1];
    %     imSample = curP*curSample;
    %     imSample = imSample./imSample(3,1);
    % 
    %     reprojCurve(s,1) = imSample(1,1);
    %     reprojCurve(s,2) = imSample(2,1);
    %     
    %     %text(reprojCurve(s,1)+1,reprojCurve(s,2)+1,num2str(s),'Color','w');
    % 
    % end
    % 
    % plot(reprojCurve(:,1)+1,reprojCurve(:,2)+1,'dy');
end
