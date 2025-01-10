import numpy as np
import pandas as pd
import random as rd
from PIL import Image, ImageDraw
import pathlib

white = (230,230,230)
black = (30,30,30)
gray = (150,150,150)

print("Hello World")

def one_ball(n,width,height):
    path = pathlib.Path(__file__).parent.resolve()
    columns = ['x', 'y', 'r']
    data = []
    for i in range(n):
        r = rd.randint(30,80)
        x = rd.randint(0,width)
        y = rd.randint(r//2,height-r)
        im = Image.new("RGB", (width, height), white)
        if rd.random()<0.92:
            draw = ImageDraw.Draw(im)
            draw.circle((x,y),r,black)
            im.save(str(path / 'generated_tests' / 'ball_simple_one' / (str(i)+'.png')))
            data.append((x,y,r))
        else:
            im.save(str(path / 'generated_tests' / 'ball_simple_one' / (str(i)+'.png')))
            data.append((-1,-1,0))
    df = pd.DataFrame(data=data, columns=columns)
    df.to_csv(str(path / 'generated_tests' / 'ball_simple_one' / 'df.csv'), index=False)


if __name__=="__main__":
    n = 1000
    one_ball(n,480,240)
