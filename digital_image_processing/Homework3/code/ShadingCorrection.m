function [shading, padded_img, F, G]=ShadingCorrection(img, D_0)
 
[M, N] = size(img);
P = 2*M;
Q = 2*N;

% size of img : M by N
% size of padded_img : 2M by 2N
padded_img = padarray(img,[M N],'symmetric','post');
F = fftshift(fft2(padded_img));
H = GaussianLPF(P, Q, D_0);
G = H.*F;
shading = real(ifft2(ifftshift(G)));
shading = shading(1:M, 1:N);
end