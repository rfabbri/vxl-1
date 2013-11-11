% uniformly distributed samples on a unit sphere
function T = random_t()
  T = randn(3,1);
  T = T/norm(T);
