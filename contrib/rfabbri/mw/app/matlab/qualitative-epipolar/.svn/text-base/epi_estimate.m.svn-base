% Geometric estimation of epipolar geometry. This is used as a non-robust
% minimal algorithm. Any number of points greater or equal to 3 can be provided
% isheet_vcost: indices is0, is1 of the sheets leading to vcost
%
function [top_epi,top_poly_idx,polys0,polys1,vcost,isheet_vcost] = epi_estimate(pts0,pts1,box,n_polys_to_inspect)

if nargin < 4
  refine_using_random_pts = true;
  n_polys_to_inspect = 15;
end

% - construct polygons
try
  [polys0,polys1] = episearch(pts0,pts1,box);
catch
  disp(['Error message in epi_estimate: ' lasterror.message]);
  lst=lasterror;
  disp('epi_estimate: Catched an error in episearch -- degeneracy occurred.'); 
  rethrow(lst);
end

% - rank-order
npolys = max(size(polys0));
vcost  = zeros(npolys,1);
isheet_vcost = zeros(npolys,2); % indices is0, is1 of the sheets leading to vcost
v_epi_poly = cell(npolys,1); % epi structures

for ip=1:npolys
  nsheets0 = max(size(polys0{ip}));
  nsheets1 = max(size(polys1{ip}));

  if (nsheets0 ~= nsheets1)
%    disp(['nsheets0: ' num2str(nsheets0) ' nsheets1: ' num2str(nsheets1)]);
%    warning('corresponding polygons with different nsheets!');
  end

%  if (nsheets0 > 2 || nsheets1 > 2) 
%    warning('number of sheets more than 2');
%  end
  if (nsheets0*nsheets1 == 0)
    error('no sheets');
  end

  costs_tmp    = zeros(nsheets0*nsheets1,1);
  isheet_tmp   = zeros(nsheets0*nsheets1,2); 
  v_epi_sheets = cell(nsheets0*nsheets1,1);
  i=1;
  for is0=1:nsheets0
    for is1=1:nsheets1
      poly0 = polys0{ip}{is0};
      poly1 = polys1{ip}{is1};
      c0 = [mean(poly0(:,1)); mean(poly0(:,2))];
      c1 = [mean(poly1(:,1)); mean(poly1(:,2))]; 
      [cost,cost_v,n_inst,epi_s,res] = epi_estimate_h_3pt_epipoles(c0,c1,pts0,pts1);
      v_epi_sheets{i} = epi_s;
      costs_tmp(i) = cost;

      isheet_tmp(i,1) = is0;
      isheet_tmp(i,2) = is1;
      i = i+1; 
    end
  end

  [vcost(ip),id_min] = min(costs_tmp);
  isheet_vcost(ip,:) = isheet_tmp(id_min,:);
  v_epi_poly{ip} = v_epi_sheets{id_min};
end

[vcost_s,sorted_ip] = sort(vcost,'descend');

% - Output estimated epipolar geometry and top polygons

ip_opt    = sorted_ip(end);
top_poly_idx = ip_opt;
is0_opt   = isheet_vcost(ip_opt,1);
is1_opt   = isheet_vcost(ip_opt,2);
top_epi   = v_epi_poly{ip_opt};







%% Now that the polygons have been ranked through the centroids, we will pick
% random points to further improve the estimate. The search is for each polygon,
% each sheet within each polygon, and each point within each sheet. We cannot
% just go directly to the best sheets as ranked in the previous step because
% there is an example for which the polygon is right, but the sheet is wrong
% (when only using centroids).

if refine_using_random_pts
  npolys = min(n_polys_to_inspect,max(size(sorted_ip))-1) + 1;

  vcost_ref = zeros(npolys,1);
  isheet_vcost_ref = zeros(npolys,2); % indices is0, is1 of the sheets leading to vcost
  v_epi_poly_ref = cell(npolys,1); % epi structures
  for ip=0:(npolys-1)
    ip_opt    = sorted_ip(end-ip);
    nsheets0 = max(size(polys0{ip_opt}));
    nsheets1 = max(size(polys1{ip_opt}));

    if (nsheets0*nsheets1 == 0)
      error('no sheets');
    end

    i=1;
    costs_tmp    = zeros(nsheets0*nsheets1,1);
    isheet_tmp   = zeros(nsheets0*nsheets1,2); 
    v_epi_sheets = cell(nsheets0*nsheets1,1);
    valid_sheet_run = zeros(nsheets0*nsheets1,1);
    for is0 = 1:nsheets0
      for is1=1:nsheets1
        disp (['Polygon pair#' num2str(i)]);
        opt_poly0 = polys0{ip_opt}{is0};
        opt_poly1 = polys1{ip_opt}{is1};
        try
          [d,e0_best,e1_best,epi_s] = epi_cost_of_polygon(opt_poly0,opt_poly1,pts0,pts1);
        catch
          disp('Discarding this pair');
          disp(['Error message: ' lasterror.message]);
          continue;
        end
        valid_sheet_run(i) = 1;
        v_epi_sheets{i} = epi_s;
        costs_tmp(i) = d;

        isheet_tmp(i,1) = is0;
        isheet_tmp(i,2) = is1;
        i = i + 1;
      end
    end
    id_valid_sh = find(valid_sheet_run==1);
    costs_tmp = costs_tmp(id_valid_sh);
    isheet_tmp = isheet_tmp(id_valid_sh,:);
    v_epi_sheets = v_epi_sheets(id_valid_sh);

    [vcost_ref(ip+1),id_min] = min(costs_tmp);
    isheet_vcost_ref(ip+1,:) = isheet_tmp(id_min,:);
    v_epi_poly_ref{ip+1} = v_epi_sheets{id_min};
  end

  [vcost_ref_s,sorted_ip_ref] = sort(vcost_ref,'descend');

  % - Output estimated epipolar geometry and top polygons

  ip_opt_ref = sorted_ip_ref(end);
  ip_opt    = sorted_ip(end - ip_opt_ref+1);
  top_poly_idx = ip_opt;
  is0_opt   = isheet_vcost_ref(ip_opt_ref,1);
  is1_opt   = isheet_vcost_ref(ip_opt_ref,2);
  top_epi   = v_epi_poly_ref{ip_opt_ref};
end
