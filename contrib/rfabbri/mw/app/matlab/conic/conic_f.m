set(f,'WindowStyle','modal');
mycontinue = 1;
istep_small = 1;
istep_large = 10;

Tstep_val = 5;

while mycontinue
  figure(f);
  [xi,yi,but] = my_ginput(1);

  % left: 28
  % right: 29
  % up: 30
  % down: 31

  if (but == 31)
    disp 'Down';
    if (translate == 0)
      istep = -istep_small;
      conic_increment;
    else
      Tstep = [0 , -Tstep_val];
      conic_translate;
    end
    conic_delete;
    conic_compute;
    conic_draw;

%    phi = phi+dphi;
%    delete_sphere_point;
%    draw_sphere_point;
  elseif (but == 30) 
    disp 'Up';
    if (translate == 0)
      istep = istep_small;
      conic_increment;
    else
      Tstep = [0 , Tstep_val];
      conic_translate;
    end
    conic_delete;
    conic_compute;
    conic_draw;
  elseif (but == 29)
    disp 'Right button pressed';
    if (translate == 0)
      istep = istep_large;
      conic_increment;
    else
      Tstep = [Tstep_val, 0];
      conic_translate;
    end
    conic_delete;
    conic_compute;
    conic_draw;
  elseif (but == 28)
    disp 'Left button pressed';
    if (translate == 0)
      istep = -istep_large;
      conic_increment;
    else
      Tstep = [-Tstep_val, 0];
      conic_translate;
    end
    conic_delete;
    conic_compute;
    conic_draw;
  elseif (but == 43)
    disp '(+ button pressed)'
    Tstep_val = Tstep_val + 1;
    disp(['Translation stepsize:' int2str(Tstep_val)]); 
  elseif (but == 45)
    disp '(- button pressed)'
    Tstep_val = Tstep_val - 1;
    disp(['Translation stepsize:' int2str(Tstep_val)]); 
  elseif (but == 99) 
    disp '(c button pressed)'
  elseif (but == 116) 
    disp '(t button pressed)'
    if (translate == 0)
      translate = 1
      disp 'Translate objects using arrows'
    else
      translate = 0
      disp 'Change  3D point  using arrows'
    end
  elseif (but == 113) 
    disp 'Quit (q button pressed)'
    mycontinue = false;
    set(f,'WindowStyle','normal');
  else
    disp 'unknown button pressed';
    disp(int2str(but));
  end
end
