% just the event loop; to be called by polycursor.m
%
figure(f);
set(f,'WindowStyle','modal');
mycontinue = 1;
selected = false;
while mycontinue
  figure(f);
  [xi,yi,but] = my_ginput(1);

  % Keyboard:
  %   left: 28
  %   right: 29
  %   up: 30
  %   down: 31

  if but == 1
%    disp 'Left mouse button click detected'
    delete_prv_selection;
    add_new_selection;
  elseif (but == 31)
    disp 'Down - ignored';
  elseif (but == 30) 
    disp 'Up - ignored';
  elseif (but == 29)
    disp 'Right button pressed - ignored';
  elseif (but == 28)
    disp 'Left button pressed - ignored';
  elseif (but == 99) 
    disp 'c button pressed (compute cost) - ignored'
  elseif (but == 111) 
    disp 'o button pressed (Optimize)'
    cursor_optimize_current;
  elseif (but == 113) 
    disp 'Quit (q button pressed)'
    mycontinue = false;
    delete_prv_selection;
    set(f,'WindowStyle','normal');
  else
    disp 'unknown button pressed';
    disp(int2str(but));
  end

end


