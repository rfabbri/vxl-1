mainfig=610;
figure(mainfig);
clf;
hold on;
axis equal;
xlabel x;
ylabel y;
zlabel z;
title ('Main reconstruction');

nrecs = max(size(recs))

handles = zeros(1,nrecs);
for i=1:nrecs
  handles(i) = cplot(recs{i},'-');
  set(handles(i),'displayname',num2str(i))
end
axis tight;
grid on;

% save the plot
unix('cp all_recs.fig all_recs_old.fig');
hgsave('all_recs.fig');
