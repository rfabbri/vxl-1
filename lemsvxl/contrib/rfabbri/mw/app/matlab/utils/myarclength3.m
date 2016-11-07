% Input: 
%   - curve[x y] vector
% Output: 
%   - arc length at each point, with s[1] = 0;
function s=myarclength(crv)


  

n = size(crv,1);
s = zeros(n,1);

idx = 2:n;
dx = crv(idx,1) - crv(idx-1,1);
dy = crv(idx,2) - crv(idx-1,2);
dz = crv(idx,3) - crv(idx-1,3);
s(idx) = sqrt(dx.*dx + dy.*dy + dz.*dz);

for i=idx
  s(i) = s(i-1) + s(i);
end
