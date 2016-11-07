% ellipse - sampling on analytic
% felipse - grid-quantized version
% theta - vector of thetas
function [fellipse,ellipse,theta]=ellipse(step,ra,rb, x0,y0)

theta=0:step:2*pi;

xx=ra*cos(theta)+x0;
yy=rb*sin(theta)+y0;

ellipse = [xx' yy'];

fellipse = myquantize(ellipse);
