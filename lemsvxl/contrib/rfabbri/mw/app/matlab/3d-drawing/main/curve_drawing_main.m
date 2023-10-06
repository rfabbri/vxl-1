clear all;%OK
close all;%OK
definitions_12

%-------------------------------------------------------------------------------

colors = distinguishable_colors(numIM);
all_recs = cell(numIM,1);
all_nR = zeros(numIM,1);
all_links_3d = cell(numIM,1);
all_offsets_3d = cell(numIM,1);
all_edge_support_3d = cell(numIM,1);
all_flags = cell(numIM,1);
all_num_im_contours = zeros(numIM,1);

load_curve_sketch_without_associations

load edge-curve-index_yuliang_pavilion-mixed.mat;
load edge-curve-offset_yuliang_pavilion-mixed.mat;


load_edge_and_curve_files

% %Read all the edge support evidence
% disp('READING DATA');
% for v=1:numViews
% %for v=1:1
%     all_views(1,v)
%     cons = read_cons(['./curves/',num2str(all_views(1,v),'%02d'),'.cemv'],[num2str(all_views(1,v),'%02d'),'.jpg'],0,-1);
%     num_im_contours = size(cons,2);
%     [all_links_3d{all_views(1,v)+1,1}, all_offsets_3d{all_views(1,v)+1,1}, all_edge_support_3d{all_views(1,v)+1,1}] = read_association_attributes(all_views(1,v),num_im_contours,all_nR(all_views(1,v)+1,1),numIM);
%     all_num_im_contours(all_views(1,v)+1,1) = num_im_contours;
%     
% end

%Read all the edge support evidence
disp('READING DATA');
fileNames = dir('./curves/*.cemv');
for v=1:numViews
%for v=1:1
    all_views(1,v)
    fileName = fileNames(v,1).name;
    cloc = strfind(fileName,'.cemv');
    viewName = fileName(1,1:(cloc-1));
    cons = read_cons(['./curves/',viewName,'.cemv'],[viewName,'.jpg'],0,-1);
    num_im_contours = size(cons,2);
    [all_links_3d{all_views(1,v)+1,1}, all_offsets_3d{all_views(1,v)+1,1}, all_edge_support_3d{all_views(1,v)+1,1}] = read_association_attributes_12(all_views(1,v),num_im_contours,all_nR(all_views(1,v)+1,1),numIM);
    all_num_im_contours(all_views(1,v)+1,1) = num_im_contours;
    
end

%Process the edge support evidence to map all edges to 3d curve samples
%they support
all_edges = cell(numIM,1);
all_edge_links = cell(numIM,1);
fileNames = dir('./edges/*.edg');
for v=1:numIM
    v
    fileName = fileNames(v,1).name;
    eloc = strfind(fileName,'.edg');
    viewName = fileName(1,1:(eloc-1));
    [edg edgmap] = load_edg(['./edges/',viewName,'.edg']);
    num_im_edges = size(edg,1);
    all_edge_links{v,1} = cell(num_im_edges,1);
    all_edges{v,1} = edg;
end

%Process all the reconstruction links to add all the first anchor edges to
%the edge link table

%Also build a map for 3D curve -> Set of 2D edges
all_inverse_links_3d = cell(numIM,1);
fileNames = dir('./curves/*.cemv');
for vv=1:numViews
    
    vv
    vview = all_views(1,vv)+1;
    links_3d = all_links_3d{vview,1};
    offsets_3d = all_offsets_3d{vview,1};
    
    fileName = fileNames(vv,1).name;
    cloc = strfind(fileName,'.cemv');
    viewName = fileName(1,1:(eloc-1));
    
    all_inverse_links_3d{vview,1} = cell(all_nR(vview,1),1);
    
    cons = read_cons(['./curves/',viewName,'.cemv'],[viewName,'.jpg'],0,-1);
    num_im_contours = size(cons,2);
    
    %Load the image curve-edge links for this view
    fid = fopen(['./curve-edge/',viewName,'.txt']);
    
    for ic=1:num_im_contours
        numCurEdges = fscanf(fid,'%d',[1 1]);
        edgeIDs = fscanf(fid,'%d',[1 numCurEdges]); 
        
        cur_curves = links_3d{ic,1}+1;
        cur_offsets = offsets_3d{ic,1};
        
        for c=1:size(cur_curves,2)
           curveID = cur_curves(1,c); 
           cur_curve = all_recs{vview,1}{1,curveID}; 
           cur_size = size(cur_curve,1);
           cur_offset = cur_offsets(1,c);
           
           %Fill the inverse maps
           all_inverse_links_3d{vview,1}{curveID,1} = cell(cur_size,1);
           
           for s=1:cur_size
               cur_edge = edgeIDs(1,s+cur_offset);
               all_inverse_links_3d{vview,1}{curveID,1}{s,1} = cur_edge;
               all_edge_links{vview,1}{cur_edge+1,1} = [all_edge_links{vview,1}{cur_edge+1,1}; [vview curveID s]];
           end
           
        end
        
    end
    
    fclose(fid);
end

