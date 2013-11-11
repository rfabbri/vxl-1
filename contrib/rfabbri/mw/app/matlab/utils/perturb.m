function cp=perturb(c,noise_level)

if (nargin == 1)
  noise_level = 0.5;
end
cp = c + (rand(size(c))-0.5)*noise_level;
