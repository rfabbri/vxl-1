plot(c(1),c(2),'ko');
if (translate ~=0)
  hcirc=cplot2(Gamma);
end
hmymodel=cplot2(Gamma_model','r');
[C,hsha]=contour(xx,yy,zz,[0 0]);
clear C;
hpt=cplot2(Tbar,'o');
