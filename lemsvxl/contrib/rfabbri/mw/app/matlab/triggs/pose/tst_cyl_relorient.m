% Test routine for relative orientation from ncyl>=2 cylinders.  Generates
% random relative camera motion of about camrot degrees rotation and
% camtrans units translation, about camdist units from the scene, which
% contains ncyl cylinders of random radius near 1 unit, rotated randomly
% by around cylrot degrees from the x axis, and translated randomly by
% about cyltrans units. Noise of noise radians is added to the
% calibrated images of the edge-lines of the cylinders. (Things get
% pretty random for more than about 1e-3 radians noise).

function tst_cyl_relorient(ncyl,camrot,camtrans,camdist,cylrot,cyltrans,noise,do_radii)

   % camera 1 translates by t1 then rotates by R1 w.r.t. camera 2

   R1 = quat_to_rot([pi*camrot/180 * randn(3,1); 1]);
   t1 = camtrans * randn(3,1);
   P1 = [ R1, -R1 * t1 ];
   P2 = [ eye(3,4) ];

   % Make ncyl random cylinders and project them into the two
   % images. Each cylinder is defined by rotating a cylinder along 
   % the x-axis by R, then translating it by t. The `centre' is a point
   % on the axis (image of the origin), used only to decide which
   % image region is `inside' the cylinder. 

   radii = []; 
   dirns = [];  
   centres = [];
   cyls1 = [];
   cyls2 = [];
   i = 1;
   while (i<=ncyl)
      R = quat_to_rot([pi*cylrot/180 * randn(3,1); 1]);
      t = cyltrans * randn(3,1) + [0; 0; camdist];
      Rt = [R,t; zeros(1,3),1];
      radius = exp( randn(1) );
      dirn = [R(:,1); 0];
      centre = [t; 1];
      section = Rt * diag([0, radius^2, radius^2, -1]) * Rt';

      [ea,eb] = proj_cone(P1,dirn,section,centre,noise);
      [ec,ed] = proj_cone(P2,dirn,section,centre,noise);

      % Reject cylinders which happen to contain camera centre,
      % as their images aren't real.

      if (all(imag([ea,eb,ec,ed])==0)) 
	 cyls1 = [ cyls1, ea, eb ];
	 cyls2 = [ cyls2, ec, ed ];
	 dirns = [ dirns, dirn(1:3) ];
	 centres = [ centres, centre(1:3) ];
	 radii = [ radii, radius ];
	 i = i+1;
	 % else warning("Rejecting cylinder containing camera");
      end;
   end;

   % Solve for relative orientation and 3D structure from cylinder
   % images. Check results by choosing 3D scale so that translation
   % norms agree, then comparing [R,t] to [R1,t1] and recovered 3D
   % cylinders to ground truth. Each column of errs contains
   % (1) error in measured cylinder axis direction, (2) offset of measured
   % centre from true axis, (3) error in measured radius.

   if do_radii
      [R, t, cond, cyls3D] = cyl_relorient(cyls1,cyls2,radii);
   else
      [R, t, cond, cyls3D] = cyl_relorient(cyls1,cyls2);
   end

   scale = norm(t1,2)/norm(t,2);
   t = scale * t;
   cyls3D(4:7,:) = scale * cyls3D(4:7,:);
   [R * R1', t - t1]
   [ norm(R - R1), norm(t - t1)/camdist, cond, scale]
   errs = zeros(3,ncyl);
   errs(1,:) = sqrt(sumsq( dirns - cyls3D(1:3,:) ));
   errs(3,:) = (radii - cyls3D(7,:)) ./ radii;
   for i = 1:ncyl
      errs(2,i) = norm(vcross(dirns(:,i),cyls3D(4:6,i)-centres(:,i)))/camdist;
   end;
   errs
%end;
