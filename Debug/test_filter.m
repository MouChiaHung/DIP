clear;
close all;
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% FILTER LAB
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

file_raw = 'cell';
row=199; col=200;

file_raw = 'finger';
row=180; col=80;

file_raw = 'finger2030A';
row=376; col=240;

file_raw = 'edge';
row=256; col=256;

file_raw = '2object';
row=320; col=240;

file_raw = 'lena';
row=256; col=256;

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% GAUSSIAN LAB
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%{
du = 1;
N = 16;
u = (0:du:N-1);
dv = 1;
M = 16;
v = (0:dv:M-1);

sigma = 2.0;
y = zeros(M*2,N*2);

%Quadrant 4
for j = (M+1) : (M+M)
    for i = (N+1) : (N+N)
        ii = mod(i,N);
        jj = mod(j, M);
        if (ii == 0) 
            ii = N;
        end
        if (jj == 0) 
            jj = M;
        end
        uu = ii;
        vv = jj;
        y(j,i) = exp(-1*((u(uu)^2+v(vv)^2)/sigma));
        %y(i,j) = exp(-1*((u(uu)^2+v(vv)^2)/sigma));
    end    
end
%

%Quadrant 1
for j = (1) : (M)
    for i = (N+1) : (N+N)
        ii = mod(i,N);
        jj = mod(j,M);
        if (ii == 0)
            ii = N;
        end
        if (jj == 0)
           jj = M; 
        end
        uu = ii;
        vv = (M+1)-jj;
        y(j, i) = exp(-1*(u(uu)^2+v(vv)^2)/sigma); 
    end
end
%


%Quadrant 2
for j = 1 : (M)
    for i = 1 : (N)
        ii = (N+1)-i;
        jj = (M+1)-j;
        uu = ii;
        vv = jj;
        y(j,i) = exp(-1*((u(uu)^2+v(vv)^2)/sigma));
    end    
end
%

%Quadrant 3
for j = (M+1) : (M+M)
    for i = 1 : N
        ii = mod(i,N);
        jj = mod(j,M);
        if (ii == 0)
            ii = N;
        end
        if (jj == 0) 
            jj = M;
        end
        uu = (N+1)-ii;
        vv = jj;
        y(j,i) = exp(-1*(u(uu)^2+v(vv)^2)/sigma); 
    end
end

%axis equal
%plot3(u,v,y);
figure('Name','Gaussian','NumberTitle','off');
subplot(1,2,1);
surf(y);
title('Gaussian on spatial domain');
F = fft2(y);
F = fftshift(F);

%figure('Name','DFT of Gaussian','NumberTitle','off');
subplot(1,2,2);
%imshow((abs(F)), []);
surf((abs(F))/(1));
title('Gaussian on frequency domain');
return;
%}

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% DEMO
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%{
FILTER = fft2(f, 512, 512);
figure(1);
imshow(fftshift(log(abs(FILTER))), []);
figure(2);
surf(fftshift((abs(FILTER))), 'EdgeColor', 'none');
return;
%}

%raw data
tmp = file_raw;
file_raw_gaussian = append(tmp,'Gaussianed');
fin=fopen(file_raw_gaussian,'r');
Raw=fread(fin,row*col,'uint8=>uint8'); 
Raw=reshape(Raw,row,col);
Raw=Raw';
fclose(fin);
show(Raw, file_raw_gaussian, row, col);

%filtered data
file_raw_filtered = append(tmp,'Filtered');
fin=fopen(file_raw_filtered,'r');
Filtered=fread(fin,row*col,'uint8=>uint8');
Filtered=reshape(Filtered,row,col);
Filtered=Filtered';
fclose(fin);
show(Filtered, file_raw_filtered, row, col);

%gradient2 data
file_raw_gradient2 = append(tmp,'Gradient2');
fin=fopen(file_raw_gradient2,'r');
Gradient2=fread(fin,row*col,'uint8=>uint8');
Gradient2=reshape(Gradient2,row,col);
Gradient2=Gradient2';
fclose(fin);
show(Gradient2, file_raw_gradient2, row, col);

figure('Name','EdgeDet','NumberTitle','off');
subplot(2,2,1);
imshow(Raw);
title('Gaussianed image')
subplot(2,2,3);
imshow(Filtered);
title('Edge-detected by Sobel filter')
subplot(2,2,4);
imshow(Gradient2);
title('Edge-detected by Gradient2 filter')

%Filtering by matlab
%{
filter = ones(8)/16;
im1 = ones(64);
im2 = imfilter(im1,filter);

ZLP=imfilter(ZRaw,filter);
figure('Name','LP filtering by matlab','NumberTitle','off');
imshow(ZLP);
title('Filtering by matlab')
%}

function ret=show(im, string, row, col)
    figure('Name',string,'NumberTitle','off');
    subplot(2,1,1);
    imshow(im);
    title(string)

    %fft2
    F = fft2(im);
    FAbs = abs(F);
    %remove DC
    %
    for j = 1 : 1
        for i = 1 : 1
            FAbs(i,j) = 0;
            FAbs((col+1)-i,j) = 0;
            FAbs(i,(row+1)-j) = 0;
            FAbs((col+1)-i,(row+1)-j) = 0;    
        end
    end
    FSpec = (fftshift(FAbs));
    %FSpec = log(fftshift(FAbs));
    %
    subplot(2,2,3);
    imshow(FSpec, []);
    title('DFT of filtered data')
    q = mod(row,2);
    if (mod(col,2)==0)
        subplot(2,2,4);
        verticalCL = FSpec(:,col/2);
        plot(verticalCL);
        title('Central vertical line of DFT of filtered data')
    elseif (mod(row,2)==0)
        subplot(2,2,4);
        horizontal = FSpec(row/2,:);
        plot(horizontal);
        title('Central horizontal line of DFT of filtered data') 
    end
    ret = 0;
end


