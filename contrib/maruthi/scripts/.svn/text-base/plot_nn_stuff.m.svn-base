clear all
close all


data1=[128	19	9 ;...
354	24	4 ; ...
511	22	6 ; ...
599	23	1 ; ...
693	5	2 ; ...
699	1	0 ; ...
700	0	0 ; ...
700	0	0 ];


data2=[127	19	4; ...
343	33	4 ; ...
502	24	7 ; ...
585	28	3 ; ...
687	10	3 ; ...
696	2	2 ; ...
700	0	0 ; ...
700	0	0];

data3=[112	26	10; ...
307	41	10 ; ...
461	31	16 ; ...
544	34	11 ; ...
680	15	2 ; ...
691	9	0 ; ...
699	1	0 ; ...
700	0	0 
];

data4=[95	28	7 ; ...
288	43	12 ; ...
413	48	18 ; ...
509	41	17 ; ...
668	24	2 ; ...
684	14	1 ; ...
692	6	1 ; ...
700	0	0 
];
            %1st nn    %2nd nn    %3rd nn
rng_data=[ data1(1,1) data1(1,2) data1(1,3) ; ...
           data2(1,1) data2(1,2) data2(1,3) ; ...
           data3(1,1) data3(1,2) data3(1,3) ; ...
           data4(1,1) data4(1,2) data4(1,3) ];

            %1st nn    %2nd nn    %3rd nn
beta14_data=[ data1(2,1) data1(2,2) data1(2,3) ; ...
           data2(2,1) data2(2,2) data2(2,3) ; ...
           data3(2,1) data3(2,2) data3(2,3) ; ...
           data4(2,1) data4(2,2) data4(2,3) ];
       
              %1st nn    %2nd nn    %3rd nn
beta11_data=[ data1(3,1) data1(3,2) data1(3,3) ; ...
           data2(3,1) data2(3,2) data2(3,3) ; ...
           data3(3,1) data3(3,2) data3(3,3) ; ...
           data4(3,1) data4(3,2) data4(3,3) ];
       
                  %1st nn    %2nd nn    %3rd nn
gg_data=[ data1(4,1) data1(4,2) data1(4,3) ; ...
           data2(4,1) data2(4,2) data2(4,3) ; ...
           data3(4,1) data3(4,2) data3(4,3) ; ...
           data4(4,1) data4(4,2) data4(4,3) ];
       
                   %1st nn    %2nd nn    %3rd nn
beta95_data=[ data1(5,1) data1(5,2) data1(5,3) ; ...
           data2(5,1) data2(5,2) data2(5,3) ; ...
           data3(5,1) data3(5,2) data3(5,3) ; ...
           data4(5,1) data4(5,2) data4(5,3) ];
       
             %1st nn    %2nd nn    %3rd nn
beta9_data=[ data1(6,1) data1(6,2) data1(6,3) ; ...
           data2(6,1) data2(6,2) data2(6,3) ; ...
           data3(6,1) data3(6,2) data3(6,3) ; ...
           data4(6,1) data4(6,2) data4(6,3) ];      
       
            %1st nn    %2nd nn    %3rd nn
beta8_data=[ data1(7,1) data1(7,2) data1(7,3) ; ...
           data2(7,1) data2(7,2) data2(7,3) ; ...
           data3(7,1) data3(7,2) data3(7,3) ; ...
           data4(7,1) data4(7,2) data4(7,3) ];
       
              %1st nn    %2nd nn    %3rd nn
beta5_data=[ data1(8,1) data1(8,2) data1(8,3) ; ...
             data2(8,1) data2(8,2) data2(8,3) ; ...
             data3(8,1) data3(8,2) data3(8,3) ; ...
             data4(8,1) data4(8,2) data4(8,3) ];
       
       
 color=[0 0 1; ...
       0 1 0; ...
       0 1 1; ...
       1 0 0; ...
       1 0 1; ...
       1 1 0; ...
       1 0.64705 0; ...
       0 0 0 ];

