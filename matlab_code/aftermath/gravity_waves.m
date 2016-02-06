%--------------------------------------------------------------------------
% This script creates an endless animation of water gravity waves
%
% Copyright Kristofer Krus, 2014
%--------------------------------------------------------------------------
 
% CodeStart ---------------------------------------------------------------
% Resetting MATLAB environment
    close all;
    clear all;
    clc;
% Simulation settings
    simulate     = 1;           %[flag]
    horizontalDisplacement = 1; %[flag]
    gridRes      = .1;          %[m]
    gridPoints   = 256;         %[1] (# points in each direction)
    nx = 1;  % Number of times to tile in x-direction
    ny = nx; % Number of times to tile in y-direction
gridLength = gridRes*gridPoints;
    windVelocity = [1,0]';  %[m/s]
    waterDepth   = gridRes*gridPoints*10;      %[m]
    L = 40*gridRes; % V^2/g; % Approximate average wave length divided by 2 pi
    %L=gridRes*gridPoints/25;
    timeStep     = 1/20;      %[s]
% Appearance settings
    % A [1] Approx. amplitude to length ratio for long waves
    %A            = .00001; %Very calm
    %A            = .0001; %Calm
    %A            = .001; %Fairly calm
    %A            = .003; %Still fairly calm
    A            = .01; %Whiped up
    %A            = .02; %Uncomfortable
    %A            = .03; %Rough!
    %A            = .10; %Chaotic!
    hOffset      = 1; %[1] Horizontal offset factor
    lightPos     = .5*gridLength*[1 .2 .3]; %[1] Light position
    lightStyle   = 'infinite';
    %lightPos     = .5*gridLength*[1 1 .3]; %[1] Light position
    %lightStyle   = 'local ';
    %color        = .7*[0   191 255]/255; %Deep sky blue
    color        = .7*[135 206 235]/255; %Sky blue
    %color        = .7*[135 206 250]/255; %Light sky blue
% Set up fields and variables
    allOnes = ones(gridPoints,gridPoints);
    allZeros = zeros(gridPoints,gridPoints);
    g = 9.82; % [m/s^2]
    rho = 1000; % [kg/m^3] Density of the water
    V = (windVelocity'*windVelocity)^.5; %[m/s]
    freqRes = 2*pi/gridLength; %[Rad/m]
    rComp = ([0:gridPoints-1] - gridPoints/2)*gridRes; %[m]
    kComp = ([[0:gridPoints/2-1],[-gridPoints/2:-1]])*freqRes; %[Rad/m]
    % Generate meshgrids
    [x , y ] = meshgrid(rComp, rComp);
    [kx, ky] = meshgrid(kComp, kComp);
    k = (kx.^2 + ky.^2).^.5;
    colors        = color(1)*ones(gridPoints,gridPoints);
    colors(:,:,2) = color(2)*ones(gridPoints,gridPoints);
    colors(:,:,3) = color(3)*ones(gridPoints,gridPoints);
    
    %z = gridLength/20*(x > -gridLength/4).*(x<gridLength/4).*(y > -gridLength/4).*(y<gridLength/5);
    %z = gridLength/50*exp(i*x/(2*gridRes)).*exp(-(x/(gridLength/4)).^2-(y/(gridLength/4)).^2);
    %z = gridLength/20*((x/(gridLength/4)).^2+(y/(gridLength/4)).^2 < 1);
    %z = gridLength/20*(exp(-(x/(gridLength/5)-allOnes).^2-(y/(gridLength/5)).^2) + ...
                     %exp(-(x/(gridLength/5)+allOnes).^2-(y/(gridLength/5)).^2) > .55);

% Generate waves in frequency domain
% Use Phillips spectrum (http://graphics.ucsd.edu/courses/rendering/2005/jdewall/tessendorf.pdf)
% In the future, maybe the improved version of the JONSWAP spectrum
% developed in "A unified directional spectrum for long and short wind-
% driven waves" should be used (http://archimer.ifremer.fr/doc/00091/20226/17877.pdf should be used
    if exist('z')
        h = fft2(z);
        %w = angle(h);
        %h = abs(h).*(allOnes+.3*(-.5*allOnes+rand(gridPoints,gridPoints)));
    else
        % Wave height spectrum
        P = A*exp(-1./(k*L).^2)./k.^4.*abs((kx.*windVelocity(1) + ky.*windVelocity(2))./(V*k)).^2;
        % Fix division by zero problem in P by finding NaN elements and setting them to 0:
        %P(find(isnan(P))) = 0;
        P(1) = 0;
        %ones(1,gridPoints)*P*ones(gridPoints,1)*freqRes^2
        % Rayleigh distributed numbers with square mean 2
        R = (normrnd(0,1,gridPoints,gridPoints).^2+normrnd(0,1,gridPoints,gridPoints).^2).^.5;
        %R = 2^.5*ones(gridPoints,gridPoints);
        % Height for the different Fourier components (ifft2 will compensate for the gridPoints^2 factor)
        h = R.*(P./2).^.5*freqRes*gridPoints^2;
        % Generate phase angle for the components
        w = rand(gridPoints, gridPoints)*2*pi;
        %w = zeros(gridPoints, gridPoints);
        %w = eye(gridPoints)*pi;
        %w = (-1).^[1:gridPoints]'*(-1).^[1:gridPoints];
        %w = i.^[1:gridPoints]'*i.^[1:gridPoints];
        h = h.*exp(1i*w);
    end
    hx = -hOffset*h.*kx./k;
    hy = -hOffset*h.*ky./k;
    % Fix division by zero problem in hx and hy
    hx(1) = 0;
    hy(1) = 0;
    wdt = timeStep*(g*k.*tanh(k*waterDepth)).^.5;
    eniwdt = exp(-1i*wdt);
    z = real(ifft2(h));
    
    %surf(kx, ky, P, 'EdgeColor','none');
    %surf(kx, ky, h, 'EdgeColor','none')
    surf(x, y, z, 'EdgeColor','none');
    
    % Prepare tiled matrices
    txComp = [0:nx*gridPoints-1]*gridRes;
    tyComp = [0:ny*gridPoints-1]*gridRes;
    [tx, ty] = meshgrid(txComp, tyComp);
    tcolors = repmat(colors,nx,ny,1);
%Executing infinite loop to animate base plot
    z = allZeros;
    idealz = -1*allOnes;
    idealZ = fft2(idealz);
    n = 0;
    mask = ((x.^2 + y.^2) < (gridLength/6)^2);
    ztemp = ifft2(h);
    %ztemp = allZeros;
    ztemp(find(mask)) = 1;
    h = fft2(ztemp);
    %h(1,15)=gridPoints^2/8;
    while simulate
        ztemp = ifft2(h);
        %ztemp = allZeros;
        %ztemp(find(mask)) = 1;
        h = fft2(ztemp);
        F = allZeros;
        %j = 0;
        j = 5;
        for a=1:j
            %Calculate an intermediate surface update without external forces
            deltah = F./(g*rho);
            htemp = (h+deltah).*eniwdt - deltah;
            ztemp = real(ifft2(htemp));

            % Define a surface to interact with the water surface
            otherSurface = (abs(x+1i*y))+gridLength/20 - min(.005*gridLength*n*timeStep, gridLength/4);
            idealz = min(otherSurface, ztemp)*0+1+.1*sin(n*timeStep*3);
            %idealz = mask*1 + (1-mask)*ztemp;
            %idealZ = fft2(idealz);
            zdiff = mask.*(ztemp-idealz);
            % Calculate external pressure on surface and its Fourier transform
            Fcomplex=F+(.5+.5*cos(pi*(a-.5)*j))*g*rho./(1-cos(wdt)).*fft2(zdiff);
            %Fcomplex=F+g*rho./(1-cos(wdt)).*fft2(zdiff);
            % Fix divizion by zero errors
            Fcomplex(1) = 0;
            mask = ((x.^2 + y.^2) < (gridLength/6)^2);
            f = mask.*real(ifft2(Fcomplex));
            %f = (-1*g*rho)* mask;
            %f = allZeros;
            F = fft2(f);
            F(1) = 0; % Assume an average pressure of 0
        end
        
        % Update surface
        deltah = F./(g*rho);
        h = (h+deltah).*eniwdt - deltah;
        z = real(ifft2(h));
        hx = h.*(-kx./k);
        hy = h.*(-ky./k);
        % Fix divizion by zero errors
        hx(1) = 0;
        hy(1) = 0;
        if horizontalDisplacement
            xoffset = imag(ifft2(hx));
            yoffset = imag(ifft2(hy));
        else
            xoffset = allZeros;
            yoffset = allZeros;
        end
        
        % Tile surface
        txoffset=repmat(xoffset,nx,ny);
        tyoffset=repmat(yoffset,nx,ny);
        tz=repmat(z,nx,ny);
        
        % Plot the surface
        %ph = surf(x+xoffset, y+yoffset, z,colors, 'EdgeColor','none');
        %xlim([-gridLength/2 gridLength/2-gridRes]);
        %ylim([-gridLength/2 gridLength/2-gridRes]);
        %zlim([-gridLength/4 gridLength/4]);
        ph = surf(tx+txoffset,ty+tyoffset,tz,tcolors, 'EdgeColor','none');
        xlim([0 nx*gridLength-gridRes]);
        ylim([0 ny*gridLength-gridRes]);
        zlim([-gridLength/4 gridLength/4]);
        xlabel('x');
        ylabel('y');
        zlabel('z(x,y)');
        shading interp;
        set(ph,'FaceLighting','gouraud','FaceColor','interp',...
        'AmbientStrength',0.5)
        light('Position',lightPos,'Style',lightStyle);
        
        %Delaying animation
        pause(timeStep);
        n = n + 1;
    end
%CodeEnd-------------------------------------------------------------------