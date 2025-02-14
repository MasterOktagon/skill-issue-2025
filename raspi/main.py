import time
import pigpio
from picamera2 import Picamera2
import os
import numpy as np
import tensorflow as tf

camera = Picamera2()

I2C_ADDR=0x18

last_class = 0
last_angle = 0
last_dist = 0

def i2c(id, tick):
    global pi

    s, b, d = pi.bsc_i2c(I2C_ADDR)
    if b:
        if d[0] == 0xE0:
            print("something")

            print("sent={} FR={} received={} [{}]".format(s>>16, s&0xfff,b,d))

            s, b, d = pi.bsc_i2c(I2C_ADDR, [0x00])

        else:
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
    frame = np.array(camera.capture_array())
    print(frame.shape)
    yp_class, yp_box = model.predict(frame)
    print("Model Prediction Class: ", yp_class)
    print("Model Prediction Box: ", yp_box)
    if(yp_class>TRUST):
        angle = (yp_box[0][0]+yp_box[0][2]/2)/640 *camera_fov - camera_fov/2
        print("Ball Angle", angle)
    else:
        print("no Ball")



e.cancel()

pi.bsc_i2c(0) # Disable BSC peripheral

pi.stop()
