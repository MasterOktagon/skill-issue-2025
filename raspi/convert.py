import pandas as pd
import pathlib
import numpy as np

file = ((pathlib.Path(__file__).parent.resolve() / 'images/train') / '_annotations.csv')
df = pd.read_csv(file)
df.drop(columns=['width', 'height'], inplace=True)
df.rename(columns={'class':'ball_exists'}, inplace=True)
df.rename(columns={'filename':'path'}, inplace=True)
xs = []
ys = []
widths = []
heights = []

unique = df['path'].unique()
mult = [unique, np.zeros(len(unique), dtype=int), np.zeros(len(unique), dtype=int)]

for i in df.index:
    xs.append((df.loc[i, 'xmax']+df.loc[i, 'xmin'])//2)
    ys.append((df.loc[i, 'ymax']+df.loc[i, 'ymin'])//2)
    widths.append(df.loc[i, 'xmax']-df.loc[i, 'xmin'])
    heights.append(df.loc[i, 'ymax']-df.loc[i, 'ymin'])
    df.loc[i, 'ball_exists'] = int(1)

    j = np.where(unique==df.loc[i, 'path'])
    if widths[i]>mult[1][j]:
        mult[1][j] = widths[i]
        mult[2][j] = i

df['x'] = xs
df['y'] = ys
df['w'] = widths
df['h'] = heights
df.drop(columns=['xmin','xmax','ymin','ymax'], inplace=True)

for i in df.index:
    if i in mult[2]: continue
    else: df.drop(i, inplace=True)

print(df)
df.to_csv(str(file.parent / 'dataset.csv'), index=False)
