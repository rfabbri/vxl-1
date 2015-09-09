clf
t=[0:0.1:5*%pi]';
v = [cos(t) sin(t) sin(0.1*t).*t]
cplot(v,-2)

// To set style:
//h=gca()
//h=a.children; //get the handle of the param3d entity: an Compound composed of 2 curves
//h.children(1).foreground = 3; // first curve
//curve2 = h.children(2);
//curve2.foreground = 6;
//curve2.mark_style = 2;
