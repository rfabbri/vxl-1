%
% just the event loop; to be called by mycursor3.m
%
figure(f);
set(f,'WindowStyle','modal');
mycontinue = 1;
while mycontinue
  figure(f);
  [xi,yi,but] = my_ginput(1);

  % left: 28
  % right: 29
  % up: 30
  % down: 31

  if (but == 31)
    disp 'Down';
    phi = phi+dphi;
    delete_sphere_point;
    draw_sphere_point;
  elseif (but == 30) 
    disp 'Up';
    phi = phi-dphi;
    delete_sphere_point;
    draw_sphere_point;
  elseif (but == 29)
    disp 'Right button pressed';
    theta = theta+dtheta;
    delete_sphere_point;
    draw_sphere_point;
  elseif (but == 28)
    disp 'Left button pressed';
    theta = theta-dtheta;
    delete_sphere_point;
    draw_sphere_point;
  elseif (but == 99) 
    disp 'Computing reconstruction (c button pressed)'
    system(mycmd);
    plot_corresp;
    plot_rec_error;
  elseif (but == 113) 
    disp 'Quit (q button pressed)'
    mycontinue = false;
    set(f,'WindowStyle','normal');
  else
    disp 'unknown button pressed';
    disp(int2str(but));
  end

end

