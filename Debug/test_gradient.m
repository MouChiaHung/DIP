close all;
clear;
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% GRADIENT LAB
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

file_raw = 'finger';
row=180; col=80;

file_raw = 'edge';
row=256; col=256;

file_raw = 'cell';
row=199; col=200;

file_raw = 'Finger2030A';
row=376; col=240;

file_raw = 'lena';
row=256; col=256;

file_raw = 'virus';
row=212; col=219;

file_raw = '2object';
row=320; col=240;
%{
N = 5;

x = (1:1:N);
y = zeros(1,N);

y = 2./(x); %xy=2
figure('name','gradient test');
subplot(3,2,1);
plot(x,y);
title('xy=2');

x = (0:1:N-1);
y = (0:1:N-1);
z = zeros(N,N);
for j = 1 : N
   for i = 1 : N
       z(j,i) = x(i)*y(j);
   end
end    

subplot(3,2,3);
plot(z);
title('z=xy');


z = (1:2:N*2);
x = (1:1:N);
y = zeros(N,N);
for iz = 1 : N
    for ix = 1 : N
        y(iz,ix) = z(iz)/x(ix);
    end
end
  
subplot(3,2,5);
plot(x,y(1,:),x,y(2,:),x,y(3,:),x,y(4,:),x,y(5,:));
title('y=z/x');

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


%raw data with matlab Gaussian
%
tmp = file_raw;
file_raw = append(tmp,'');
fin=fopen(file_raw,'r');
r=fread(fin,row*col,'uint8=>uint8'); 
r=reshape(r,row,col);
r=r';
r = imgaussfilt(r,2);
fclose(fin);
show(r, file_raw, row, col);
%return;
%



%raw data
tmp = file_raw;
file_raw_gaussian = append(tmp,'Gaussianed');
fin=fopen(file_raw_gaussian,'r');
Raw=fread(fin,row*col,'uint8=>uint8'); 
Raw=reshape(Raw,row,col);
Raw=Raw';
fclose(fin);
show(Raw, file_raw_gaussian, row, col);

%gradient magnitude 
file_raw_gradient_mag = append(tmp,'GradientMAG');
fin=fopen(file_raw_gradient_mag,'r');
GradientMAG=fread(fin,row*col,'uint8=>uint8');
GradientMAG=reshape(GradientMAG,row,col);
GradientMAG=GradientMAG';
fclose(fin);
show(GradientMAG, file_raw_gradient_mag, row, col);

%gradient orientation
file_raw_gradient_ori = append(tmp,'GradientORI');
fin=fopen(file_raw_gradient_ori,'r');
GradientORI=fread(fin,row*col,'uint8=>uint8');
GradientORI=reshape(GradientORI,row,col);
GradientORI=GradientORI';
fclose(fin);
show(GradientORI, file_raw_gradient_ori, row, col);

%gradient x
file_raw_gradient_x = append(tmp,'GX');
fin=fopen(file_raw_gradient_x,'r');
GradientX=fread(fin,row*col,'uint8=>uint8');
GradientX=reshape(GradientX,row,col);
GradientX=GradientX';
fclose(fin);
show(GradientX, file_raw_gradient_x, row, col);

%gradient y
file_raw_gradient_y = append(tmp,'GY');
fin=fopen(file_raw_gradient_y,'r');
GradientY=fread(fin,row*col,'uint8=>uint8');
GradientY=reshape(GradientY,row,col);
GradientY=GradientY';
fclose(fin);
show(GradientY, file_raw_gradient_y, row, col);

figure('Name','Gradient','NumberTitle','off');
subplot(3,2,1);
imshow(Raw);
title('Gaussianed image')
subplot(3,2,3);
imshow(GradientMAG);
title('Gradient Magnitude')
subplot(3,2,4);
imshow(GradientORI);
title('Gradient Orientation')
subplot(3,2,5);
imshow(GradientX);
title('Gradient X')
subplot(3,2,6);
imshow(GradientY);
title('Gradient Y')

figure('Name','Threshold','NumberTitle','off');
subplot(2,2,1);
imshow(GradientMAG);
title('GradientMAG')
subplot(2,2,2);
GradientMAG_s = GradientMAG - 64;
imshow(GradientMAG_s);
title('GradientMAG - 64')
subplot(2,2,3);
GradientMAG_m = GradientMAG - 32;
imshow(GradientMAG_m);
title('GradientMAG - 32')
subplot(2,2,4);
GradientMAG_l = GradientMAG - 16;
imshow(GradientMAG_l);
title('GradientMAG -16')


%laplacian
file_raw_laplacianed = append(tmp,'Laplacianed');
fin=fopen(file_raw_laplacianed,'r');
Laplacianed=fread(fin,row*col,'uint8=>uint8');
Laplacianed=reshape(Laplacianed,row,col);
Laplacianed=Laplacianed';
fclose(fin);

figure('Name','Laplacianed','NumberTitle','off');
subplot(2,2,1);
imshow(Laplacianed);
title('Laplacianed')
subplot(2,2,2);
Laplacianed_s = Laplacianed - 64;
imshow(Laplacianed_s);
title('Laplacianed - 64')
subplot(2,2,3);
Laplacianed_m = Laplacianed - 32;
imshow(Laplacianed_m);
title('Laplacianed - 32')
subplot(2,2,4);
Laplacianed_l = Laplacianed - 16;
imshow(Laplacianed_l);
title('Laplacianed -16')


%figure('Name','quiver','NumberTitle','off');
%quiver(GradientX,GradientY);

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
    title('DFT')
    q = mod(row,2);
    if (mod(col,2)==0)
        subplot(2,2,4);
        verticalCL = FSpec(:,col/2);
        plot(verticalCL);
        title('Central vertical line of DFT')
    elseif (mod(row,2)==0)
        subplot(2,2,4);
        horizontal = FSpec(row/2,:);
        plot(horizontal);
        title('Central horizontal line of DFT') 
    end
    ret = 0;
end


