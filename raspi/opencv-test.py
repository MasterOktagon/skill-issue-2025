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

            #print(list(zip(circlesx, circlesy)))
            circles2 = kmeans.k_means(list(zip(circlesx, circlesy)), 3)
            circles3 = circles2.copy()            

            counters = [0]*len(circles2)
            radiuses = [[]]*(len(circles2))
            print(radiuses)
            i = 0
            for point in list(zip(circlesx, circlesy)):
                mini_val = math.inf
                mini_idx = 0
                for c in range(len(circles2)):
                    if math.dist(point, circles2[c]) < 60:
                        counters[c] += 1
                    elif math.dist(point, circles2[c]) < mini_val:
                        mini_val = math.dist(point, circles2[c])
                        mini_idx = c
                radiuses[mini_idx] = radiuses[mini_idx] + [circlesr[i] + math.sqrt(math.dist(point, circles2[c]))]
            
                i += 1
            
            radiuses2 = []
            if [] in radiuses:
                radiuses.remove([])
            for c in radiuses:
                print(c)
                if len(c) > 0:
                    radiuses2.append(round(sum(c)/len(c)/.95))

            radiuses = radiuses2            
            print(radiuses)
                
        
            circles2 = np.uint16(np.around(circles2))
            circles3 = np.uint16(np.around(circles3))

            circles = np.uint16(np.around(circles))

            frame = cv.cvtColor(frame, cv.COLOR_GRAY2BGR)

            for i in circles[0,:]:
                # draw the outer circle
                cv.circle(frame,(i[0],i[1]),i[2],(0,0,0),2)
                # draw the center of the circle
                cv.circle(frame,(i[0],i[1]),2,(0,0,0),3)

            for i in circles3:
                # draw the outer circle
                cv.circle(frame,(i[0],i[1]),40,(255,0,0),2)
                # draw the center of the circle
                cv.circle(frame,(i[0],i[1]),2,(255,0,0),3)
            
            c = 0
            for i in circles2:
                if counters[c] > 0:
                    # draw the outer circle
                    cv.circle(frame,(i[0],i[1]),radiuses[-c-1],(0,255,0),2)
                    # draw the center of the circle
                    cv.circle(frame,(i[0],i[1]),2,(0,255,0),3)
                c += 1

        cv.imshow('frame', frame)
        key = cv.waitKey(10000)
        if key == ord('q'):
            break
        elif key == ord('d'):
            continue
 
# When everything done, release the capture
cap.release()
cv.destroyAllWindows()



