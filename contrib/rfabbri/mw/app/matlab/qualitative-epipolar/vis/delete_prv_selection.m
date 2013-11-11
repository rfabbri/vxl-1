% to be called after polycursor
if (selected) %could just use ishandle
  if ishandle(h_poly)
    delete(h_poly)
  end
  if ishandle(h_pt)
    delete(h_pt)
  end
  if exist('h_poly_otherview') && ishandle(h_poly_otherview)
    delete(h_poly_otherview);
  end
end
