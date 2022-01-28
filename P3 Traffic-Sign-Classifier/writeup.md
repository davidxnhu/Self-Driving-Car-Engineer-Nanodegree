# **Traffic Sign Recognition** 

## Writeup

### You can use this file as a template for your writeup if you want to submit it as a markdown file, but feel free to use some other method and submit a pdf if you prefer.

---

**Build a Traffic Sign Recognition Project**

The goals / steps of this project are the following:
* Load the data set (see below for links to the project data set)
* Explore, summarize and visualize the data set
* Design, train and test a model architecture
* Use the model to make predictions on new images
* Analyze the softmax probabilities of the new images
* Summarize the results with a written report


## Rubric Points
### Here I will consider the [rubric points](https://review.udacity.com/#!/rubrics/481/view) individually and describe how I addressed each point in my implementation.  

---
### Writeup / README

#### 1. Provide a Writeup / README that includes all the rubric points and how you addressed each one. You can submit your writeup as markdown or pdf. You can use this template as a guide for writing the report. The submission includes the project code.

You're reading it! and the code is included in the same workplace named "Traffic_Sign_Classifier_with_data_augmentation_2.ipynb".

### Data Set Summary & Exploration

#### 1. Provide a basic summary of the data set. In the code, the analysis should be done using python, numpy and/or pandas methods rather than hardcoding results manually.

I used the pandas library to calculate summary statistics of the traffic
signs data set:

* The size of training set is 34799 (X_train.shape[0])
* The size of the validation set is 4410 (X_valid.shape[0])
* The size of test set is 12630 (X_test.shape[0])
* The shape of a traffic sign image is (32,32,3) (X_train[0].shape)
* The number of unique classes/labels in the data set is 43 (len(set(y_train)))

#### 2. Include an exploratory visualization of the dataset.

Here is an exploratory visualization of the data set. It is a bar chart showing how the occurance of each label in the training dataset.

[image1]: ./examples/barchart.jpg "Visualization"

### Design and Test a Model Architecture

#### 1. Describe how you preprocessed the image data. What techniques were chosen and why did you choose these techniques? Consider including images showing the output of each preprocessing technique. Pre-processing refers to techniques such as converting to grayscale, normalization, etc. (OPTIONAL: As described in the "Stand Out Suggestions" part of the rubric, if you generated additional data for training, describe why you decided to generate additional data, how you generated the data, and provide example images of the additional data. Then describe the characteristics of the augmented training set like number of images in the set, number of images for each class, etc.)

I decided to convert the images to grayscale because it is easier for the classifier to learn.
And, I also normalized the image data because it speeds up learning and leads to faster convergence.

Here is an example of a traffic sign image before and after preprocessing.

[image2]: ./examples/processing_image.jpg "Preprocessing"



I decided to generate additional data because the amounts of data for certain labels are limited and it does not matter with the position, size, etc. of the original images since we are aiming to classify the images.


To add more data to the the data set, we just need to make minor alterations to our existing dataset. Minor changes such as flips or translations or rotations. Our neural network would think these are distinct images anyway. "ImageDataGenerator" from Keras is used to generate more data.


Here is an example of an original image and an augmented image:

[image3]: ./examples/original_image.jpg
[image4]: ./examples/augmented_image.jpg

The difference between the original data set and the augmented data set is the following:
1. with rotation
2. zooming
3. horizontal shift of the image
4. vertical shift of the image


#### 2. Describe what your final model architecture looks like including model type, layers, layer sizes, connectivity, etc.) Consider including a diagram and/or table describing the final model.

My final model consisted of the following layers:

| Layer         		|     Description	                        					| 
|:---------------------:|:-------------------------------------------------------------:| 
| Input         		| 32x32x1 normalized grayscale image   							| 
| Convolution 5x5     	| 1x1 stride, valid padding, outputs 28x28x6                	|
| RELU					|																|
| Max pooling	      	| 2x2 stride,  outputs 14x14x6                 			 		|
| Convolution 5x5	    | 1x1 stride, valid padding, outputs 10x10x16                	|
| RELU					|																|
| Max pooling	      	| 2x2 stride,  outputs 5x5x16                 			 		|
| Flatten	      		| input 400                 									|
| Fully connected		| input 400, output 120                     					|
| RELU					|																|
| Dropout				| %50 keep														|
| Fully connected		| input 120, output 84                  						|
| RELU					|																|
| Dropout				| %50 keep														|
| Fully connected		| input 84, output 43               	   						|
| Softmax				| etc.                     										|

 


#### 3. Describe how you trained your model. The discussion can include the type of optimizer, the batch size, number of epochs and any hyperparameters such as learning rate.

To train the model, I used the AdamOptimizer, a batch size of 128 and 41 epochs (to print the accuracy for every 10th epochs from 0th to 40th). 

Other hyperparameters include a learning rate of 0.0003, mu of 0 and sigma of 0.1 for randomizing the weights in the first convolutional layer. Dropout is chosen as 0.5. 

