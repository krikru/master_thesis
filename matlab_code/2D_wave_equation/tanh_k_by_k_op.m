tol = 1.0e-6; % Default (1.0e-6)
fun = @(r) 1./(2*pi*r.*(1+r)) .* e.^(-r.^2/4);
cut_off = 20;
r_max = 25;
res = 0.2;
%r_vec = [0 : res : r_max];
r_vec = [0 : res : r_max].^2/r_max;
ref_fun = @(xi) tanh(xi)./xi;
F = (2*pi) * hankel_transform(0, fun, cut_off, r_vec, tol);
F_ref = ref_fun(r_vec);
F_ref(1) = 1;

% Clear these structures from any earlier use
clear c p P;

% Compare functions
%c.title = 'Comparison';
c.filename = 'comparison.tex';
c.standalone = 0;
c.runtex = c.standalone;
c.clearauxfiles = 1;
c.axistype = 'axis';
c.xlabel = '$\xi$';
%c.ylabel = 'Y Axis';
c.scale = scale;
c.ymin = '0';
%% Plot 1
p.x = r_vec(1:elements);
p.y = F(1:elements);
p.color = 'blue';
p.legend = '$\fdfunc{K}^*(\xi)$';
P{1} = p;
clear p
%% Plot 2
p.x = r_vec;
p.y = F_ref;
p.color = 'black';
p.style = 'dotted';
p.legend = '$\fdfunc{K}(\xi)$';
P{2} = p;
clear p
%%% Plot 3
%p.x = r_vec;
%p.y = 0*r_vec;
%p.color = 'gray';
%p.linewidth = '0.125pt';
%p.legend = '$\fdfunc{K}(\xi)$';
%P{3} = p;
%clear p
%% Generate plots
printpgf(c,P);
clear c p P

tol = 1.0e-6; % Default (1.0e-6)
fun = @(r) 1./(2*pi*r.*(1+r)) .* e.^(-r.^2/4);
cut_off = 20;
r_max = 50;
res = 0.05;
%r_vec = [0 : res : r_max];
r_vec = [0 : res : r_max].^2/r_max;
ref_fun = @(xi) tanh(xi)./xi;
F = (2*pi) * hankel_transform(0, fun, cut_off, r_vec, tol);
F_ref = ref_fun(r_vec);
F_ref(1) = 1;

% Ratio
%c.title = 'Ratio';
c.filename = 'ratio.tex';
c.standalone = 0;
c.runtex = c.standalone;
c.clearauxfiles = 1;
c.axistype = 'axis';
c.xlabel = '$\xi$';
c.ylabel = '$\frac{\fdfunc{K}^*(\xi)}{\fdfunc{K}(\xi)}$';
c.scale = scale;
%% Plot 1
p.x = r_vec;
p.y = F ./ F_ref;
p.color = 'blue';
P{1} = p;
clear p
%%% Plot 2
%p.x = r_vec;
%p.y = 0*r_vec + 1;
%p.color = 'gray';
%p.linewidth = '0.125pt';
%P{2} = p;
%clear p
%% Generate plots
printpgf(c,P);
clear c p P;
