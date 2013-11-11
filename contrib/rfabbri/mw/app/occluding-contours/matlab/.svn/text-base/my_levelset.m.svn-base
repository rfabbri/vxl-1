function phi3 = my_levelset(phi3, img3, dxdydz, iterations, direction, delta_t, T, beta, moviename)
% function phi3 = anisotropic_evolve_g3(phi3, img3, dx, dy, dz, iterations, direction, delta_t, T, beta, moviename)
% Compute 3D levelset evolution given an anisotropic 3D volume data img3 and
% initial surface phi3

phi_orig=phi3;
[h w d] = size(phi3);

fig1 = figure(1);
%figure(fig1);
%preset_view = view;

%anisotropic_showiso(phi3, dxdydz);
%view(preset_view);

%movie recording------------------------------------------
record = 0;
if(findstr(moviename,'mpg') > 0) record =1; end
%record

if(record)
  winsize = get(fig1,'Position');
  winsize(1:2) = [0 0];
  frames = moviein(iterations+1,fig1,winsize);
  set(fig1,'NextPlot','replacechildren');

  title(sprintf('Iteration %d, dt = %1.4f, T = %3.2f',0,delta_t,T));
  frames(:,1)= getframe(fig1,winsize);
end
%-------------------------------------------------------





%image_information3(img,T,exponent for g,sigma for gaussian)

fprintf('image_information3 ...');
tic
sigma = 0.5;
[img_gradient_mag, gmap, gx, gy, gz] = anisotropic_image_information3(img3, dxdydz, T, 2, sigma);
%[img_gradient_mag gmap gx gy gz] = image_information3(img,T,2,0.5);
toc

clear img_gradient_mag

% % REMOVE ME - temporarily added ----------------
% exponent = 2;
% gmap = (ones(h,w,d)./((ones(h,w,d) +(img3/T).^exponent)));

for t = 1:iterations	
  fprintf('%d ',t);
  
	%compute upwinded grad phi (hj_flux) and curvature term
	fprintf('surface_information3 ...');
	tic
  [hj_flux, curvature, Dxc, Dyc, Dzc ] = anisotropic_surface_information3(phi3, dxdydz, direction);
	%[hj_flux,curvature, Dxc, Dyc,Dzc] = surface_information3(phi,direction);
	toc

	%temporarily calculate speed value at every level set
	speed1 = gmap.*(beta*curvature - direction);

	%expand speed values from zero level set to other level sets
%	speed = speed_expand(phi,speed1);
	speed = speed1;

	phi3 = phi3 + delta_t*(speed.*hj_flux);

%	if(record)
%		figure(1); 
%		clf;
%		anisotropic_showiso(phi3, dxdydz);
%		title(sprintf('Iteration %d, dt = %1.4f, T = %3.2f',t,delta_t,T));
		%view(preset_view);
		%frames(:,t+1)= getframe(fig1,winsize);
%  else
%  elseif (mod(t,10) == 0)
%    anisotropic_showiso(phi3, dxdydz);
%		view(preset_view);
%  end;
  showseg2(4,t,img3,phi_orig,phi3);


	%every 60 or so iterations, reinitialize phi to be
	%a distance transform
	%if(mod(t,60) == 0)
	%	phi = dt_from_contour(phi);
	%	fprintf('.');
	%end


end

if(record)
  mpgwrite(frames,colormap,moviename);
end


fprintf('\n');
