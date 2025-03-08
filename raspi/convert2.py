import pandas as pd
import pathlib
import numpy as np

file = ((pathlib.Path(__file__).parent.resolve() / 'images/validate') / '_annotations.csv')
df = pd.read_csv(file)
df.drop(columns=['width', 'height'], inplace=True)
df.rename(columns={'class':'ball_exists'}, inplace=True)
df.rename(columns={'filename':'path'}, inplace=True)
xs = []
ys = []
widths = []
heights = []

unique = df['path'].unique()
mult = [unique, np.zeros(len(unique), dtype=int), np.zeros(len(unique), dtype=int), np.zeros(len(unique), dtype=int)]

for i in df.index:
    xs.append(df.loc[i, 'xmin'])
    ys.append(df.loc[i, 'ymin'])
    widths.append(df.loc[i, 'xmax']-df.loc[i, 'xmin'])
    heights.append(df.loc[i, 'ymax']-df.loc[i, 'ymin'])
    df.loc[i, 'ball_exists'] = int(1)

    j = np.where(unique==df.loc[i, 'path'])
    mult[3][j] += 1
    if widths[i]>mult[1][j]:
        mult[1][j] = widths[i]
        mult[2][j] = i

df['x'] = xs
df['y'] = ys
df['w'] = widths
df['h'] = heights
df.drop(columns=['xmin','xmax','ymin','ymax'], inplace=True)

for i in df.index:
    if i in mult[2]:
        j = np.where(mult[2] == i)
        if mult[3][j]>2:
            df.drop(i, inplace=True)
        else: continue
    else: df.drop(i, inplace=True)

from PIL import Image

path = (pathlib.Path(__file__).parent.resolve() / 'recorded_images')
paths = []

# drehen, abschneiden zum resizen und 400 bis 599 nicht trainieren und testen
for i in range(0,400):
    try:
        im = Image.open(path / str("frame"+str(i)+".jpg"))
    except:
        continue
    im = im.rotate(180)
    im2 = im.copy()
    im3 = im.copy()
    im = im.crop((0,150,640,414))
    paths.append(str("frame"+str(i)+".jpg"))
    im.save(str(file.parent / ("frame"+str(i)+'.jpg')))

    im2 = im2.crop((0, 180, 420, 430))
    im2 = im2.resize((640,264))
    paths.append(str("frame"+str(i)+"_2.jpg"))
    im2.save(str(file.parent / ("frame"+str(i)+'_2.jpg')))

    im3 = im3.crop((220, 180, 640, 430))
    im3 = im3.resize((640,264))
    paths.append(str("frame"+str(i)+"_3.jpg"))
    im3.save(str(file.parent / ("frame"+str(i)+'_3.jpg')))

for i in range(600,1200):
    try:
        im = Image.open(path / str("frame"+str(i)+".jpg"))
    except:
        continue
    im = im.rotate(180)
    im2 = im.copy()
    im3 = im.copy()
    im = im.crop((0,150,640,414))
    paths.append(str("frame"+str(i)+".jpg"))
    im.save(str(file.parent / ("frame"+str(i)+'.jpg')))

    im2 = im2.crop((0, 180, 420, 430))
    im2 = im2.resize((640,264))
    paths.append(str("frame"+str(i)+"_2.jpg"))
    im2.save(str(file.parent / ("frame"+str(i)+'_2.jpg')))

    im3 = im3.crop((220, 180, 640, 430))
    im3 = im3.resize((640,264))
    paths.append(str("frame"+str(i)+"_3.jpg"))
    im3.save(str(file.parent / ("frame"+str(i)+'_3.jpg')))

df2 = pd.DataFrame([], columns=['path', 'ball_exists', 'x', 'y', 'w', 'h'])
df2['path'] = paths
df2['ball_exists'] = np.zeros(len(paths),dtype=int)
df2['x'] = np.zeros(len(paths),dtype=int)
df2['y'] = np.zeros(len(paths),dtype=int)
df2['w'] = np.zeros(len(paths),dtype=int)
df2['h'] = np.zeros(len(paths),dtype=int)
print(df2)
df = pd.concat([df, df2])

print(df)
df.to_csv(str(file.parent / 'dataset2.csv'), index=False)
