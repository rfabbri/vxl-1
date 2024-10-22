%disp('COMPUTING ASSOCIATIONS');
                    allEdges = [];
                    fileNames = dir('/media/biao/ubuntuSSD/datasets/pavilion-midday-half/*.projmatrix');
                    for v=1:numIM

                        edge_support = [];
                        in_first_anchor = 0;

                        if(v==first_anchor+1)

                            %The first anchor edge support is measured using
                            %edges grouped into the hypothesis image curve

                            edge_support = all_inverse_links_3d{v,1}{queryID,1};
                            in_first_anchor = 1;
                        else
                            edge_support = querySupport{v,1}; 
                        end

                        if(isempty(edge_support))
                            continue;
                        end

                        fileName = fileNames(v,1).name;
                        ploc = strfind(fileName,'.projmatrix');
                        viewName = fileName(1,1:(ploc-1));
                        
                        fid = fopen(['/media/biao/ubuntuSSD/datasets/pavilion-midday-half/',viewName,'.projmatrix']);
                        curP = (fscanf(fid,'%f',[4 3]))';
                        fclose(fid);

                        for s=1:numSamples

                            edges = edge_support{s,1};

                            for e=1:size(edges,2)

                                edge = edges(1,e);

%                                 if(edge>=size(curveIndices{v,1},1))
%                                     disp('WARNING: Weird edge ID!!!');
%                                     continue;
%                                 end

                                if(~in_first_anchor)

                                    cur_sample = [queryCurve(s,:)';1];
                                    imSample = curP*cur_sample;
                                    imSample = imSample./imSample(3,1);
                                    im_pixel = imSample(1:2,1)+1;

                                    edg = all_edges{v,1};
                                    cur_edge = edg(edge+1,1:2)';
                                    edge_dist = norm(cur_edge - im_pixel);

                                    if(edge_dist>1)
                                        continue;
                                    end

                                end

                                cur_edge_links = all_edge_links{v,1}{edge+1,1};

                                for el=1:size(cur_edge_links,1)
                                    cur_link = cur_edge_links(el,:);

                                    if(cur_link(1,1)==first_anchor+1)
                                        continue;
                                    end

%                                     clusters{cur_link(1,1),1} = unique([clusters{cur_link(1,1),1} cur_link(1,2)]);
%                                     cur_cluster_size = size(clusters{cur_link(1,1),1},2);
%                                     cur_flag_size = size(process_flags{cur_link(1,1),1},2);
%                                     if(cur_cluster_size > cur_flag_size)
%                                         cluster_index = find(clusters{cur_link(1,1),1}==cur_link(1,2));
%                                         if(size(cluster_index,1)>1)
%                                             disp('ERROR: There is duplication in the cluster IDs!!');
%                                         end
%                                         if(cluster_index==1)
%                                             process_flags{cur_link(1,1),1} = [0 process_flags{cur_link(1,1),1}];
%                                             corr_native{cur_link(1,1),1} = [cell(1,1) corr_native{cur_link(1,1),1}];
%                                             corr_other{cur_link(1,1),1} = [cell(1,1) corr_other{cur_link(1,1),1}];
%                                             corr_colors{cur_link(1,1),1} = [cell(1,1) corr_colors{cur_link(1,1),1}];
%                                             %alignment_curves{cur_link(1,1),1} = [cell(1,1) alignment_curves{cur_link(1,1),1}];
%                                         elseif(cluster_index>cur_flag_size)
%                                             process_flags{cur_link(1,1),1} = [process_flags{cur_link(1,1),1} 0];
%                                             corr_native{cur_link(1,1),1} = [corr_native{cur_link(1,1),1} cell(1,1)];
%                                             corr_other{cur_link(1,1),1} = [corr_other{cur_link(1,1),1} cell(1,1)];
%                                             corr_colors{cur_link(1,1),1} = [corr_colors{cur_link(1,1),1} cell(1,1)];
%                                             %alignment_curves{cur_link(1,1),1} = [alignment_curves{cur_link(1,1),1} cell(1,1)];
%                                         else
%                                             process_flags{cur_link(1,1),1} = [process_flags{cur_link(1,1),1}(1,1:cluster_index-1) 0 process_flags{cur_link(1,1),1}(1,cluster_index:cur_flag_size)];
%                                             corr_native{cur_link(1,1),1} = [corr_native{cur_link(1,1),1}(1,1:cluster_index-1) cell(1,1) corr_native{cur_link(1,1),1}(1,cluster_index:cur_flag_size)];
%                                             corr_other{cur_link(1,1),1} = [corr_other{cur_link(1,1),1}(1,1:cluster_index-1) cell(1,1) corr_other{cur_link(1,1),1}(1,cluster_index:cur_flag_size)];
%                                             corr_colors{cur_link(1,1),1} = [corr_colors{cur_link(1,1),1}(1,1:cluster_index-1) cell(1,1) corr_colors{cur_link(1,1),1}(1,cluster_index:cur_flag_size)];
%                                             %alignment_curves{cur_link(1,1),1} = [alignment_curves{cur_link(1,1),1}(1,1:cluster_index-1) cell(1,1) alignment_curves{cur_link(1,1),1}(1,cluster_index:cur_flag_size)];
%                                         end
%                                     end

                                    alignmentBefore = queryAlignment{s,1};

                                    if(~isempty(alignmentBefore))

                                        reg_index = find(alignmentBefore(:,1)==cur_link(1,1) & alignmentBefore(:,2)==cur_link(1,2) & alignmentBefore(:,3)==cur_link(1,3));

                                        if(size(reg_index,1)>1)
                                            disp('ERROR: Duplicate copies of correspondences were added. Votes will be wrong!!');
                                        elseif(size(reg_index,1)==1)
                                            %If found, increase vote by 1

                                            %Don't do this if narrow baseline
                                            prev_views = queryAlignment_views{s,1}{reg_index,1};
                                            %reg_index

                                            if(isempty(prev_views))
                                                disp('ERROR: Even though alignment curve is not empty, view data is empty!!');
                                                continue;
                                            end

                                            diff_views = abs(prev_views - v);

                                            if(min(diff_views)<1)
                                                %disp('ERROR: Single view contributing more than 1 vote per link!!');
                                                continue;
    %                                         elseif(min(diff_views)==1)
    %                                             disp('Skipping narrow baseline evidence.');
    %                                             continue;
                                            end

                                            alignmentBefore(reg_index,4) = alignmentBefore(reg_index,4)+1;
                                            queryAlignment{s,1} = alignmentBefore;

                                            prev_views = [prev_views v];
                                            queryAlignment_views{s,1}{reg_index,1} = prev_views;
                                        else                                        
                                            queryAlignment{s,1} = [queryAlignment{s,1}; [cur_link 1]];
                                            queryAlignment_views{s,1} = [queryAlignment_views{s,1}; cell(1,1)];

                                            num_cells = size(queryAlignment_views{s,1},1);
                                            queryAlignment_views{s,1}{num_cells,1} = v;
                                        end

                                    else
                                        queryAlignment{s,1} = [cur_link 1];
                                        queryAlignment_views{s,1} = cell(1,1);
                                        queryAlignment_views{s,1}{1,1} = v;
                                    end

                                end

                            end
                        end
                    end