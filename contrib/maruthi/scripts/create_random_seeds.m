function []=create_random_seeds(seeds_index,clusters,ppc)


numb_shapes=clusters*ppc;

%create string
attached_string=['p000000'];
shapes={};

%Create shapes
for k=1:numb_shapes
    index=num2str(k);
    offset=5-length(index);
    shape_name=[attached_string(1:offset) index];
    shapes{k}=shape_name; 
end


%Write out shape names
file_name=['random_seeds'];

offset=[0:ppc:(numb_shapes-ppc)];
for i=1:length(seeds_index)
    
    output_file_name=[file_name num2str(seeds_index(i)) '.txt'];
    seeds_per_cluster=seeds_index(i)/clusters;
    random_indices=randperm(ppc);
    proto_indices=repmat(sort(random_indices(1:seeds_per_cluster).'),clusters,1);
    final_offset=repmat(offset,[seeds_per_cluster 1]);
    final_protos=proto_indices+final_offset(:);
    
    fid=fopen(output_file_name,'w');
    for b=1:length(final_protos)
        fprintf(fid,'%s\n',shapes{final_protos(b)});
    end
    fclose(fid);
       
    
    
end