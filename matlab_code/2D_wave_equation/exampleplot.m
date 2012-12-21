x = [1 2 3 4 5 6 7 8 9 10];
y1 = [.16 .08 .04 .02 .013 .007 .004 .002 .001 .0008 ];
y2 = [.16 .07 .03 .01 .008 .003 .0008 .0003 .00007 .00002 ];

% Clear these structures from any earlier use
clear c p P;

c.title = 'Plot title';
c.filename = 'tikz.tex';
c.runtex = 1;
c.standalone = '1';
c.axistype = 'semilogyaxis';
c.xlabel = 'X Axis';
c.ylabel = 'Y Axis';
p.x = x;
p.y = y1;
p.color = 'blue';
p.mark = 'o';
p.legend = 'y1';
P{1} = p;
p.y = y2;
p.color = 'black';
p.mark = 'x';
p.legend = 'y2';
P{2} = p;
printpgf(c,P);
clear c p P;