%Process all the edge support data of each curve to fill in the edge link
%table for all non-anchor views
for vv=1:numViews
    vv
    vview = all_views(1,vv)+1;
    other_views = [];
    for ov=1:numViews
        if(ov~=vv)
            other_views = [other_views all_views(1,ov)];
        end
    end
    
    edge_support_3d = all_edge_support_3d{vview,1};
    if(isempty(edge_support_3d))
        continue;
    end
    
    for crv = 1:all_nR(vview,1)      
        queryCurve = all_recs{vview,1}{1,crv}; 
        querySupport = edge_support_3d{crv,1};
        numSamples = size(queryCurve,1);
        
        fileNames = dir('./calibration/*.projmatrix');
        
        for v=1:numIM
            
            if(v==vview)
                continue;
            end
            
            fileName = fileNames(v,1).name;
            ploc = strfind(fileName,'.projmatrix');
            viewName = fileName(1,1:(ploc-1));

            edge_support = querySupport{v,1};
            edg = all_edges{v,1};

            fid = fopen(['./calibration/',viewName,'.projmatrix']);
            curP = (fscanf(fid,'%f',[4 3]))';
            fclose(fid);
            
            for s=1:numSamples
                
                if(isempty(edge_support))
                    continue;
                end
                
                edges = edge_support{s,1};

                for e=1:size(edges,2)

                    edge = edges(1,e);     
                    
                    cur_sample = [queryCurve(s,:)';1];
                    imSample = curP*cur_sample;
                    imSample = imSample./imSample(3,1);
                    im_pixel = imSample(1:2,1)+1;
                    cur_edge = edg(edge+1,1:2)';
                    edge_dist = norm(cur_edge - im_pixel);
                    
                    if(edge_dist<=1)
                        all_edge_links{v,1}{edge+1,1} = [all_edge_links{v,1}{edge+1,1}; [vview crv s]];
                    end
                    
                end
            end
            
        end
        
    end
end

% clear all;
% close all;

save intermediate-pavilion-mixed-half_12.mat;
all_clusters = cell(0,0);
num_shared_edges = 3;
junction_dist_threshold = 0.01;
branch_length_threshold = 0.01;
sample_merge_threshold = 0.005;
sample_break_threshold = 3*sample_merge_threshold;

% %DEFAULTS
% all_clusters = cell(0,0);
% num_shared_edges = 3;
% junction_dist_threshold = 0.03;
% branch_length_threshold = 0.1;
% sample_dist_threshold = 0.01;

%As curves are modified, the modified versions go here.
all_recs_iter = all_recs;

weird_edges = [];
counter = 0;

num_colors = 0;

for colv=1:numIM
    num_colors = num_colors + all_nR(colv,1);
end

colors = distinguishable_colors(5000);
colors(1,:) = [];
color_counter = 0;

complete_curve_graph = [];
complete_lock_map = [];
all_junctions = [];

branch_size_threshold = 3;

%Form the links between corresponding 3D curve samples
for fa=1:numViews
    fa_view = all_views(1,fa)+1;
    for crv = 1:all_nR(fa_view,1)
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

        if(all_flags{fa_view,1}(crv,1)==num_shared_edges)
            continue;
        end
        
        color_counter = color_counter+1;

        clusters = cell(numIM,1);
        process_flags = cell(numIM,1);

        %The ID of the initial seed/query curve where clustering will begin
        clusters{fa_view,1} = crv;
        process_flags{fa_view,1} = 0;
        
        extra_junctions = [];

        alignment_curves = cell(numIM,1);
        alignment_curves{fa_view,1} = cell(1,1);

        corr_native = cell(numIM,1);
        corr_other = cell(numIM,1);
        corr_colors = cell(numIM,1);

        corr_native{fa_view,1} = cell(1,1);
        corr_other{fa_view,1} = cell(1,1);
        corr_colors{fa_view,1} = cell(1,1);

        all_flags{fa_view,1}(crv,1) = num_shared_edges;
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
                    fileNames = dir('./calibration/*.projmatrix');
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
                        
                        fid = fopen(['./calibration/',viewName,'.projmatrix']);
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

                            if(all_flags{corr_view,1}(corr_curve,1)==num_shared_edges)
                                continue;
                            end

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

    %                             if(~isempty(all_flags{corr_view,1}))
    %                                 all_flags{corr_view,1}(corr_curve,1) = num_shared_edges;
    %                             end

                                cluster_index = find(new_clusters{corr_view,1}==corr_curve);
                                if(size(cluster_index,1)>1)
                                    disp('ERROR: There is duplication in the cluster IDs!!');
                                end
%                                 flag_index = find(clusters{corr_view,1}==corr_curve);
%                                 if(size(flag_index,1)>1)
%                                     disp('ERROR: There is duplication in the cluster IDs!!');
%                                 end

                                if(cluster_index==1)
                                    %new_process_flags{corr_view,1} = [process_flags{corr_view,1}(1,flag_index) new_process_flags{corr_view,1}];
                                    new_process_flags{corr_view,1} = [0 new_process_flags{corr_view,1}];
                                    new_corr_native{corr_view,1} = [cell(1,1) new_corr_native{corr_view,1}];
                                    new_corr_other{corr_view,1} = [cell(1,1) new_corr_other{corr_view,1}];
                                    new_corr_colors{corr_view,1} = [cell(1,1) new_corr_colors{corr_view,1}];
                                    alignment_curves{corr_view,1} = [cell(1,1) alignment_curves{corr_view,1}];
                                elseif(cluster_index>cur_flag_size)
                                    new_process_flags{corr_view,1} = [new_process_flags{corr_view,1} 0];
                                    new_corr_native{corr_view,1} = [new_corr_native{corr_view,1} cell(1,1)];
                                    new_corr_other{corr_view,1} = [new_corr_other{corr_view,1} cell(1,1)];
                                    new_corr_colors{corr_view,1} = [new_corr_colors{corr_view,1} cell(1,1)];
                                    alignment_curves{corr_view,1} = [alignment_curves{corr_view,1} cell(1,1)];
                                else
                                    new_process_flags{corr_view,1} = [new_process_flags{corr_view,1}(1,1:cluster_index-1) 0 new_process_flags{corr_view,1}(1,cluster_index:cur_flag_size)];
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
                             %disp('Cluster candidate with insufficient votes is being ignored');
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

        averaging_iterations_done = 0;

        %Flags marking converged samples
        converge_flags = cell(numIM,1);

        for cv=1:numIM
            if(isempty(clusters{cv,1}))
                continue;
            end
            converge_flags{cv,1} = cell(1,size(clusters{cv,1},2));
            for ccrv=1:size(clusters{cv,1},2)
                cid = clusters{cv,1}(1,ccrv);
                curve = all_recs{cv,1}{1,cid};
                curve_size = size(curve,1);
                converge_flags{cv,1}{1,ccrv} = zeros(curve_size,1);
            end
        end

        %Process the query alignment to create boolean correspondence
        %masks for each other curve in the cluster

        all_corresp_masks = cell(numIM,1);
        for acm=1:numIM
            if(~isempty(clusters{acm,1}))
                num_clustered_curves = size(clusters{acm,1},2);
                all_corresp_masks{acm,1} = cell(1,num_clustered_curves);
            end
        end

        for av=1:numViews

            cur_clusters = clusters{all_views(1,av)+1,1};

            if(isempty(cur_clusters))
                continue;
            end

            num_curves_in_cluster = size(cur_clusters,2);

            for cc=1:num_curves_in_cluster

                curveID = cur_clusters(1,cc);
                queryCurve = all_recs_iter{all_views(1,av)+1,1}{1,curveID};
                queryAlignment = alignment_curves{all_views(1,av)+1,1}{1,cc};
                numSamples = size(queryCurve,1);

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
%                             if(cur_mask(ss-1,1)==1 && cur_mask(ss+1,1)==1)
%                                 new_mask(ss,1) = 1;
%                             elseif(cur_mask(ss-2,1)==1 && cur_mask(ss+2,1)==1)
%                                 new_mask(ss-1,1) = 1;
%                                 new_mask(ss,1) = 1;
%                                 new_mask(ss+1,1) = 1;
%                             end
                            if(ss==3 || ss==numSamples-2)
                                mask_size = 2;
                            elseif(ss==4 || ss==numSamples-3)
                                mask_size = 3;
                            elseif(ss==5 || ss==numSamples-4)
                                mask_size = 4;
                            else
                                mask_size = 5;
                            end
                          
                            prev_found = 0;
                            post_found = 0;
                            
                            for sp=mask_size:-1:1
                                if(cur_mask(ss-sp,1))
                                    prev_mark = sp;
                                    prev_found = 1;
                                    break;
                                end
                            end
                            for sa=mask_size:-1:1
                                if(cur_mask(ss+sa,1))
                                    post_mark = sa;
                                    post_found = 1;
                                    break;
                                end
                            end
                            
                            if(prev_found==1 && post_found==1)
                                new_mask((ss-prev_mark):(ss+post_mark),1) = 1;
                            end
                            
                        end
                        new_corresp_masks{vcl,1}(:,cl) = new_mask;
                    end
                end

                corresp_masks = new_corresp_masks;

                %Post process the correspondence mask once again to remove
                %outlier correspondences using neighborhood info
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
                            
                            if(ss==3 || ss==numSamples-2)
                                mask_size = 2;
                            elseif(ss==4 || ss==numSamples-3)
                                mask_size = 3;
                            elseif(ss==5 || ss==numSamples-4)
                                mask_size = 4;
                            else
                                mask_size = 5;
                            end
                          
                            prev_found = 0;
                            post_found = 0;
                            
                            for sp=mask_size:-1:1
                                if(~cur_mask(ss-sp,1))
                                    prev_mark = sp;
                                    prev_found = 1;
                                    break;
                                end
                            end
                            for sa=mask_size:-1:1
                                if(~cur_mask(ss+sa,1))
                                    post_mark = sa;
                                    post_found = 1;
                                    break;
                                end
                            end
                            
                            if(prev_found==1 && post_found==1)
                                new_mask((ss-prev_mark):(ss+post_mark),1) = 0;
                            end
                       
                        end
                        new_corresp_masks{vcl,1}(:,cl) = new_mask;
                    end
                end

                corresp_masks = new_corresp_masks;
                all_corresp_masks{all_views(1,av)+1,1}{1,cc} = corresp_masks;

                %Samples that have no correspondence on any other curve in the
                %cluster, are already converged
                fused_corresp_masks = zeros(numSamples,1);
                for vcl=1:numIM
                    cur_view = corresp_masks{vcl,1};
                    if(isempty(cur_view))
                        continue;
                    end
                    for cl=1:size(cur_view,2)
                        cur_mask = cur_view(:,cl);
                        fused_corresp_masks = fused_corresp_masks | cur_mask;
                    end
                end
                converge_flags{all_views(1,av)+1,1}{1,cc} = ~fused_corresp_masks;

            end

        end


        coll_motion1 = [];
        coll_motion2 = [];
        av_cntr = 0;

        while(~averaging_iterations_done)
            av_cntr = av_cntr+1

            for av=1:numViews
                av
                cur_clusters = clusters{all_views(1,av)+1,1};

                if(isempty(cur_clusters))
                    continue;
                end

                num_curves_in_cluster = size(cur_clusters,2);

                for cc=1:num_curves_in_cluster

                    %Check to see if this curve has converged at all samples
                    cur_converge_flags = converge_flags{all_views(1,av)+1,1}{1,cc};

                    if(all(cur_converge_flags))
                        continue;
                    end

                    curveID = cur_clusters(1,cc);
                    queryCurve = all_recs_iter{all_views(1,av)+1,1}{1,curveID};
                    queryAlignment = alignment_curves{all_views(1,av)+1,1}{1,cc};
                    numSamples = size(queryCurve,1); 

                    corresp_masks = all_corresp_masks{all_views(1,av)+1,1}{1,cc};

                    %Process the masks to iterate curve samples
                    queryCurve_next = queryCurve;%zeros(numSamples,3);

                    for ss=1:numSamples

                        if(cur_converge_flags(ss,1))
                            continue;
                        end

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

                        cur_motion = norm(querySample - newSample);

                        if(cur_motion <= 0.001)
                            converge_flags{all_views(1,av)+1,1}{1,cc}(ss,1) = 1;
                        end

                    end


                    all_recs_iter{all_views(1,av)+1,1}{1,curveID} = queryCurve_next;
                    %write_curve_to_vrml(queryCurve_next,[255 0 0],['./ply/after/view',num2str(all_views(1,av)),'_curve',num2str(curveID),'.ply']);

                end

            end


            averaging_iterations_done = 1;

            % Check whether all curve samples converged
            for av=1:numViews
                cur_converge_flags = converge_flags{all_views(1,av)+1,1};
                num_cluster_curves = size(cur_converge_flags,2);
                for ccv=1:num_cluster_curves
                    cur_flags = cur_converge_flags{1,ccv};
                    if(any(~cur_flags))
                        averaging_iterations_done = 0;
                        break;
                    end
                end
            end

        end


        %Now that the cluster has converged, we will merge the curves at
        %overlapping segments

        %First we find the longest curve in the cluster
        longest_curve_size = 0;
        longest_curve_view = -1;
        longest_curve_id = -1;

        for lcv=1:numViews
            cur_clusters = clusters{all_views(1,lcv)+1,1};
            if(isempty(cur_clusters))
                continue;
            end
            num_curves_in_cluster = size(cur_clusters,2);
            for cc=1:num_curves_in_cluster
                curveID = cur_clusters(1,cc);
                queryCurve = all_recs_iter{all_views(1,lcv)+1,1}{1,curveID};
                cur_size = size(queryCurve,1);
                if(cur_size > longest_curve_size)
                    longest_curve_view = all_views(1,lcv)+1;
                    longest_curve_view_index = lcv;
                    longest_curve_id = curveID;
                    longest_curve_cluster_id = cc;
                    longest_curve_size = cur_size;
                end
            end
        end
        
        %We process the longest curve to break it in discontinuities
        longest_curve_pieces = [];
        longest_curve = all_recs_iter{longest_curve_view,1}{1,longest_curve_id};
        start_point = 1;
        
        for lcs=2:size(longest_curve,1)
            cur_dist = norm(longest_curve(lcs,:)-longest_curve(lcs-1,:));
            if(cur_dist>sample_break_threshold)
                current_piece = longest_curve(start_point:lcs-1,:);
                %Be more lenient here with length threshold
                if(4*get_length(current_piece)>branch_length_threshold)
                    longest_curve_pieces = [longest_curve_pieces; cell(1,1)];
                    lcp_size = size(longest_curve_pieces,1);
                    longest_curve_pieces{lcp_size,1} = current_piece;
                end
                start_point=lcs;
            end
            if(lcs==size(longest_curve,1))
                current_piece = longest_curve(start_point:lcs-1,:);
                %Be more lenient here with length threshold
                if(4*get_length(current_piece)>branch_length_threshold)
                    longest_curve_pieces = [longest_curve_pieces; cell(1,1)];
                    lcp_size = size(longest_curve_pieces,1);
                    longest_curve_pieces{lcp_size,1} = current_piece;
                end
            end
        end
        
        %We initialize the curve graph with these longest curve pieces
        curve_graph = longest_curve_pieces;
        lcp_size = size(longest_curve_pieces,1);
        %curve_graph{1,1} = all_recs_iter{longest_curve_view,1}{1,longest_curve_id};
        curve_graph_content = cell(lcp_size,1);
        for lcp=1:lcp_size
            curve_graph_content{lcp,1} = [longest_curve_view longest_curve_cluster_id];
        end
		%A pair of flags per branch, marking whether endpoints are locked with a junction
		lock_map = zeros(lcp_size,2);
        
        merge_flags = cell(numViews,1);
        for mfv=1:numViews
            merge_flags{mfv,1} = zeros(size(clusters{all_views(1,mfv)+1,1}));
        end
        merge_flags{longest_curve_view_index,1}(1,longest_curve_cluster_id) = 1;

        merging_iterations_done=0;
        cntr = 0;
        while(~merging_iterations_done)
            cntr = cntr+1
            
%             if(cntr>1)
%                 break;
%             end
            
            new_curve_merged = 0;
            
            %Problem curve
%             if(crv==5 && cntr==2)
%                 numViews=25;
%             end

            for mrv=1:numViews
                mrv
                cur_clusters = clusters{all_views(1,mrv)+1,1};
                if(isempty(cur_clusters))
                    continue;
                end
                num_curves_in_cluster = size(cur_clusters,2);
                
%                 if(mrv==3)
%                     num_curves_in_cluster = 4;
%                 end
                
                for cc=1:num_curves_in_cluster
                    if(merge_flags{mrv,1}(1,cc))
                        continue;
                    end
                    curveID = cur_clusters(1,cc);
                    queryCurve = all_recs_iter{all_views(1,mrv)+1,1}{1,curveID};
                    cur_size = size(queryCurve,1);

                    query_corresp_masks = all_corresp_masks{all_views(1,mrv)+1,1}{1,cc};
                    %For each branch in the graph,
                    %Grab all the correspondence maps this curve has with all the
                    %curves contained so far in that branch

                    num_branches = size(curve_graph,1);
                    merged_corresp_mask_all = cell(num_branches,1);
                    merged_corresp_mask_cumulative = zeros(cur_size,1);
                    
                    for cgbr=1:num_branches
                        merged_corresp_mask = zeros(cur_size,1);
                        for cgc=1:size(curve_graph_content{cgbr,1},1)
                            corr_view = curve_graph_content{cgbr,1}(cgc,1);
                            corr_curve = curve_graph_content{cgbr,1}(cgc,2);
                            merged_corresp_mask = (merged_corresp_mask | query_corresp_masks{corr_view,1}(:,corr_curve));
                            merged_corresp_mask_cumulative = (merged_corresp_mask_cumulative | query_corresp_masks{corr_view,1}(:,corr_curve));
                        end
                        merged_corresp_mask_all{cgbr,1} = merged_corresp_mask;
                    end

                    %Go over each sample, merge overlapping samples, break branches
                    new_branch_start = 0;
                    currently_traversing_branch = 0;
                    no_init_junction = 0;
                    all_new_branches = [];
                    all_new_locks = [];
                    
                    %A list that's filled everytime a merge happens
                    %The branch ids and the closest points are stored
                    last_merged_branches = [];
                    
                    %The branch ids that were used in the last merge
                    arr_branch_id_cur = [];
                    arr_branch_id_last = [];
                    arr_junction_indices_last = [];
                    arr_prev_sample_last = [];
                    arr_next_sample_last = [];
                    
                    %Equivalence table for broken branches in current and
                    %previous sample
                    
                    equiv_table_cur = [];
                    equiv_table_last = [];
                    
                    %If there is no correspondence on any sample, move on
                    if(all(~merged_corresp_mask_cumulative))
                        continue;
                    %Otherwise, mark this curve as merged
                    else
                        merge_flags{mrv,1}(1,cc) = 1;
                        new_curve_merged = 1;
                    end
                    
                    is_merged = 0;
                    
                    for ms=1:cur_size
                        
                         num_branches = size(curve_graph,1);
                        
                         %Arrays to store all relevant info
                         arr_outside_init_range=zeros(num_branches,1);
                         arr_outside_final_range=zeros(num_branches,1);
                         arr_prev_sample = zeros(num_branches,1);
                         arr_next_sample = zeros(num_branches,1);
                         arr_branch_dist = (-1).*ones(num_branches,1);
                         arr_closest_pt = zeros(num_branches,3);
                         arr_junction_indices = zeros(num_branches,1);
                         
                         equiv_table_cur = cell(num_branches,1);
                         
                         closest_branch = 0;
                         
                         %Current sample should add at most
                         %1 junction to the graph, even if
                         %it connects to more than 1 branch
                         is_junction_saved = 0;
                         is_last_junction_saved = 0;
                         
                         num_attachments_last = size(arr_branch_id_last,2);
                         
                         dist_to_prev_sample = 0;
                         no_init_flag = 0;
                         if(ms>1) 
                            dist_to_prev_sample = norm(queryCurve(ms,:)-queryCurve(ms-1,:));
                         end
                         
                         if(dist_to_prev_sample>sample_break_threshold && currently_traversing_branch)
                             query_branch_size = ms-new_branch_start+1;

                             %Break the existing branch
                             currently_traversing_branch = 0;

                             %If the whole query curve is a new branch,
                             %then it's not really a branch but rather
                             %an erroneously grouped 3D curve, which
                             %should now be broken free
                             if(isempty(arr_branch_id_last))
                                 new_branch = queryCurve(new_branch_start:ms-1,:);
                                 all_new_branches = [all_new_branches; cell(1,1)];
                                 anb_size = size(all_new_branches,1);
                                 all_new_branches{anb_size,1} = new_branch;
                                 all_new_locks = [all_new_locks; [0 0]];
                             else

                                 is_elongated = 0;
                                 %CHECK FOR ELONGATION AT (A): YES
                                 if(num_attachments_last==1 && ~no_init_junction)
                                     rb_id = arr_branch_id_last(1,1);

                                     target_curve = curve_graph{rb_id,1};
                                     target_size = size(target_curve,1);
                                     target_lock_init = lock_map(rb_id,1);
                                     target_lock_final = lock_map(rb_id,2);

                                     size_beginning_branch = arr_junction_indices_last(rb_id,1);
                                     size_ending_branch = target_size - arr_junction_indices_last(rb_id,1) + 1;

                                     %If we're close to the
                                     %beginning on this branch and
                                     %If our native branch is not long 
                                     %enough, and the new branch attaches to 
                                     %only 1 existing branch, then elongate
                                     if(size_beginning_branch<size_ending_branch && ~target_lock_init && (size_beginning_branch<3 || get_length(target_curve(1:arr_junction_indices_last(rb_id,1),:)) < branch_length_threshold))
                                         target_curve = [flipud(queryCurve(new_branch_start:ms-1,:)); target_curve(arr_junction_indices_last(rb_id,1):target_size,:)];
                                         curve_graph{rb_id,1} = target_curve; 
                                         is_elongated = 1;
                                         %Set the locks
                                         lock_map(rb_id,1) = 0;
                                         %TODO: Adjust the content of the merged branch
                                         curve_graph_content{rb_id,1} = unique([curve_graph_content{rb_id,1}; [all_views(1,mrv)+1 cc]],'rows');
                                     %If we're close to the ending and      
                                     %If our native branch is not long 
                                     %enough, and the new branch attaches to 
                                     %only 1 existing branch, then elongate
                                     elseif(size_ending_branch<size_beginning_branch && ~target_lock_final && (size_ending_branch<3 || get_length(target_curve(arr_junction_indices_last(rb_id,1):target_size,:)) < branch_length_threshold))
                                         target_curve = [target_curve(1:arr_junction_indices_last(rb_id,1),:); queryCurve(new_branch_start:ms-1,:)];
                                         curve_graph{rb_id,1} = target_curve; 
                                         is_elongated = 1;
                                         %Set the locks
                                         lock_map(rb_id,2) = 0;
                                         %TODO: Adjust the content of the merged branch
                                         curve_graph_content{rb_id,1} = unique([curve_graph_content{rb_id,1}; [all_views(1,mrv)+1 cc]],'rows');
                                     end
                                 end
                                 %CHECK FOR ELONGATION AT (A): NO
                                 %Otherwise this is a junction     
                                 if(~is_elongated)
                                     %Create and add the new branch
                                     if(no_init_junction)
                                         new_branch = queryCurve(new_branch_start:ms-1,:);
                                         all_new_branches = [all_new_branches; cell(1,1)];
                                         anb_size = size(all_new_branches,1);
                                         all_new_branches{anb_size,1} = new_branch;
                                         all_new_locks = [all_new_locks; [0 0]];
                                     elseif(query_branch_size >= branch_size_threshold && get_length(queryCurve(new_branch_start-1:ms-1,:)) >= branch_length_threshold)
                                         new_branch = queryCurve(new_branch_start-1:ms-1,:);
                                         all_new_branches = [all_new_branches; cell(1,1)];
                                         anb_size = size(all_new_branches,1);
                                         all_new_branches{anb_size,1} = new_branch; 
                                         all_new_locks = [all_new_locks; [1 0]];

                                         %Add the junction if it's not added already
                                         if(~is_last_junction_saved)
                                             all_junctions = [all_junctions; queryCurve(new_branch_start-1,:)];
                                             is_last_junction_saved = 1;
                                         end

                                         %Go over each relevant branch to
                                         %process the junctions of each
                                         for rb=1:num_attachments_last

                                             rb_id = arr_branch_id_last(1,rb);

                                             target_curve = curve_graph{rb_id,1};
                                             target_size = size(target_curve,1);

                                             %Junction breaks the existing branch it's
                                             %being attached to
                                             target_curve1 = target_curve(1:arr_junction_indices_last(rb_id,1),:);
                                             target_curve2 = target_curve(arr_junction_indices_last(rb_id,1):target_size,:);

                                             %Add the broken pieces
                                             curve_graph{rb_id,1} = target_curve1;
                                             curve_graph = [curve_graph; cell(1,1)];
                                             cg_size = size(curve_graph,1);
                                             curve_graph{cg_size,1} = target_curve2;
                                             %Set the junction locks for the new branches
                                             cur_locks = lock_map(rb_id,:);
                                             cur_lock_init = cur_locks(1,1);
                                             cur_lock_final = cur_locks(1,2);
                                             lock_map(rb_id,:) = [cur_lock_init 1];
                                             lock_map = [lock_map; [1 cur_lock_final]];

                                             %Set the contents of the new branch
                                             curve_graph_content = [curve_graph_content; cell(1,1)];
                                             curve_graph_content{cg_size,1} = curve_graph_content{rb_id,1};
                                             %Set the correspondence flags for the new branch
                                             merged_corresp_mask_all = [merged_corresp_mask_all; cell(1,1)];
                                             merged_corresp_mask_all{cg_size,1} = merged_corresp_mask_all{rb_id,1};
                                             %Set the equivalence table for the new branch
                                             equiv_table_cur{rb_id,1} = [equiv_table_cur{rb_id,1} cg_size];
                                             equiv_table_cur = [equiv_table_cur; cell(1,1)];
                                             equiv_table_cur{cg_size,1} = [equiv_table_cur{cg_size,1} rb_id];

                                         end
                                     end
                                 end
                             end
                            arr_branch_id_last = [];
                            arr_junction_indices_last = [];
                            arr_prev_sample_last = [];
                            arr_next_sample_last = [];
                            num_attachments_last = 0;
                         end
                        
                         %If this sample is marked to have a correspondence
                         %Record all distances to all branches.
                         %Record closest branch distance and ID
                         %If this closest distance is still too far, remove
                         %the correspondence link
                         if(merged_corresp_mask_cumulative(ms,1))
                             query_sample = queryCurve(ms,:);
                             min_branch_dist = 10^10;
                             
                             for br=1:num_branches
                                 %If branch has only 1 point, this
                                 %operation is not meaningful
                                 if(size(curve_graph{br,1},1)<=1)
                                     continue;
                                 end
                                 
                                 %If the current branch has no
                                 %correspondence, move onto the next one
                                 if(~merged_corresp_mask_all{br,1}(ms,1))
                                     continue;
                                 end
                                 
                                 [arr_closest_pt(br,:),arr_outside_init_range(br,1),arr_outside_final_range(br,1),arr_prev_sample(br,1),arr_next_sample(br,1)] = find_closest_point_on_curve(curve_graph{br,1},query_sample);
                                 arr_branch_dist(br,1) = norm(arr_closest_pt(br,:) - query_sample);
                                 %Only save the ID of the closest branch,
                                 %all other attributes are saved.
                                 if(arr_branch_dist(br,1) < min_branch_dist)
                                     closest_branch = br;
                                     min_branch_dist = arr_branch_dist(br,1);
                                 end
                             end
                             if(min_branch_dist > sample_merge_threshold)
                                merged_corresp_mask_cumulative(ms,1) = 0;
                             end
                         end
                         
                         if(dist_to_prev_sample>sample_break_threshold && ~merged_corresp_mask_cumulative(ms,1))
                             no_init_flag = 1;
                         end
                                           
                         %A) If the sample has no link
                         if(~merged_corresp_mask_cumulative(ms,1))
                             %Ai) If the previous sample had a link, we
                             %start a new branch
                             if(~currently_traversing_branch)
                                new_branch_start = ms;
                                currently_traversing_branch = 1;
                                no_init_junction = no_init_flag;
                                
                             %Aii) If the previous sample did not have a
                             %link, but we reached the end, we need to
                             %finish the started branch and add it
                             
                             elseif(ms==cur_size)
                                 query_branch_size = ms-new_branch_start+1;
                                 
                                 %Break the existing branch
                                 currently_traversing_branch = 0;
                                 
                                 %If the whole query curve is a new branch,
                                 %then it's not really a branch but rather
                                 %an erroneously grouped 3D curve, which
                                 %should now be broken free
                                 if(isempty(arr_branch_id_last))
                                     
                                     new_branch = queryCurve(new_branch_start:ms,:);
                                     all_new_branches = [all_new_branches; cell(1,1)];
                                     anb_size = size(all_new_branches,1);
                                     all_new_branches{anb_size,1} = new_branch;
                                     all_new_locks = [all_new_locks; [0 0]];
                                 else
                                     is_elongated = 0;
                                     %CHECK FOR ELONGATION AT (A): YES
                                     if(num_attachments_last==1 && ~no_init_junction)
                                         rb_id = arr_branch_id_last(1,1);

                                         target_curve = curve_graph{rb_id,1};
                                         target_size = size(target_curve,1);
                                         target_lock_init = lock_map(rb_id,1);
                                         target_lock_final = lock_map(rb_id,2);

                                         size_beginning_branch = arr_junction_indices_last(rb_id,1);
                                         size_ending_branch = target_size - arr_junction_indices_last(rb_id,1) + 1;

                                         %If we're close to the
                                         %beginning on this branch and
                                         %If our native branch is not long 
                                         %enough, and the new branch attaches to 
                                         %only 1 existing branch, then elongate
                                         if(size_beginning_branch<size_ending_branch && ~target_lock_init && (size_beginning_branch<3 || get_length(target_curve(1:arr_junction_indices_last(rb_id,1),:)) < branch_length_threshold))
                                             target_curve = [flipud(queryCurve(new_branch_start:ms-1,:)); target_curve(arr_junction_indices_last(rb_id,1):target_size,:)];
                                             curve_graph{rb_id,1} = target_curve; 
                                             is_elongated = 1;
                                             %Set the locks
                                             lock_map(rb_id,1) = 0;
                                             %TODO: Adjust the content of the merged branch
                                             curve_graph_content{rb_id,1} = unique([curve_graph_content{rb_id,1}; [all_views(1,mrv)+1 cc]],'rows');
                                         %If we're close to the ending and      
                                         %If our native branch is not long 
                                         %enough, and the new branch attaches to 
                                         %only 1 existing branch, then elongate
                                         elseif(size_ending_branch<size_beginning_branch && ~target_lock_final && (size_ending_branch<3 || get_length(target_curve(arr_junction_indices_last(rb_id,1):target_size,:)) < branch_length_threshold))
                                             target_curve = [target_curve(1:arr_junction_indices_last(rb_id,1),:); queryCurve(new_branch_start:ms-1,:)];
                                             curve_graph{rb_id,1} = target_curve; 
                                             is_elongated = 1;
                                             %Set the locks
                                             lock_map(rb_id,2) = 0;
                                             %TODO: Adjust the content of the merged branch
                                             curve_graph_content{rb_id,1} = unique([curve_graph_content{rb_id,1}; [all_views(1,mrv)+1 cc]],'rows');
                                         end
                                     end
                                     %CHECK FOR ELONGATION AT (A): NO
                                     %Otherwise this is a junction     
                                     if(~is_elongated)
                                         %Create and add the new branch
                                         if(no_init_junction)
                                             new_branch = queryCurve(new_branch_start:ms,:);
                                             all_new_branches = [all_new_branches; cell(1,1)];
                                             anb_size = size(all_new_branches,1);
                                             all_new_branches{anb_size,1} = new_branch;
                                             all_new_locks = [all_new_locks; [0 0]];
                                         elseif(query_branch_size > branch_size_threshold || get_length(queryCurve(new_branch_start-1:ms,:)) > branch_length_threshold)
                                             new_branch = queryCurve(new_branch_start-1:ms,:);
                                             all_new_branches = [all_new_branches; cell(1,1)];
                                             anb_size = size(all_new_branches,1);
                                             all_new_branches{anb_size,1} = new_branch; 
                                             all_new_locks = [all_new_locks; [1 0]];

                                             %Add the junction if it's not added already
                                             if(~is_last_junction_saved)
                                                 all_junctions = [all_junctions; queryCurve(new_branch_start-1,:)];
                                                 is_last_junction_saved = 1;
                                             end

                                             %Go over each relevant branch to
                                             %process the junctions of each
                                             for rb=1:num_attachments_last

                                                 rb_id = arr_branch_id_last(1,rb);

                                                 target_curve = curve_graph{rb_id,1};
                                                 target_size = size(target_curve,1);

                                                 %Junction breaks the existing branch it's
                                                 %being attached to
                                                 target_curve1 = target_curve(1:arr_junction_indices_last(rb_id,1),:);
                                                 target_curve2 = target_curve(arr_junction_indices_last(rb_id,1):target_size,:);

                                                 %Add the broken pieces
                                                 curve_graph{rb_id,1} = target_curve1;
                                                 curve_graph = [curve_graph; cell(1,1)];
                                                 cg_size = size(curve_graph,1);
                                                 curve_graph{cg_size,1} = target_curve2;
                                                 %Set the junction locks for the new branches
                                                 cur_locks = lock_map(rb_id,:);
                                                 cur_lock_init = cur_locks(1,1);
                                                 cur_lock_final = cur_locks(1,2);
                                                 lock_map(rb_id,:) = [cur_lock_init 1];
                                                 lock_map = [lock_map; [1 cur_lock_final]];
                                                 
                                                 %Set the contents of the new branch
                                                 curve_graph_content = [curve_graph_content; cell(1,1)];
                                                 curve_graph_content{cg_size,1} = curve_graph_content{rb_id,1};
                                                 %Set the correspondence flags for the new branch
                                                 merged_corresp_mask_all = [merged_corresp_mask_all; cell(1,1)];
                                                 merged_corresp_mask_all{cg_size,1} = merged_corresp_mask_all{rb_id,1};
                                                 %Set the equivalence table for the new branch
                                                 equiv_table_cur{rb_id,1} = [equiv_table_cur{rb_id,1} cg_size];
                                                 equiv_table_cur = [equiv_table_cur; cell(1,1)];
                                                 equiv_table_cur{cg_size,1} = [equiv_table_cur{cg_size,1} rb_id];
                                                 
                                             end
                                         end
                                     end
                                 end
                             end
                             %Aiii) Otherwise (if currently traversing a
                             %new branch, and if we're not at the end yet)
                             %do nothing
                             
                         %B) If the sample has a link, it should be merged
                         %with the relevant branches
                         else
                             %Get the shortest distance
                             shortest_dist = arr_branch_dist(closest_branch,1);
                             %Define the threshold
                             %TODO: Make the multiplication factor a 
                             %system parameter
                             dist_threshold = 2*shortest_dist;
                             is_merged = 1;
                             
                             arr_branch_id_cur = [];
                             arr_branch_id_cur = closest_branch;
                             
                             only_merge_to_single_branch = 0;
                             %Check to see if we are close to an existing
                             %junction, avoid merging this sample with
                             %multiple branches if that's the case
                             for jn=1:size(all_junctions,1)
                                 if(norm(queryCurve(ms,:) - all_junctions(jn,:)) < junction_dist_threshold)
                                     only_merge_to_single_branch=1;
                                     break;
                                 end                                     
                             end
                             %only_merge_to_single_branch = 0;
                             if(~only_merge_to_single_branch)
                                 %Go over each branch with correspondence, then
                                 %store the IDs of the 'close enough' branches
                                 for br=1:num_branches
                                     if(arr_branch_dist(br,1)>-1 && arr_branch_dist(br,1)<dist_threshold && br~=closest_branch)
                                         arr_branch_id_cur = [arr_branch_id_cur br];
                                     end
                                 end
                             end
                             
                             num_attachments_cur = size(arr_branch_id_cur,2);
                             
                             %Insert the current sample into all
                             %relevant branches computed above
                             num_relevant_branches = size(arr_branch_id_cur,2);
                             for br=1:num_relevant_branches
                                 rel_br_id = arr_branch_id_cur(1,br);
                                 
                                 %Store the junction index, meaning the
                                 %index into the branch samples that
                                 %indicates a junction, for each branch
                                 
                                 target_curve = curve_graph{rel_br_id,1};
                                 target_size = size(target_curve,1);
                                 if(arr_prev_sample(rel_br_id,1)==1 && arr_outside_init_range(rel_br_id,1))
                                     curve_graph{rel_br_id,1} = [query_sample; target_curve];
                                     arr_junction_indices(rel_br_id,1) = 1;
                                 elseif(arr_next_sample(rel_br_id,1)==target_size && arr_outside_final_range(rel_br_id,1))
                                     curve_graph{rel_br_id,1} = [target_curve; query_sample];
                                     arr_junction_indices(rel_br_id,1) = target_size+1;
                                 else
                                     curve_graph{rel_br_id,1} = [target_curve(1:arr_prev_sample(rel_br_id,1),:); query_sample; target_curve(arr_next_sample(rel_br_id,1):target_size,:)];
                                     arr_junction_indices(rel_br_id,1) = arr_prev_sample(rel_br_id,1)+1;
                                 end
                                 
                                 %Adjust the contents of the target
                                 %branch after this operation
                                 curve_graph_content{rel_br_id,1} = unique([curve_graph_content{rel_br_id,1}; [all_views(1,mrv)+1 cc]],'rows');
                             end
                             
                             %DO (A) AND (B) ATTACH TO DIFFERENT BRANCHES?
                             %Is there a new current branch?
                             is_cur_different = 0;
                             is_cur_non_equivalent = 0;
                             for nac=1:num_attachments_cur
                                 curID = arr_branch_id_cur(1,nac);
                                 %Check to see if a new branch got
                                 %involved, mark the current sample as
                                 %a junction if one did
                                 if(isempty(find(arr_branch_id_last==curID,1)) && ~isempty(arr_branch_id_last))
                                    is_cur_different = 1;
                                    is_cur_non_equivalent = 1;
                                    for eq=1:size(equiv_table_cur{curID,1},2)
                                        eqID = equiv_table_cur{curID,1}(1,eq);
                                        if(~isempty(find(arr_branch_id_last==eqID,1)))
                                            is_cur_non_equivalent = 0;
                                            break;
                                        end
                                    end
                                    if(is_cur_non_equivalent)
                                        break;
                                    end
                                 end
                             end

                             %Did we lose an existing last branch?
                             is_last_different = 0;
                             is_last_non_equivalent = 0;
                             for nal=1:num_attachments_last
                                 curID = arr_branch_id_last(1,nal);
                                 %Check to see if an existing branch
                                 %lost touch, mark the previous sample
                                 %as a junction if one did
                                 if(isempty(find(arr_branch_id_cur==curID,1)) && ~isempty(arr_branch_id_cur))
                                    is_last_different =1;
                                    is_last_non_equivalent = 1;
                                    for eq=1:size(equiv_table_last{curID,1},2)
                                        eqID = equiv_table_last{curID,1}(1,eq);
                                        if(~isempty(find(arr_branch_id_cur==eqID,1)))
                                            is_last_non_equivalent = 0;
                                            break;
                                        end
                                    end
                                    if(is_last_non_equivalent)
                                        break;
                                    end
                                 end
                             end
                             
                             %Bi) If we had been building up a new branch,
                             %now is the time to create and add it
                             if(currently_traversing_branch)
                                 %Break the existing branch
                                 currently_traversing_branch = 0;
                                 query_branch_size = ms-new_branch_start+1;
                                 
								 %If we don't have a starting point for
								 %the branch, then this branch attaches
								 %only at the current sample
								 if(isempty(arr_branch_id_last))
									 is_elongated = 0;
									 %CHECK FOR ELONGATION AT (B): YES
									 if(num_attachments_cur==1)
										 rb_id = arr_branch_id_cur(1,1);

										 target_curve = curve_graph{rb_id,1};
										 target_size = size(target_curve,1);
										 target_lock_init = lock_map(rb_id,1);
										 target_lock_final = lock_map(rb_id,2);

										 size_beginning_branch = arr_junction_indices(rb_id,1);
										 size_ending_branch = target_size - arr_junction_indices(rb_id,1) + 1;
										 
										 %If we're close to the
										 %beginning on this branch and
										 %If our native branch is not long 
										 %enough, and the new branch attaches to 
										 %only 1 existing branch, then elongate
										 if(size_beginning_branch<size_ending_branch && ~target_lock_init && (size_beginning_branch<3 || get_length(target_curve(1:arr_junction_indices(rb_id,1),:)) < branch_length_threshold))
											 target_curve = [queryCurve(new_branch_start:ms-1,:); target_curve(arr_junction_indices(rb_id,1):target_size,:)];
											 curve_graph{rb_id,1} = target_curve; 
											 is_elongated = 1;
											 %Set the locks
											 lock_map(rb_id,1) = 0;
											 %TODO: Adjust the content of the merged branch
											 curve_graph_content{rb_id,1} = unique([curve_graph_content{rb_id,1}; [all_views(1,mrv)+1 cc]],'rows');
										 %If we're close to the ending and      
										 %If our native branch is not long 
										 %enough, and the new branch attaches to 
										 %only 1 existing branch, then elongate
										 elseif(size_ending_branch<size_beginning_branch && ~target_lock_final && (size_ending_branch<3 || get_length(target_curve(arr_junction_indices(rb_id,1):target_size,:)) < branch_length_threshold))
											 target_curve = [target_curve(1:arr_junction_indices(rb_id,1),:); flipud(queryCurve(new_branch_start:ms-1,:))];
											 curve_graph{rb_id,1} = target_curve; 
											 is_elongated = 1;
											 %Set the locks
											 lock_map(rb_id,2) = 0;
											 %TODO: Adjust the content of the merged branch
											 curve_graph_content{rb_id,1} = unique([curve_graph_content{rb_id,1}; [all_views(1,mrv)+1 cc]],'rows');
										 end
									 end
									 %CHECK FOR ELONGATION AT (B): NO
									 %Otherwise this is a junction     
									 if(~is_elongated && (query_branch_size > branch_size_threshold || get_length(queryCurve(new_branch_start:ms,:)) > branch_length_threshold))
										 %Create and add the new branch
										 new_branch = queryCurve(new_branch_start:ms,:);
										 all_new_branches = [all_new_branches; cell(1,1)];
										 anb_size = size(all_new_branches,1);
										 all_new_branches{anb_size,1} = new_branch; 
										 all_new_locks = [all_new_locks; [0 1]];
										 
										 %Add the junction if it's not added already
										 if(~is_junction_saved)
											all_junctions = [all_junctions; queryCurve(ms,:)];
											is_junction_saved = 1;
										 end

										 %Go over each relevant branch to
										 %process the junctions of each
										 for rb=1:num_attachments_cur

											 rb_id = arr_branch_id_cur(1,rb);

											 target_curve = curve_graph{rb_id,1};
											 target_size = size(target_curve,1);

											 %Junction breaks the existing branch it's
											 %being attached to
											 target_curve1 = target_curve(1:arr_junction_indices(rb_id,1),:);
											 target_curve2 = target_curve(arr_junction_indices(rb_id,1):target_size,:);

											 %Add the broken pieces
											 curve_graph{rb_id,1} = target_curve1;
											 curve_graph = [curve_graph; cell(1,1)];
											 cg_size = size(curve_graph,1);
											 curve_graph{cg_size,1} = target_curve2;
											 %Set the junction locks for the new branches
											 cur_locks = lock_map(rb_id,:);
											 cur_lock_init = cur_locks(1,1);
											 cur_lock_final = cur_locks(1,2);
											 lock_map(rb_id,:) = [cur_lock_init 1];
											 lock_map = [lock_map; [1 cur_lock_final]];
											 
											 %Set the contents of the new branch
											 curve_graph_content = [curve_graph_content; cell(1,1)];
											 curve_graph_content{cg_size,1} = curve_graph_content{rb_id,1};
											 %Set the correspondence flags for the new branch
											 merged_corresp_mask_all = [merged_corresp_mask_all; cell(1,1)];
											 merged_corresp_mask_all{cg_size,1} = merged_corresp_mask_all{rb_id,1};
											 %Set the equivalence table for the new branch
											 equiv_table_cur{rb_id,1} = [equiv_table_cur{rb_id,1} cg_size];
											 equiv_table_cur = [equiv_table_cur; cell(1,1)];
											 equiv_table_cur{cg_size,1} = [equiv_table_cur{cg_size,1} rb_id];
											 
										 end
									 end
										 
								 %(A) AND (B) ATTACH TO DIFFERENT BRANCHES 
								 elseif(is_cur_different || is_last_different)
									 is_init_elongated = 0;
									 appended_curve = [];
									 appended_graph_content = [];
									 appended_lock = [];
									 init_branch_id = 0;
									 flip_before_second_append = 0;
									 init_target_min = 0;
									 init_target_max = 0;
									 %Check for elongation at (A) if
									 %there is only 1 attaching branch
									 if(num_attachments_last==1 && ~no_init_junction)
										 rb_id = arr_branch_id_last(1,1);
										 init_branch_id = rb_id;

										 target_curve = curve_graph{rb_id,1};
										 target_size = size(target_curve,1);
										 target_lock_init = lock_map(rb_id,1);
										 target_lock_final = lock_map(rb_id,2);

										 size_beginning_branch = arr_junction_indices_last(rb_id,1);
										 size_ending_branch = target_size - arr_junction_indices_last(rb_id,1) + 1;
										 
										 is_inverted = 0;
										 is_two_junctions = 0;
										 id_ind = find(arr_branch_id_cur==rb_id,1);
										 %We need to see if this branch
										 %is attached at two points
										 if(~isempty(id_ind))
											 is_two_junctions = 1;
											 if(arr_junction_indices_last(rb_id,1) > arr_junction_indices(rb_id,1))
												 is_inverted = 1;
											 end
										 end

										 %If we're close to the
										 %beginning on this branch and
										 %If our native branch is not long 
										 %enough, and the new branch attaches to 
										 %only 1 existing branch, then elongate
										 if(size_beginning_branch<size_ending_branch && (~is_two_junctions || ~is_inverted) && ~target_lock_init && (size_beginning_branch<3 || get_length(target_curve(1:arr_junction_indices_last(rb_id,1),:)) < branch_length_threshold))
											 appended_curve = [flipud(queryCurve(new_branch_start:ms-1,:)); target_curve(arr_junction_indices_last(rb_id,1):target_size,:)];
											 %curve_graph{rb_id,1} = target_curve; 
											 is_init_elongated = 1;
											 flip_before_second_append = 1;
											 init_target_min = arr_junction_indices_last(rb_id,1);
											 init_target_max = target_size;
											 %Set the locks
											 appended_lock = lock_map(rb_id,2);
											 %TODO: Adjust the content of the merged branch
											 appended_graph_content = unique([curve_graph_content{rb_id,1}; [all_views(1,mrv)+1 cc]],'rows');
										 %If we're close to the ending and      
										 %If our native branch is not long 
										 %enough, and the new branch attaches to 
										 %only 1 existing branch, then elongate
										 elseif(size_ending_branch<size_beginning_branch && (~is_two_junctions || is_inverted) && ~target_lock_final && (size_ending_branch<3 || get_length(target_curve(arr_junction_indices_last(rb_id,1):target_size,:)) < branch_length_threshold))
											 appended_curve = [target_curve(1:arr_junction_indices_last(rb_id,1),:); queryCurve(new_branch_start:ms-1,:)];
											 %curve_graph{rb_id,1} = target_curve; 
											 is_init_elongated = 1;
											 init_target_min = 1;
											 init_target_max = arr_junction_indices_last(rb_id,1);
											 %Set the locks
											 appended_lock = lock_map(rb_id,1);
											 %TODO: Adjust the content of the merged branch
											 appended_graph_content = unique([curve_graph_content{rb_id,1}; [all_views(1,mrv)+1 cc]],'rows');
										 end
									 end
										 
									 if(~is_init_elongated)
										 if(no_init_junction)
											 appended_curve = queryCurve(new_branch_start:ms-1,:);
											 appended_lock = 0;
										 else
											 appended_curve = queryCurve(new_branch_start-1:ms-1,:);
											 appended_lock = 1;
										 end
										 appended_graph_content = [all_views(1,mrv)+1 cc];
									 end
										
									 %Check for elongation at (B) if
									 %there is only 1 attaching branch
									 is_final_elongated = 0;
									 final_target_min = 0;
									 final_target_max = 0;
									 if(num_attachments_cur==1)
										 rb_id = arr_branch_id_cur(1,1);

										 target_curve = curve_graph{rb_id,1};
										 target_size = size(target_curve,1);
										 target_lock_init = lock_map(rb_id,1);
										 target_lock_final = lock_map(rb_id,2);

										 size_beginning_branch = arr_junction_indices(rb_id,1);
										 size_ending_branch = target_size - arr_junction_indices(rb_id,1) + 1;
										 
										 is_inverted = 0;
										 is_two_junctions = 0;
										 id_ind = find(arr_branch_id_last==rb_id,1);
										 %We need to see if this branch
										 %is attached at two points
										 if(~isempty(id_ind))
											 is_two_junctions = 1;
											 if(arr_junction_indices_last(rb_id,1) > arr_junction_indices(rb_id,1))
												 is_inverted = 1;
											 end
										 end
										 
										 %If we're close to the
										 %beginning on this branch and
										 %If our native branch is not long 
										 %enough, and the new branch attaches to 
										 %only 1 existing branch, then elongate
										 if(size_beginning_branch<size_ending_branch && (~is_two_junctions || is_inverted) && ~target_lock_init && (size_beginning_branch<3 || get_length(target_curve(1:arr_junction_indices(rb_id,1),:)) < branch_length_threshold))
											 if(flip_before_second_append)
												 appended_curve = flipud(appended_curve);
											 end
											 target_curve = [appended_curve; target_curve(arr_junction_indices(rb_id,1):target_size,:)];
											 curve_graph{rb_id,1} = target_curve; 
											 is_final_elongated = 1;
											 final_target_min = arr_junction_indices(rb_id,1);
											 final_target_max = target_size;
											 %Set the locks
											 lock_map(rb_id,1) = appended_lock;
											 %TODO: Adjust the content of the merged branch
											 curve_graph_content{rb_id,1} = unique([curve_graph_content{rb_id,1}; appended_graph_content],'rows');
										 %If we're close to the ending and      
										 %If our native branch is not long 
										 %enough, and the new branch attaches to 
										 %only 1 existing branch, then elongate
										 elseif(size_ending_branch<size_beginning_branch && (~is_two_junctions || ~is_inverted) && ~target_lock_final && (size_ending_branch<3 || get_length(target_curve(arr_junction_indices(rb_id,1):target_size,:)) < branch_length_threshold))
											 if(flip_before_second_append)
												 appended_curve = flipud(appended_curve);
											 end
											 target_curve = [target_curve(1:arr_junction_indices(rb_id,1),:); flipud(appended_curve)];
											 curve_graph{rb_id,1} = target_curve; 
											 is_final_elongated = 1;
											 final_target_min = 1;
											 final_target_max = arr_junction_indices(rb_id,1);
											 %Set the locks
											 lock_map(rb_id,2) = appended_lock;
											 %TODO: Adjust the content of the merged branch
											 curve_graph_content{rb_id,1} = unique([curve_graph_content{rb_id,1}; appended_graph_content],'rows');
										 end
									 end
									 
									 at_least_one_branch_broken = 0;
									 %If both sides are elongated,
									 %delete the earlier branch
									 if(is_init_elongated && is_final_elongated)
										 curve_graph{init_branch_id,1} = [];
										 curve_graph_content{init_branch_id,1} = [];
										 lock_map(init_branch_id,:) = [0 0];

									 %Break init at junctions if not
									 %elongated
									 elseif(~is_init_elongated && is_final_elongated && ~no_init_junction)

										 %Go over each relevant branch to
										 %process the junctions of each
										 for rb=1:num_attachments_last

											 rb_id = arr_branch_id_last(1,rb);

											 target_curve = curve_graph{rb_id,1};
											 target_size = size(target_curve,1);
											 
