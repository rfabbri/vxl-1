% to be called in the end of t_simulation

n_squares = 50;
thetam = zeros(n_squares,n_squares);
km = zeros(n_squares,n_squares);

vtheta1_n = round((vtheta1/pi)*(n_squares-1)) + 1;
vtheta2_n = round((vtheta2/pi)*(n_squares-1)) + 1;

vk1_n = round(((vk1 + 2000)/4000)*(n_squares-1)) + 1;
vk2_n = round(((vk2 + 2000)/4000)*(n_squares-1)) + 1;

for i=1:npts
  thetam(vtheta2_n(i),vtheta1_n(i)) = thetam(vtheta2_n(i),vtheta1_n(i)) + 1;
  if (vk1_n(i) < n_squares &  vk1_n(i) > 0 & ...
      vk2_n(i) < n_squares &  vk2_n(i) > 0)
    km(vk1_n(i),vk2_n(i)) = km(vk1_n(i),vk2_n(i)) + 1;
  end
end
