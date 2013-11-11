% input: err_v  where the rows represent error levels, and the columns the
% number of views (minus one)
figure
clf
hold on
x = 1:size(err_v,2);
x = x + 1;
plot(err_v(1,:),'k.-');
plot(err_v(2,:),'r.-');
xlabel('nviews');
ylabel('ntuplets');
legend('\Delta = 1', '\Delta = 2');
title('very short baseline');
