% perturb curve c along normal, +- 0.5
function cp = perturb_normal(c,nrm)
  rnd = rand(size(nrm,1),1) - 0.5;
  rnd = [rnd rnd];

  cp = c + nrm.*rnd;

  % fix endpts
  cp(1,:) = c(1,:);
  cp(size(c,1),:) = c(size(c,1),:);
