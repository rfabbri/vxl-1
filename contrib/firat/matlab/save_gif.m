% This is save_gif.m.

% Created on: Apr 21, 2013
%     Author: firat

function save_gif(outfile, delay, do_maximize, do_append)
if do_maximize
    set(gcf, 'Position', get(0,'Screensize'));  
end
if ~exist(outfile, 'file')
   do_append = false; 
end
frame = getframe(1);
im = frame2im(frame);
[imind,cm] = rgb2ind(im,256);
if do_append
    imwrite(imind,cm,outfile,'gif','WriteMode','append','DelayTime', delay);    
else
    imwrite(imind,cm,outfile,'gif', 'Loopcount',inf,'DelayTime', delay);    
end
end

