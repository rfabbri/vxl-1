clear all;
close all;

load intermediate.mat;

duplicate_found = 0;

for av=1:size(all_edge_support_3d,1)
   av
    for ci=1:size(all_edge_support_3d{av,1},1)
       
        for iv=1:size(all_edge_support_3d{av,1}{ci,1},1)
            
            for si=1:size(all_edge_support_3d{av,1}{ci,1}{iv,1},1)
                
                edges = all_edge_support_3d{av,1}{ci,1}{iv,1}{si,1};
                num_edges = size(edges,2);
                num_unique_edges = size(unique(edges),2);
                
                if(num_edges > num_unique_edges)
                   av
                   ci
                   iv
                   si
                   duplicate_found = 1;
                   break;
                end
                
            end
            if(duplicate_found)
                break;
            end
        end
        if(duplicate_found)
            break;
        end
    end
    if(duplicate_found)
        break;
    end
end