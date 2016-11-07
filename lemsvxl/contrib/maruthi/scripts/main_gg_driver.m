clear all
close all

results_dir='/home/mn/lemsvxl/src/contrib/maruthi/en256_project/categorization';

cpwd=cd;

eval(['cd ' results_dir])

model_dirs=dir('model*');
pr_curve=zeros(size(model_dirs,1),2);


for model=1:size(model_dirs,1)
    model_name=model_dirs(model).name;
    indices=strfind(model_name,'_');
    nlabel=str2num(model_name((indices(end)+1):end));
    disp([ 'Working on ' model_name ] )
    eval(['cd ' model_name '/train'])
    [a,pdistmat]=textread('dist_mat.txt','%s %f');
    exemplar_matrix=squareform(pdistmat);
    categories=textread('train_set.txt','%s');
    for k=1:length(categories)
        temp=(findstr(categories{k},'_'));
        strstart=temp(2)+1;
        strend=findstr(categories{k},'.')-1;
        temp=categories{k};
        categories{k}=temp(strstart:strend);
    end
    
    
    cd ../results
    files=dir('*.txt');
    
    %results array
    classification=zeros(length(files),1);

    for f=1:length(files)
        query_name=files(f).name;
        [a,query]=textread(query_name,'%s %f');
        column_add=query;
        row_add=[query.' 0];
        total_matrix=[exemplar_matrix column_add];
        total_matrix=[total_matrix ; row_add];
        [gg_graph_all_pair_distance,gg_graph_dist_matrix,gg_graph]=create_rng_graph(total_matrix,[]);
        [nn_i,nn_j]=find(gg_graph(end,:));
        
        %Create set
        for bb=1:length(nn_j)
            eval(['set.' categories{nn_j(bb)} '=0;']);
            
        end
        
        %Reloop over set
        for bb=1:length(nn_j)
            eval(['set.' categories{nn_j(bb)} '=set.' categories{nn_j(bb)} '+1;']);
        end
        
        %Create final datastructure to use
        c = struct2array(set);
        vv = fieldnames(set);
        [a,loc]=max(c);
        nearest_neighbor=vv(loc);
        
        %Correct category
        back_index=findstr(query_name,'_results.txt');
        strip_file_name=query_name(1:(back_index-1));
        front_index=findstr(strip_file_name,'_');
        ground_truth=strip_file_name((front_index(2)+1):end);
        
        
        disp([ query_name ' ' ground_truth ' ' nearest_neighbor{1} ]);
        if ( strcmpi(nearest_neighbor{1},ground_truth))
            classification(f)=1;
        end
        
    end

    precision=(sum(classification)./length(classification))*100;
    pr_curve(model,1)=nlabel;
    pr_curve(model,2)=precision;
    
    eval(['cd ' results_dir])
    
end

eval(['cd ' cpwd ]);



% % plot caltech 1 curve
% pr_curve_s=sortrows(pr_curve);
% plot(pr_curve_s(:,1),pr_curve_s(:,2),'b-s','LineWidth',3);
% axis([0 30 0 100])
% set(gca,'YTick',[0 5 10 15 25 35 45 55 65 75 85 95])
% set(gca,'XTick',[0 5 10 15 20 25 30])
% grid on
% hold on








