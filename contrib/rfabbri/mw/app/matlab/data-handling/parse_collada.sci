function res = child_by_name(node, name)
  for i = 1:node.children.size
    if node.children(i).name == name
      res = node.children(i);
      return
    end
  end
  res = -1
  error('not found')
endfunction

d=xmlRead('chair-test.dae')
v = child_by_name(d.root, 'library_visual_scenes');

found = %f
for i = 1:v.children.size
  if v.children(i).attributes('id') == '_-_time__night'
    v = v.children(i)
    found = %t
    break;
  end
end
if ~found
  error('not found 2')
end

// v is visual_scene for night scene

// for each node, get the object matrix and the points
for i=1:v.children.size
  node = v.children(i)
  prefix = node.attributes('id')
  m = child_by_name(node, 'matrix')
  if m.attributes('sid' ~= 'transform')
    error('matrix weird')
  end
  m = m.content
  execstr('tr = [' + m + ']');
  tr = matrix(tr,4,4)
end
