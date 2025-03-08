import cv2
import numpy as np

# np.set__printoptions(precision=2, suppress=True)

wp = 9
hp = 6
length = 18 # mm
directory = "/home/gulee/Desktop/ComputerVision/Project3/intrinsic/"
imageExtension = '.jpg' # ext
startImageNum = 0
endImageNum = 17

objp = np.zeros((wp*hp, 3), np.float32)
objp[:, :2] = np.mgrid[0:wp, 0:hp].T.reshape(-1,2)
objp[:, :2] *= length # checkboard 길이 

objpoints = []
imgpoints = []
findImages = []

for i in range(startImageNum, endImageNum):
    img = cv2.imread(directory + str(i) + imageExtension)
    gray = cv2.cvtColor(img, cv2.COLOR_BGR2GRAY)
    ret, corners = cv2.findChessboardCorners(gray, (wp, hp), None)
    img_shape = gray.shape[::-1] # 사진 가로, 세로
    
    if ret==True:
        print(f'{i}th image calib pattern finding success')
        objpoints.append(objp)
        imgpoints.append(corners)
        findImages.append(i)
    
    else:
        print(f'{i}th image calib pattern finding failed')
        
rt, M, D, rvecs, tvecs = cv2.calibrateCamera(objpoints, imgpoints, img_shape, None, None)
print(f'Intrinsic matrix M: \n {M}')
np.savetxt(directory + "Cal_intrinsic.txt", M, fmt="%.2f")
np.savetxt(directory + "Cal_distortion.txt", D, fmt="%.2f")


W = np.full((3, 4), 0.0)
R = np.full((3, 3), 0.0)

for i, no in enumerate(findImages):
    rvec = rvecs[i]
    tvec = tvecs[i]
    
    cv2.Rodrigues(rvec, R)
    W[0:3, 0:3] = R
    W[0:3, 3:4] = tvec
    print(f'{no}th Extrinsic matrix W: \n{W}')
    np.savetxt(directory + "Cal_extrinsic" + str(no) + ".txt", W, fmt='%.2f')
    
print(f"Distorsion coefficient: \n{D}")
print(f"Reprojection error: {rt}")