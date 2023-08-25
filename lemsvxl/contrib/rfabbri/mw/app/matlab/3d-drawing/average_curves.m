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
% load edge-curve-index_yuliang.mat;
% load edge-curve-offset_yuliang.mat;
% 
% %All the views that will be used in the clustering.
% %The first view is where the initial seed/query curve is located.
% all_views = [10 13 6 7 8];
% numViews = size(all_views,2);
% num_shared_edges = 1;
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
% 
% %Process the edge support evidence to map all edges to 3d curve samples
% %they support
% all_edges = cell(numIM,1);
% all_edge_links = cell(numIM,1);
% for v=1:numIM
%     v
%     [edg edgmap] = load_edg(['./edges/',num2str(v-1,'%08d'),'.edg']);
%     num_im_edges = size(edg,1);
%     all_edge_links{v,1} = cell(num_im_edges,1);
%     all_edges{v,1} = edg;
% end
% 
% %Process all the reconstruction links to add all the first anchor edges to
% %the edge link table
% 
% %Also build a map for 3D curve -> Set of 2D edges
% all_inverse_links_3d = cell(numIM,1);
% 
% for vv=1:numViews
%     
%     vv
%     vview = all_views(1,vv)+1;
%     links_3d = all_links_3d{vview,1};
%     offsets_3d = all_offsets_3d{vview,1};
%     
%     all_inverse_links_3d{vview,1} = cell(all_nR(vview,1),1);
%     
%     cons = read_cons(['./curves_yuliang/',num2str(all_views(1,vv),'%08d'),'.cemv'],[num2str(all_views(1,vv),'%08d'),'.jpg'],0,-1);
%     num_im_contours = size(cons,2);
%     
%     %Load the image curve-edge links for this view
%     fid = fopen(['./curve-edge_yuliang/',num2str(all_views(1,vv),'%08d'),'.txt']);
%     
%     for ic=1:num_im_contours
%         numCurEdges = fscanf(fid,'%d',[1 1]);
%         edgeIDs = fscanf(fid,'%d',[1 numCurEdges]); 
%         
%         cur_curves = links_3d{ic,1}+1;
%         cur_offsets = offsets_3d{ic,1};
%         
%         for c=1:size(cur_curves,2)
%            curveID = cur_curves(1,c); 
%            cur_curve = all_recs{vview,1}{1,curveID}; 
%            cur_size = size(cur_curve,1);
%            cur_offset = cur_offsets(1,c);
%            
%            %Fill the inverse maps
%            all_inverse_links_3d{vview,1}{curveID,1} = cell(cur_size,1);
%            
%            for s=1:cur_size
%                cur_edge = edgeIDs(1,s+cur_offset);
%                all_inverse_links_3d{vview,1}{curveID,1}{s,1} = cur_edge;
%                all_edge_links{vview,1}{cur_edge+1,1} = [all_edge_links{vview,1}{cur_edge+1,1}; [vview curveID s]];
%            end
%            
%         end
%         
%     end
%     
%     fclose(fid);
% end
% 
% %Process all the edge support data of each curve to fill in the edge link
% %table for all non-anchor views
% for vv=1:numViews
%     vv
%     vview = all_views(1,vv)+1;
%     other_views = [];
%     for ov=1:numViews
%         if(ov~=vv)
%             other_views = [other_views all_views(1,ov)];
%         end
%     end
%     
%     edge_support_3d = all_edge_support_3d{vview,1};
%     if(isempty(edge_support_3d))
%         continue;
%     end
%     
%     for crv = 1:all_nR(vview,1)      
%         queryCurve = all_recs{vview,1}{1,crv}; 
%         querySupport = edge_support_3d{crv,1};
%         numSamples = size(queryCurve,1);
%         
%         for v=1:numIM
%             
%             if(v==vview)
%                 continue;
%             end
% 
%             edge_support = querySupport{v,1};
%             edg = all_edges{v,1};
% 
%             fid = fopen(['./calibration/',num2str(v-1,'%08d'),'.projmatrix']);
%             curP = (fscanf(fid,'%f',[4 3]))';
%             fclose(fid);
%             
%             for s=1:numSamples
%                 
%                 if(isempty(edge_support))
%                     continue;
%                 end
%                 
%                 edges = edge_support{s,1};
% 
%                 for e=1:size(edges,2)
% 
%                     edge = edges(1,e);     
%                     
%                     cur_sample = [queryCurve(s,:)';1];
%                     imSample = curP*cur_sample;
%                     imSample = imSample./imSample(3,1);
%                     im_pixel = imSample(1:2,1)+1;
%                     cur_edge = edg(edge+1,1:2)';
%                     edge_dist = norm(cur_edge - im_pixel);
%                     
%                     if(edge_dist<=1)
%                         all_edge_links{v,1}{edge+1,1} = [all_edge_links{v,1}{edge+1,1}; [vview crv s]];
%                     end
%                     
%                 end
%             end
%             
%         end
%         
%     end
% end

clear all;
close all;
load intermediate.mat;
all_clusters = cell(0,0);
num_shared_edges = 3;

%As curves are modified, the modified versions go here.
all_recs_iter = all_recs;

weird_edges = [];
counter = 0;

%Form the links between corresponding 3D curve samples
for crv = 1:all_nR(11,1)
    crv
    
    %Zero out any flag that did not reach the minimum number of votes
    %required
    
    for v=1:numIM
       if(isempty(all_flags{v,1}))
           continue;
       end
       num_curves = size(all_flags{v,1},1);
       for c=1:num_curves
          if(all_flags{v,1}(c,1)<num_shared_edges)
              all_flags{v,1}(c,1)=0;
          end
       end
    end
    
    if(all_flags{11,1}(crv,1)>=num_shared_edges)
        continue;
    end
        
    clusters = cell(numIM,1);
    process_flags = cell(numIM,1);

    %The ID of the initial seed/query curve where clustering will begin
    clusters{11,1} = crv;
    process_flags{11,1} = 0;
    
    alignment_curves = cell(numIM,1);
    alignment_curves{11,1} = cell(1,1);
    
    corr_native = cell(numIM,1);
    corr_other = cell(numIM,1);
    corr_colors = cell(numIM,1);
    
    corr_native{11,1} = cell(1,1);
    corr_other{11,1} = cell(1,1);
    corr_colors{11,1} = cell(1,1);

    all_flags{11,1}(crv,1) = num_shared_edges;
    keep_processing = 1;

    while(keep_processing)

        for av=1:numViews
            
            av

            first_anchor = all_views(1,av);

            other_views = [];
            for ov=1:numViews
                if(ov~=av)
                    other_views = [other_views all_views(1,ov)];
                end
            end

            edge_support_3d = all_edge_support_3d{first_anchor+1,1};
            cur_cluster = clusters{first_anchor+1,1};
            cur_flags = process_flags{first_anchor+1,1};
            c_size = size(cur_cluster,2);

            for cc=1:c_size

                queryID = cur_cluster(1,cc); 
                
                if(cur_flags(1,cc))
                    continue;
                end
                
                cur_flags(1,cc) = 1;
                process_flags{first_anchor+1,1} = cur_flags;
                
                previous_clusters = clusters;
                previous_flags = process_flags;
                
                previous_corr_native = corr_native;
                previous_corr_other = corr_other;
                previous_corr_colors = corr_colors;               

                queryCurve = all_recs{first_anchor+1,1}{1,queryID}; 
                querySupport = edge_support_3d{queryID,1};
                numSamples = size(queryCurve,1);

                %Alignment curve
                %viewid-curveid-sampleid-votes
                queryAlignment = cell(numSamples,1);
                
                %Views used in the alignment curve
                %We use this to eliminate votes from narrow baseline views
                queryAlignment_views = cell(numSamples,1);

                %disp('COMPUTING ASSOCIATIONS');
                allEdges = [];
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
                    
                    fid = fopen(['./calibration/',num2str(v-1,'%08d'),'.projmatrix']);
                    curP = (fscanf(fid,'%f',[4 3]))';
                    fclose(fid);

                    for s=1:numSamples

                        edges = edge_support{s,1};

                        for e=1:size(edges,2)

                            edge = edges(1,e);

                            if(edge>=size(curveIndices{v,1},1))
                                disp('WARNING: Weird edge ID!!!');
                                continue;
                            end
                            
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
                                
                                clusters{cur_link(1,1),1} = unique([clusters{cur_link(1,1),1} cur_link(1,2)]);
                                cur_cluster_size = size(clusters{cur_link(1,1),1},2);
                                cur_flag_size = size(process_flags{cur_link(1,1),1},2);
                                if(cur_cluster_size > cur_flag_size)
                                    cluster_index = find(clusters{cur_link(1,1),1}==cur_link(1,2));
                                    if(size(cluster_index,1)>1)
                                        disp('ERROR: There is duplication in the cluster IDs!!');
                                    end
                                    if(cluster_index==1)
                                        process_flags{cur_link(1,1),1} = [0 process_flags{cur_link(1,1),1}];
                                        corr_native{cur_link(1,1),1} = [cell(1,1) corr_native{cur_link(1,1),1}];
                                        corr_other{cur_link(1,1),1} = [cell(1,1) corr_other{cur_link(1,1),1}];
                                        corr_colors{cur_link(1,1),1} = [cell(1,1) corr_colors{cur_link(1,1),1}];
                                        %alignment_curves{cur_link(1,1),1} = [cell(1,1) alignment_curves{cur_link(1,1),1}];
                                    elseif(cluster_index>cur_flag_size)
                                        process_flags{cur_link(1,1),1} = [process_flags{cur_link(1,1),1} 0];
                                        corr_native{cur_link(1,1),1} = [corr_native{cur_link(1,1),1} cell(1,1)];
                                        corr_other{cur_link(1,1),1} = [corr_other{cur_link(1,1),1} cell(1,1)];
                                        corr_colors{cur_link(1,1),1} = [corr_colors{cur_link(1,1),1} cell(1,1)];
                                        %alignment_curves{cur_link(1,1),1} = [alignment_curves{cur_link(1,1),1} cell(1,1)];
                                    else
                                        process_flags{cur_link(1,1),1} = [process_flags{cur_link(1,1),1}(1,1:cluster_index-1) 0 process_flags{cur_link(1,1),1}(1,cluster_index:cur_flag_size)];
                                        corr_native{cur_link(1,1),1} = [corr_native{cur_link(1,1),1}(1,1:cluster_index-1) cell(1,1) corr_native{cur_link(1,1),1}(1,cluster_index:cur_flag_size)];
                                        corr_other{cur_link(1,1),1} = [corr_other{cur_link(1,1),1}(1,1:cluster_index-1) cell(1,1) corr_other{cur_link(1,1),1}(1,cluster_index:cur_flag_size)];
                                        corr_colors{cur_link(1,1),1} = [corr_colors{cur_link(1,1),1}(1,1:cluster_index-1) cell(1,1) corr_colors{cur_link(1,1),1}(1,cluster_index:cur_flag_size)];
                                        %alignment_curves{cur_link(1,1),1} = [alignment_curves{cur_link(1,1),1}(1,1:cluster_index-1) cell(1,1) alignment_curves{cur_link(1,1),1}(1,cluster_index:cur_flag_size)];
                                    end
                                end
                                
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
                                        
                                        if(min(diff_views)<2)
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
                
                queryAlignment_dense = cell(size(queryAlignment,1),1);
                
                %Here we filter correspondences to throw out unreliable
                %ones
                for ss=1:size(queryAlignment,1)
                   cur_alignment = queryAlignment{ss,1};
                   filtered_alignment = [];
                   filtered_alignment_dense = [];
                   for a=1:size(cur_alignment,1)
                       
                       if(cur_alignment(a,4)>2)
                           filtered_alignment = [filtered_alignment; cur_alignment(a,:)];
                       end
                       
                       if(cur_alignment(a,4)>1)
                           filtered_alignment_dense = [filtered_alignment_dense; cur_alignment(a,:)];
                       end
                       
                   end
                   queryAlignment{ss,1} = filtered_alignment;
                   queryAlignment_dense{ss,1} = filtered_alignment_dense;
                end

                new_clusters = previous_clusters;
                new_process_flags = previous_flags;
                
                new_corr_native = previous_corr_native;
                new_corr_other = previous_corr_other;
                new_corr_colors = previous_corr_colors;
                
                %We process the reliable correspondences to do clustering &
                %averaging
                for ss=1:size(queryAlignment,1)
                   cur_alignment = queryAlignment{ss,1};
                   for a=1:size(cur_alignment,1)
                        cur_corr = cur_alignment(a,:);
                      
                        corr_view = cur_corr(1,1);
                        corr_curve = cur_corr(1,2);
                        corr_sample = cur_corr(1,3);
                        corr_votes = cur_corr(1,4);
                        
                        %This correspondence votes on the respective curve
                        all_flags{corr_view,1}(corr_curve,1) = all_flags{corr_view,1}(corr_curve,1)+1;
                        
                        %If we have received the minimum amount of votes
                        %required, add the curve to the cluster
                        if(all_flags{corr_view,1}(corr_curve,1)<num_shared_edges)
                            continue;
                        end
                        
                        new_clusters{corr_view,1} = unique([new_clusters{corr_view,1} corr_curve]);
                        cur_cluster_size = size(new_clusters{corr_view,1},2);
                        cur_flag_size = size(new_process_flags{corr_view,1},2);
                        if(cur_cluster_size > cur_flag_size)
                            
                            if(~isempty(all_flags{corr_view,1}))
                                all_flags{corr_view,1}(corr_curve,1) = num_shared_edges;
                            end
                            
                            cluster_index = find(new_clusters{corr_view,1}==corr_curve);
                            if(size(cluster_index,1)>1)
                                disp('ERROR: There is duplication in the cluster IDs!!');
                            end
                            flag_index = find(clusters{corr_view,1}==corr_curve);
                            if(size(flag_index,1)>1)
                                disp('ERROR: There is duplication in the cluster IDs!!');
                            end
                            
                            if(cluster_index==1)
                                new_process_flags{corr_view,1} = [process_flags{corr_view,1}(1,flag_index) new_process_flags{corr_view,1}];
                                new_corr_native{corr_view,1} = [cell(1,1) new_corr_native{corr_view,1}];
                                new_corr_other{corr_view,1} = [cell(1,1) new_corr_other{corr_view,1}];
                                new_corr_colors{corr_view,1} = [cell(1,1) new_corr_colors{corr_view,1}];
                                alignment_curves{corr_view,1} = [cell(1,1) alignment_curves{corr_view,1}];
                            elseif(cluster_index>cur_flag_size)
                                new_process_flags{corr_view,1} = [new_process_flags{corr_view,1} process_flags{corr_view,1}(1,flag_index)];
                                new_corr_native{corr_view,1} = [new_corr_native{corr_view,1} cell(1,1)];
                                new_corr_other{corr_view,1} = [new_corr_other{corr_view,1} cell(1,1)];
                                new_corr_colors{corr_view,1} = [new_corr_colors{corr_view,1} cell(1,1)];
                                alignment_curves{corr_view,1} = [alignment_curves{corr_view,1} cell(1,1)];
                            else
                                new_process_flags{corr_view,1} = [new_process_flags{corr_view,1}(1,1:cluster_index-1) process_flags{corr_view,1}(1,flag_index) new_process_flags{corr_view,1}(1,cluster_index:cur_flag_size)];
                                new_corr_native{corr_view,1} = [new_corr_native{corr_view,1}(1,1:cluster_index-1) cell(1,1) new_corr_native{corr_view,1}(1,cluster_index:cur_flag_size)];
                                new_corr_other{corr_view,1} = [new_corr_other{corr_view,1}(1,1:cluster_index-1) cell(1,1) new_corr_other{corr_view,1}(1,cluster_index:cur_flag_size)];
                                new_corr_colors{corr_view,1} = [new_corr_colors{corr_view,1}(1,1:cluster_index-1) cell(1,1) new_corr_colors{corr_view,1}(1,cluster_index:cur_flag_size)];
                                alignment_curves{corr_view,1} = [alignment_curves{corr_view,1}(1,1:cluster_index-1) cell(1,1) alignment_curves{corr_view,1}(1,cluster_index:cur_flag_size)];
                            end
                            %new_process_flags{corr_view,1} = [new_process_flags{corr_view,1}(1,1:cluster_index-1) process_flags{corr_view,1}(1,flag_index) new_process_flags{cur_link(1,1),1}(1,cluster_index:cur_flag_size)];
                        end
                        
                   end
                end
                
                clusters = new_clusters;
                process_flags = new_process_flags;
                
                corr_native = new_corr_native;
                corr_other = new_corr_other;
                corr_colors = new_corr_colors; 
                
                %Now the output should be written to vrml files
                
                %First gather the corresponding point data
                vote_colors(1,:) = [1 0 1];
                vote_colors(2,:) = [0 1 0];
                vote_colors(3,:) = [0 0 1];
                vote_colors(4,:) = [1 1 0];
                vote_colors(5,:) = [0 1 1];
                
                for ss=1:size(queryAlignment_dense,1)
                   cur_alignment = queryAlignment_dense{ss,1};
                   for a=1:size(cur_alignment,1)
                      cur_corr = cur_alignment(a,:);
                      
                      corr_view = cur_corr(1,1);
                      corr_curve = cur_corr(1,2);
                      corr_sample = cur_corr(1,3);
                      corr_votes = cur_corr(1,4);
                      
                      corr_curve_3d = all_recs{corr_view,1}{1,corr_curve};
                      
                      %Find the index in the clusters
                      clust_ind = find(clusters{corr_view,1}==corr_curve);
                      
                      if(size(clust_ind,1)>1)
                          disp('ERROR: Cluster vectors contain duplicate IDs!!');
                      end
                      
                      if(isempty(clust_ind))
                         disp('Cluster candidate with insufficient votes is being ignored');
                         continue;
                      end
                      
                      corr_native{corr_view,1}{1,clust_ind} = [corr_native{corr_view,1}{1,clust_ind}; corr_curve_3d(corr_sample,:)];
                      corr_other{corr_view,1}{1,clust_ind} = [corr_other{corr_view,1}{1,clust_ind}; queryCurve(ss,:)];
                      
                      if(corr_votes>4)
                          color_index = 5;
                      elseif(corr_votes>3)
                          color_index = 4;
                      elseif(corr_votes>2)
                          color_index = 3;
                      elseif(corr_votes>1)
                          color_index = 2;
                      else
                          color_index = 1;
                      end
                      
                      corr_colors{corr_view,1}{1,clust_ind} = [corr_colors{corr_view,1}{1,clust_ind}; vote_colors(color_index,:)];
                      
                   end
                end
                
                alignment_curves{first_anchor+1,1}{1,cc} = queryAlignment_dense;
                
            end
        end  
        
        keep_processing = 0;
        
        % Check whether you need to keep processing
        for av=1:numViews
            cur_flags = process_flags{all_views(1,av)+1,1};
            if(any(~cur_flags))
                keep_processing = 1;
                break;
            end
        end

    end
    
    %All correspondence info has been computed, now we need to
    %do the averaging
    
    for av=1:numViews
        
        cur_clusters = clusters{all_views(1,av)+1,1};
        
        if(isempty(cur_clusters))
            continue;
        end
        
        num_curves_in_cluster = size(cur_clusters,2);
        
        for cc=1:num_curves_in_cluster
            
            curveID = cur_clusters(1,cc);
            queryCurve = all_recs{all_views(1,av)+1,1}{1,curveID};
            queryAlignment = alignment_curves{all_views(1,av)+1,1}{1,cc};
            numSamples = size(queryCurve,1);
            
            %Process the query alignment to create boolean correspondence
            %masks for each other curve in the cluster
            
            corresp_masks = cell(numIM,1);
            for vcrsp=1:numIM
                if(~isempty(clusters{vcrsp,1}))
                    corresp_masks{vcrsp,1} = zeros(numSamples,size(clusters{vcrsp,1},2));
                end
            end
            
            for ss=1:size(queryAlignment,1)
                
                cur_alignment = queryAlignment{ss,1};
                
                if(isempty(cur_alignment))
                    continue;
                end
                
                querySample = queryCurve(ss,:);
                sampleSum = querySample;
                sampleCount = 1;

                view_curve = unique(cur_alignment(:,1:2),'rows');
                num_corresps = size(view_curve,1);
                
                %List of [viewID curveID] that need to be updated
                update_map = zeros(num_corresps,2);

                for crsp=1:num_corresps

                    view = view_curve(crsp,1);
                    corresp_curveID = view_curve(crsp,2);
                    
                    clust_index = find(clusters{view,1}==corresp_curveID);
                    if(isempty(clust_index))
                        continue;
                    end
                    
                    corresp_masks{view,1}(ss,clust_index) = 1;
                    
                    update_map(crsp,:) = [view corresp_curveID];

                end
            end
            
            new_corresp_masks = corresp_masks;
            
            %Post process the correspondence masks to fill the gaps
            for vcl=1:numIM
                cur_view = corresp_masks{vcl,1};
                if(isempty(cur_view))
                    continue;
                end
                for cl=1:size(cur_view,2)
                    cur_mask = cur_view(:,cl);
                    new_mask = cur_mask;
                    for ss=3:numSamples-2
                        if(cur_mask(ss-1,1)==1 && cur_mask(ss+1,1)==1)
                            new_mask(ss,1) = 1;
                        elseif(cur_mask(ss-2,1)==1 && cur_mask(ss+2,1)==1)
                            new_mask(ss-1,1) = 1;
                            new_mask(ss,1) = 1;
                            new_mask(ss+1,1) = 1;
                        end
                    end
                    new_corresp_masks{vcl,1}(:,cl) = new_mask;
                end
            end
            
            corresp_masks = new_corresp_masks;
    
            %Process the masks to iterate curve samples
            queryCurve_next = zeros(numSamples,3);
            
            for ss=1:numSamples
                
                querySample = queryCurve(ss,:);
                sampleSum = querySample;
                sampleCount = 1;

                for vcl=1:numIM

                    cur_mask = corresp_masks{vcl,1};
                    
                    if(isempty(cur_mask))
                        continue;
                    end

                    for cl=1:size(cur_mask,2)
                        
                        if(~cur_mask(ss,cl))
                            continue;
                        end                    
                        
                        curve = all_recs_iter{vcl,1}{1,clusters{vcl,1}(1,cl)};
                        
                        if(size(curve,1)<2)
                            continue;
                        end

                        [closest_pt,outside_init_range,outside_final_range,prev_pt,next_pt] = find_closest_point_on_curve(curve,querySample);

                        sampleSum = sampleSum + closest_pt;
                        sampleCount = sampleCount + 1;

                    end
                        
                end
                newSample = sampleSum./sampleCount;
                queryCurve_next(ss,:) = newSample;
            end

%             for ss=1:size(queryAlignment,1)
%                 cur_alignment = queryAlignment{ss,1};
% 
%                 if(isempty(cur_alignment))
%                     queryCurve_next(ss,:) = queryCurve(ss,:);
%                     continue;
%                 end
% 
%                 querySample = queryCurve(ss,:);
%                 sampleSum = querySample;
%                 sampleCount = 1;
% 
%                 view_curve = unique(cur_alignment(:,1:2),'rows');
%                 num_corresps = size(view_curve,1);
%                 
%                 %List of [viewID curveID] that need to be updated
%                 update_map = zeros(num_corresps,2);
% 
%                 for crsp=1:num_corresps
% 
%                     view = view_curve(crsp,1);
%                     corresp_curveID = view_curve(crsp,2);
%                     curve = all_recs_iter{view,1}{1,corresp_curveID};
%                     
%                     clust_index = find(clusters{view,1}==corresp_curveID);
%                     if(isempty(clust_index))
%                         continue;
%                     end
% 
%                     [closest_pt,outside_init_range,outside_final_range,prev_pt,next_pt] = find_closest_point_on_curve(curve,querySample);
% 
%                     sampleSum = sampleSum + closest_pt;
%                     sampleCount = sampleCount + 1;
%                     
%                     update_map(crsp,:) = [view corresp_curveID];
% 
%                 end
% 
%                 newSample = sampleSum./sampleCount;
%                 queryCurve_next(ss,:) = newSample;
%                 
% %                 %Insert this new sample into the other curves
% %                 %TODO: In the future, do these after a distance check.
% %                 
% %                 for crsp=1:num_corresps
% %                     
% %                     view = update_map(crsp,1);
% %                     corresp_curveID = update_map(crsp,2);
% %                     curve = all_recs_iter{view,1}{1,corresp_curveID};
% %                     
% %                     [closest_pt,outside_init_range,outside_final_range,prev_pt,next_pt] = find_closest_point_on_curve(curve,newSample);
% % 
% %                     if((next_pt-prev_pt)~=1)
% %                         disp('ERROR: Insertion indices are wrong! Skipping insertion...');
% %                         continue;
% %                     end
% %                     
% %                     %Continue here...
% %                     
% %                 end
%                 
%             end
            
            write_curve_to_vrml(queryCurve_next,[255 0 0],['./ply/after/view',num2str(all_views(1,av)),'_curve',num2str(curveID),'.ply']);
            
        end
    
    end
    
    
    counter = size(all_clusters,1)+1;        
    for vv=1:numViews
        for cl=1:size(clusters{all_views(1,vv)+1,1},2)
            curveID = clusters{all_views(1,vv)+1,1}(1,cl);
            curve = all_recs{all_views(1,vv)+1,1}{1,curveID};
            %write_curve_to_vrml(curve,[255 255 255],['./ply/before/view',num2str(all_views(1,vv)),'_curve',num2str(curveID),'_cluster',num2str(counter),'.ply']);
            write_curve_and_correspondence_to_vrml(curve,[255 255 255],['./ply/before/view',num2str(all_views(1,vv)),'_curve',num2str(curveID),'_cluster',num2str(counter),'.ply'],corr_native{all_views(1,vv)+1,1}{1,cl},corr_other{all_views(1,vv)+1,1}{1,cl},255.*corr_colors{all_views(1,vv)+1,1}{1,cl});
            %counter = counter + 1;
        end
    end

    all_clusters = [all_clusters; cell(1,1)];
    all_clusters{size(all_clusters,1),1} = clusters;
end

% %VISUALIZE
% 
% n=15;
% 
% curIM = imread(['./images/',num2str(n-1,'%08d'),'.jpg']);
% figure;imshow(curIM);
% set(figure(1),'Units','Normalized','OuterPosition',[0 0 1 1]);
% hold on;
% %draw_cons(['./curves_yuliang/',num2str(n-1,'%08d'),'.cemv'],[num2str(n-1,'%08d'),'.jpg'],0,-1);
% 
% fid = fopen(['./calibration/',num2str(n-1,'%08d'),'.projmatrix']);
% curP = (fscanf(fid,'%f',[4 3]))';
% 
% view_colors = distinguishable_colors(numViews);
% 
% for vv=1:numViews
%     
%     vview = all_views(1,vv)+1;
%     num_curves = size(clusters{vview,1},2);
%     
%     for cc=1:num_curves
%         curveID = clusters{vview,1}(1,cc);
%         cur_curve = all_recs{vview,1}{1,curveID};
%         numSamples = size(cur_curve,1);
%         reprojCurve = zeros(numSamples,2);
% 
%         for s=1:numSamples
% 
%             curSample = [(cur_curve(s,:))';1];
%             imSample = curP*curSample;
%             imSample = imSample./imSample(3,1);
% 
%             reprojCurve(s,1) = imSample(1,1);
%             reprojCurve(s,2) = imSample(2,1);
%         end
% 
%         plot(reprojCurve(:,1)+1,reprojCurve(:,2)+1,'Color',view_colors(vv,:),'LineWidth',1);
%     end
% 
% end

