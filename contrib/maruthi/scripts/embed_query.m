function [pg_stats,nn_stats] = embed_query(exemplar_matrix,query_matrix,exemp_labels,fnn_stats,nn)

%results=load(data_file);

[query, b,groundtruth, d, e, f, g, h, i, j]=textread(fnn_stats,...
    '%s %s %s %s %s %s %s %s %s %s');

pg_stats=zeros(size(query_matrix,1),1);
nn_stats=zeros(size(query_matrix,1),1);

for i=1:size(query_matrix,1)
    
    %Create total_matrix
    column_add=query_matrix(i,:).';
    row_add=[query_matrix(i,:) 0];
    total_matrix=[exemplar_matrix column_add];
    total_matrix=[total_matrix ; row_add];
    [gg_graph_all_pair_distance,gg_graph_dist_matrix,gg_graph]=create_gabriel_graph(total_matrix,[]);
    [nn_i,nn_j]=find(gg_graph(end,:));
    
    %Check first nn
    for kk=1:length(nn_j)
        
        if(strcmpi(exemp_labels{nn_j(kk)},groundtruth{i}))
            pg_stats(i)=1;
            break;
        end
    end
    
    if (nn >1 )
        %Check second neighbordhood
        if ( pg_stats(i) ~= 1 )
            disp('Checking 2nd Neighborhood')
            for kk=1:length(nn_j)
                [nn2_i,nn2_j]=find(gg_graph(nn_j(kk),:));
                
                %Check each neighbors
                for mm=1:length(nn2_j)
                    if ( nn2_j(mm) <= length(exemp_labels))
                        if(strcmpi(exemp_labels{nn2_j(mm)},groundtruth{i}))
                            pg_stats(i)=1;
                            break;
                        end
                    end
                end
                
                if (pg_stats(i))
                    break
                end
            end
            
            
        end
    end
    
    
    nn_value=groundtruth{i};
    nn_value(1)=[];
    nn_value=str2double(nn_value);
    [a,b]=sort(query_matrix(i,:));
    nn_not=find(b==nn_value);
    nn_stats(i)=nn_not;
    
end

pg_percentage=(pg_stats./length(pg_stats))*100;

