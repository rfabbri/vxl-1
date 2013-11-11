
% input: xi yi - position of a point inside desired polygon

mycolor = [0.5 0.5 0.5];
[ip_gt,is_gt] = find_poly(polys,[xi yi]);


found_p = (ip_gt ~= -1);

if (found_p)
  h_poly = fill(polys{ip_gt}{is_gt}(:,1),polys{ip_gt}{is_gt}(:,2),mycolor);
  h_pt   = cplot2([xi yi],'.');
  disp('  ');
  disp (['selected poly: ip= ' num2str(ip_gt) '  is= ' num2str(is_gt)]);
%  if f == figure(n_fig_cost) || f == figure(n_fig_cost+1)
  if exist('vcost')
    disp (['polygon cost: ' num2str(vcost(ip_gt)) ]);
    % compute all costs for selected polygon

    if (viewnum == 0) 
      nsheets1 = max(size(polys1{ip_gt}));
      costs = zeros(nsheets1,1);
      vv_lst = cell(nsheets1,1);
      for is=1:nsheets1
        ctroid = [mean(polys1{ip_gt}{is}(:,1)),mean(polys1{ip_gt}{is}(:,2))];
        [cost_tmp,vv_tmp] = four_line_all_tests([xi;yi],ctroid',pts0,pts1);
        vv_lst{is} = vv_tmp;
        costs(is) = cost_tmp;
      end
      [cost,is_min] = min(costs);
      vv = vv_lst{is_min};
%      figure(f+1);
%      h_poly_otherview = fill(polys1{ip_gt}{is_min}(:,1),polys1{ip_gt}{is_min}(:,2),mycolor);
%      figure(f);
    else
      nsheets0 = max(size(polys0{ip_gt}));
      costs = zeros(nsheets0,1);
      vv_lst = cell(nsheets0,1);
      for is=1:nsheets0
        ctroid = [mean(polys0{ip_gt}{is}(:,1)),mean(polys0{ip_gt}{is}(:,2))];
        [cost_tmp,vv_tmp] = four_line_all_tests(ctroid',[xi;yi],pts0,pts1);
        vv_lst{is} = vv_tmp;
        costs(is) = cost_tmp;
      end
      [cost,is_min] = min(costs);
      vv = vv_lst{is_min};
%      figure(f-1);
%      h_poly_otherview = fill(polys0{ip_gt}{is_min}(:,1),polys0{ip_gt}{is_min}(:,2),mycolor);
%      figure(f);
    end


    disp (['point final cost: ' num2str(cost) ]);
    disp (['point 4-costs: ']);
    pnum = [1234;2345;3456;4567;5678];
    disp([ vv pnum ]);
  end
else
  disp('Point not found in any polygon');
end
selected = true;
