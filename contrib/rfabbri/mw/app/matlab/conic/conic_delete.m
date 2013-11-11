% to be called after conic_draw
delete(hpt);
if (translate ~=0)
  delete(hcirc);
end
delete(hmymodel);
for i=1:length(hsha)
  delete(hsha(i));
end
