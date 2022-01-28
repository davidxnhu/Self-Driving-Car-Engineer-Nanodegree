import csv
import cv2
import numpy as np

print ('here')

#read datafile
lines = []
with open('./test/driving_log.csv') as csvfile:
    reader = csv.reader(csvfile)
    # skip the header
    next(reader)

    for line in reader:
        lines.append(line)



#read images, and make data augmentation
images = []
measurements = []
from scipy import ndimage
for line in lines:
    source_path = line[0]
    filename = source_path.split('/')[-1]
    current_path = './test/IMG/' + filename
    image = ndimage.imread(current_path)

    images.append(image)
    measurement = float(line[3])
    measurements.append(measurement)

augmented_images, augmented_measurements = [], []
for image, measurement in zip(images, measurements):
    augmented_images.append(image)
    augmented_measurements.append(measurement)
    augmented_images.append(np.fliplr(image))
    augmented_measurements.append(-measurement*1.0)




X_train = np.array(augmented_images)
y_train = np.array(augmented_measurements)


print (len(y_train))




