function [fcirc,circ]=circle(step,r, x0,y0)

theta=0:step:2*pi;

xx=r*cos(theta)+x0;
yy=r*sin(theta)+y0;

circ = [xx' yy'];

rcirc = round(circ);

% remove dups
k=2;
fcirc = rcirc(1,:);
for i=2:size(theta,2)
  if ((rcirc(i,1) ~= fcirc(k-1,1)) | (rcirc(i,2) ~= fcirc(k-1,2)))
    fcirc(k,:) = rcirc(i,:);
    k = k+1;
  end
end