%                                                  %Break only if the
%                                                  %relevant segment remains
%                                                  if(arr_junction_indices_last(rb_id,1)<final_target_min || arr_junction_indices_last(rb_id,1)>final_target_max)
%                                                      continue;
%                                                  end
											 at_least_one_branch_broken = 1;

											 %Junction breaks the existing branch it's
											 %being attached to
											 target_curve1 = target_curve(1:arr_junction_indices_last(rb_id,1),:);
											 target_curve2 = target_curve(arr_junction_indices_last(rb_id,1):target_size,:);

											 %Add the broken pieces
											 curve_graph{rb_id,1} = target_curve1;
											 curve_graph = [curve_graph; cell(1,1)];
											 cg_size = size(curve_graph,1);
											 curve_graph{cg_size,1} = target_curve2;
											 %Set the junction locks for the new branches
											 cur_locks = lock_map(rb_id,:);
											 cur_lock_init = cur_locks(1,1);
											 cur_lock_final = cur_locks(1,2);
											 lock_map(rb_id,:) = [cur_lock_init 1];
											 lock_map = [lock_map; [1 cur_lock_final]];
											 
											 %Set the contents of the new branch
											 curve_graph_content = [curve_graph_content; cell(1,1)];
											 curve_graph_content{cg_size,1} = curve_graph_content{rb_id,1};
											 %Set the correspondence flags for the new branch
											 merged_corresp_mask_all = [merged_corresp_mask_all; cell(1,1)];
											 merged_corresp_mask_all{cg_size,1} = merged_corresp_mask_all{rb_id,1};
											 %Set the equivalence table for the new branch
											 equiv_table_cur{rb_id,1} = [equiv_table_cur{rb_id,1} cg_size];
											 equiv_table_cur = [equiv_table_cur; cell(1,1)];
											 equiv_table_cur{cg_size,1} = [equiv_table_cur{cg_size,1} rb_id];
										 end
										 
										 %Add the junction if it's not added already
										 if(~is_last_junction_saved && at_least_one_branch_broken)
											 all_junctions = [all_junctions; queryCurve(new_branch_start-1,:)];
											 is_last_junction_saved = 1;
										 end
									 elseif(is_init_elongated && ~is_final_elongated)
										 
										 curve_graph{init_branch_id,1} = appended_curve;
										 curve_graph_content{init_branch_id,1} = appended_graph_content;

										 %Go over each relevant branch to
										 %process the junctions of each
										 for rb=1:num_attachments_cur

											 rb_id = arr_branch_id_cur(1,rb);

											 target_curve = curve_graph{rb_id,1};
											 target_size = size(target_curve,1);
											 
