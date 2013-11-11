% Plot lines for fundamental matrix
%
% Returns handle to lines as a composite object
%
% Input: fig handle for each view 
% TODO: make input an axis handle
%
function [hg0,hg1,el0_lst,el1_lst] = epi_plot_lines_epi(epi_s,fig0,fig1,pts0,pts1,radius)

  if nargin < 5
    radius = 2.5; % in units of epi_s.r
  end

  figure(fig0);
  hold on;
  hg0 = hggroup;
  npts = size(pts0,1);
  el0_lst = zeros(npts,2);
  for i=1:npts
    [el0,retval]= epi_line(epi_s,pts0(i,:)',0);
    if retval == false
      warning('points on top of epipoles - skipping this match');
      continue;
    end
    el0_lst(i,:) = el0';
    pa = epi_s.e(1,:)';
    pb = pa + 2.5*el0;
    pa = pa - 2.5*el0;
    h_line=cplot2([pa'; pb'],'y');
    set(h_line,'Tag','epiline');
    set(h_line,'Parent',hg0);
  end

  figure(fig1);
  hold on;
  hg1 = hggroup;
  el1_lst = zeros(npts,2);
  for i=1:npts
    [el1,retval]= epi_line(epi_s,pts1(i,:)',1);
    if retval == false
      warning('points on top of epipoles - skipping this match');
      continue;
    end
    el1_lst(i,:) = el1';

    pa = epi_s.e(2,:)';
    pb = pa + 2.5*el1;
    pa = pa - 2.5*el1;
    h_line=cplot2([pa'; pb'],'y');
    set(h_line,'Tag','epiline');
    set(h_line,'Parent',hg1);
  end

