%Digital image processing
close all;
%{
row=400;  col=480;
file_raw = 'tif.raw';
fin=fopen(file_raw,'r');
I=fread(fin,row*col,'uint8=>uint8'); 
Z=reshape(I,row,col);
%}

%
row=376;  col=240;
file_raw = 'pgm.raw';
fin=fopen(file_raw,'r');
I=fread(fin,row*col,'uint8=>uint8');
Z=uint8(I);
Z=reshape(I,row,col);
%

Z=Z';
fclose(fin);
figure('Name',file_raw,'NumberTitle','off');
subplot(3,3,1);
imshow(Z);
title(file_raw)  

B=zeros(size(I));
B1=bitset(B,1,bitget(I,1));
B2=bitset(B,2,bitget(I,2));
B3=bitset(B,3,bitget(I,3));
B4=bitset(B,4,bitget(I,4));
B5=bitset(B,5,bitget(I,5));
B6=bitset(B,6,bitget(I,6));
B7=bitset(B,7,bitget(I,7));
B8=bitset(B,8,bitget(I,8));
B=B1+B2+B3+B4+B5+B6+B7+B8;
B=uint8(B);
Br=reshape(B,row,col);
Br=Br';
subplot(3,3,2);
imshow(Br);
title('reconstruct');

C=255*bitget(I,5);
C=uint8(C);
Cr=reshape(C,row,col);
Cr=Cr';
subplot(3,3,3);
imshow(Cr);
title('bitget 5');

D=255*bitget(I, 8);
D=uint8(D);
Dr=reshape(D,row,col);
Dr=Dr';
subplot(3,3,4);
imshow(Dr);
title('bitget 8');

file_bprint = 'bprint7.raw';
fin=fopen(file_bprint,'r');
I=fread(fin,row*col,'uint8=>uint8'); 
E=reshape(I,row,col);
E=E';
fclose(fin);
subplot(3,3,5);
imshow(E);
title(file_bprint);

file_bprint = 'bright.raw';
fin=fopen(file_bprint,'r');
I=fread(fin,row*col,'uint8=>uint8'); 
F=reshape(I,row,col);
F=F';
fclose(fin);
subplot(3,3,6);
imshow(F);
title(file_bprint);

file_inverse = 'inverse.raw';
fin=fopen(file_inverse,'r');
I=fread(fin,row*col,'uint8=>uint8'); 
G=reshape(I,row,col);
G=G';
fclose(fin);
subplot(3,3,7);
imshow(G);
title(file_inverse);

file_halftone_dots = 'halftone_dots.raw';
fin=fopen(file_halftone_dots,'r');
I=fread(fin,row*col,'uint8=>uint8'); 
H=reshape(I,row,col);
H=H';
fclose(fin);

subplot(3,3,8);
imshow(H);
title('halftone dots');

figure('Name',file_halftone_dots,'NumberTitle','off');
imshow(H);
title(file_halftone_dots);
















