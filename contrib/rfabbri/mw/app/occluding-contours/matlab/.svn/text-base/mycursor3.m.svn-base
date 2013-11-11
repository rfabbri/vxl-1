r = 1;
s_center = [0 0 0]';

f=figure(40);
clf

nsphere = 20; % number of divisions to sphere
nangles = 120; % number of divisions to sphere
mymap=gray;
sphere(nsphere);
hold on
mymap(:,2)=1;
mymap(:,1)=mymap(:,1).^1.5;
mymap(:,3)=mymap(:,3).^1.5;


colormap(mymap);
alpha(0.7);

% Plots vector indicating (1,0,0);
%
%h=plot3([0 1],[0 0], [0 0],'k');
%set(h,'LineWidth',3.0);
%h2=plot3([1],[0], [0],'.k');
%set(h2,'MarkerSize',20.0);

readcam;

% Plots vector indicating original camera position
dC=c1-c0;
dC = dC/norm(dC);
%vpn = vp/norm(vp);

h=plot3([0 dC(1)],[0 dC(2)], [0 dC(3)]);
set(h,'LineWidth',3.0);
h2=cplot(dC','.');
set(h2,'MarkerSize',20.0);


dtheta = (2*pi)/nangles;
dphi = dtheta/2;

[theta,phi] = cart2sph(dC(1),dC(2),dC(3));
phi = 0.5*pi-phi;
%theta = 0;
%phi   = pi/2;

draw_sphere_point;

axis equal
xlabel x;
ylabel y;
zlabel z;

%camorbit(95,-10)
camorbit(-80,-10)

mycursor3_f;

