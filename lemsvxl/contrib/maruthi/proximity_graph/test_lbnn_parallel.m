function []=test_lbnn_parallel(beta,f_index)

addpath /home/mn/lemsvxl/src/contrib/maruthi/proximity_graph/
addpath /home/mn/lemsvxl/src/contrib/maruthi/assignment/
addpath /home/mn/lemsvxl/src/contrib/ntrinh/matlab/utils/

files=dir('*_dist_*.txt');
cats=textread('../101_category_list.txt','%s');

load color_data.mat

s=cell2struct(cats,cats);

for k=1:length(cats)
    s=setfield(s,cats{k},colors(k,:));
    
end



for f=1:length(files)
   
    
    if ( f ~= f_index )
        continue;
    end
    
    temp=files(f).name;
    index=strfind(files(f).name,'0');
    if( isempty(index))
        index=strfind(files(f).name,'1');
    end
    category=strtok(files(f).name,temp(index));
    category(end)=[];
    disp(['Working on ' category ])
    
    prefix=strfind(files(f).name,'_dist');
    label_name=[temp(1:prefix) 'label_mat.txt'];
    file_name=[temp(1:prefix) 'file_mat.txt'];
    
    full_file_dist_name=[eval('pwd') '/' files(f).name];
    full_file_label_name=[eval('pwd') '/' label_name];
    full_file_file_name=[eval('pwd') '/' file_name];
    
    [dists]=textread(full_file_dist_name);
    [labels]=textread(full_file_label_name,'%s');
    if ( exist(full_file_file_name))
        [files_labels]=textread(full_file_file_name,'%s');
    else
        files_labels=[];
    end
    distance_matrix=squareform(dists)./16129.0;
    
    tic
    [gg_graph]=compute_beta_graph(distance_matrix,beta);
    toc
    tic
    gg_graph_dist_matrix=gg_graph.*distance_matrix;
    toc
    
    nn=find(gg_graph(1,:));
    disp(['Number of Neighbors outside 200: ' num2str(length(find(nn>200)))])
   
    absolute_index=cell(length(labels),1);
    for d=2:length(labels)+1
        absolute_index{d-1}=num2str(d);
    end
    delim=cellstr(repmat('-',length(labels),1));
    unique_string=strcat(files_labels,labels,delim,absolute_index);

    
