function [fcirc,circ]=circle_any_theta(theta,r, x0,y0)
xx=r*cos(theta)+x0;
yy=r*sin(theta)+y0;

circ = [xx yy];

rcirc = round(circ);

// discretize and remove dups
k=2;
fcirc = rcirc(1,:);
for i=2:size(theta,2)
  if ((rcirc(i,1) ~= fcirc(k-1,1)) | (rcirc(i,2) ~= fcirc(k-1,2)))
    fcirc(k,:) = rcirc(i,:);
    k = k+1;
  end
end

endfunction