query=[sqrt(3) 2 3 5];
plot(query, rng_data(:,1)/700,'Color',color(1,:),'LineWidth',3)
axis([1.5 5.5 0 1])
grid on
hold on
plot(query, beta14_data(:,1)/700,'Color',color(2,:),'LineWidth',3)
plot(query, beta11_data(:,1)/700,'Color',color(3,:),'LineWidth',3)
plot(query, gg_data(:,1)/700,'Color',color(4,:),'LineWidth',3)
plot(query, beta95_data(:,1)/700,'Color',color(5,:),'LineWidth',3)
plot(query, beta9_data(:,1)/700,'Color',color(6,:),'LineWidth',3)
plot(query, beta8_data(:,1)/700,'Color',color(7,:),'LineWidth',3)
plot(query, beta5_data(:,1)/700,'Color',color(8,:),'LineWidth',3)
   
 legend('Rng Graph','Beta=1.4 Graph','Beta=1.1 Graph','Gabriel Graph','Beta=.95 Graph',...
    'Beta=.9 Graph','Beta=.8 Graph','Beta=.5 Graph')       
 xlabel('Query Sigma','FontSize',12)
 ylabel('% of 1st Nearest Neighbor','FontSize',12)
 title('1st Nearest Neighbor versus Query Sigma','FontSize',12)  
       
%        
%  figure
%  plot(query, rng_data(:,2)/700,'Color',color(1,:),'LineWidth',3)
% axis([1.5 5.5 0 1])
% grid on
% hold on
% plot(query, beta14_data(:,2)/700,'Color',color(2,:),'LineWidth',3)
% plot(query, beta11_data(:,2)/700,'Color',color(3,:),'LineWidth',3)
% plot(query, gg_data(:,2)/700,'Color',color(4,:),'LineWidth',3)
% plot(query, beta95_data(:,2)/700,'Color',color(5,:),'LineWidth',3)
% plot(query, beta9_data(:,2)/700,'Color',color(6,:),'LineWidth',3)
% plot(query, beta8_data(:,2)/700,'Color',color(7,:),'LineWidth',3)
% plot(query, beta5_data(:,2)/700,'Color',color(8,:),'LineWidth',3)
%    
%  legend('Rng Graph','Beta=1.4 Graph','Beta=1.1 Graph','Gabriel Graph','Beta=.95 Graph',...
%     'Beta=.9 Graph','Beta=.8 Graph','Beta=.5 Graph')       
%  xlabel('Query Sigma','FontSize',12)
%  ylabel('% of 2nd Nearest Neighbor','FontSize',12)
%  title('2nd Nearest Neighbor versus Query Sigma','FontSize',12)  
%  
%  
%   figure
%  plot(query, rng_data(:,3)/700,'Color',color(1,:),'LineWidth',3)
% axis([1.5 5.5 0 1])
% grid on
% hold on
% plot(query, beta14_data(:,3)/700,'Color',color(2,:),'LineWidth',3)
% plot(query, beta11_data(:,3)/700,'Color',color(3,:),'LineWidth',3)
% plot(query, gg_data(:,3)/700,'Color',color(4,:),'LineWidth',3)
% plot(query, beta95_data(:,3)/700,'Color',color(5,:),'LineWidth',3)
% plot(query, beta9_data(:,3)/700,'Color',color(6,:),'LineWidth',3)
% plot(query, beta8_data(:,3)/700,'Color',color(7,:),'LineWidth',3)
% plot(query, beta5_data(:,3)/700,'Color',color(8,:),'LineWidth',3)
%    
%  legend('Rng Graph','Beta=1.4 Graph','Beta=1.1 Graph','Gabriel Graph','Beta=.95 Graph',...
%     'Beta=.9 Graph','Beta=.8 Graph','Beta=.5 Graph')       
%  xlabel('Query Sigma','FontSize',12)
%  ylabel('% of 3nd Nearest Neighbor','FontSize',12)
%  title('3nd Nearest Neighbor versus Query Sigma','FontSize',12)  
%  
     
 
 

figure
nn=[1 2 3];
plot([1 2 3], cumsum(rng_data(1,:))/700,'Color',color(1,:),'LineWidth',3)
axis([1 3 0 1])
grid on
hold on
plot(nn, cumsum(beta14_data(1,:))/700,'Color',color(2,:),'LineWidth',3)
plot(nn, cumsum(beta11_data(1,:))/700,'Color',color(3,:),'LineWidth',3)
plot(nn, cumsum(gg_data(1,:))/700,'Color',color(4,:),'LineWidth',3)
plot(nn, cumsum(beta95_data(1,:))/700,'Color',color(5,:),'LineWidth',3)
plot(nn, cumsum(beta9_data(1,:))/700,'Color',color(6,:),'LineWidth',3)
plot(nn, cumsum(beta8_data(1,:))/700,'Color',color(7,:),'LineWidth',3)
plot(nn, cumsum(beta5_data(1,:))/700,'Color',color(8,:),'LineWidth',3)
   
 legend('Rng Graph','Beta=1.4 Graph','Beta=1.1 Graph','Gabriel Graph','Beta=.95 Graph',...
    'Beta=.9 Graph','Beta=.8 Graph','Beta=.5 Graph')       
 xlabel('Nearest Neighbor','FontSize',12)
 ylabel('% of 700 dataset','FontSize',12)
 title('Percentage of Queries within first three neighbors at query sigma of 1.73','FontSize',12)  
 
