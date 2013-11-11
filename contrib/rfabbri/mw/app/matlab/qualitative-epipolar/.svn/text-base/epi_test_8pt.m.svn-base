% make sure we get all 1127 pts
synth_point_data;

npts = max(size(sp0));

nsets = floor(npts/8);

d_mvl=zeros(1,nsets);

for i=1:nsets

  idx = randperm(8) + i-1;
%  pts8_0 = sp0(i:i+7,:);
%  pts8_1 = sp1(i:i+7,:);

  pts8_0 = sp0(idx,:);
  pts8_1 = sp1(idx,:);

  % estimate using only 8pts
  f = fmatrix(pts8_0,pts8_1,'mvl8pt');

  %measure error using all points
  d_mvl(i) = epi_geometric_error_f(f,sp0,sp1);

  % estimate using only 8pts
  f = fmatrix(pts8_0,pts8_1,'vpgl8pt');

  %measure error using all points
  d_vpgl(i) = epi_geometric_error_f(f,sp0,sp1);

end

d_vpgl = sqrt(d_vpgl)/npts;
d_mvl  = sqrt(d_mvl)/npts;

disp ('mvl:');
disp (['max: ' num2str(max(d_mvl)) '  min: ' num2str(min(d_mvl)) ...
 '  avg: ' num2str(mean(d_mvl)) '  std: ' num2str(std(d_mvl)) '  med: ' num2str(median(d_mvl))]);
disp ('vpgl:');
disp (['max: ' num2str(max(d_vpgl)) '  min: ' num2str(min(d_vpgl)) ...
 '  avg: ' num2str(mean(d_vpgl)) '  std: ' num2str(std(d_vpgl)) '  med: ' num2str(median(d_vpgl))]);
