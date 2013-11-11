% This script plots the sigmoid-like curve f(x) = sqrt(a+1)*x/sqrt(1+a*x*x)
% (c) Nhon Trinh
% Date: Nov 12, 2008

x = [0 : 0.01 : 1];
as = [0 1 2 3 5 10]

h = figure;
colors = hsv(length(as));
hold on;
for i = 1 : length(as)
  a = as(i);
  y = sqrt(a+1)*x ./ sqrt(1 + a*x.*x);
  plot(x, y, 'Color', colors(i, :));
end;
hold off;

grid on;
xlabel('x');
ylabel('f(x)');
title('plots of f(x) = sqrt(a+1)x / sqrt(1 + ax^2)');
legend('a=0', 'a=1', 'a=2', 'a=3', 'a=5', 'a=10');




