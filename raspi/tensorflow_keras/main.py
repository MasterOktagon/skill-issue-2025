import tensorflow as tf
import pathlib
import numpy as np
import pandas as pd
from PIL import Image
import tkinter as tk
import time

def show_result(df: pd.DataFrame):
	def next_entry():
		global _entry
		global _im_label
		if _entry==len(df.index):
			return
		_entry+=1
		im = tk.PhotoImage(str(path / 'generated_tests' / 'ball_simple_one' / (str(_entry)+'.png')))
		_im_label.configure(image=im)
		_im_label.image = im
		print(_im_label.info)

	global _entry
	_entry = 0
	path = pathlib.Path(__file__).parents[1].resolve()
	root = tk.Tk()
	root.title("Result of Victim AI")
	root.geometry('1000x600+50+50')

	im = tk.PhotoImage(file=str(path / 'generated_tests' / 'ball_simple_one' / '0.png'))
	global _im_label
	_im_label = tk.Label(root, image=im)
	_im_label.pack()

	next_button = tk.Button(root, text="Next Entry", command=next_entry)
	next_button.place(x=700,y=500)

	root.mainloop()



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

