% clear optimized points.

if exist('h_opt') && ishandle(h_opt(1))
  for i=1:max(size(h_opt))
    delete(h_opt(i));
  end
  clear h_opt;
end
