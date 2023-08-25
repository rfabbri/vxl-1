%mainfig=610;
%figure(mainfig);
figure;
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
    handles(i) = cplot(recs{i},'-b');
    if(i==1)
        subs = recs{i}(1:5:size(recs{i},1),:);
        subs_t = tangs{i}(1:5:size(tangs{i},1),:);
        quiver3(subs(:,1),subs(:,2),subs(:,3),subs_t(:,1),subs_t(:,2),subs_t(:,3),0.1,'Color','r');
    end
    set(handles(i),'displayname',num2str(i))
end

axis tight;
grid on;

% save the plot
unix('cp all_recs.fig all_recs_old.fig');
hgsave('all_recs.fig');
% hold off;