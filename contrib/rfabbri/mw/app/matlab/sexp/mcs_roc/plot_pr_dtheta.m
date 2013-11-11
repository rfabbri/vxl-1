% To be called after read_pr_mcs


dtheta_precision = cell(1,90);
dtheta_recall = cell(1,90);
dtheta_distance = cell(1,90);
dtheta_v = [];
dist_v = [];

for i=1:prPoints
  stat = tree.dataset.stats(i);
  dtheta_i = stat.ATTRIBUTE.dtheta_threshold;
  dist_i = stat.ATTRIBUTE.distance_threshold;

  dtheta_precision{ceil(dtheta_i)}(end+1) = precision(i); 
  dtheta_recall{ceil(dtheta_i)}(end+1) = recall(i); 
  dtheta_distance{ceil(dtheta_i)}(end+1) = dist_i; 

  dtheta_v(end+1) = dtheta_i;
  dist_v(end+1) = dist_i;

%   recall_v.(['a' num2str(dtheta_i)]) with precision,recall
end

dtheta=unique(dtheta_v);
dist=unique(dist_v);

num_dtheta = length(dtheta);
clf;
hold on;
mrkv=['.o*+s'];
for i=1:num_dtheta
  [dtheta_recall{ceil(dtheta(i))}, ix] = sort(dtheta_recall{ceil(dtheta(i))});
  dtheta_precision{ceil(dtheta(i))} = dtheta_precision{ceil(dtheta(i))}(ix);
  mrk = mrkv(mod(i-1,length(mrkv))+1);
  h=plot(dtheta_recall{ceil(dtheta(i))},dtheta_precision{ceil(dtheta(i))},[mrk '-']);
  mycolor = rand(1,3)*0.7;
  mycolor(1) = min(mycolor(1)+rand()*0.4,1);
  mycolor(2) = min(mycolor(2)+rand()*0.2,1);
  mycolor(3) = min(mycolor(3)+rand()*0.1,1);
  set(h,'color',mycolor);
  set(h,'linewidth',2);
  set(h,'markersize',8);
  M{i} = [num2str(dtheta(i)) ' deg'];
  dtheta_distance{ceil(dtheta(i))} = dtheta_distance{ceil(dtheta(i))}(ix);
end
legend(M);
axis([0 1 0 1]);
xlabel('recall','FontSize',12)
ylabel('precision','FontSize',12)
h=title(['PR curve for ' tree.dataset.ATTRIBUTE.name],'FontSize',12);
set(h,'Interpreter','none');
grid on;
