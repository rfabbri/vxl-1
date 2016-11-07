% epi - euclidean epipolar geometry data structure
% pts0 and pts1: nx2 point vectors
% retval - true if no instabilities; false if stabilities occurred for some
% point (e.g. epipole coincides with some point). Cost is not accumulated for
% such points.
% n_inst - number of points for which cost was ignored
function [d,retval,n_inst,vv] = epi_geometric_error(epi,pts0,pts1)



  n_inst = 0;
  d=0;
  vv = zeros(size(pts0,1),1);
%  clear_epilines;
  for i=1:size(pts0,1)
    p0 = pts0(i,:)';
    p1 = pts1(i,:)';
    [el1,retval1] = epi_point_line_0_to_1(epi,p0);
    [el0,retval2] = epi_point_line_1_to_0(epi,p1);

    retval = true;
    if retval1 == false || retval2 == false
%      warning('Instabilities in transferring epipolar lines');
      retval = false;
      n_inst = n_inst+1;
      vv(i) = Inf;
      continue;
    end


%    clear_epilines;
%    figure(1);
%    hold on;
%    epi_plot_line(epi,el0,0,'y'); 
%    figure(2);
%    hold on;
%    epi_plot_line(epi,el1,1,'y'); 
%    i
%    keyboard;

%   el1_hmg = epi_line_to_homg_coords(epi,el1,1);
%   INLINE
    N = [-el1(2) el1(1)];
    el1_hmg = [N(1); N(2); -N*(epi.e(2,:)' + el1)];

%   el0_hmg = epi_line_to_homg_coords(epi,el0,0);
%   INLINE
    N = [-el0(2) el0(1)];
    el0_hmg = [N(1); N(2); -N*(epi.e(1,:)' + el0)];

%    plot_homg_line(el0_hmg); 

    d0 = perp_dist_squared_homg([p0', 1]',el0_hmg);
    d1 = perp_dist_squared_homg([p1', 1]',el1_hmg);

    d = d + d0 + d1;

    vv(i) = d0 + d1;
  end

  vv = sqrt(vv/2); % rms
%  hold off;
