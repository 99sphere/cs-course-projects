% DIP Assignment 3. 3-b
% Percentage of Removed Power

img = imread('checkerboard1024-shaded.tif');
img = im2double(img);


% Applying Gaussian Lowpass Filter in Frequency Domain, D_0=1
D_0 = 1;
[shading_pattern, padded_img, F, G] = ShadingCorrection(img, D_0);
corrected_img = img ./ shading_pattern;

P_padded_img = sum(abs(F).^2,'all');
P_result_img = sum(abs(G).^2,'all');
percentage_D0_1 = 100 * P_result_img / P_padded_img

% Applying Gaussian Lowpass Filter in Frequency Domain, D_0=5
D_0 = 5;
[shading_pattern, padded_img, F, G] = ShadingCorrection(img, D_0);
corrected_img = img ./ shading_pattern;

P_padded_img = sum(abs(F).^2,'all');
P_result_img = sum(abs(G).^2,'all');
percentage_D0_5 = 100 * P_result_img / P_padded_img

% Applying Gaussian Lowpass Filter in Frequency Domain, D_0=10
D_0 = 10;
[shading_pattern, padded_img, F, G] = ShadingCorrection(img, D_0);
corrected_img = img ./ shading_pattern;

P_padded_img = sum(abs(F).^2,'all');
P_result_img = sum(abs(G).^2,'all');
percentage_D0_10 = 100 * P_result_img / P_padded_img