% DIP Assignment 1. 4

img = imread('HW1_Q4.tif');

% Set the value of x, y scaling factor
fx = 3;
fy = 3;

% 4-(a). 
scaled_img = Scaling(img, fx, fy);
imwrite(scaled_img, 'result/HW1_Q4-a.tif');

% 4-(b).
A = [fx 0 0; 0 fy 0; 0 0 1]; % Set Affine Matrix
tform = affinetform2d(A);
affine_transformed_img = imwarp(img, tform);
imwrite(affine_transformed_img, 'result/HW1_Q4-b.tif');

% 4-(c).
bilinear_interp_img = Bilinear_Interp(img, fx, fy);
imwrite(bilinear_interp_img, 'result/HW1_Q4-c.tif');

% For visualize
figure(1), imshow(img, [0 255]);
title("original")
axis on

figure(2), imshow(scaled_img, [0 255]);
title("4-(a)")
axis on

figure(3), imshow(affine_transformed_img, [0 255]);
title("4-(b)")
axis on

figure(4), imshow(bilinear_interp_img, [0 255]);
title("4-(c)")
axis on
