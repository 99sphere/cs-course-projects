function [result]=LaplacianKernel(P,Q)
result = ones(P, Q) .* -1;
M = P/2;
N = Q/2;
for u = 1:P
    for v = 1:Q
        result(u,v)=laplacian_kernel_val(u,v,P,Q);
    end
end
end

function H=laplacian_kernel_val(u,v,P,Q)
M = P/2;
N = Q/2;
H = 2*cos(2*pi*u/M) + 2*cos(2*pi*v/N) + 4*cos(2*pi*v/N)*cos(2*pi*u/M) - 8;
end