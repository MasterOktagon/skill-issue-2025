import time
import pigpio
from picamera2 import Picamera2
import os
import numpy as np
import tensorflow as tf
from tensorflow.keras import backend as K
import keras
from PIL import Image

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

@keras.saving.register_keras_serializable(package="custom_package", name="custom_mse")
def custom_mse(y_true, y_pred):
    mask = K.not_equal(K.sum(y_true, axis=1), 0.0)
    y_true_custom = y_true[mask]
    y_pred_custom = y_pred[mask]
    mse = tf.keras.losses.MeanSquaredError()
    result = mse(y_true_custom, y_pred_custom) # * 0.3
    return result

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


def main():
    while  True:
        print("wating")
        #time.sleep(1)
        if balls_active:
            last_ball_time = time.time()
            frame = np.array(camera.capture_array())
            frame = frame.crop((0,150,640,414))
            frame = frame.transpose(Image.FLIP_TOP_BOTTOM)
            frame = np.expand_dims(frame/255, axis=0)
            print(frame.shape)
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

if __name__ == "__main__":
    main()
