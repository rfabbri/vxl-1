% to be called by polycursor_f

if ~selected
  disp('No polygon selected - command IGNORED');
else
  ip_opt = ip_gt;
  delete_hopt;
  if viewnum == 0
    is0_opt = is_gt;
    for is=1:nsheets1
      is1_opt = is;
      ctroid = [mean(polys1{ip_gt}{is1_opt}(:,1));mean(polys1{ip_gt}{is1_opt}(:,2))];
      ev_ini  = [xi;yi; ctroid];
      epioptimize_f; 
      plot_opt
    end
  else
    warning('Ignoring - OPTIMIZE not yet tested for view 2');
%    is0_opt = is_min;
%    is1_opt = is_gt;
%    ctroid = [mean(polys0{ip_gt}{is0_opt}(:,1));mean(polys0{ip_gt}{is0_opt}(:,2))];
%    ev_ini  = [ctroid; xi; yi];
  end
end

