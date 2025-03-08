% DIP Assignment 1. 4-(a)

function [scaled_img] = Scaling(orig_img, fx, fy)
[h, w] = size(orig_img);

% set new img size (image size must be integer).
scaled_w = round(w * fx);
scaled_h = round(h * fy);

% init scaled_img with -1 values.
scaled_img = ones(scaled_h, scaled_w) .* -1;

% fill scaled img based on nearest neighbor interpolation
for scaled_i = 1 : scaled_h
    for scaled_j = 1 : scaled_w
        % find corresponding pixel coordinate between orig img pixel coord(orig_j, orig_i) and scaled img pixel coord(scaled_j, scaled_i).
        orig_i = round(scaled_i / fy);
        orig_j = round(scaled_j / fx);

        % index starts at 1 and ends at h and w, respectively.
        if orig_j < 1
            orig_j = 1;
        end

        if orig_i < 1
            orig_i = 1;
        end
           
        % apply nearest neighbor interpolation.
        scaled_img(scaled_i, scaled_j) = orig_img(orig_i, orig_j);
    end
end
% casting for image saving.
scaled_img = uint8(scaled_img);
end