There is also a upper limit "BATCHES_PER_EPOCH = 5000" that defines the maximum number of batches in each epoch, with the data augmentation.

#### 4. Describe the approach taken for finding a solution and getting the validation set accuracy to be at least 0.93. Include in the discussion the results on the training, validation and test sets and where in the code these were calculated. Your approach may have been an iterative process, in which case, outline the steps you took to get to the final solution and why you chose those steps. Perhaps your solution involved an already well known implementation or architecture. In this case, discuss why you think the architecture is suitable for the current problem.

My final model results were:
* training set accuracy of 0.994
* validation set accuracy of 0.974 
* test set accuracy of 0.956.

I referred to LeNet architecture, because it is performing well in recognization of 10 classes.

Since the accuracy for training, validation and test dataset are all quite high, there is no evidence that the model is overfitted/underfitted.

 

### Test a Model on New Images

#### 1. Choose five German traffic signs found on the web and provide them in the report. For each image, discuss what quality or qualities might be difficult to classify.

Here are five German traffic signs that I found on the web:

[image5]: ./Newsigns/pedestrian.jpg
[image6]: ./Newsigns/speed_limit_100.jpg
[image7]: ./Newsigns/speed_limit_120.jpg
[image8]: ./Newsigns/stop.jpg
[image9]: ./Newsigns/trafic_lights.jpg


The first image might be difficult to classify because there is "watermark" in the image..

#### 2. Discuss the model's predictions on these new traffic signs and compare the results to predicting on the test set. At a minimum, discuss what the predictions were, the accuracy on these new predictions, and compare the accuracy to the accuracy on the test set (OPTIONAL: Discuss the results in more detail as described in the "Stand Out Suggestions" part of the rubric).

Here are the results of the prediction:

| Image			        |     Prediction	        					| 
|:---------------------:|:---------------------------------------------:| 
| Pedestrians      		| Pedestrians  									| 
| 100 km/h     			| 30 km/h 										|
| 120 km/h				| 20 km/h										|
| Stop	     	 		| Stop							 				|
| Trafic lights			| General caution      							|


The model was able to correctly guess 2 of the 5 traffic signs, which gives an accuracy of 40%. This is much lower than the accuracy on test dataset.

#### 3. Describe how certain the model is when predicting on each of the five new images by looking at the softmax probabilities for each prediction. Provide the top 5 softmax probabilities for each image along with the sign type of each probability. (OPTIONAL: as described in the "Stand Out Suggestions" part of the rubric, visualizations can also be provided such as bar charts)

The code for making predictions on my final model is located in the th cell of the Ipython notebook.

For the first image, the model is relatively sure that this is a Pedestrians sign (probability of 0.77), and the image does contain a Pedestrians sign. The top soft max probabilities were

| Probability         	|     Prediction	        					| 
|:---------------------:|:---------------------------------------------:| 
| .77         			| Pedestrians    								| 
| .23     				| Road narrows on the right 					|
<0.001 for the rest


For the second image, the model is very sure that this is a 30 km/h sign (probability of 0.99), but the image does contain a 100 km/h sign. The top soft max probabilities were

| Probability         	|     Prediction	        					| 
|:---------------------:|:---------------------------------------------:| 
| .99         			| 30 km/h    									| 
| 0.005     			| 80 km/h 										|
| <0.001     			| 100 km/h 										|
<0.001 for the rest


For the third image, the model is relatively sure that this is a 20 km/h sign (probability of 0.60), but the image does contain a 120 km/h sign. The top soft max probabilities were 

| Probability         	|     Prediction	        					| 
|:---------------------:|:---------------------------------------------:| 
| .60         			| 20 km/h    									| 
| .31     				| 30 km/h 										|
| .06     				| 80 km/h 										|
| .02     				| 120 km/h 										|
| .01     				| 70 km/h 										|


For the 4th image, the model is very sure that this is a stop sign (probability of 0.95), and the image does contain a stop sign. The top soft max probabilities were 

| Probability         	|     Prediction	        					| 
|:---------------------:|:---------------------------------------------:| 
| .95         			| stop    										| 
| .03     				| 120 km/h 										|
| .02     				| 60 km/h 										|
<0.001 for the rest

For the 5th image, the model is very sure that this is a general caution sign (probability of 0.99), but the image does contain a trafic lights sign. The top soft max probabilities were 

| Probability         	|     Prediction	        					| 
|:---------------------:|:---------------------------------------------:| 
| .99         			| general caution   							| 
| .01     				| trafic lights     							|
<0.001 for the rest

### (Optional) Visualizing the Neural Network (See Step 4 of the Ipython notebook for more details)
#### 1. Discuss the visual output of your trained network's feature maps. What characteristics did the neural network use to make classifications?
will try again for this for another training (it is quite time-consuming for the training of the network :). 

did not output the intermediate layers when defining the network. It should work once runing the given function 'outputFeatureMap' and the following tf session, as in the last 2 cells in python notebook. 

This might also help on further improvement of the model, and the accuracy on both test dataset and the 5 images that are randomly chosen from the internet.
