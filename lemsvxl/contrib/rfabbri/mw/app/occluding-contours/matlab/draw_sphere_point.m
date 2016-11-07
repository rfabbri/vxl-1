s = sphere_point(theta,phi);
h=cplot(s,'.r');
set(h,'MarkerSize',20.0);
h2=plot3([0 s(1)],[0 s(2)],[0 s(3)],'r');
set(h2,'LineWidth',2.0);

mycmd = ['perturb_cmd -theta ' num2str(180*theta/pi) ' -phi ' num2str(180*phi/pi)]

