import torch
import torchvision.datasets as datasets
import torchvision.transforms as transforms
import numpy as np
import cv2
from PIL import Image


def saveimage(img, name):
    cv2.imwrite(name, img)
    return 

if __name__=="__main__":
    train_dataset = datasets.MNIST(root='./data', train=True, transform=transforms.ToTensor(), download=True)
    test_dataset = datasets.MNIST(root='./data', train=False, transform=transforms.ToTensor(), download=True)


    for i in range(len(train_dataset)):
        data = train_dataset[i]
        img = 255 * (1 - np.array(data[0]).reshape(28, 28, 1))

        label = data[1]
        filname=f"img_{i}.png"
        
        if label == 1:
            path = "./MNIST_original/1/" + filname
        elif label == 2:
            path = "./MNIST_original/2/" + filname
        elif label == 3:
            path = "./MNIST_original/3/" + filname
        elif label == 4:
            path = "./MNIST_original/4/" + filname
        else:
            continue   
        
        saveimage(img, path)
        
    for i in range(len(test_dataset)):
        data = test_dataset[i]
        img = np.array(data[0]).reshape(28, 28, 1)
        label = data[1]
        filname=f"img_{i}.png"
        
        if label == 1:
            path = "./MNIST_original/1/" + filname
        elif label == 2:
            path = "./MNIST_original/2/" + filname
        elif label == 3:
            path = "./MNIST_original/3/" + filname
        elif label == 4:
            path = "./MNIST_original/4/" + filname
        else:   
            continue   
        saveimage(img, path)