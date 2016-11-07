function myerrorbar(x,y,e,n)
  sz=size(x);
  sz=sz(1)*sz(2);

  i=1:n:sz;
  plot(x(i),y(i));
  e = e/2;
  hold
  plot(x(i),y(i)+e(i),'r');
  plot(x(i),y(i)+e(i),'r^');
  plot(x(i),y(i)-e(i),'rv');
  plot(x(i),y(i)-e(i),'r');
