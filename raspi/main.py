import time
import pigpio

I2C_ADDR=0x18

def i2c(id, tick):
    global pi

    s, b, d = pi.bsc_i2c(I2C_ADDR)
    if b:
        if d[0] == ord('t'): # 116 send 'HH:MM:SS*'

            print("sent={} FR={} received={} [{}]".format(s>>16, s&0xfff,b,d))

            s, b, d = pi.bsc_i2c(I2C_ADDR, "{}*".format(time.asctime()[11:19]))

        elif d[0] == ord('d'): # 100 send 'Sun Oct 30*'

            print("sent={} FR={} received={} [{}]".format(s>>16, s&0xfff,b,d))

            s, b, d = pi.bsc_i2c(I2C_ADDR, "{}*".format(time.asctime()[:10]))

pi = pigpio.pi()

if not pi.connected:
    print("no i2c connection")
    exit()

# Respond to BSC slave activity

e = pi.event_callback(pigpio.EVENT_BSC, i2c)

pi.bsc_i2c(I2C_ADDR) # Configure BSC as I2C slave

time.sleep(10000)

e.cancel()

pi.bsc_i2c(0) # Disable BSC peripheral

pi.stop()