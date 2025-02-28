
import numpy as np
import cv2 as cv
import os
import math

FOV = 90

def main():
    """
    python is weird, it runs faster in a function
    """
    cap = cv.VideoCapture(0)
    if not cap.isOpened():
        print("corners.py: Cannot open camera")
        exit()
    
    ret, frame = cap.read()

    while True:
        ret, frame = cap.read()
        if not ret: 
            print("corners.py: End in video stream, releasing...")
            break

        # resize frame to get a better performance
        frame = cv.resize(frame, (160, 120), interpolation=cv.INTER_CUBIC)

        # extract color channels
        blueChannel  = frame[:, :, 0]
        greenChannel = frame[:, :, 1]
        redChannel   = frame[:, :, 2]

        # subtract channels to get a map
        f = np.subtract(greenChannel.astype(np.int16)*2,redChannel.astype(np.int16))
        f = np.subtract(f,blueChannel.astype(np.int16))
        border = round(f.max()/255*45)
        #print("corners.py: border: ", border)
        # select border
        f[f < border] = 0
        f = f.astype(np.uint8)

        # get center point of selection
        indices = np.indices((120, 160), dtype=np.uint8)
        y = indices[0,f>border]
        x = indices[1,f>border]
        center = int(x.sum() / len(x)), int(y.sum() / len(y))

        angle = (f.shape[1]/2-center[0])/f.shape[0]*FOV
        print(angle)

        # **debug** comment out before use in robot
        showframe = cv.cvtColor(f, cv.COLOR_GRAY2BGR)

        if (border > 25):
            showframe[f > border] = np.array([0,255,0])
            cv.circle(showframe, center, 2, (255,0,0))

        # **debug** comment out before use in robot
        cv.imshow("frame", showframe)
        #cv.imshow("red",   redChannel)
        #cv.imshow("green", greenChannel)
        #cv.imshow("blue",  blueChannel)
        if cv.waitKey(1) == ord('q'):
            print("corners.py: Manual stop requested, releasing...")
            break

    # When everything done, release the capture
    cap.release()
    cv.destroyAllWindows()
    print("corners.py: ...done")


if __name__ == "__main__":
    main()

