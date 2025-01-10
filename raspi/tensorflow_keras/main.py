import tensorflow as tf
import pathlib
import numpy as np
import pandas as pd
from PIL import Image
import matplotlib.pyplot as plt
import matplotlib
matplotlib.use("Qt5agg")
import time


def show_result(ai: pd.DataFrame):
	plt.ion()
	plt.show(block=False)
	plt.axis("off")
	path = pathlib.Path(__file__).parents[1].resolve()
	i = 0
	while True:
		img = np.asarray(Image.open(str(path / 'generated_tests' / 'ball_simple_one' / (str(i)+'.png'))))
		imgplot = plt.imshow(img)
		plt.text(100, 100, "ai kacke")
		time.sleep(0.2)
		ipt = input("")
		if(ipt=="b"):
			i-=1
		if(ipt=="n"):
			i+=1
		if(ipt==""):
			i+=1
		if(ipt=="x"):
			break



def simple():
	pass



if __name__ == "__main__":
	print("TensorFlow version:", tf.__version__)
	simple()
	
	path = pathlib.Path(__file__).parent.parent.resolve()
	path  = str(((path / 'generated_tests') / 'ball_simple_one') / 'df.csv')
	train_df = pd.read_csv(path)
	print(train_df)
	show_result(train_df)