%                                                  %Break only if the
%                                                  %relevant segment remains
%                                                  if(arr_junction_indices(rb_id,1)<init_target_min || arr_junction_indices(rb_id,1)>init_target_max)
%                                                      continue;
%                                                  end
											 at_least_one_branch_broken = 1;
											 
											 size_beginning_branch = arr_junction_indices(rb_id,1);
											 size_ending_branch = target_size - arr_junction_indices(rb_id,1) + 1;

											 %Junction breaks the existing branch it's
											 %being attached to
											 target_curve1 = target_curve(1:arr_junction_indices(rb_id,1),:);
											 target_curve2 = target_curve(arr_junction_indices(rb_id,1):target_size,:);

											 %Add the broken pieces
											 curve_graph{rb_id,1} = target_curve1;
											 curve_graph = [curve_graph; cell(1,1)];
											 cg_size = size(curve_graph,1);
											 curve_graph{cg_size,1} = target_curve2;
											 %Set the junction locks for the new branches
											 cur_locks = lock_map(rb_id,:);
											 cur_lock_init = cur_locks(1,1);
											 cur_lock_final = cur_locks(1,2);
											 lock_map(rb_id,:) = [cur_lock_init 1];
											 lock_map = [lock_map; [1 cur_lock_final]];
											 
											 %Set the contents of the new branch
											 curve_graph_content = [curve_graph_content; cell(1,1)];
											 curve_graph_content{cg_size,1} = curve_graph_content{rb_id,1};
											 %Set the correspondence flags for the new branch
											 merged_corresp_mask_all = [merged_corresp_mask_all; cell(1,1)];
											 merged_corresp_mask_all{cg_size,1} = merged_corresp_mask_all{rb_id,1};
											 %Set the equivalence table for the new branch
											 equiv_table_cur{rb_id,1} = [equiv_table_cur{rb_id,1} cg_size];
											 equiv_table_cur = [equiv_table_cur; cell(1,1)];
											 equiv_table_cur{cg_size,1} = [equiv_table_cur{cg_size,1} rb_id];
										 end
										 
										 %Add the junction if it's not added already
										 if(~is_junction_saved && at_least_one_branch_broken)
											all_junctions = [all_junctions; queryCurve(ms,:)];
											is_junction_saved = 1;
										 end
									 elseif(~is_init_elongated && ~is_final_elongated && (query_branch_size > branch_size_threshold || get_length(queryCurve(new_branch_start:ms,:)) > branch_length_threshold))
									 
										 if(no_init_junction)
											 new_branch = queryCurve(new_branch_start:ms,:);
											 all_new_branches = [all_new_branches; cell(1,1)];
											 anb_size = size(all_new_branches,1);
											 all_new_branches{anb_size,1} = new_branch;
											 all_new_locks = [all_new_locks; [0 1]];
										 else
											 new_branch = queryCurve(new_branch_start-1:ms,:);
											 all_new_branches = [all_new_branches; cell(1,1)];
											 anb_size = size(all_new_branches,1);
											 all_new_branches{anb_size,1} = new_branch; 
											 all_new_locks = [all_new_locks; [1 1]];
											 
											 %Add the junction if it's not added already
											 if(~is_last_junction_saved)
												all_junctions = [all_junctions; queryCurve(new_branch_start-1,:)];
												is_last_junction_saved = 1;
											 end
										 end

										 %Add the junction if it's not added already
										 if(~is_junction_saved)
											all_junctions = [all_junctions; queryCurve(ms,:)];
											is_junction_saved = 1;
										 end

										 for b=1:num_branches

											 if(size(curve_graph{b,1})<=1)
												 continue;
											 end
											 
											 target_curve = curve_graph{b,1};
											 target_size = size(target_curve,1);

											 ind_last = find(arr_branch_id_last==b);
											 ind_cur = find(arr_branch_id_cur==b);

											 %If found in both arrays, this
											 %branch will be broken in two
											 %places
											 if((~isempty(ind_last)) && (~isempty(ind_cur)) && (~no_init_junction))

												 target_curve1 = [];
												 target_curve2 = [];
												 target_curve3 = [];
												 %Junction breaks the existing branch it's
												 %being attached to
												 if(arr_junction_indices_last(b,1) < arr_junction_indices(b,1))
													 target_curve1 = target_curve(1:arr_junction_indices_last(b,1),:);
													 target_curve2 = target_curve(arr_junction_indices_last(b,1):arr_junction_indices(b,1),:);
													 target_curve3 = target_curve(arr_junction_indices(b,1):target_size,:);
												 else
													 target_curve1 = target_curve(1:arr_junction_indices(b,1),:);
													 target_curve2 = target_curve(arr_junction_indices(b,1):arr_junction_indices_last(b,1),:);
													 target_curve3 = target_curve(arr_junction_indices_last(b,1):target_size,:);
												 end
																									  
												 cur_locks = lock_map(b,:);
												 cur_lock_init = cur_locks(1,1);
												 cur_lock_final = cur_locks(1,2);
												 
												 %Add the broken pieces
												 curve_graph{b,1} = target_curve2;
												 %Set the junction locks for the new branches
												 lock_map(rb_id,:) = [1 1];

												 curve_graph = [curve_graph; cell(1,1)];
												 cg_size = size(curve_graph,1);
												 curve_graph{cg_size,1} = target_curve1;
												 %Set the junction locks for the new branches
												 lock_map = [lock_map; [cur_lock_init 1]];
												 %Set the contents of the new branch
												 curve_graph_content = [curve_graph_content; cell(1,1)];
												 curve_graph_content{cg_size,1} = curve_graph_content{b,1};
												 %Set the correspondence flags for the new branch
												 merged_corresp_mask_all = [merged_corresp_mask_all; cell(1,1)];
												 merged_corresp_mask_all{cg_size,1} = merged_corresp_mask_all{b,1};
												 %Set the equivalence table for the new branch
												 equiv_table_cur{b,1} = [equiv_table_cur{b,1} cg_size];
												 equiv_table_cur = [equiv_table_cur; cell(1,1)];
												 equiv_table_cur{cg_size,1} = [equiv_table_cur{cg_size,1} b];

												 curve_graph = [curve_graph; cell(1,1)];
												 cg_size = size(curve_graph,1);
												 curve_graph{cg_size,1} = target_curve3;
												 %Set the junction locks for the new branches
												 lock_map = [lock_map; [1 cur_lock_final]];
												 %Set the contents of the new branch
												 curve_graph_content = [curve_graph_content; cell(1,1)];
												 curve_graph_content{cg_size,1} = curve_graph_content{b,1};
												 %Set the correspondence flags for the new branch
												 merged_corresp_mask_all = [merged_corresp_mask_all; cell(1,1)];
												 merged_corresp_mask_all{cg_size,1} = merged_corresp_mask_all{b,1};
												 %Set the equivalence table for the new branch
												 equiv_table_cur{b,1} = [equiv_table_cur{b,1} cg_size];
												 equiv_table_cur = [equiv_table_cur; cell(1,1)];
												 equiv_table_cur{cg_size,1} = [equiv_table_cur{cg_size,1} b];
												 equiv_table_cur{cg_size,1} = [equiv_table_cur{cg_size,1} cg_size-1];
												 equiv_table_cur{cg_size-1,1} = [equiv_table_cur{cg_size-1,1} cg_size];

											 %Otherwise, break at the only
											 %existing connection
											 elseif(~isempty(ind_last) && (~no_init_junction))

												 %Junction breaks the existing branch it's
												 %being attached to
												 target_curve1 = target_curve(1:arr_junction_indices_last(b,1),:);
												 target_curve2 = target_curve(arr_junction_indices_last(b,1):target_size,:);

												 %Add the broken pieces
												 curve_graph{b,1} = target_curve1;
												 curve_graph = [curve_graph; cell(1,1)];
												 cg_size = size(curve_graph,1);
												 curve_graph{cg_size,1} = target_curve2;
												 %Set the junction locks for the new branches
												 cur_locks = lock_map(b,:);
												 cur_lock_init = cur_locks(1,1);
												 cur_lock_final = cur_locks(1,2);
												 lock_map(b,:) = [cur_lock_init 1];
												 lock_map = [lock_map; [1 cur_lock_final]];

												 %Set the contents of the new branch
												 curve_graph_content = [curve_graph_content; cell(1,1)];
												 curve_graph_content{cg_size,1} = curve_graph_content{rb_id,1};
												 %Set the correspondence flags for the new branch
												 merged_corresp_mask_all = [merged_corresp_mask_all; cell(1,1)];
												 merged_corresp_mask_all{cg_size,1} = merged_corresp_mask_all{rb_id,1};
												 %Set the equivalence table for the new branch
												 equiv_table_cur{rb_id,1} = [equiv_table_cur{rb_id,1} cg_size];
												 equiv_table_cur = [equiv_table_cur; cell(1,1)];
												 equiv_table_cur{cg_size,1} = [equiv_table_cur{cg_size,1} rb_id];

											 elseif(~isempty(ind_cur))

												 %Junction breaks the existing branch it's
												 %being attached to
												 target_curve1 = target_curve(1:arr_junction_indices(b,1),:);
												 target_curve2 = target_curve(arr_junction_indices(b,1):target_size,:);

												 %Add the broken pieces
												 curve_graph{b,1} = target_curve1;
												 curve_graph = [curve_graph; cell(1,1)];
												 cg_size = size(curve_graph,1);
												 curve_graph{cg_size,1} = target_curve2;
												 %Set the junction locks for the new branches
												 cur_locks = lock_map(b,:);
												 cur_lock_init = cur_locks(1,1);
												 cur_lock_final = cur_locks(1,2);
												 lock_map(b,:) = [cur_lock_init 1];
												 lock_map = [lock_map; [1 cur_lock_final]];

												 %Set the contents of the new branch
												 curve_graph_content = [curve_graph_content; cell(1,1)];
												 curve_graph_content{cg_size,1} = curve_graph_content{rb_id,1};
												 %Set the correspondence flags for the new branch
												 merged_corresp_mask_all = [merged_corresp_mask_all; cell(1,1)];
												 merged_corresp_mask_all{cg_size,1} = merged_corresp_mask_all{rb_id,1};
												 %Set the equivalence table for the new branch
												 equiv_table_cur{rb_id,1} = [equiv_table_cur{rb_id,1} cg_size];
												 equiv_table_cur = [equiv_table_cur; cell(1,1)];
												 equiv_table_cur{cg_size,1} = [equiv_table_cur{cg_size,1} rb_id];
												 
											 end
										 end
									 end
								 else       
									 
									 is_elongated = 0;
									 
									 elong_curve = [];
									 other_curve1 = [];
									 other_curve2 = [];
									 
									 elong_lock = [];
									 other_lock1 = [];
									 other_lock2 = [];
																		 
									 %CHECK FOR ELONGATION AT (A): YES
									 if(num_attachments_last==1 && ~no_init_junction)
										 
																					 
										 %Retrieve target branches and
										 %curves as if the new branch
										 %only attach
										 rb_id = arr_branch_id_last(1,1);

										 target_curve = curve_graph{rb_id,1};
										 target_size = size(target_curve,1);
										 target_lock_init = lock_map(rb_id,1);
										 target_lock_final = lock_map(rb_id,2);

										 size_beginning_branch = arr_junction_indices_last(rb_id,1);
										 size_ending_branch = target_size - arr_junction_indices_last(rb_id,1) + 1;
										 
										 is_inverted = 0;
										 %We first need to break the
										 %relevant branch at the next
										 %junction
										 if(arr_junction_indices_last(rb_id,1) < arr_junction_indices(rb_id,1))
											 elong_curve = target_curve(arr_junction_indices_last(rb_id,1):arr_junction_indices(rb_id,1),:);
											 other_curve1 = target_curve(1:arr_junction_indices_last(rb_id,1),:);
											 other_curve2 = target_curve(arr_junction_indices(rb_id,1):target_size,:);
										 else
											 elong_curve = target_curve(arr_junction_indices(rb_id,1):arr_junction_indices_last(rb_id,1),:);
											 other_curve1 = target_curve(1:arr_junction_indices(rb_id,1),:);
											 other_curve2 = target_curve(arr_junction_indices_last(rb_id,1):target_size,:);
											 is_inverted = 1;
										 end
									 
										 %If we're close to the
										 %beginning on this branch and
										 %If our native branch is not long 
										 %enough, and the new branch attaches to 
										 %only 1 existing branch, then elongate
										 if(size_beginning_branch<size_ending_branch && ~is_inverted && ~target_lock_init && (size_beginning_branch<3  || get_length(other_curve1) < branch_length_threshold))
											 target_curve = [flipud(queryCurve(new_branch_start:ms,:)); elong_curve];
											 curve_graph{rb_id,1} = target_curve; 
											 is_elongated = 1;
											 %Set the locks
											 lock_map(rb_id,:) = [1 1];
											 %Add the junction if it's not added already
											 if(~is_junction_saved)
												all_junctions = [all_junctions; queryCurve(ms,:)];
												is_junction_saved = 1;
											 end
											 
											 curve_graph = [curve_graph; cell(1,1)];
											 cg_size = size(curve_graph,1);
											 curve_graph{cg_size,1} = other_curve2;
											 %Set the locks
											 lock_map = [lock_map; [1 target_lock_final]];
											 %Set the contents of the new branch
											 curve_graph_content = [curve_graph_content; cell(1,1)];
											 curve_graph_content{cg_size,1} = curve_graph_content{rb_id,1};
											 %Set the correspondence flags for the new branch
											 merged_corresp_mask_all = [merged_corresp_mask_all; cell(1,1)];
											 merged_corresp_mask_all{cg_size,1} = merged_corresp_mask_all{rb_id,1};
											 %Set the equivalence table for the new branch
											 equiv_table_cur{rb_id,1} = [equiv_table_cur{rb_id,1} cg_size];
											 equiv_table_cur = [equiv_table_cur; cell(1,1)];
											 equiv_table_cur{cg_size,1} = [equiv_table_cur{cg_size,1} rb_id];
											 
											 %TODO: Adjust the content of the merged branch
											 curve_graph_content{rb_id,1} = unique([curve_graph_content{rb_id,1}; [all_views(1,mrv)+1 cc]],'rows');
											 
										 %If we're close to the ending and      
										 %If our native branch is not long 
										 %enough, and the new branch attaches to 
										 %only 1 existing branch, then elongate
										 elseif(size_ending_branch<size_beginning_branch && is_inverted && ~target_lock_final && (size_ending_branch<3 || get_length(other_curve2) < branch_length_threshold))
											 target_curve = [elong_curve; queryCurve(new_branch_start:ms,:)];
											 curve_graph{rb_id,1} = target_curve; 
											 is_elongated = 1;
											 %Set the locks
											 lock_map(rb_id,:) = [1 1];
											 %Add the junction if it's not added already
											 if(~is_junction_saved)
												all_junctions = [all_junctions; queryCurve(ms,:)];
												is_junction_saved = 1;
											 end
											 
											 curve_graph = [curve_graph; cell(1,1)];
											 cg_size = size(curve_graph,1);
											 curve_graph{cg_size,1} = other_curve1;
											 %Set the locks
											 lock_map = [lock_map; [target_lock_init 1]];
											 %Set the contents of the new branch
											 curve_graph_content = [curve_graph_content; cell(1,1)];
											 curve_graph_content{cg_size,1} = curve_graph_content{rb_id,1};
											 %Set the correspondence flags for the new branch
											 merged_corresp_mask_all = [merged_corresp_mask_all; cell(1,1)];
											 merged_corresp_mask_all{cg_size,1} = merged_corresp_mask_all{rb_id,1};
											 %Set the equivalence table for the new branch
											 equiv_table_cur{rb_id,1} = [equiv_table_cur{rb_id,1} cg_size];
											 equiv_table_cur = [equiv_table_cur; cell(1,1)];
											 equiv_table_cur{cg_size,1} = [equiv_table_cur{cg_size,1} rb_id];
											 
											 %TODO: Adjust the content of the merged branch
											 curve_graph_content{rb_id,1} = unique([curve_graph_content{rb_id,1}; [all_views(1,mrv)+1 cc]],'rows');
											 
										 end
										 
										 if(is_elongated)
											 %Break all the extra branches
											 %(B) connects to
											 for rb=1:num_attachments_cur

												 rb_id = arr_branch_id_cur(1,rb);

												 if(rb_id==arr_branch_id_last(1,1))
													 continue;
												 end
												 disp('WARNING: Branches should be identical, something is wrong!');

												 target_curve = curve_graph{rb_id,1};
												 target_size = size(target_curve,1);

												 size_beginning_branch = arr_junction_indices(rb_id,1);
												 size_ending_branch = target_size - arr_junction_indices(rb_id,1) + 1;

												 %Junction breaks the existing branch it's
												 %being attached to
												 target_curve1 = target_curve(1:arr_junction_indices(rb_id,1),:);
												 target_curve2 = target_curve(arr_junction_indices(rb_id,1):target_size,:);
												 
												 %Add the broken pieces
												 curve_graph{rb_id,1} = target_curve1;
												 curve_graph = [curve_graph; cell(1,1)];
												 cg_size = size(curve_graph,1);
												 curve_graph{cg_size,1} = target_curve2;
												 %Set the junction locks for the new branches
												 cur_locks = lock_map(rb_id,:);
												 cur_lock_init = cur_locks(1,1);
												 cur_lock_final = cur_locks(1,2);
												 lock_map(rb_id,:) = [cur_lock_init 1];
												 lock_map = [lock_map; [1 cur_lock_final]];

												 %Set the contents of the new branch
												 curve_graph_content = [curve_graph_content; cell(1,1)];
												 curve_graph_content{cg_size,1} = curve_graph_content{rb_id,1};
												 %Set the correspondence flags for the new branch
												 merged_corresp_mask_all = [merged_corresp_mask_all; cell(1,1)];
												 merged_corresp_mask_all{cg_size,1} = merged_corresp_mask_all{rb_id,1};
												 %Set the equivalence table for the new branch
												 equiv_table_cur{rb_id,1} = [equiv_table_cur{rb_id,1} cg_size];
												 equiv_table_cur = [equiv_table_cur; cell(1,1)];
												 equiv_table_cur{cg_size,1} = [equiv_table_cur{cg_size,1} rb_id];
											 end
										 end
									 end
									 
									 %CHECK FOR ELONGATION AT (A): NO
									 %CHECK FOR ELONGATION AT (B): YES
									 if(num_attachments_cur==1 && (~is_elongated))
										 
										 %Retrieve target branches and
										 %curves as if the new branch
										 %only attach
										 rb_id = arr_branch_id_cur(1,1);
										 
										 
										 
										 target_curve = curve_graph{rb_id,1};
										 target_size = size(target_curve,1);
										 target_lock_init = lock_map(rb_id,1);
										 target_lock_final = lock_map(rb_id,2);
										 
										 size_beginning_branch = arr_junction_indices(rb_id,1);
										 size_ending_branch = target_size - arr_junction_indices(rb_id,1) + 1;
										 
										 is_inverted = 0;
										 %We first need to break the
										 %relevant branch at the next
										 %junction
										 if(arr_junction_indices_last(rb_id,1) < arr_junction_indices(rb_id,1))
											 if(no_init_junction)
												 elong_curve = target_curve(1:arr_junction_indices(rb_id,1),:);
												 other_curve2 = target_curve(arr_junction_indices(rb_id,1):target_size,:);
											 else
												 elong_curve = target_curve(arr_junction_indices_last(rb_id,1):arr_junction_indices(rb_id,1),:);
												 other_curve1 = target_curve(1:arr_junction_indices_last(rb_id,1),:);
												 other_curve2 = target_curve(arr_junction_indices(rb_id,1):target_size,:);
											 end
										 else
											 if(no_init_junction)
												 elong_curve = target_curve(arr_junction_indices(rb_id,1):target_size,:);
												 other_curve1 = target_curve(1:arr_junction_indices(rb_id,1),:);
											 else
												 elong_curve = target_curve(arr_junction_indices(rb_id,1):arr_junction_indices_last(rb_id,1),:);
												 other_curve1 = target_curve(1:arr_junction_indices(rb_id,1),:);
												 other_curve2 = target_curve(arr_junction_indices_last(rb_id,1):target_size,:);
											 end
											 is_inverted = 1;
										 end

										 %If we're close to the
										 %beginning on this branch and
										 %If our native branch is not long 
										 %enough, and the new branch attaches to 
										 %only 1 existing branch, then elongate
										 if(size_beginning_branch<size_ending_branch && is_inverted && ~target_lock_init && (size_beginning_branch<3 || get_length(other_curve1) < branch_length_threshold))
											 if(no_init_junction)
												 target_curve = [queryCurve(new_branch_start:ms-1,:); elong_curve];
												 %Set the locks
												 lock_map(rb_id,:) = [0 target_lock_final];
											 else
												 target_curve = [queryCurve(new_branch_start-1:ms-1,:); elong_curve];
												 %Set the locks
												 lock_map(rb_id,:) = [1 1];
											 end
											 curve_graph{rb_id,1} = target_curve;
											 is_elongated = 1;
											 
											 %Add the junction if it's not added already
											 if(~is_last_junction_saved && ~no_init_junction)
												all_junctions = [all_junctions; queryCurve(new_branch_start-1,:)];
												is_last_junction_saved = 1;
											 end

											 if(~isempty(other_curve2))
												 curve_graph = [curve_graph; cell(1,1)];
												 cg_size = size(curve_graph,1);
												 curve_graph{cg_size,1} = other_curve2;
												 %Set the locks
												 lock_map = [lock_map; [1 target_lock_final]];
												 %Set the contents of the new branch
												 curve_graph_content = [curve_graph_content; cell(1,1)];
												 curve_graph_content{cg_size,1} = curve_graph_content{rb_id,1};
												 %Set the correspondence flags for the new branch
												 merged_corresp_mask_all = [merged_corresp_mask_all; cell(1,1)];
												 merged_corresp_mask_all{cg_size,1} = merged_corresp_mask_all{rb_id,1};
												 %Set the equivalence table for the new branch
												 equiv_table_cur{rb_id,1} = [equiv_table_cur{rb_id,1} cg_size];
												 equiv_table_cur = [equiv_table_cur; cell(1,1)];
												 equiv_table_cur{cg_size,1} = [equiv_table_cur{cg_size,1} rb_id];
											 end

											 %TODO: Adjust the content of the merged branch
											 curve_graph_content{rb_id,1} = unique([curve_graph_content{rb_id,1}; [all_views(1,mrv)+1 cc]],'rows');

										 %If we're close to the ending and      
										 %If our native branch is not long 
										 %enough, and the new branch attaches to 
										 %only 1 existing branch, then elongate
										 elseif(size_ending_branch<size_beginning_branch && ~is_inverted && ~target_lock_final && (size_ending_branch<3 || get_length(other_curve2) < branch_length_threshold))
											 if(no_init_junction)
												 target_curve = [elong_curve; flipud(queryCurve(new_branch_start:ms-1,:))];
												 %Set the locks
												 lock_map(rb_id,:) = [target_lock_init 0];
											 else
												 target_curve = [elong_curve; flipud(queryCurve(new_branch_start-1:ms-1,:))];
												 %Set the locks
												 lock_map(rb_id,:) = [1 1];
											 end
											 curve_graph{rb_id,1} = target_curve; 
											 is_elongated = 1;
											 %Add the junction if it's not added already
											 if(~is_last_junction_saved && ~no_init_junction)
												 all_junctions = [all_junctions; queryCurve(new_branch_start-1,:)];
												 is_last_junction_saved = 1;
											 end
											 
											 

											 if(~isempty(other_curve1))
												 curve_graph = [curve_graph; cell(1,1)];
												 cg_size = size(curve_graph,1);
												 curve_graph{cg_size,1} = other_curve1;
												 %Set the locks
												 lock_map = [lock_map; [target_lock_init 1]];
												 %Set the contents of the new branch
												 curve_graph_content = [curve_graph_content; cell(1,1)];
												 curve_graph_content{cg_size,1} = curve_graph_content{rb_id,1};
												 %Set the correspondence flags for the new branch
												 merged_corresp_mask_all = [merged_corresp_mask_all; cell(1,1)];
												 merged_corresp_mask_all{cg_size,1} = merged_corresp_mask_all{rb_id,1};
												 %Set the equivalence table for the new branch
												 equiv_table_cur{rb_id,1} = [equiv_table_cur{rb_id,1} cg_size];
												 equiv_table_cur = [equiv_table_cur; cell(1,1)];
												 equiv_table_cur{cg_size,1} = [equiv_table_cur{cg_size,1} rb_id];
											 end

											 %TODO: Adjust the content of the merged branch
											 curve_graph_content{rb_id,1} = unique([curve_graph_content{rb_id,1}; [all_views(1,mrv)+1 cc]],'rows');

										 end
										 
										 if(is_elongated && ~no_init_junction)
											 %Break all the extra branches
											 %(A) connects to
											 for rb=1:num_attachments_last

												 rb_id = arr_branch_id_last(1,rb);

												 if(rb_id==arr_branch_id_cur(1,1))
													 continue;
												 end
												 disp('WARNING: Branches should be identical, something is wrong!');

												 target_curve = curve_graph{rb_id,1};
												 target_size = size(target_curve,1);

												 size_beginning_branch = arr_junction_indices_last(rb_id,1);
												 size_ending_branch = target_size - arr_junction_indices_last(rb_id,1) + 1;

												 %Junction breaks the existing branch it's
												 %being attached to
												 target_curve1 = target_curve(1:arr_junction_indices_last(rb_id,1),:);
												 target_curve2 = target_curve(arr_junction_indices_last(rb_id,1):target_size,:);

												 %Add the broken pieces
												 curve_graph{rb_id,1} = target_curve1;
												 curve_graph = [curve_graph; cell(1,1)];
												 cg_size = size(curve_graph,1);
												 curve_graph{cg_size,1} = target_curve2;
												 %Set the junction locks for the new branches
												 cur_locks = lock_map(rb_id,:);
												 cur_lock_init = cur_locks(1,1);
												 cur_lock_final = cur_locks(1,2);
												 lock_map(rb_id,:) = [cur_lock_init 1];
												 lock_map = [lock_map; [1 cur_lock_final]];

												 %Set the contents of the new branch
												 curve_graph_content = [curve_graph_content; cell(1,1)];
												 curve_graph_content{cg_size,1} = curve_graph_content{rb_id,1};
												 %Set the correspondence flags for the new branch
												 merged_corresp_mask_all = [merged_corresp_mask_all; cell(1,1)];
												 merged_corresp_mask_all{cg_size,1} = merged_corresp_mask_all{rb_id,1};
												 %Set the equivalence table for the new branch
												 equiv_table_cur{rb_id,1} = [equiv_table_cur{rb_id,1} cg_size];
												 equiv_table_cur = [equiv_table_cur; cell(1,1)];
												 equiv_table_cur{cg_size,1} = [equiv_table_cur{cg_size,1} rb_id];
											 
											 end
										 end
									 end
									 
									 %CHECK FOR ELONGATION AT (B): NO
									 if(~is_elongated && (query_branch_size > branch_size_threshold || get_length(queryCurve(new_branch_start:ms,:)) > branch_length_threshold))
										 if(no_init_junction)
											 %Create and add the new branch
											 new_branch = queryCurve(new_branch_start:ms,:);
											 all_new_branches = [all_new_branches; cell(1,1)];
											 anb_size = size(all_new_branches,1);
											 all_new_branches{anb_size,1} = new_branch;
											 all_new_locks = [all_new_locks; [0 1]];
										 else
											 %Create and add the new branch
											 new_branch = queryCurve(new_branch_start-1:ms,:);
											 all_new_branches = [all_new_branches; cell(1,1)];
											 anb_size = size(all_new_branches,1);
											 all_new_branches{anb_size,1} = new_branch;
											 all_new_locks = [all_new_locks; [1 1]];

											 %Add the junction if it's not added already
											 if(~is_last_junction_saved)
												all_junctions = [all_junctions; queryCurve(new_branch_start-1,:)];
												is_last_junction_saved = 1;
											 end
										 end

										 %Add the junction if it's not added already
										 if(~is_junction_saved)
											all_junctions = [all_junctions; queryCurve(ms,:)];
											is_junction_saved = 1;
										 end

										 %Go over each branch to see if it
										 %needs to be broken at up to 2
										 %points

										 for b=1:num_branches
											 
											 if(size(curve_graph{b,1})<=1)
												 continue;
											 end

											 target_curve = curve_graph{b,1};
											 target_size = size(target_curve,1);

											 ind_last = find(arr_branch_id_last==b);
											 ind_cur = find(arr_branch_id_cur==b);

											 %If found in both arrays, this
											 %branch will be broken in two
											 %places
											 if((~isempty(ind_last)) && (~isempty(ind_cur)) &&(~no_init_junction))

												 target_curve1 = [];
												 target_curve2 = [];
												 target_curve3 = [];
												 %Junction breaks the existing branch it's
												 %being attached to
												 if(arr_junction_indices_last(b,1) < arr_junction_indices(b,1))
													 target_curve1 = target_curve(1:arr_junction_indices_last(b,1),:);
													 target_curve2 = target_curve(arr_junction_indices_last(b,1):arr_junction_indices(b,1),:);
													 target_curve3 = target_curve(arr_junction_indices(b,1):target_size,:);
												 else
													 target_curve1 = target_curve(1:arr_junction_indices(b,1),:);
													 target_curve2 = target_curve(arr_junction_indices(b,1):arr_junction_indices_last(b,1),:);
													 target_curve3 = target_curve(arr_junction_indices_last(b,1):target_size,:);
												 end
												 
												 cur_locks = lock_map(b,:);
												 cur_lock_init = cur_locks(1,1);
												 cur_lock_final = cur_locks(1,2);
												 
												 %Add the broken pieces
												 curve_graph{b,1} = target_curve2;
												 %Set the junction locks for the new branches
												 lock_map(rb_id,:) = [1 1];

												 curve_graph = [curve_graph; cell(1,1)];
												 cg_size = size(curve_graph,1);
												 curve_graph{cg_size,1} = target_curve1;
												 %Set the junction locks for the new branches
												 lock_map = [lock_map; [cur_lock_init 1]];
												 %Set the contents of the new branch
												 curve_graph_content = [curve_graph_content; cell(1,1)];
												 curve_graph_content{cg_size,1} = curve_graph_content{b,1};
												 %Set the correspondence flags for the new branch
												 merged_corresp_mask_all = [merged_corresp_mask_all; cell(1,1)];
												 merged_corresp_mask_all{cg_size,1} = merged_corresp_mask_all{b,1};
												 %Set the equivalence table for the new branch
												 equiv_table_cur{b,1} = [equiv_table_cur{b,1} cg_size];
												 equiv_table_cur = [equiv_table_cur; cell(1,1)];
												 equiv_table_cur{cg_size,1} = [equiv_table_cur{cg_size,1} b];

												 curve_graph = [curve_graph; cell(1,1)];
												 cg_size = size(curve_graph,1);
												 curve_graph{cg_size,1} = target_curve3;
												 %Set the junction locks for the new branches
												 lock_map = [lock_map; [1 cur_lock_final]];
												 %Set the contents of the new branch
												 curve_graph_content = [curve_graph_content; cell(1,1)];
												 curve_graph_content{cg_size,1} = curve_graph_content{b,1};
												 %Set the correspondence flags for the new branch
												 merged_corresp_mask_all = [merged_corresp_mask_all; cell(1,1)];
												 merged_corresp_mask_all{cg_size,1} = merged_corresp_mask_all{b,1};
												 %Set the equivalence table for the new branch
												 equiv_table_cur{b,1} = [equiv_table_cur{b,1} cg_size];
												 equiv_table_cur = [equiv_table_cur; cell(1,1)];
												 equiv_table_cur{cg_size,1} = [equiv_table_cur{cg_size,1} b];
												 equiv_table_cur{cg_size,1} = [equiv_table_cur{cg_size,1} cg_size-1];
												 equiv_table_cur{cg_size-1,1} = [equiv_table_cur{cg_size-1,1} cg_size];

											 %Otherwise, break at the only
											 %existing connection
											 elseif(~isempty(ind_last) && ~no_init_junction)

												 %Junction breaks the existing branch it's
												 %being attached to
												 target_curve1 = target_curve(1:arr_junction_indices_last(b,1),:);
												 target_curve2 = target_curve(arr_junction_indices_last(b,1):target_size,:);

												 %Add the broken pieces
												 curve_graph{b,1} = target_curve1;
												 curve_graph = [curve_graph; cell(1,1)];
												 cg_size = size(curve_graph,1);
												 curve_graph{cg_size,1} = target_curve2;
												 %Set the junction locks for the new branches
												 cur_locks = lock_map(b,:);
												 cur_lock_init = cur_locks(1,1);
												 cur_lock_final = cur_locks(1,2);
												 lock_map(b,:) = [cur_lock_init 1];
												 lock_map = [lock_map; [1 cur_lock_final]];

												 %Set the contents of the new branch
												 curve_graph_content = [curve_graph_content; cell(1,1)];
												 curve_graph_content{cg_size,1} = curve_graph_content{rb_id,1};
												 %Set the correspondence flags for the new branch
												 merged_corresp_mask_all = [merged_corresp_mask_all; cell(1,1)];
												 merged_corresp_mask_all{cg_size,1} = merged_corresp_mask_all{rb_id,1};
												 %Set the equivalence table for the new branch
												 equiv_table_cur{rb_id,1} = [equiv_table_cur{rb_id,1} cg_size];
												 equiv_table_cur = [equiv_table_cur; cell(1,1)];
												 equiv_table_cur{cg_size,1} = [equiv_table_cur{cg_size,1} rb_id];

											 elseif(~isempty(ind_cur))

												 %Junction breaks the existing branch it's
												 %being attached to
												 target_curve1 = target_curve(1:arr_junction_indices(b,1),:);
												 target_curve2 = target_curve(arr_junction_indices(b,1):target_size,:);

												 %Add the broken pieces
												 curve_graph{b,1} = target_curve1;
												 curve_graph = [curve_graph; cell(1,1)];
												 cg_size = size(curve_graph,1);
												 curve_graph{cg_size,1} = target_curve2;
												 %Set the junction locks for the new branches
												 cur_locks = lock_map(b,:);
												 cur_lock_init = cur_locks(1,1);
												 cur_lock_final = cur_locks(1,2);
												 lock_map(b,:) = [cur_lock_init 1];
												 lock_map = [lock_map; [1 cur_lock_final]];

												 %Set the contents of the new branch
												 curve_graph_content = [curve_graph_content; cell(1,1)];
												 curve_graph_content{cg_size,1} = curve_graph_content{rb_id,1};
												 %Set the correspondence flags for the new branch
												 merged_corresp_mask_all = [merged_corresp_mask_all; cell(1,1)];
												 merged_corresp_mask_all{cg_size,1} = merged_corresp_mask_all{rb_id,1};
												 %Set the equivalence table for the new branch
												 equiv_table_cur{rb_id,1} = [equiv_table_cur{rb_id,1} cg_size];
												 equiv_table_cur = [equiv_table_cur; cell(1,1)];
												 equiv_table_cur{cg_size,1} = [equiv_table_cur{cg_size,1} rb_id];
												 
											 end
										 end
									 end
								 end
                                 
                             else 
                                 
                                 %If this sample has been merged to a branch that the previous
                                 %sample did not merge to, we need to mark this
                                 %as a junction
                                 
