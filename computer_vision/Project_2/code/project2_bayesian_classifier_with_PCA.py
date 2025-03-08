import numpy as np
import os
from PIL import Image
from torch.utils.data import Dataset, DataLoader
# import torchvision.dataset as datasets
import numpy as np
import torch
import cv2
from sklearn.decomposition import PCA

class CustomImageDataset(Dataset):
    def read_data_set(self):
        all_img_files = []
        all_labels = []
        
        class_names = os.walk(self.data_set_path).__next__()[1]
        
        for index, class_name in enumerate(class_names):
            label = index
            img_dir = os.path.join(self.data_set_path, class_name)
            img_files = os.walk(img_dir).__next__()[2]
            
            for img_file in img_files:
                img_file = os.path.join(img_dir, img_file)
                img = Image.open(img_file)
                if img is not None:
                    all_img_files.append(img_file)
                    all_labels.append(label)
                    
        return all_img_files, all_labels, len(all_img_files), len(class_names)
    
    def __init__(self, data_set_path, transforms=None):
        self.data_set_path = data_set_path
        self.image_files_path, self.labels, self.length, self.num_classes = self.read_data_set()
        self.transforms = transforms
        
    def __getitem__(self, index):
        image = Image.open(self.image_files_path[index])
        image = image.convert("RGB")
        
        if self.transforms is not None:
            image = self.transforms(image)
            
        return {'image': image, 'label': self.labels[index]}
    
    def __len__(self):
        return self.length
    
    
def bc(inp, meanp, cv, pr): # cv is covariance matrix of {cl} * {descriptor} x {descriptor}, pr is probability of classes {cl}
    cl, dim = np.shape(meanp) # 4, 784
    dfn = np.full(cl, 0.0)
    mdfn = 10 ** 100
    inp = np.matrix(inp)
    meanp = np.matrix(meanp)
    for i in range(cl):
        cv[i] = np.matrix(cv[i])
        
    for i in range(cl):
        dfn[i] = np.log(pr[i]) - 0.5*np.linalg.det(cv[i]) + 0.5*(inp-meanp[i,:]) * np.linalg.inv(cv[i])*np.transpose(inp-meanp[i,:])
        
        if dfn[i] < mdfn:
            mdfn = dfn[i]
            mcl = i
    return dfn, mcl


def run_exp(number):
    torch.manual_seed(7)
    np.random.seed(7)
    selected_dataset = 'Gray'

    if selected_dataset== 'RGB':
        print("Select RGB Dataset")
        train_data_set = CustomImageDataset(data_set_path="./data/MNIST_rgb_data/train")
        train_loader = DataLoader(train_data_set, batch_size=1, shuffle=False)

        test_data_set = CustomImageDataset(data_set_path='./data/MNIST_rgb_data/test')
        test_loader = DataLoader(test_data_set, batch_size=1, shuffle=False)

    else:
        print("Select Gray Dataset")
        train_data_set = CustomImageDataset(data_set_path="./data/MNIST_gray_data/train")
        train_loader = DataLoader(train_data_set, batch_size=1, shuffle=False)

        test_data_set = CustomImageDataset(data_set_path='./data/MNIST_gray_data/test')
        test_loader = DataLoader(test_data_set, batch_size=1, shuffle=False)

    # class 별 데이터 읽어오기. cl1, cl2, ... 
    cl1 = [] # class 별 이미지들
    cl2 = []
    cl3 = [] 
    cl4 = []

    train_label = []
    for data in train_data_set:
        if selected_dataset=='RGB':
            img = np.array(data['image']).flatten() # 784 * 3

        else:
            img = np.array(data['image'])[:,:,1].flatten() # 784
            
        label = data['label']
        train_label.append(label)
        if label == 0:
            cl1.append(img)
        elif label == 1:
            cl2.append(img)
        elif label == 2:
            cl3.append(img)
        elif label == 3:
            cl4.append(img)

    train_total = np.array(cl1 + cl2 + cl3 + cl4) # total img (80 * 784)

    pca = PCA(n_components=number)  
    train_feature = pca.fit_transform(train_total)


    # ... 모든 class 값 읽어오기
    test_imgs = [] # test data로 추정
    test_labels = []

    for data in test_data_set:
        if selected_dataset=='RGB':
            img = np.array(data['image']).flatten() # 784 * 3

        else:
            img = np.array(data['image'])[:,:,1].flatten() # 784
        label = data['label']
        test_imgs.append(img)
        test_labels.append(label)    


    cl1_feature = train_feature[:20]
    cl2_feature = train_feature[20:40]
    cl3_feature = train_feature[40:60]
    cl4_feature = train_feature[60:]

    cl1c = np.cov(cl1_feature.T)
    cl2c = np.cov(cl2_feature.T)
    cl3c = np.cov(cl3_feature.T)
    cl4c = np.cov(cl4_feature.T)

    mp = np.array([np.mean(cl1_feature, axis=0), np.mean(cl2_feature, axis=0), np.mean(cl3_feature, axis=0), np.mean(cl4_feature, axis=0)])

    cv = np.array([cl1c, cl2c, cl3c, cl4c])
    pr = np.array([1/len(cv) for _ in range(len(cv))])


    test_feature = pca.transform(test_imgs)

    test_pred = []
    correct = 0

    for i in range(len(test_feature)):
        input1 = test_feature[i] # 32-dim feature vector, results from PCA that is fitted with training data
        df, decide = bc(input1, mp, cv, pr)
        test_pred.append(decide)
        if decide == test_labels[i]:
            correct += 1

    total = len(test_labels)
    print("PCA n_components : ", number)
    print("Test lables       :", test_labels)
    print("Predicted lables  :", test_pred)
    print("Test Accuracy of the model on the {} test images: {} %".format(len(test_labels), 100 * correct / total))
    print()    
    return

if __name__=="__main__":
    run_exp(41)