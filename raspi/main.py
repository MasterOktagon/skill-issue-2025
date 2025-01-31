import time
import pigpio

I2C_ADDR=0x18

def i2c(id, tick):
    global pi

    s, b, d = pi.bsc_i2c(I2C_ADDR)
    if b:
        if d[0] == 0xE0:
            print("something")

            print("sent={} FR={} received={} [{}]".format(s>>16, s&0xfff,b,d))

            s, b, d = pi.bsc_i2c(I2C_ADDR, 0x00)

        else:
            print("nothing")

pi = pigpio.pi()

if not pi.connected:
    print("no i2c connection")
    exit()
e = pi.event_callback(pigpio.EVENT_BSC, i2c)
pi.bsc_i2c(I2C_ADDR)

#while  True:
print("wating")
time.sleep(15)

e.cancel()

pi.bsc_i2c(0) # Disable BSC peripheral

pi.stop()
