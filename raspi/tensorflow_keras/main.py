import tensorflow as tf
import pathlib
import numpy as np
import pandas as pd
from PIL import Image
import matplotlib.pyplot as plt
import matplotlib
matplotlib.use("Qt5agg")
import time


def show_result(df: pd.DataFrame):
	path = pathlib.Path(__file__).parents[1].resolve()
	i = 0
	while True:
		img = np.asarray(Image.open(str(path / 'generated_tests' / 'ball_simple_one' / (str(i)+'.png'))))
		imgplot = plt.imshow(img)
		plt.show(block=False)
		plt.pause(1)
		ipt = input("")
		plt.close()
		if(ipt=="b"):
			i-=1
		if(ipt=="n"):
			i+=1
		if(ipt==""):
			i+=1
		if(ipt=="x"):
			break



def default():
	mnist = tf.keras.datasets.mnist

	(x_train, y_train), (x_test, y_test) = mnist.load_data()
	x_train, x_test = x_train / 255.0, x_test / 255.0
	model = tf.keras.models.Sequential([
		tf.keras.layers.Flatten(input_shape=(28, 28)),
		tf.keras.layers.Dense(128, activation='relu'),
		tf.keras.layers.Dropout(0.2),
		tf.keras.layers.Dense(10)
	])
	predictions = model(x_train[:1]).numpy()
	print(predictions)



if __name__ == "__main__":
	print("TensorFlow version:", tf.__version__)
	default()
	
	path = pathlib.Path(__file__).parent.parent.resolve()
	path  = str(((path / 'generated_tests') / 'ball_simple_one') / 'df.csv')
	train_df = pd.read_csv(path)
	print(train_df)
	show_result(train_df)

