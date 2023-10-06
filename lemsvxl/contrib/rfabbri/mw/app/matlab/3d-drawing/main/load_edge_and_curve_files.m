% to be called in _main.m

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