figure
nn=[1 2 3];
plot([1 2 3], cumsum(rng_data(2,:))/700,'Color',color(1,:),'LineWidth',3)
axis([1 3 0 1])
grid on
hold on
plot(nn, cumsum(beta14_data(2,:))/700,'Color',color(2,:),'LineWidth',3)
plot(nn, cumsum(beta11_data(2,:))/700,'Color',color(3,:),'LineWidth',3)
plot(nn, cumsum(gg_data(2,:))/700,'Color',color(4,:),'LineWidth',3)
plot(nn, cumsum(beta95_data(2,:))/700,'Color',color(5,:),'LineWidth',3)
plot(nn, cumsum(beta9_data(2,:))/700,'Color',color(6,:),'LineWidth',3)
plot(nn, cumsum(beta8_data(2,:))/700,'Color',color(7,:),'LineWidth',3)
plot(nn, cumsum(beta5_data(2,:))/700,'Color',color(8,:),'LineWidth',3)
   
 legend('Rng Graph','Beta=1.4 Graph','Beta=1.1 Graph','Gabriel Graph','Beta=.95 Graph',...
    'Beta=.9 Graph','Beta=.8 Graph','Beta=.5 Graph')       
 xlabel('Nearest Neighbor','FontSize',12)
 ylabel('% of 700 dataset','FontSize',12)
 title('Percentage of Queries within first three neighbors at query sigma of 2','FontSize',12)
 
 figure
nn=[1 2 3];
plot([1 2 3], cumsum(rng_data(3,:))/700,'Color',color(1,:),'LineWidth',3)
axis([1 3 0 1])
grid on
hold on
plot(nn, cumsum(beta14_data(3,:))/700,'Color',color(2,:),'LineWidth',3)
plot(nn, cumsum(beta11_data(3,:))/700,'Color',color(3,:),'LineWidth',3)
plot(nn, cumsum(gg_data(3,:))/700,'Color',color(4,:),'LineWidth',3)
plot(nn, cumsum(beta95_data(3,:))/700,'Color',color(5,:),'LineWidth',3)
plot(nn, cumsum(beta9_data(3,:))/700,'Color',color(6,:),'LineWidth',3)
plot(nn, cumsum(beta8_data(3,:))/700,'Color',color(7,:),'LineWidth',3)
plot(nn, cumsum(beta5_data(3,:))/700,'Color',color(8,:),'LineWidth',3)
   
 legend('Rng Graph','Beta=1.4 Graph','Beta=1.1 Graph','Gabriel Graph','Beta=.95 Graph',...
    'Beta=.9 Graph','Beta=.8 Graph','Beta=.5 Graph')       
 xlabel('Nearest Neighbor','FontSize',12)
 ylabel('% of 700 dataset','FontSize',12)
 title('Percentage of Queries within first three neighbors at query sigma of 3','FontSize',12)
 
 figure
nn=[1 2 3];
plot([1 2 3], cumsum(rng_data(4,:))/700,'Color',color(1,:),'LineWidth',3)
axis([1 3 0 1])
grid on
hold on
plot(nn, cumsum(beta14_data(4,:))/700,'Color',color(2,:),'LineWidth',3)
plot(nn, cumsum(beta11_data(4,:))/700,'Color',color(3,:),'LineWidth',3)
plot(nn, cumsum(gg_data(4,:))/700,'Color',color(4,:),'LineWidth',3)
plot(nn, cumsum(beta95_data(4,:))/700,'Color',color(5,:),'LineWidth',3)
plot(nn, cumsum(beta9_data(4,:))/700,'Color',color(6,:),'LineWidth',3)
plot(nn, cumsum(beta8_data(4,:))/700,'Color',color(7,:),'LineWidth',3)
plot(nn, cumsum(beta5_data(4,:))/700,'Color',color(8,:),'LineWidth',3)
   
 legend('Rng Graph','Beta=1.4 Graph','Beta=1.1 Graph','Gabriel Graph','Beta=.95 Graph',...
    'Beta=.9 Graph','Beta=.8 Graph','Beta=.5 Graph')       
 xlabel('Nearest Neighbor','FontSize',12)
 ylabel('% of 700 dataset','FontSize',12)
 title('Percentage of Queries within first three neighbors at query sigma of 5','FontSize',12)
 
 
       