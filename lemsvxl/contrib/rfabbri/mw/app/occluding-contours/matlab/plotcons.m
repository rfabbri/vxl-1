function plotcons(prefix,n)
  
for i=1:n
  cc=loadcon([prefix '-' int2str(i) '.con']);
  figure(1);
  hold on;
  cplot2(cc,'g');
end
