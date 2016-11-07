% Epipole has finite representation (?)
% View: 0 or 1
function lh = epi_line_to_homg_coords(epi,el,view)
  N = [-el(2) el(1)];
  lh = [N(1); N(2); -N*(epi.e(view+1,:)' + el)];
