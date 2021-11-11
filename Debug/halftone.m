function out = halftone(im, blocktype);

if (nargin<2 | strcmp(blocktype, 'dots'))
    hblock = [40,60,150,90,10,90,60,40
              80,170,240,200,110,200,170,180
              140,210,250,220,130,220,210,140
              120,190,230,180,70,180,230,120
              20,100,160,50,30,160,100,20
              120,190,230,180,70,230,190,120
              140,210,250,220,130,220,210,140
              40,60,150,90,10,150,60,40
              ];
elseif (strcmp(blocktype, 'bayer'))
    hblock =  0.6 * [40 ,160,50 ,190,10 ,190,60 ,140
                   180,17 ,240,20 ,110,20 ,170,18 
                   14 ,210,25 ,220,13 ,220,21 ,140
                   120,19 ,230,18 ,170,18 ,230,12 
                   20 ,100,16 ,150,30 ,160,10 ,120
                   120,19 ,230,18 ,170,23 ,190,20 
                   14 ,210,25 ,220,30 ,220,21 ,140
                   140,60 ,150,90 ,100,15 ,160,40
                   ];
elseif (strcmp(blocktype, 'bayer_4x4'))
    hblock =  1 * [40 ,160,50 ,190
                   180,17 ,240,20 
                   14 ,210,25 ,220
                   120,19 ,230,18 
                  ];
elseif (strcmp(blocktype, 'bayer_2x2'))
    hblock =  1 * [40 ,160
                   180,17];                  
end     

[mh,nh] = size(hblock);
[m, n] = size(im);
lip_row = floor(m/mh);
lip_col = floor(n/nh);

hmask = repmat(hblock, [lip_row,lip_col]);
im = im(1:(mh*lip_row), 1:(nh*lip_col));

imn = double(im) + hmask
out = imn > 256;