%                                  if(is_cur_non_equivalent && (num_attachments_cur>1 || num_attachments_last>1))
%                                      extra_junctions = [extra_junctions; queryCurve(ms,:)];
%                                  end
%                                  
%                                  if(is_last_non_equivalent && (num_attachments_cur>1 || num_attachments_last>1))
%                                      extra_junctions = unique([extra_junctions; queryCurve(ms-1,:)],'rows');
%                                  end
                                 
                             end
                             
                            %Some branches might've changed in the process
                            %of merging the sample, so we need to recompute
                            %the junction indices so that future curves are
                            %appended at the right location
                            for jn=1:size(arr_junction_indices,1)
                                if(arr_junction_indices(jn,1)<=0)
                                    continue;
                                end
                                inspect_curve = curve_graph{jn,1};
                                junc_index = find(inspect_curve(:,1)==queryCurve(ms,1) & inspect_curve(:,2)==queryCurve(ms,2) & inspect_curve(:,3)==queryCurve(ms,3));
                                
                                if(isempty(junc_index))
                                    arr_junction_indices(jn,1)=0;
                                    arr_prev_sample(jn,1)=0;
                                    arr_next_sample(jn,1)=0;
                                    arr_branch_id_cur(arr_branch_id_cur==jn)=[];
                                else
                                    junction_different = 1;
                                    for jin=1:size(junc_index,2)
                                        if(junc_index(1,jin)==arr_junction_indices(jn,1))
                                            junction_different = 0;
                                            break;
                                        end
                                    end
                                    if(junction_different)
                                        %TODO: How do we pick among many?
                                        %Currently we pick the first one
                                        arr_junction_indices(jn,1) = junc_index(1,1);
                                    end
                                end
                            end
                             
                            %Previous merge info is stored here 
                            arr_branch_id_last = arr_branch_id_cur;
                            arr_junction_indices_last = arr_junction_indices;
                            arr_prev_sample_last = arr_prev_sample;
                            arr_next_sample_last = arr_next_sample;
                            equiv_table_last = equiv_table_cur;
                            
                         end
                    end
                    
                    if(is_merged)
                        %All new branches and their content is added
                        curve_graph = [curve_graph; all_new_branches];
                        lock_map = [lock_map; all_new_locks];
                        new_content = [all_views(1,mrv)+1 cc];
                        num_new_branches = size(all_new_branches,1);
                        new_content_set = cell(num_new_branches,1);
                        for nc=1:num_new_branches
                            new_content_set{nc,1} = new_content;
                        end
                        curve_graph_content = [curve_graph_content; new_content_set];
                    else
                        all_flags{all_views(1,mrv)+1,1}(curveID,1) = 0;
                    end
                end

            end

            merging_iterations_done = 1;
            % Check whether all curves have been merged yet
            for av=1:numViews
                cur_merge_flags = merge_flags{av,1};
                if(any(~cur_merge_flags))
                    merging_iterations_done = 0;
                    break;
                end
            end
            if((~merging_iterations_done) && (~new_curve_merged))
                 merging_iterations_done = 1;

                 for av=1:numViews
                    cur_merge_flags = merge_flags{av,1};
                    for cmf=1:size(cur_merge_flags,2)
