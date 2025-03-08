% DIP Assignment 3. 2-a
% Filtering in the spatial domain

img = imread('moon-blurry.tif');
img = im2double(img);

% Construct Laplacian Filter including diagonal term
h = [1 1 1; 1 -8 1; 1 1 1];

% Applying Filtering in the Spatial Domain (Laplacian Filter)
spatial_laplacian = imfilter(img, h);
result_img = img - spatial_laplacian;

% Result Scaling (for visualize)
spatial_laplacian_scaled = rescale(spatial_laplacian, 0, 255);

figure(1), imshow(spatial_laplacian);
title("Result of Laplacian in Spatial Domain")
axis on

figure(2), imshow(spatial_laplacian_scaled, [0, 255]);
title("Result of Laplacian in Spatial Domain (Scaled)")
axis on

figure(3), imshow(img);
title("Original Image")
axis on

figure(4), imshow(result_img);
title("Result of Edge Enhancing")
axis on


