import numpy as np
import cv2 as cv
import os
import math
#import random
#import asyncio
import kmeans

def unzip(c):
    # WTF???
    circles = list(zip(*list(zip(*c))))[0]
    circlesx = [list(thing)[0] for thing in circles]
    circlesy = [list(thing)[1] for thing in circles]
    circlesr = [list(thing)[2] for thing in circles]        
    #print(circlesx)
    #print(circlesy)
    #print(circlesr)

    return circlesx, circlesy, circlesr
    

cap = cv.VideoCapture(0)
if not cap.isOpened():
    print("Cannot open camera")
    exit()
for root, d, files in os.walk("testimages"):
    for file in files:
        # Capture frame-by-frame
        #ret, frame = cap.read()
        frame = cv.imread("testimages/" + file)
        
        # if frame is read correctly ret is True
        #if not ret:
        #    print("Can't receive frame (stream end?). Exiting ...")
        #    break
        #    #

        frame = cv.cvtColor(frame, cv.COLOR_BGR2GRAY)
        
        circles = cv.HoughCircles(frame,cv.HOUGH_GRADIENT,1,10,
            param1=30,param2=40,minRadius=0,maxRadius=200)

        if circles is not None:

            
            circlesx, circlesy, circlesr = unzip(circles)
            
            #circles = [(sum(circlesx)/len(circlesx), sum(circlesy)/len(circlesy), sum(circlesr)/len(circlesr))]
            #v1 = sum([abs(math.dist(circlesn[0][:2], (circlesx[i], circlesy[i]))) for i in range(len(circlesx))])            
            #print(v1, circlesn)

            print(list(zip(circlesx, circlesy)))
            circles = kmeans.k_means(list(zip(circlesx, circlesy)), 2)
        
            circles = np.uint16(np.around(circles))
            
            for i in circles:
                # draw the outer circle
                cv.circle(frame,(i[0],i[1]),40,(0,255,0),2)
                # draw the center of the circle
                cv.circle(frame,(i[0],i[1]),2,(0,0,255),3)

            cv.imshow('frame', frame)
        if cv.waitKey(10000) == ord('q'):
            break
 
# When everything done, release the capture
cap.release()
cv.destroyAllWindows()



