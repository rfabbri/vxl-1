function myerrorbar(x,y,e,n)
  sz=size(x);
  sz=sz(1)*sz(2);



  clf
  hold

  step = floor(sz/4);
  for k=0:3
    i=(floor(step*k)+1):n:floor(step*(k+1));
    errorbar(x(i),y(i),e(i));
  end

  plot(x,y,'r');
%  plot(x,y,'ro');
