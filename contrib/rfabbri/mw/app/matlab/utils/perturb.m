function cp=perturb(c,noise_level)

if (nargin == 1)
  noise_level = 0.5;
end
cp = c + (2*rand(size(c))-1)*noise_level;
