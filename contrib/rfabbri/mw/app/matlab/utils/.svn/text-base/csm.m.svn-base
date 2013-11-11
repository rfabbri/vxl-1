% c: digital curve
% psi: how much to evolve at a step
% nsteps: number of steps
%
function [cs,nrm]=csm(c,psi,nsteps)
  n = size(c,1);
  
  nrm = zeros(size(c));
  cs = zeros(size(c));
  % fix endpts 
  cs(1,:) = c(1,:);
  cs(n,:) = c(n,:);

for ns=1:nsteps
  for i=2:(n-1)
    v1 = c(i-1,:) - c(i,:);
    v2 = c(i+1,:) - c(i,:);
    nv1 = norm(v1);
    nv2 = norm(v2);

    v1n = v1/nv1;
    v2n = v2/nv2;

    if nv1 < nv2
      nrm(i,:) = (v1 + (v2n)*nv1)/4;
    else
      nrm(i,:) = (v2 + (v1n)*nv2)/4;
    end

    cs(i,:) = c(i,:) + psi*nrm(i,:);
  end
  c = cs;
end
