clear all;
close all;

load clusters_view-10_10-edges.mat;

num_clusters = size(all_clusters,1);

colors = distinguishable_colors(num_clusters);

base_color = [0 0 255];
color_index = 2;

views = [6 7 8 10 13];
num_views = size(views,2);

for cl=1:num_clusters
    cur_clusters = all_clusters{cl,1};

    if(~isempty(cur_clusters))
        %Check to see if there is more than 1 curve in the cluster
        
        first_curve_found = 0;
        multiple_curves_found = 0;

        for v=1:num_views
            curveIDs = cur_clusters{views(1,v)+1,1};
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

        for v=1:num_views

            curveIDs = cur_clusters{views(1,v)+1,1};
            num_curves = size(curveIDs,2);

            for c=1:num_curves
                cur_curve = all_recs{views(1,v)+1,1}{1,curveIDs(1,c)};
                
                if(multiple_curves_found)
                    cur_color(1,1) = round(255.*colors(color_index,1));
                    cur_color(1,2) = round(255.*colors(color_index,2));
                    cur_color(1,3) = round(255.*colors(color_index,3));
                else
                    cur_color = base_color; 
                end
                
                write_curve_to_vrml(cur_curve,cur_color,['./ply/view-',num2str(views(1,v)),'_cluster-',num2str(cl),'_crv-',num2str(curveIDs(1,c)),'.ply']);  
            end

        end

    end
    
    if(multiple_curves_found)
        color_index = color_index+1;
    end

end

