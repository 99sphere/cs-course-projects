import os
from PIL import Image
import numpy as np
import cv2

def saveimage(img, name):
    img=np.uint8(img)
    img=Image.fromarray(img)
    img.save(name)
    return 

def make_MNIST(img, col, row, grayscale=False):
    size = max(col, row)
    if grayscale:
        template = 255 * np.ones((size, size, 1))
    else:
        template = 255 * np.ones((size, size, 3))
    
    if size == col:
        template[(size-row)//2:(size-row)//2+row, :] = img
    else:
        template[:, (size-col)//2:(size-col)//2+col] = img
        
    img = cv2.resize(template, (28, 28))
    return np.array(img)

if __name__=="__main__":
    dir_path = "./data/Provided"

    for (root, directories, files) in os.walk(dir_path):
        for file in files:
            file_path = os.path.join(root, file)
            temp = file_path.split('/')
            file_name = temp[-1]
            print("filename: ", file_name)
                            
            temp[2] = "MNIST_rgb_data"
            MNIST_rgb_path = '/'.join(temp)
            temp[2] = "MNIST_gray_data"
            MNIST_gray_path = '/'.join(temp)
            
            img = Image.open(file_path).convert('RGB') # RGBA -> RGB
            col, row = img.size
            img = np.array(img)
            
            rgb_img = make_MNIST(img, col, row, grayscale=False)
            rcimg = img[:,:,0] # r-channel img
            gcimg = img[:,:,1] # g-channel img
            bcimg = img[:,:,2] # b-channel img
            gimg = np.uint8(0.299 * rcimg + 0.587 * gcimg + 0.114 * bcimg).reshape(row, col, 1) # Convert a color image to a grayscale image 
            gray_img = make_MNIST(gimg, col, row, grayscale=True)
 
            cv2.imwrite(MNIST_rgb_path, rgb_img)
            saveimage(gray_img, MNIST_gray_path)