%     options.method = 'smacof';
%     options.dim = 2;
%     options.iter = 100;
%     [Y,hist]=mds(gg_graph_dist_matrix,options);
%     
%     %b.MaxIter=1000;
%     %[Y,stress,disparities] = mdscale(distance_matrix,2,'criterion','metricstress','Options',b);
%     
%     paths={};
%     handles=[];
%     
%     figure('units','normalized','outerposition',[0 0 1 1])
%     
%     % plot k=20 neighbors
%     for k=2:21
%         
%         category_class=labels{k-1};
%         h1=plot(Y(k,1),Y(k,2),'.','Color',getfield(s,category_class),'MarkerSize',20);
%         
%         
%         [token,rest]=strtok(category_class,'_');
%         if ( ~isempty(rest) )
%             token=[token '\' rest];
%         end
%         
%         paths=[paths; token];
%         handles=[handles ; h1];
%         
%         hold on
%     end
%     
%     % plot query point
%     hq=plot(Y(1,1),Y(1,2),'k*','MarkerSize',15);
%     hold on
    
    gg_neighbors=find(gg_graph(1,:));
    disp([' max neighbor = ' num2str(max(gg_neighbors)) ])
    [gg_d,gg_index]=min(gg_graph_dist_matrix(1,gg_neighbors));
    gg_class=labels{gg_neighbors(gg_index)-1};
    
    [knn_d,knn_index]=min(distance_matrix(1,2:21));
    knn_class=labels{knn_index};
    %Find minimum distance of all neighbors
    
%     nn_hanlde=plot(Y(2:21,1),Y(2:21,2),'r^','MarkerSize',15,'LineWidth',2);
%     
%     
%     for g=1:length(gg_neighbors)
%         
%         gg_handle=plot(Y(gg_neighbors(g),1),Y(gg_neighbors(g),2),'ks','MarkerSize',15,'LineWidth',2);
%         h2=plot(Y(gg_neighbors(g),1),Y(gg_neighbors(g),2),'.','Color',getfield(s,labels{gg_neighbors(g)-1}),'MarkerSize',20);
%         
%         [token,rest]=strtok(labels{gg_neighbors(g)-1},'_');
%         if ( ~isempty(rest) )
%             token=[token '\' rest];
%         end
%         paths=[paths; token];
%         handles=[handles ; h2];
%         
%         
%         %hold on
%         text(Y(gg_neighbors(g),1)+0.00000005,Y(gg_neighbors(g),2)+0.00000005,num2str(gg_neighbors(g)),'VerticalAlignment','Baseline','FontSize',15,'Color',[0, 100, 0]./255)
%         
%     end
%     
%     num_knn_classes=length(unique(labels(1:20)));
%     num_gg_classes=length(unique(labels(gg_neighbors-1)));
%     
%     if ( num_knn_classes == num_gg_classes )
%         
%         title(['(\beta=' num2str(beta) ' Num class ' num2str(num_gg_classes) ')  = ( k_{nn}=20 Num class ' num2str(num_knn_classes) ')'],'FontSize',14)
%         postfix=['same_beta_' num2str(num_gg_classes) '_knn_' num2str(num_knn_classes)];
%     else
%         title(['(\beta=' num2str(beta) ' Num class ' num2str(num_gg_classes) ') \neq ( k_{nn}=20 Num class ' num2str(num_knn_classes) ')'],'FontSize',14)
%         postfix=['diff_beta_' num2str(num_gg_classes) '_knn_' num2str(num_knn_classes)];
%     end
%     
%     set(gca,'FontSize',14)
%     set(gca,'LineWidth',2)
%     grid on
%     
%     [b,ii,jj] = unique(paths);
%     
%     legend([handles(ii); nn_hanlde; gg_handle; hq],[b; 'K_{nn} = 20'; ['\beta=' num2str(beta) ' Neighbor'];'Query\_Point'],'FontSize',15,'Location','Best')
%     
%     if ( strcmp(gg_class,category))
%         eval(['print_pdf ' temp(1:prefix) 'correct_class_' gg_class '_' postfix '.pdf'])
%     else
%         eval(['print_pdf ' temp(1:prefix) 'incorrect_class_' gg_class '_' postfix '.pdf'])
%     end
%     close(gcf)
%     disp(['gg dist: ' num2str(gg_d*16129) ' knn_d: ' num2str(knn_d*16129) ])
%     
%     
%     
%     
%     figure('units','normalized','outerposition',[0 0 1 1])
%     flag=1;
%     for g=1:length(gg_neighbors)
%         
%         h2=plot(0,gg_neighbors(g),'.','Color',getfield(s,labels{gg_neighbors(g)-1}),'MarkerSize',20);
%         hold on
%         [token,rest]=strtok(labels{gg_neighbors(g)-1},'_');
%         if ( ~isempty(rest) )
%             token=[token '\' rest];
%         end
%         if ( flag == 1 )
%             text(0,gg_neighbors(g),token,'VerticalAlignment','Baseline','HorizontalAlignment','Left');
%             flag=0;
%         else
%             text(0,gg_neighbors(g),token,'VerticalAlignment','Baseline','HorizontalAlignment','Right')
%             flag=1;
%         end
%         
%         nn_pg=find(gg_graph(gg_neighbors(g),:));
%         flag2=1;
%         for k=1:length(nn_pg)
%             
%             %gg_handle=plot(nn_pg(k),nn_pg(k),2),'ks','MarkerSize',15,'LineWidth',2);
%             if ( (nn_pg(k)-1)==0 )
%                 
%                 h2=plot(g,nn_pg(k),'.','Color','k','MarkerSize',20);
%                 hold on
%                 text(g,nn_pg(k),'Query','VerticalAlignment','Baseline')
%             else
%                 h2=plot(g,nn_pg(k),'.','Color',getfield(s,labels{nn_pg(k)-1}),'MarkerSize',20);
%                 hold on
%                 
%                 [token,rest]=strtok(labels{nn_pg(k)-1},'_');
%                 if ( ~isempty(rest) )
%                     token=[token '\' rest];
%                 end
%                 if ( flag2 == 1)
%                     text(g,nn_pg(k),token,'VerticalAlignment','Baseline','HorizontalAlignment','Left')
%                     flag2=0;
%                 else
%                     text(g,nn_pg(k),token,'VerticalAlignment','Baseline','HorizontalAlignment','Right')
%                     flag2=1;
%                 end
%             end
%             
%             
%             
%             %             hold on
%             %             text(g+1,nn_pg(k),labels{nn_pg(k)-1},'VerticalAlignment','Baseline','FontSize',15)
%             
%             
%             %             [token,rest]=strtok(labels{gg_neighbors(g)-1},'_');
%             %             if ( ~isempty(rest) )
%             %                 token=[token '\' rest];
%             %             end
%             %             paths=[paths; token];
%             %             handles=[handles ; h2];
%             
%         end
%         %hold on
%         %text(Y(gg_neighbors(g),1)+0.00000005,Y(gg_neighbors(g),2)+0.00000005,num2str(gg_neighbors(g)),'VerticalAlignment','Baseline','FontSize',15,'Color',[0, 100, 0]./255)
%         
%     end
%     
%     set(gca,'FontSize',14)
%     set(gca,'LineWidth',2)
%     grid on
%     eval(['print_pdf ' temp(1:prefix) 'neighbor_structure_.pdf'])
%     close gcf
%     %hold on
%     
%     total_data=[gg_graph(1,:) ; gg_graph(gg_neighbors,:)];
%     hamming_dist=pdist(total_data,'Hamming');
%     jaccard_dist=pdist(total_data,'Jaccard');
%     cosine_dist=pdist(total_data,'Cosine');
%     
%     
%     figure('units','normalized','outerposition',[0 0 1 1])
%     subplot(3,1,1)
%     plot(1:length(gg_neighbors),hamming_dist(1:length(gg_neighbors)),'Color','r','LineWidth',3)
%     set(gca,'FontSize',14)
%     grid on
%     legend('Hamming Distance')
%     xlabel('Neighbors')
%     ylabel('Distance to Neighbors')
%     
%     
%     subplot(3,1,2)
%     plot(1:length(gg_neighbors),jaccard_dist(1:length(gg_neighbors)),'Color','g','LineWidth',3)
%     set(gca,'FontSize',14)
%     grid on
%     legend('Jaccard Distance')
%     xlabel('Neighbors')
%     ylabel('Distance to Neighbors')
%     
%     subplot(3,1,3)
%     plot(1:length(gg_neighbors),cosine_dist(1:length(gg_neighbors)),'Color','b','LineWidth',3)
%     set(gca,'FontSize',14)
%     grid on
%     legend('Cosine Dist')
%     xlabel('Neighbors')
%     ylabel('Distance to Neighbors')
%     
%     eval(['print_pdf ' temp(1:prefix) 'neighbor_similiarity.pdf'])    
%     
%     close gcf

    cost_matrix_sizes=zeros(length(gg_neighbors),1);
    
    for g=1:length(gg_neighbors)
        
        nn_pg=symmetric_assign(gg_graph,distance_matrix,unique_string,g,beta);
        gg_graph(gg_neighbors(g),:)=0;
        gg_graph(gg_neighbors(g),nn_pg)=1;
        cost_matrix_sizes(g)=length(nn_pg);
    end
    
  
    
    for d=1:length(cost_matrix_sizes)
        cost_matrix=zeros(length(gg_neighbors),cost_matrix_sizes(d));
        nn_pg=find(gg_graph(gg_neighbors(d),:));
        [sorted_dist,sorted_indices]=sort(distance_matrix(gg_neighbors(d),nn_pg));
        nn_pg=nn_pg(sorted_indices);
        %index_to_remove=find(nn_pg==1);
        
        
        for g=1:length(gg_neighbors)
            for k=1:length(nn_pg)
                cost_matrix(g,k)=distance_matrix(nn_pg(k),gg_neighbors(g));
            end
        end
        %cost_matrix(:,index_to_remove)=[];
        cost_matrix(d,:)=[];
        [assignment,cost] = assignmentoptimal(cost_matrix);
        
        string=[temp(1:prefix) 'query_vs_' num2str(d) '_pg_neighbor.pdf'];
        query_distance=distance_matrix(1,gg_neighbors(d));
        plotter(gg_neighbors,nn_pg,labels,s,d,assignment,cost_matrix,cost,string,category,query_distance,distance_matrix,files_labels);
        
    end
   
end

function [query_neighbors]=symmetric_assign(gg_graph,distance_matrix,unique_string,index,beta)
orig_string=unique_string;
gg_neighbors=find(gg_graph(1,:));
exact_index=strmatch(unique_string{gg_neighbors(index)-1},unique_string,'exact');
[T,R]=strtok(unique_string{gg_neighbors(index)-1},'-');
indices_to_remove =strmatch(T, unique_string);
indices_to_remove(find(indices_to_remove==exact_index))=[];
distance_matrix(1,:)=[];
distance_matrix(:,1)=[];
distance_matrix(indices_to_remove,:)=[];
distance_matrix(:,indices_to_remove)=[];
unique_string(indices_to_remove)=[];

tic
[beta_graph]=compute_beta_graph(distance_matrix,beta);
toc

query_index=strmatch(orig_string{gg_neighbors(index)-1},unique_string,'exact');
nn_pg=find(beta_graph(query_index,:));

query_neighbors=[];
for k=1:length(nn_pg);
    [T,R]=strtok(unique_string{nn_pg(k)},'-');
    query_neighbors=[query_neighbors str2num(R(2:end))];
end

function plotter(gg_neighbors,nn_pg,labels,s,index,assignment,cost_matrix,cost,string,category,query_distance,distance_matrix,files)

figure('units','normalized','outerposition',[0 0 1 1])

%temp_pg=nn_pg;
%temp_pg(find(nn_pg==1))=[];
bbd=find(nn_pg==1);
% 
%temp_gg=gg_neighbors;
%temp_gg(index)=[];

for c=1:length(assignment)
    if ( assignment(c) )
        yindex=assignment(c);
        xindex=c;
        if ( yindex >= bbd )
            yindex=yindex+1;
        end
        
        if ( xindex >= index )
            xindex=c+1;
        end
        
        midpoint=interp1([1 2].',[xindex yindex],1.1);
        plot([ 1  2],[xindex yindex],'k');
        hold on
        text(1.1,midpoint,num2str(cost_matrix(c,assignment(c))))
    end
end

flag=1;
for g=1:length(gg_neighbors)
    dist_gg=distance_matrix(1,gg_neighbors(g));
    h2=plot(1,g,'.','Color',getfield(s,labels{gg_neighbors(g)-1}),'MarkerSize',20);
    hold on
    [token,rest]=strtok(labels{gg_neighbors(g)-1},'_');
    if ( ~isempty(rest) )
        token=[token '\' rest];
    end
 if ( ~isempty(files) )
        token=['f: ' files{gg_neighbors(g)-1} ', d: ' num2str(dist_gg) ', ' token '\_knn_{' num2str(gg_neighbors(g)-1)  '}\_pg_{' num2str(g) '}' ];
    else
        token=['d: ' num2str(dist_gg) ', ' token '\_knn_{' num2str(gg_neighbors(g)-1)  '}\_pg_{' num2str(g) '}' ];
    end
    cb='k';
    if ( g == index )
        cb='r';
    end
    if ( flag == 1 )
        text(1-0.02,g,token,'VerticalAlignment','Baseline','HorizontalAlignment','Right','Color',cb);
        flag=0;
    else
        text(1-0.02,g,token,'VerticalAlignment','Baseline','HorizontalAlignment','Right','Color',cb)
        flag=1;
    end
end
    
flag2=1;
for k=1:length(nn_pg)
    dist_nnpg=distance_matrix(nn_pg(k),gg_neighbors(index));
    
    if ( (nn_pg(k)-1)==0 )
        
        h2=plot(2,k,'.','Color','k','MarkerSize',20);
        hold on
        text(2+0.02,k,['Query\_pg{' num2str(k) '}, d: ' num2str(dist_nnpg)] ,'VerticalAlignment','Bottom','Color','r')
    else
        h2=plot(2,k,'.','Color',getfield(s,labels{nn_pg(k)-1}),'MarkerSize',20);
        hold on
        
        [token,rest]=strtok(labels{nn_pg(k)-1},'_');
        if ( ~isempty(rest) )
            token=[token '\' rest];
        end
        if ( ~isempty(files))
            token=[token '\_knn_{' num2str(nn_pg(k)-1)  '}\_pg_{' num2str(k) '}, d: ' num2str(dist_nnpg) ', f: ' files{nn_pg(k)-1}];
        else
            token=[token '\_knn_{' num2str(nn_pg(k)-1)  '}\_pg_{' num2str(k) '}, d: ' num2str(dist_nnpg) ];
        end
        if ( flag2 == 1)
            text(2+0.02,k,token,'VerticalAlignment','Baseline','HorizontalAlignment','Left')
            flag2=0;
        else
            text(2+0.02,k,token,'VerticalAlignment','Baseline','HorizontalAlignment','Left')
            flag2=1;
        end
    end

    
end

set(gca,'FontSize',14)
set(gca,'LineWidth',2)
grid off
axis([0 3 0 max(length(gg_neighbors),length(nn_pg))+1 ])

[token,rest]=strtok(labels{gg_neighbors(index)-1},'_');
token_basic=token;
if ( ~isempty(rest) )
    token=[token '\' rest];
end
tempy=token;
token=[token_basic '_knn_' num2str(gg_neighbors(index)-1) '_pg_' num2str(index) ];
tokenfixed=[tempy '\_knn_{' num2str(gg_neighbors(index)-1) '}\_pg_{' num2str(index) '}'];

set(gca,'XTickLabel',{['Query (' category ')'],token})
set(gca,'XTick',[1,2])
set(gca,'YTickLabel',[])
set(gca,'YTick',[])
title(['Assignment : ' num2str(cost) ', Query to ' tokenfixed ' D: ' num2str(query_distance) ', Max D: ' num2str(max(max(distance_matrix))) ])
%print(gcf, '-depsc2', '-painters', '-r864', string);
eval(['print_pdf ' string])


close gcf
 
%  nn_pg(1)=[];
% 
% for c=1:length(assignment)
%     if ( assignment(c) )
%         midpoint=interp1([1 2].',[gg_neighbors(c) nn_pg(assignment(c))],0.5);
%         semilogy([ 1  2],[gg_neighbors(c) nn_pg(assignment(c))],'k');
%         text(1.5,midpoint,num2str(cost_matrix(c,assignment(c))))
%     end
% end

% set(gca,'XTick',[1,2])    
% set(gca,'XTickLablel',{'Query',getfield(s,labels{gg_neighbors(g)-1})

