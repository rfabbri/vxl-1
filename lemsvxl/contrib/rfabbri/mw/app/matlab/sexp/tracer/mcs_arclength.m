% Run after read_curve_sketch
l=zeros(nrecs,1);
for i=1:nrecs
  s = myarclength3(recs{i});
  l(i) = s(end);
end
