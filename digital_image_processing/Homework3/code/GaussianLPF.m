function [H]=GaussianLPF(P, Q, D_0)
H = ones(P, Q) .* -1;

for u = 1:P
    for v = 1:Q
        H(u,v)=GaussianLPF_val(u, v, P, Q, D_0);
    end
end
end

function H_val=GaussianLPF_val(u, v, P, Q, D_0)
center_u = P/2;
center_v = Q/2;
D = Distance(u, v, center_u, center_v);
H_val = exp(-1/2 * (D^2)/(D_0^2));
end

function val=Distance(u, v, center_u, center_v)
val = sqrt((u-center_u)^2+(v-center_v)^2);
end