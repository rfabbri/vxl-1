function [exemplar_data,query_data]=create_synthetic_clusters(dimension,...
    centroids,exemplar_ppc,query_ppc,sigma,query_sigma,grid_a,grid_b)

%Create base exemplar and query data
total_exemplar_points=centroids*exemplar_ppc;
total_query_points   =centroids*query_ppc;

exemplar_data = 0+sigma.*randn(total_exemplar_points,dimension);
query_data    = 0+query_sigma.*randn(total_query_points,dimension);
a=grid_a;
b=grid_b;

%Move points over to means
centroid_means=a+(b-a).*rand(centroids,dimension);

%Create offsets
exemplar_means=reshape(repmat(centroid_means.',exemplar_ppc,1),dimension,total_exemplar_points).';
query_means   =reshape(repmat(centroid_means.',query_ppc,1),dimension,total_query_points).';

exemplar_data=exemplar_data+exemplar_means;
query_data   =query_data + query_means;
% 
% for i=1:points_per_cluster:length(offset_matrix)
%     offsetr=a+(b-a).*rand(1,dimension);
%     eoffset=repmat(offsetr,points_per_cluster,1);
%     qoffset=repmat(offsetr,points_per_cluster,
%     qoffsets=[qoffsets ; offset];
% end


% cluster_data=cluster_data+offset_matrix;
% 
% for i=1:query_ppc:length(offset_qmatrix)
%     offset_qmatrix(i:(i+(points_per_cluster-1)),:)=offset(i-(i*qpoints_per_cluster);
%     
% end
% query_data=query_data+offset_qmatrix;

% %Determine circle centers
% 
% vv=500:-5:-500;
% 
% centroid_centers=[];
% for i=2:length(vv)
%     centers=mean([vv(i-1) vv(i)]);
%     centroid_centers=[centroid_centers ; centers];
%     
% end
% 
% 
% %grab 30 centers
% centers=30;
% index_matrix=zeros(centers,dimension);
% 
% for d=1:dimension
%     indices=randi([1 length(vv)],30,1);
%     index_matrix(:,d)=indices;
% end
% 
% %Get circle centers
% %dd=linspace(0,2*pi,20);
% 
% centroids=zeros(centers,dimension);
% for d=1:dimension
%     centroids(:,d)=centroid_centers(index_matrix(:,d));
% end
% 
% 
% store_minor_centers=[];
% X=[];
% keyboard
% for k=1:length(centroids)
%     
%     %rand_r=randi([5 20],1,1);
%     dd = 0 + (pi).*rand(20,1);
%     
%     for angles=1:length(dd)
%         rand_ra = 10 + 50.*randn(1,1);
%         rand_rb = 10 + 50.*randn(1,1);
%         
%         minor_center=[centroids(k,1)+rand_ra*cos(dd(angles)) ...
%             centroids(k,2)+rand_rb*sin(dd(angles)) ];
%         
%         points=[1+5.*randn(dimension,1)];
%         x = [1+5.*randn(1,1)];
%         
%         y = [1+5.*randn(1,1) ];
%         
%         X=[X ; x+minor_center(1) y+minor_center(2)];
%     end
%     
%     
% end
 


