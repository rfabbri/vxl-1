
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
    disp (['polygon rank cost (ssq): ' num2str(vcost(ip_gt)) ]);
    % compute all costs for selected polygon

    if viewnum == 0
      xi0 = xi;
      yi0 = yi;
      nsheets1 = max(size(polys1{ip_gt}));
      costs = zeros(nsheets1,1);
      vv_lst = cell(nsheets1,1);
      ctroid0 = [mean(polys0{ip_gt}{is_gt}(:,1)),mean(polys0{ip_gt}{is_gt}(:,2))];

      for is=1:nsheets1
        ctroid1 = [mean(polys1{ip_gt}{is}(:,1)),mean(polys1{ip_gt}{is}(:,2))];
        [cost_tmp,vv_tmp] = four_line_all_tests_geometric(ctroid0',ctroid1',pts0,pts1);
        vv_lst{is} = vv_tmp;
        costs(is) = cost_tmp;
      end
      [cost,is_min] = min(costs);
      vv = vv_lst{is_min};

      % Show costs for centroids
      opt_poly0 = polys0{ip_gt}{is_gt};
      opt_poly1 = polys1{ip_gt}{is_min};
      [d_tmp,e0_best,e1_best,epi_s] = epi_cost_of_polygon(opt_poly0,opt_poly1,pts0,pts1);

      [d,retval,n_inst,vv] = epi_geometric_error(epi_s,ap0,ap1);
      disp (['ctroid final cost (ssq): ' num2str(cost) ]);
      disp (['ctroid all costs: ' show_error(vv) ]);

      % xxx Show random points
%      rand_pts = random_polygon_points(polys0{ip_gt}{is_gt},15);
%      cplot2(rand_pts,'k.');

      % Show corresponding polygon in view 2
      hf2 = figure(f+1);
      h_poly_otherview = fill(polys1{ip_gt}{is_min}(:,1),polys1{ip_gt}{is_min}(:,2),mycolor);

      % Allow the user to select another point in view 2
      polycursor_f_view2;
      figure(f);

      % Show final cost
      opt_poly0 = polys0{ip_gt}{is_gt};

      [ip_gt_1,is_gt_1] = find_poly(polys1,[xi1 yi1]);
      found_p1 = (ip_gt_1 ~= -1);
      if (found_p1)
        disp (['selected poly in second view: ip= ' num2str(ip_gt_1) '  is= ' num2str(is_gt_1)]);
        opt_poly1 = polys1{ip_gt_1}{is_gt_1};

        [cost_tmp,vv_tmp,n_inst,epi_s] = four_line_all_tests_geometric([xi0;yi0],[xi1;yi1],pts0,pts1);

        [d,retval,n_inst,vv] = epi_geometric_error(epi_s,ap0,ap1);
        disp (['selected final cost (ssq): ' num2str(cost) ]);
        disp (['selected all costs: ' show_error(vv) ]);
      else
        disp(['Point in Second View is NOT in any polygon']);
      end
    else
       disp('Starting in 2nd view not supported.');
%      nsheets0 = max(size(polys0{ip_gt}));
%      costs = zeros(nsheets0,1);
%      vv_lst = cell(nsheets0,1);
%      for is=1:nsheets0
%        ctroid = [mean(polys0{ip_gt}{is}(:,1)),mean(polys0{ip_gt}{is}(:,2))];
%        [cost_tmp,vv_tmp] = four_line_all_tests(ctroid',[xi;yi],pts0,pts1);
%        vv_lst{is} = vv_tmp;
%        costs(is) = cost_tmp;
%      end
%      [cost,is_min] = min(costs);
%      vv = vv_lst{is_min};
%      figure(f-1);
%      h_poly_otherview = fill(polys0{ip_gt}{is_min}(:,1),polys0{ip_gt}{is_min}(:,2),mycolor);
%      figure(f);
    end

%     if (viewnum == 0) 
%       nsheets1 = max(size(polys1{ip_gt}));
%       costs = zeros(nsheets1,1);
%       vv_lst = cell(nsheets1,1);
%       for is=1:nsheets1
%         ctroid = [mean(polys1{ip_gt}{is}(:,1)),mean(polys1{ip_gt}{is}(:,2))];
%         [cost_tmp,vv_tmp] = four_line_all_tests([xi;yi],ctroid',pts0,pts1);
%         vv_lst{is} = vv_tmp;
%         costs(is) = cost_tmp;
%       end
%       [cost,is_min] = min(costs);
%       vv = vv_lst{is_min};
% %      figure(f+1);
% %      h_poly_otherview = fill(polys1{ip_gt}{is_min}(:,1),polys1{ip_gt}{is_min}(:,2),mycolor);
% %      figure(f);
%     else
%       nsheets0 = max(size(polys0{ip_gt}));
%       costs = zeros(nsheets0,1);
%       vv_lst = cell(nsheets0,1);
%       for is=1:nsheets0
%         ctroid = [mean(polys0{ip_gt}{is}(:,1)),mean(polys0{ip_gt}{is}(:,2))];
%         [cost_tmp,vv_tmp] = four_line_all_tests(ctroid',[xi;yi],pts0,pts1);
%         vv_lst{is} = vv_tmp;
%         costs(is) = cost_tmp;
%       end
%       [cost,is_min] = min(costs);
%       vv = vv_lst{is_min};
% %      figure(f-1);
% %      h_poly_otherview = fill(polys0{ip_gt}{is_min}(:,1),polys0{ip_gt}{is_min}(:,2),mycolor);
% %      figure(f);
%     end


  end
else
  disp('Point not found in any polygon');
end
selected = true;
