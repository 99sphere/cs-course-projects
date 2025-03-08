"""
2024 Spring, Introduction to Computer Vision.
Project 1 - clock_noise.png
"""

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
            temp=[]
            for p in range(-hs, hs+1):
                for q in range(-hs, hs+1):
                    temp.append(img2d[i+p,j+q])          
            temp.sort()
            buff2d[i, j] = temp[len(temp)//2]
    return buff2d


def maxfiltering(img2d, ms):
    row, col = img2d.shape
    buff2d = np.full((row, col), 0)    
    hs = ms//2
    
    for i in range(hs, row-hs):
        for j in range(hs, col-hs):
            temp=[]
            for p in range(-hs, hs+1):
                for q in range(-hs, hs+1):
                    temp.append(img2d[i+p,j+q])          
            temp.sort()
            buff2d[i, j] = temp[-1]
    return buff2d


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


def make_ideal_LPF_f(img_fft, d0):
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

def average_filtering(img, msize):
    result = np.full((img.shape[0], img.shape[1]), 0)
    mask = np.ones((msize, msize)) / (msize*msize)
    hs = msize // 2
    
    for i in range(hs, row-hs):
        for j in range(hs, col-hs):
            sum=0.0
            for p in range(-hs, hs+1):
                for q in range(-hs, hs+1):
                    sum += img[i+p][j+q]*mask[p+hs][q+hs]
            result[i, j] = sum
    
    return result

if __name__=="__main__":
    img, row, col = readimage("input/clock_noise1.png")
    
    # make grayscale img
    rcimg = img[:,:,0] # r-channel img
    gcimg = img[:,:,1] # g-channel img
    bcimg = img[:,:,2] # b-channel img

    gimg = np.uint8(0.299 * rcimg + 0.587 * gcimg + 0.114 * bcimg) # Convert a color image to a grayscale image 
    saveimage(gimg, "output/clock/grayscale.png")

    img = gimg

    # applying lowpass filter in frequency domain
    half_row = row // 2
    half_col = col // 2
    padded_img = np.lib.pad(img, ((half_row, half_row),(half_col, half_col)), 'constant', constant_values=0)
    saveimage(np.abs(padded_img), "output/clock/histogram_padded.png")

    print(img.shape) # (981, 1741)    
    print(padded_img.shape) # (1961, 3481)
    
    img_fft = imagefft(padded_img, center=True)    
    
    g_mask = make_gaussian_LPF_mask_f(img_fft, std=150)
    saveimage(np.uint8(g_mask*255), f"output/clock/mask_f.png")

    img_fft_filterd = img_fft * g_mask

    specturm_before = np.uint8(20*np.log(np.abs(img_fft)))
    specturm_after = np.uint8(20*np.log(np.abs(img_fft_filterd)))

    fig = plt.figure()
    ax1 = fig.add_subplot(2, 1, 1)
    ax1.imshow(specturm_before, cmap='gray')

    ax2 = fig.add_subplot(2, 1, 2)
    ax2.imshow(specturm_after, cmap='gray')
    plt.savefig('output/clock/fft_spectrum.png')    
    img = imageifft(img_fft_filterd, center=True)
    
    img = img[half_row:-half_row, half_col:-half_col] # center crop
    img = np.abs(img)
    img = img/np.max(img) * 255
    saveimage(img, "output/clock/final_BLPF_f.png")
        
    # applying z_std norm
    # img = z_standard_normalization(img)
    # saveimage(img, "output/clock/z_standard_normalization.png")

    # applying min max norm
    img = minmax_normalization(img, img.min(), img.max())
    saveimage(img, "output/clock/minmax_normalization.png")

    # applying average filter in spatial domain
    average_filter_size=5
    img = average_filtering(img, msize=average_filter_size)
    img = average_filtering(img, msize=average_filter_size)
    img = average_filtering(img, msize=average_filter_size)
    saveimage(img, f"output/clock/average_filter_{average_filter_size}.png")


    # applying hist eq.
    img = np.uint8(img)
    img = histogram_equalization(img)
    saveimage(img, "output/clock/histogram_equalization.png")

    median_filter_size = 3
    img = medianfiltering(img, median_filter_size)
    img = medianfiltering(img, median_filter_size)

    saveimage(img, f"output/clock/median_filtering_{median_filter_size}.png")

    # applying median filter    