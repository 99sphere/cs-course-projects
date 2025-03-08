% DIP Assignment 3. 3-a
% Shading Correction via Gaussian Lowpass Filter in Frequency Domain

img = imread('checkerboard1024-shaded.tif');
img = im2double(img);


% Applying Gaussian Lowpass Filter in Frequency Domain
D_0 = 5;
[shading_pattern, padded_img, F, G] = ShadingCorrection(img, D_0);
corrected_img = img ./ shading_pattern;

figure(1), imshow(img, []);
title("Original Image")
axis on

figure(2), imshow(padded_img, []);
title("Padded Image (Symmetric Padding)")
axis on

figure(3), imshow(shading_pattern, []);
title("Computed Shading Pattern")
axis on

figure(4), imshow(corrected_img, []);
title("Result of Shading Correction")
axis on

