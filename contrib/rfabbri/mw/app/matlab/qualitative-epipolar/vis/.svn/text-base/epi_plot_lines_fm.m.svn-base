% Plot lines for fundamental matrix
%
% Returns handle to lines as a composite object
%
% Input: fig handle for each view 
% TODO: make input an axis handle
%
function [hg0,hg1,el0_lst,el1_lst] = epi_plot_lines_fm(fm,fig0,fig1,pts0,pts1,radius)

  if nargin < 5
    radius = 1000;
  end

  npts = size(pts0,1);
  el0_lst = zeros(npts,3);
  el1_lst = zeros(npts,3);
  for i=1:npts
    el1_lst(i,:) = fm*[pts0(i,:) 1]';
    el0_lst(i,:) = [pts1(i,:) 1]*fm;
  end

  figure(fig0);
  hold on;
  hg0 = hggroup;
  for i=1:npts
    h_line = plot_homg_line(el0_lst(i,:),radius);
    set(h_line,'Parent',hg0);
  end

  % Make composite object
  figure(fig1);
  hold on;
  hg1 = hggroup;
  for i=1:npts
    h_line = plot_homg_line(el1_lst(i,:),radius);
    set(h_line,'Parent',hg1);
  end

