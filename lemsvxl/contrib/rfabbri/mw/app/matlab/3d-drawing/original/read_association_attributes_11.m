function [links_3d, offsets_3d, edge_support_3d] = read_association_attributes_11(view,num_image_curves,num_3d_curves,num_views)

    fid=fopen(['./association_attributes/11/attributes_' num2str(view) '.txt']);
    
    links_3d = cell(num_image_curves,1);
    offsets_3d = cell(num_image_curves,1);
    edge_support_3d = cell(num_3d_curves,1);
    
    for i=1:num_3d_curves
       edge_support_3d{i,1} = cell(num_views,1); 
    end
    
    for c=1:num_3d_curves
        curve_info = fscanf(fid,'%d',[1 3]);
        links_3d{curve_info(1,1)+1,1} = [links_3d{curve_info(1,1)+1,1} curve_info(1,2)];
        offsets_3d{curve_info(1,1)+1,1} = [offsets_3d{curve_info(1,1)+1,1} curve_info(1,3)];
        
        num_samples = fscanf(fid,'%d',[1 1]);
        for s=1:num_samples
            curSupport = fscanf(fid,'%d',[1 1]);
            for e=1:curSupport
                cur_view = fscanf(fid,'%d',[1 1])+1; 
                
                if(isempty(edge_support_3d{c,1}{cur_view,1}))
                    edge_support_3d{c,1}{cur_view,1} = cell(num_samples,1);
                end
                
                num_support = fscanf(fid,'%d',[1 1]);
                support_edges = fscanf(fid,'%d',[1 num_support]);
                
                edge_support_3d{c,1}{cur_view,1}{s,1} = support_edges;
            end
        end
    end

end