%                         if(~cur_merge_flags(1,cmf))
%                              curve_graph = [curve_graph; cell(1,1)];
%                              cg_size = size(curve_graph,1);
%                              cur_id = clusters{all_views(1,av)+1,1}(1,cmf);
%                              curve_graph{cg_size,1} = all_recs_iter{all_views(1,av)+1,1}{1,cur_id};
%                         end
                    end
                 end
            end
        end
        
%         %Post-process the curve graph to break incorrect groupings and
%         %prune short curves
%         for br=1:size(curve_graph,1)
%             cur_branch = curve_graph{br,1};
%             num_branch_samples = size(cur_branch,1);
%             
%             if(num_branch_samples<2)
%                 continue;
%             end
%             
%             startSample = 1;
%             first_branch = 1;
% 
%             for bs=2:num_branch_samples
%                 dist = norm(cur_branch(bs-1,:) - cur_branch(bs,:));
%                 if(dist > 0.05)
%                     if(first_branch)
%                         first_branch = 0;
%                         curve_graph{br,1} = cur_branch(startSample:(bs-1),:);
%                     else
%                         curve_graph = [curve_graph; cell(1,1)];
%                         cg_size = size(curve_graph,1);
%                         curve_graph{cg_size,1} = cur_branch(startSample:(bs-1),:);
%                     end
%                     startSample = bs;
%                 end
%             end
%             if(~first_branch && startSample<num_branch_samples)
%                 curve_graph = [curve_graph; cell(1,1)];
%                 cg_size = size(curve_graph,1);
%                 curve_graph{cg_size,1} = cur_branch(startSample:num_branch_samples,:);
%             end
%         end

        counter = size(all_clusters,1)+1;        
