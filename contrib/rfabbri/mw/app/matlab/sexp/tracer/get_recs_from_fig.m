figure(605);
% Get all curves into a recs cell array and handles array.

handles = findobj(gca,'Type','line');

ncurves = max(size(handles)); 
recs = cell(1,ncurves);
for i=1:ncurves
  x = get(handles(i),'xdata');
  y = get(handles(i),'ydata');
  z = get(handles(i),'zdata');
  recs{1,i} = [x' y' z'];
end

disp(['ncurves = ' num2str(max(size(recs)))])
