import cv2
import numpy as np

np.set_printoptions(precision=2, suppress=True)

wp = 9
hp = 6
length = 18 # mm
directory = "/home/gulee/Desktop/ComputerVision/Project3/intrinsic/"
target_directory = "/home/gulee/Desktop/ComputerVision/Project3/try3/"
imageExtension = '.jpg' # ext
startImageNum = 0
endImageNum = 3

# checkboard 사진 최소 3장, 많을 수록 좋음 + 정면에서 찍는 것도 별로임.

objp = np.zeros((wp*hp, 3), np.float32)
objp[:, :2] = np.mgrid[0:wp, 0:hp].T.reshape(-1,2)
objp[:, :2] *= length # checkboard 길이 

intrinsic = np.loadtxt("/home/gulee/Desktop/ComputerVision/Project3/intrinsic/Cal_intrinsic.txt")
distorsion = np.loadtxt("/home/gulee/Desktop/ComputerVision/Project3/intrinsic/Cal_distortion.txt")

W = np.full((3,4), 0.0)
R = np.full((3,3), 0.0)

# try1
pts = objp[0]-30
pts[-1] = 0
pts = np.append(pts, 1)

for i in range(startImageNum, endImageNum+1):
    img = cv2.imread(target_directory + str(i) + imageExtension)
    gray = cv2.cvtColor(img, cv2.COLOR_BGR2GRAY)
    ret, corners = cv2.findChessboardCorners(gray, (wp, hp), None)
    img_shape = gray.shape[::-1]
    
    if ret==True:
        # print(f'{i}th img pattern recog success')
        ret, rvec, tvec = cv2.solvePnP(objp, corners, intrinsic, distorsion)
        
        cv2.Rodrigues(rvec, R)
        W[0:3, 0:3] = R
        W[0:3, 3:4] = tvec
        # print(f'{i}th HTM matrix W:\n{W}')
        np.savetxt(target_directory + "\HTM_"+str(i)+".txt", W, fmt="%.2f")   

        # print(f'{i}th Projection matrix P:\n{P}')        
                
        # print(f'{i}th inverse Projection matrix P:\n{inverse_P}')

        pts_3d = np.dot(W, pts) 
        print(f'{i}th 3d points:\n{pts_3d}')
    
    else:
        print(f'{i}th image pattern point recog failed')