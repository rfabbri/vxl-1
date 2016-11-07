npolys = max(size(polys0));
vcost = zeros(npolys,1);
isheet_vcost = zeros(npolys,2); % indices is0, is1 of the sheets leading to vcost

for ip=1:npolys
  nsheets0 = max(size(polys0{ip}));
  nsheets1 = max(size(polys1{ip}));

  if (nsheets0 ~= nsheets1)
%    disp(['nsheets0: ' num2str(nsheets0) ' nsheets1: ' num2str(nsheets1)]);
%    warning('corresponding polygons with different nsheets!');
  end

  if (nsheets0 > 2 || nsheets1 > 2) 
    warning('number of sheets more than 2');
  end
  if (nsheets0*nsheets1 == 0)
    error('no sheets');
  end

  costs_tmp = zeros(nsheets0*nsheets1,1);
  isheet_tmp  = zeros(nsheets0*nsheets1,2); 
  i=1;
  for is0=1:nsheets0
    for is1=1:nsheets1
      costs_tmp(i) = four_line_poly_test(polys0{ip}{is0},polys1{ip}{is1},pts0,pts1);
%      costs_tmp(i) = epi_cost_of_polygon(polys0{ip}{is0},polys1{ip}{is1},pts0,pts1);
      isheet_tmp(i,1) = is0;
      isheet_tmp(i,2) = is1;
      i = i+1; 
    end
  end

  [vcost(ip),id_min] = min(costs_tmp);
  isheet_vcost(ip,:) = isheet_tmp(id_min,:);
end

[vcost_s,sorted_ip] = sort(vcost,'descend');
