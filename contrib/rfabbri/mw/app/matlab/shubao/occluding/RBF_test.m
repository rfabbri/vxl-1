load('../debug/bunny_edge20/programdata.mat');
PN.Location = pts3d;
PN.Gradient = normal;
%fastrbf_view(PN,'fv')
Density = fastrbf_densityfromnormals(PN,0,1);
rbf = fastrbf_fit(Density, 0.001, 'reduce');
rbf = fastrbf_fit(Density, 0.0005, 'direct');
NewMesh = fastrbf_isosurf(rbf,0.01);
fastrbf_view(NewMesh);