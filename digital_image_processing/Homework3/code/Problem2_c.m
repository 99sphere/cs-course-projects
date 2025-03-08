% DIP Assignment 3. 2-c
% Difference of Spatial/Frequency Domain Filtering

img = imread('moon-blurry.tif');
img = im2double(img);

% Applying Filtering in Spatial Domain (Laplacian Filter)
h = [1 1 1; 1 -8 1; 1 1 1];
spatial_laplacian = imfilter(img, h);

% Applying Filtering in Frequency Domain (Laplacian Filter)
[M, N] = size(img);
P = 2*M;
Q = 2*N;
F = fft2(img, P, Q);
H = LaplacianKernel(P,Q);
G = H.*F;
result = real(ifft2(G));
frequency_laplacian = result(1:M, 1:N);

diff = spatial_laplacian - frequency_laplacian;
diff_scaled = rescale(diff, 0, 255);

figure(1), imshow(diff);
title("Difference of Laplacian(Spatial vs Frequency Domain Filtering)")
axis on

figure(2), imshow(diff_scaled, [0, 255]);
title("Difference of Laplacian(Spatial vs Frequency Domain Filtering), Scaled")
axis on