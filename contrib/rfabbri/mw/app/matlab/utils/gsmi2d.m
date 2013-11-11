% smooth each coordinate
% bnd - boundary condition
function csm = gsmi2d(c,sigma,s,do_rescale,bnd)

if nargin == 2
  nel = max(size(c));
  s = (1:nel)';
end

if nargin < 3
  do_rescale = 1;
end

if nargin < 5
  bnd = 'regular';
end

csm = zeros(size(c));

if strcmp(bnd,'regular')
  csm(:,1) = gsmi(c(:,1),sigma,s,do_rescale);
  csm(:,2) = gsmi(c(:,2),sigma,s,do_rescale);
else % circular
  csm(:,1) = gsmi_circ(c(:,1),sigma,s,do_rescale);
  csm(:,2) = gsmi_circ(c(:,2),sigma,s,do_rescale);
end

