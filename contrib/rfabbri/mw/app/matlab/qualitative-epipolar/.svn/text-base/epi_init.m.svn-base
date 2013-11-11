function epi = epi_init(e0,e1,r0,r1,refp)

  % initialize epi datastructure
  if (size(e0,1) > size(e0,2))
    e0 = e0';
  end
  if (size(e1,1) > size(e1,2))
    e1 = e1';
  end
  
  epi = struct('e',[e0; e1],'r',[r0,r1],'h',zeros(2,2),'refp',refp);
