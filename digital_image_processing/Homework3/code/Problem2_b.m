% DIP Assignment 3. 2-b
% Filtering in the frequency domain

img = imread('moon-blurry.tif');
img = im2double(img);

[M, N] = size(img);
P = 2*M;
Q = 2*N;

% Applying Filtering in the Frequency Domain (Laplacian Filter)
F = fftshift(fft2(img, P, Q));
% Construct Laplacian Filter including diagonal term
H = LaplacianKernel(P,Q);
G = H.*F;
result = real(ifft2(ifftshift(G)));
frequency_laplacian = result(1:M, 1:N);
result_img = img - frequency_laplacian;

% Result Scaling (for visualization)
frequency_laplacian_scaled = rescale(frequency_laplacian, 0, 255);

figure(1), imshow(frequency_laplacian);
title("Result of Laplacian in Frequency Domain")
axis on

figure(2), imshow(frequency_laplacian_scaled, [0, 255]);
title("Result of Laplacian in Frequency Domain (Scaled)")
axis on

figure(3), imshow(img);
title("Original Image")
axis on

figure(4), imshow(result_img);
title("Result of Edge Enhancing")
axis on