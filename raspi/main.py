import time
import pigpio
from picamera2 import Picamera2
import os
import numpy as np
import tensorflow as tf

camera = Picamera2()

I2C_ADDR=0x18

last_ball_class = 0
last_ball_angle = 0
last_ball_dist = 0
last_ball_time = 0

last_corner_class = 0
last_corner_angle = 0
last_corner_dist = 0
last_corner_time = 0

balls_active = False
corners_active = False
living_rescued = False

def i2c(id, tick):
    global pi

    s, b, d = pi.bsc_i2c(I2C_ADDR)
    if b:
        match d[0]:
            case 0xE0:
                print("status requested")

                print("sent={} FR={} received={} [{}]".format(s>>16, s&0xfff,b,d))

                s, b, d = pi.bsc_i2c(I2C_ADDR, [0x00])
            case 0x11:
                print("ball read")
                msg = 0x00
                if time.time()-last_ball_time<1:	# time anpassen
                    msg = 1
                s, b, d = pi.bsc_i2c(I2C_ADDR, [msg])
            case 0x12:
                print("corner read")
                msg = 0x00
                if time.time()-last_ball_time<1:	# time anpassen
                    msg = 1
                s, b, d = pi.bsc_i2c(I2C_ADDR, [msg])
            case 0xF1:
                print("balls activate")
                balls_active = True
                corners_active = False
            case 0xF2:
                print("corner activate")
                balls_active = False
                corners_active = True
            case 0x21:
                print("corner delivered")
                living_rescued = True
            case _:
                print("nothing")

pi = pigpio.pi()

if not pi.connected:
    print("no i2c connection")
    exit()
e = pi.event_callback(pigpio.EVENT_BSC, i2c)
pi.bsc_i2c(I2C_ADDR)
print(camera.start())

# path = "./recorded_images/"
# for i in range(400,600,1):
#     camera.capture_file(str(os.path.join(path, 'frame%d.jpg') % i))
#     time.sleep(1)

model = tf.keras.models.load_model("ball_detection_V2.keras")

camera_fov = 53.50  # richtig?
TRUST = 0.5         # verändern?

while  True:
    print("wating")
    #time.sleep(1)
    if balls_active:
        last_ball_time = time.time()
        frame = np.array(camera.capture_array())
        #print(frame.shape)
        yp_class, yp_box = model.predict(frame)
        print("Model Prediction Class: ", yp_class)
        print("Model Prediction Box: ", yp_box)
        if(yp_class>TRUST):
            angle = (yp_box[0][0]+yp_box[0][2]/2)/640 *camera_fov - camera_fov/2
            print("Ball Angle", angle)
        else:
            print("no Ball")
    elif corners_active:
        pass
    else:
        pass



e.cancel()

pi.bsc_i2c(0) # Disable BSC peripheral

pi.stop()
