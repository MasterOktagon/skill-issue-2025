import cv2 as cv
import os
import time


path = "./recorded_images/"
cap = cv.VideoCapture(0)
if not cap.isOpened():
    print("Cannot open camera")
    exit()
for i in range(100):
    ret, frame = cap.read()
    if ret:
        cv.imwrite(os.path.join(path, 'frame%d.jpg') % i, frame)
    time.sleep(1)

cap.release()