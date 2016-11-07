clear all
close all

%--------------------- Specify input parameters ---------------------------
dimension=3;
sigma=sqrt(dimension);
query_sigma=sqrt(25);
categories=10;
exemplar_ppc=10;
query_ppc=10;
grid_a=-50;
grid_b=50;
exemplars=categories*exemplar_ppc;
queries=categories*query_ppc;

%------------------- Create clustered data --------------------------------
[exemplar_data,query_data]=create_synthetic_clusters(dimension,...
    categories,exemplar_ppc,query_ppc,sigma,query_sigma,grid_a,grid_b);

%---------------- calculate euclidian distance matrix ---------------------
%Create distance matrices
total_points   = [exemplar_data ; query_data];
total_matrix   = squareform(pdist(total_points));
exemplar_matrix= total_matrix(1:exemplars,1:exemplars);
query_matrix   = total_matrix((exemplars+1):end,1:exemplars);


%---------------- create arbitrary object names ---------------------------
attached_string=['p000000'];

for k=1:size(total_matrix,1)
    index=num2str(k);
    offset=5-length(index);
    shape_name=[attached_string(1:offset) index];
    shapes{k}=shape_name; 
end


%--------------- Create names for queries and exemplars -------------------
%Query labels
exemplar_labels = cell(exemplars,1);
query_labels    = cell(queries,1);


for e=1:length(exemplar_labels)
   exemplar_labels{e}=shapes{e};
end

for q=1:length(query_labels)
    query_labels{q}=shapes{q+exemplars};
end

%------------------------- Write out data matrices ------------------------
% Write exemplar matrix
fid=fopen('exemplar_dataset_scg3d.txt','w');
numb_shapes=size(exemplar_matrix,1);
for b=1:numb_shapes
    for j=1:numb_shapes
        if ( j ~= numb_shapes)
            fprintf(fid,'%6.5f ',exemplar_matrix(b,j));
        else
            fprintf(fid,'%6.5f',exemplar_matrix(b,j));
        end
    end
    fprintf(fid,'\n');
end
fclose(fid);

%Write query matrix
fid=fopen('query_dataset_scg3d.txt','w');
for b=1:size(query_matrix,1)
    for j=1:size(query_matrix,2)
        if ( j ~= size(query_matrix,2))
            fprintf(fid,'%6.5f ',query_matrix(b,j));
        else
            fprintf(fid,'%6.5f',query_matrix(b,j));
        end
    end
    fprintf(fid,'\n');
end
fclose(fid);

%------------------- Write out label files --------------------------------

%Write out quieries.txt
fid=fopen('queries_scg3d.txt','w');

for i=1:length(query_labels)
    fprintf(fid,'%s\n',query_labels{i});
end

fclose(fid)


%Write out seeds.txt
fid=fopen('exemplar_scg3d.txt','w');

for i=1:length(exemplar_labels)
    fprintf(fid,'%s\n',exemplar_labels{i});
end

fclose(fid)

%----------------- Create and write ground truth --------------------------

%Create nearest_shape for ground truth
nearest_shape=cell(queries,1);

%find nearest neighbor for each query
for b=1:size(query_matrix,1)
   row_of_shapes=query_matrix(b,:);
   [y,index]=sort(row_of_shapes);
   nearest_shape{b}=exemplar_labels{(index(1))};
end

%Write out seeds.txt
fid=fopen('ground_truth_scg3d_1.txt','w');

for i=1:size(query_matrix,1)
    fprintf(fid,'%s %s\n',query_labels{i},nearest_shape{i});
end

fclose(fid)

%find nearest neighbor for each query
for b=1:size(query_matrix,1)
   row_of_shapes=query_matrix(b,:);
   [y,index]=sort(row_of_shapes);
   nearest_shape{b}=exemplar_labels{(index(2))};
end

%Write out seeds.txt
fid=fopen('ground_truth_scg3d_2.txt','w');

for i=1:size(query_matrix,1)
    fprintf(fid,'%s %s\n',query_labels{i},nearest_shape{i});
end

fclose(fid)

%find nearest neighbor for each query
for b=1:size(query_matrix,1)
   row_of_shapes=query_matrix(b,:);
   [y,index]=sort(row_of_shapes);
   nearest_shape{b}=exemplar_labels{(index(3))};
end

%Write out seeds.txt
fid=fopen('ground_truth_scg3d_3.txt','w');

for i=1:size(query_matrix,1)
    fprintf(fid,'%s %s\n',query_labels{i},nearest_shape{i});
end

fclose(fid)

%------------------- Write out category file ------------------------------

v{1}='cat';
v{2}='dog';
v{3}='hippo';
v{4}='tiger';
v{5}='lion';
v{6}='dolphin';
v{7}='turtle';
v{8}='fish';
v{9}='ant';
v{10}='bird';

bb=repmat(v,10,1);

categories=bb(:);

%Write out seeds.txt
fid=fopen('exemp_cat.txt','w');

for i=1:size(categories,1)
    fprintf(fid,'%s\n',categories{i});
end

fclose(fid)








