% DIP Assignment 1. 4-(c)

function [bilinear_inter_img] = Bilinear_Interp(orig_img, fx, fy)
[h, w] = size(orig_img);

% set new img size (image size must be integer).
scaled_w = round(w * fx);
scaled_h = round(h * fy);

% init bilinear_inter_img with -1 values.
bilinear_inter_img = ones(scaled_h, scaled_w) .* -1;

% fill scaled img based on nearest neighbor interpolation.
for scaled_i = 1 : scaled_h
    for scaled_j = 1 : scaled_w
        % find corresponding pixel coordinate between orig img pixel coord(orig_j, orig_i) and scaled img pixel coord(scaled_j, scaled_i).
        orig_i = scaled_i / fy;
        orig_j = scaled_j / fx;

        % find the top-left pixel of the corresponding pixel. 
        orig_tl_i = floor(orig_i);
        orig_tl_j = floor(orig_j);
        
        % index starts at 1 and ends at h and w, respectively.
        if orig_tl_i < 1
            orig_tl_i = 1;
        elseif orig_tl_i + 1 >= h
            orig_tl_i = h-1;
        end

        if orig_tl_j < 1
            orig_tl_j = 1;
        elseif orig_tl_j + 1 >= w
            orig_tl_j = w-1;
        end

        % calc distance between corresponding pixel and top-left pixel.
        v = orig_i - orig_tl_i;
        u = orig_j - orig_tl_j;
        
        % apply bilinear interpolation.
        bilinear_inter_img(scaled_i, scaled_j) = (1-v)*(1-u)*orig_img(orig_tl_i, orig_tl_j) + (1-v)*u*orig_img(orig_tl_i, orig_tl_j+1) ...
                                                 + v*(1-u)*orig_img(orig_tl_i+1, orig_tl_j) + v*u*orig_img(orig_tl_i+1, orig_tl_j+1);
    end
end
% casting for image saving.
bilinear_inter_img = uint8(bilinear_inter_img);
end