%         for vv=1:numViews
%             for cl=1:size(clusters{all_views(1,vv)+1,1},2)
%                 curveID = clusters{all_views(1,vv)+1,1}(1,cl);
%                 curve = all_recs{all_views(1,vv)+1,1}{1,curveID};
%                 curve_iter = all_recs_iter{all_views(1,vv)+1,1}{1,curveID};
%                 write_curve_to_vrml(curve,[255 0 0],['./ply/before-nocorr/view',num2str(all_views(1,vv)),'_curve',num2str(curveID),'_cluster',num2str(counter),'.ply']);
%                 write_curve_to_vrml(curve_iter,[255 0 0],['./ply/after/view',num2str(all_views(1,vv)),'_curve',num2str(curveID),'_cluster',num2str(counter),'.ply']);
%                 %write_curve_and_correspondence_to_vrml(curve,[255 0 0],['./ply/before/view',num2str(all_views(1,vv)),'_curve',num2str(curveID),'_cluster',num2str(counter),'.ply'],corr_native{all_views(1,vv)+1,1}{1,cl},corr_other{all_views(1,vv)+1,1}{1,cl},255.*corr_colors{all_views(1,vv)+1,1}{1,cl});
%                 %counter = counter + 1;
%             end
%         end

        all_clusters = [all_clusters; cell(1,1)];
        all_clusters{size(all_clusters,1),1} = clusters;
        complete_curve_graph = [complete_curve_graph; curve_graph];
        complete_lock_map = [complete_lock_map; lock_map];
        %save('curve_graph_amsterdam_half.mat','complete_curve_graph','all_junctions');
        %save('curve_graph_amsterdam.mat');
    end
end

% for v=1:numIM
%     if(isempty(all_flags{v,1}))
%        continue;
%     end
%     num_curves = size(all_flags{v,1},1);
%     for c=1:num_curves
%       if(all_flags{v,1}(c,1)<num_shared_edges)
%         complete_curve_graph = [complete_curve_graph; cell(1,1)];
%         cg_size = size(complete_curve_graph,1);
%         complete_curve_graph{cg_size,1} = all_recs{all_views(1,av)+1,1}{1,c};
%       end
%     end
% end

all_junctions = [all_junctions; extra_junctions];
write_curve_graph_to_vrml_12;
save('curve_drawing_pavilion_mixed_half_12.mat','complete_curve_graph','all_junctions');

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
