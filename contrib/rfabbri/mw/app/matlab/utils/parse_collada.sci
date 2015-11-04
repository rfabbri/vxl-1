function mat = eul_to_mat3(eul)
	// double ci, cj, ch, si, sj, sh, cc, cs, sc, ss;

	ci = cos(eul(1));
	cj = cos(eul(2));
	ch = cos(eul(3));
	si = sin(eul(1));
	sj = sin(eul(2));
	sh = sin(eul(3));
	cc = ci * ch;
	cs = ci * sh;
	sc = si * ch;
	ss = si * sh;

	mat(1)(1) = (cj * ch);
	mat(2)(1) = (sj * sc - cs);
	mat(3)(1) = (sj * cc + ss);
	mat(1)(2) = (cj * sh);
	mat(2)(2) = (sj * ss + cc);
	mat(3)(2) = (sj * cs - sc);
	mat(1)(3) = -sj;
	mat(2)(3) = (cj * si);
	mat(3)(3) = (cj * ci);
endfunction

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

function res = child_by_att(node, att, val)
  found = %f;
  for i = 1:node.children.size
    if node.children(i).attributes(att) == val
      res = node.children(i);
      found = %t;
      break;
    end
  end
  if ~found
    error('not found by id')
  end
endfunction

d=xmlRead('chair-test2.dae')
v = child_by_name(d.root, 'library_visual_scenes');
v = child_by_att(v, 'id', '_-_time__night');

// v is visual_scene for night scene

total_npts = 0;
allcrvs = list();
// for each node, get the object matrix and the points
for i=1:v.children.size
  node = v.children(i);
  prefix = node.attributes('id')

  // --- Get the object transformations
  transl = [];
  Rz = [];
  Ry = [];
  Rx = [];
  scale = [];
  for ic = 1:node.children.size
    nc = node.children(ic);
    if nc.name == 'translate'
      execstr('transl = [' + nc.content + ']');
    elseif nc.name == 'rotate'
      if nc.attributes('sid') == 'rotationZ'
        execstr('Rz = [' + nc.content + ']');
      elseif nc.attributes('sid') == 'rotationY'
        execstr('Ry = [' + nc.content + ']');
      elseif nc.attributes('sid') == 'rotationX'
        execstr('Rx = [' + nc.content + ']');
      end
    elseif nc.name == 'scale'
      execstr('scale = diag([' + nc.content + '])');
    elseif nc.name == 'instance_geometry'
      mname = nc.attributes('url');
      mname = strsubst(mname,'#','');
      aname = mname + '-positions-array';
      // search element
      disp(mname)
    else
      warning('unprocessed element: ' + nc.name)
    end
  end
  if transl == [] | Rz == [] | Ry == [] | Rz == [] | scale == []
    error('transformations not read properly - null size');
  end
  R = eul_to_mat3([Rx(4), Ry(4), Rz(4)]*%pi/180)';

  // --- Get the points

  l=child_by_name(d.root, 'library_geometries'); 
  g = child_by_att(l, 'id', mname); 
  arr = g.children(1).children(1).children(1);
  if arr.attributes('id') ~= aname
    error('assumed ordering inside geometry tag is broken')
  end
  npts = arr.attributes('count')
  total_npts = total_npts + eval(npts);
  disp('npts: ' + npts);
  // stack overflow: execstr('pts = [' + arr.content + ']')
  po = eval(strsplit(arr.content,' '));
  po=matrix(po,3,-1);
  // object-to-world transform
  // tested formula against corresponding object's matrix_world and it matches
  // perfectly
  transl = transl';
  p = R*scale*po + transl*ones(1,size(po,2)) 
  allcrvs(i) = p';
end

// stack em 
allpts = allcrvs(1)
num_curves = length(allcrvs)
for i=2:num_curves
  allpts = [allpts; allcrvs(i)];
end
