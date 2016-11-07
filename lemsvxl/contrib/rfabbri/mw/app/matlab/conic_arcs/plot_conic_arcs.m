% Plot all conics as 0-contours:

% input: crv = nx2 array of points
% input: conics = nx5 array of conics 
%figure;
%clf;
conics = cs;

% We just show all conics plotted within the bounding box of the curve.

min_x = min(crv(:,1));
min_y = min(crv(:,2));

max_x = max(crv(:,1));
max_y = max(crv(:,2));

xx = (min_x -20): 1 : (max_x+20);
yy = (min_y -20): 1 : (max_y+20);

nc = size(conics,1);
zz = zeros(length(yy),length(xx));

for ic=1:nc, 
  for i=1:length(xx), 
    for j = 1:length(yy)
      zz(j,i) = ...
      [ xx(i)*xx(i), xx(i)*yy(j), yy(j)*yy(j), xx(i), yy(j), 1] * cs(ic,:)';
    end
  end
  [C,hsha]=contour(xx,yy,zz,[0 0]);
  set(hsha,'color',rand(1,3)/2+0.3);
  hold on;
end
clear C;
