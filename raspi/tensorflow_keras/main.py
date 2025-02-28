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

def plot_bbox(image, yt_box, yp_box=None, norm=False):
    # If image is normalized (/255.) reconstruct (inverse) the operation
    if norm:
        image = image * 255.
        image = image.astype("uint8")
    
    # Convert image to array if not converted
    try:
        pil_img = Image.fromarray(image)
    except:
        pil_img = Image.fromarray(image.astype('uint8'))
        
    draw_img = ImageDraw.Draw(pil_img)
    
    x1, y1, w = yt_box
    h = w
    x2, y2 = x1+w, y1+h
    draw_img.rectangle((x1, y1, x2, y2), outline='green')
    
    # If y_pred box is given, draw it
    if yp_box is not None:
        x1, y1, w = yp_box
        h = w
        x2, y2 = x1+w, y1+h
        draw_img.rectangle((x1, y1, x2, y2), outline='red')
    return pil_img

model = tf.keras.models.load_model("ball_detection_V2.keras")
valid_dataset = pd.read_csv('./images/valid/dataset.csv')
def visualize_evaluation(model, name):    
    # Get image
    im = Image.open("./images/valid/"+name)
    image = np.asarray(im)
    image = np.expand_dims(image, axis=0)
    image /= 255.
    print(image.shape)
    
    # Set y_true & y_pred for class & bounding box
    print(valid_dataset.loc[valid_dataset['path']==name]['x'], valid_dataset.loc[valid_dataset['path']==name]['y'], valid_dataset.loc[valid_dataset['path']==name]['w'])
    yt_box = np.array([valid_dataset.loc[valid_dataset['path']==name]['x'], valid_dataset.loc[valid_dataset['path']==name]['y'], valid_dataset.loc[valid_dataset['path']==name]['w']])
    yt_class = np.array(valid_dataset.loc[valid_dataset['path']==name]['ball_exists'])
    
    yp_class, yp_box = model.predict(image)

    camera_fov = 53.50
    angle = (yp_box[0][0]+yp_box[0][2]/2)/640 *camera_fov - camera_fov/2
    print("Ball Angle", angle)

    # Plot bounding box on image & show it
    image_plotted = plot_bbox(image[0], yt_box[0], yp_box[0], norm=False) #True?
    plt.imshow(image_plotted)
    plt.axis('off')
    
    # Print y_true class & y_pred class
    print("Class: y_true=", yt_class, " | y_pred=", int(yp_class >= 0.5))
    plt.show()

if __name__ == "__main__":
	visualize_evaluation(model, "frame599.jpg")

