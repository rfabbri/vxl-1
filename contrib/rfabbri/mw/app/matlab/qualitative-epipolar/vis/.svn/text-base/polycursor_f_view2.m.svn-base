% just the event loop; to be called by polycursor.m
%

figure(hf2);
set(hf2,'WindowStyle','modal');
mycontinue2 = 1;
selected2 = false;
while mycontinue2
  [xi1,yi1,but] = my_ginput(1);

  % Keyboard:
  %   left: 28
  %   right: 29
  %   up: 30
  %   down: 31

  if but == 1
    disp 'Left mouse button click detected'
    disp (num2str([xi1,yi1]));
    set(hf2,'WindowStyle','normal');
    mycontinue2 = false;
    delete_prv_selection2;
    add_new_selection2;
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
    disp 'o button pressed (Optimize) - ignored'
  elseif (but == 113) 
    disp 'Quit (q button pressed) - ignored'
%    mycontinue2 = false;
%    delete_prv_selection2;
  else
    disp 'unknown button pressed';
    disp(int2str(but));
  end

end
