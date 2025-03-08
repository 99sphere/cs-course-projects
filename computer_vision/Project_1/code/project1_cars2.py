"""
2024 Spring, Introduction to Computer Vision.
Project 1 - cars2_noise.png
"""

import numpy as np
import os
from PIL import Image as pilimg
from matplotlib import pyplot as plt


import numpy as np
import os
from PIL import Image as pilimg
from matplotlib import pyplot as plt


def readimage(img_name):
    global row, col
    im = pilimg.open(img_name)
    cimg = np.array(im)
    col, row = im.size
    return cimg, row, col


def saveimage(cimg, name):
    cimg=np.uint8(cimg)
    im=pilimg.fromarray(cimg)
    im.save(name)
    return im

def medianfiltering(img2d, ms):
    row, col = img2d.shape
    buff2d = np.full((row, col), 0)    
    hs = ms//2
    for i in range(hs, row-hs):
        for j in range(hs, col-hs):
            temp=img2d[i-hs:i+hs+1, j-hs:j+hs+1].flatten()
            temp=np.sort(temp)
            buff2d[i, j] = temp[len(temp)//2]
    return buff2d[hs:-hs, hs:-hs]

def minmax_normalization(img, min, max):
    return 255.0 * (img-min) / (max-min)

def z_standard_normalization(img):
    # z_std = 1 keeping about 70%
    # z_std = 2 keeping about 95%
    z_std = 1
    mean = np.mean(img)
    std = np.std(img)
    z_score = np.clip((img-mean) / std, -z_std, z_std)
    return minmax_normalization(z_score, -z_std, z_std) 

def histogram_equalization(img):
    histogram = [0 for _ in range(256)]

    for i in range(row):
        for j in range(col):
            histogram[img[i,j]] += 1

    pdf = np.array(histogram) / (row*col)
    cdf = np.cumsum(pdf)
    result = np.zeros_like(img)
    
    for i in range(row):
        for j in range(col):
            result[i,j] = 255 * cdf[img[i, j]]
        
    return np.uint8(result)


def imagefft(img, center):
    img_fft = np.fft.fft2(img)
    if center:
        img_fft = np.fft.fftshift(img_fft)
    return img_fft


def imageifft(img_fft, center=True):
    if center:
        img_fft = np.fft.ifftshift(img_fft)
    img = np.fft.ifft2(img_fft)
    return img


def make_ideal_LPF_mask_f(img_fft, d0):
    row, col = np.shape(img_fft)
    mask = np.full((row,col), 0.0).astype(np.float32)
    center_row = row // 2
    center_col = col // 2 
    
    for i in range (row):
        for j in range(col):
            dist = np.sqrt((center_row-i)**2+(center_col-j)**2)
            if dist < d0:
                mask[i, j] = 1
            else:
                mask[i, j]= 0
    return mask


def make_gaussian_LPF_mask_f(img_fft, std):
    row, col = np.shape(img_fft)
    mask = np.full((row,col), 0.0).astype(np.float32)
    center_row = row // 2
    center_col = col // 2 
    
    for i in range (row):
        for j in range(col):
            dist = np.sqrt((center_row-i)**2+(center_col-j)**2)
            mask[i, j] = np.exp(- (dist ** 2) / (2 * (std**2)))
    return mask


def make_butterworth_LPF_mask_f(img_fft, cutoff, n):
    row, col = np.shape(img_fft)
    mask = np.full((row,col), 0.0).astype(np.float32)
    center_row = row // 2
    center_col = col // 2 
    
    for i in range (row):
        for j in range(col):
            dist = np.sqrt((center_row-i)**2+(center_col-j)**2)
            mask[i, j] = 1 / (1 + (dist / cutoff)**(2*n))
    return mask


def average_filtering(img2d, msize):
    buff2d = np.full((img2d.shape[0], img2d.shape[1]), 0)
    mask = np.ones((msize, msize)) / (msize*msize)
    hs = msize // 2
    
    for i in range(hs, row-hs):
        for j in range(hs, col-hs):
            buff2d[i, j] = np.sum(img2d[i-hs:i+hs+1, j-hs:j+hs+1] * mask)
    return buff2d[hs:-hs, hs:-hs]

def maskfiltering(img2d, mask):
    row, col = img2d.shape
    buff2d = np.full((row, col), 0)
    mr, mc = mask.shape[0], mask.shape[1]
    hs = mr // 2
    
    for i in range(hs, row-hs):
        for j in range(hs, col-hs):
            buff2d[i, j] = np.sum(img2d[i-hs:i+hs+1, j-hs:j+hs+1] * mask)
    return buff2d[hs:-hs, hs:-hs]

if __name__=="__main__":
    img, row, col = readimage("input/cars2_noise.png")
    
    # make grayscale img
    rcimg = img[:,:,0] # r-channel img
    gcimg = img[:,:,1] # g-channel img
    bcimg = img[:,:,2] # b-channel img

    gimg = np.uint8(0.299 * rcimg + 0.587 * gcimg + 0.114 * bcimg) # Convert a color image to a grayscale image 
    saveimage(gimg, "output/cars2/0_grayscale.png")

    img = gimg
    
    # applying median filter    
    median_filter_size = 3
    padded_img = np.lib.pad(img, (((median_filter_size-1)//2, (median_filter_size-1)//2),((median_filter_size-1)//2, (median_filter_size-1)//2)), 'reflect')
    img = medianfiltering(padded_img, median_filter_size)
    saveimage(img, f"output/cars2/1_median_filtering_{median_filter_size}.png")
    

    # # applying z_std norm
    # img = z_standard_normalization(img)
    # img = np.uint8(img)
    # saveimage(img, "output/cars2/z_standard_normalization.png")

    # applying hist eq
    img = histogram_equalization(img)
    saveimage(img, "output/cars2/2_histogram_equalization.png")


    # applying zero padding and BLPF in frequency domain.
    half_row = row // 2
    half_col = col // 2
    padded_img = np.lib.pad(img, ((half_row, half_row),(half_col, half_col)), 'constant', constant_values=0)
    saveimage(np.abs(padded_img), "output/cars2/3_padded.png")

    print(img.shape) # (981, 1741)    
    print(padded_img.shape) # (1961, 3481)
    
    img_fft = imagefft(padded_img, center=True)    
    b_mask = make_butterworth_LPF_mask_f(img_fft, cutoff=500, n=3)
    saveimage(b_mask*255, f"output/cars2/4_mask_in_frequency_domain.png")

    img_fft_filterd = img_fft * b_mask
    
    img = imageifft(img_fft_filterd, center=True)
    img = img[half_row:-half_row, half_col:-half_col]
    
    img = np.abs(img)
    max = img.max()
    min = img.min()
    img = (img - min) / (max-min) * 255  
    saveimage(img, f"output/cars2/5_final_reduced_noise.png")

    # applying sobel operator
    sobel_x = np.array([[-1, -2, -1],
                        [0, 0, 0],
                        [1, 2, 1]])
    sobel_y = np.array([[-1, 0, 1],
                        [-2, 0, 2],
                        [-1, 0, 1]])

    sobel_filter_size = 3
    padded_img = np.lib.pad(img, (((sobel_filter_size-1)//2, (sobel_filter_size-1)//2),((sobel_filter_size-1)//2, (sobel_filter_size-1)//2)), 'reflect')
    grad_x = maskfiltering(padded_img, sobel_x)
    max = grad_x.max()
    min = grad_x.min()
    grad_x_img = (grad_x - min) / (max-min) * 255  
    saveimage(grad_x_img, "output/cars2/6_sobel_x.png")

    grad_y = maskfiltering(padded_img, sobel_y)
    max = grad_y.max()
    min = grad_y.min()
    grad_y_img = (grad_y - min) / (max-min) * 255  
    saveimage(grad_y_img, "output/cars2/7_sobel_y.png")

    grad_img = np.sqrt((grad_x)**2 + grad_y**2)
    max = grad_img.max()
    min = grad_img.min()
    grad_img = (grad_img - min) / (max-min) * 255  
    saveimage(grad_img, "output/cars2/8_sobel_final.png")
    
    # applying padding and BHPF in frequency domain
    half_row = row // 2
    half_col = col // 2
    padded_img = np.lib.pad(img, ((half_row, half_row),(half_col, half_col)), 'constant', constant_values=0)
    img_fft = imagefft(padded_img, center=True)    
    BHPF_cutoff=200
    BHPF_order=3
    mask = make_butterworth_LPF_mask_f(img_fft, cutoff=BHPF_cutoff, n=BHPF_order) 
    HPF_mask = np.ones_like(mask) - mask # HPF_mask = 1 - LPF_mask
    saveimage(HPF_mask*255, f"output/cars2/temp_BHPF_mask.png")
        
    img_fft_filtered = np.multiply(img_fft, HPF_mask)

    img = imageifft(img_fft_filtered, center=True)
    img = np.abs(img)
    img = img[half_row:-half_row, half_col:-half_col]

    max = img.max()
    min = img.min()
    img = (img - min) / (max-min) * 255  
    saveimage(img, f"output/cars2/temp_highlighted_edge.png")
    
    threshold=50
    img[img<threshold] = 0
    saveimage(img, f"output/cars2/temp_with_thresholding.png")

