# **Behavioral Cloning** 

## Writeup Template

### You can use this file as a template for your writeup if you want to submit it as a markdown file, but feel free to use some other method and submit a pdf if you prefer.

---

**Behavioral Cloning Project**

The goals / steps of this project are the following:
* Use the simulator to collect data of good driving behavior
* Build, a convolution neural network in Keras that predicts steering angles from images
* Train and validate the model with a training and validation set
* Test that the model successfully drives around track one without leaving the road
* Summarize the results with a written report

## Rubric Points
### Here I will consider the [rubric points](https://review.udacity.com/#!/rubrics/432/view) individually and describe how I addressed each point in my implementation.  

---
### Files Submitted & Code Quality

#### 1. Submission includes all required files and can be used to run the simulator in autonomous mode

My project includes the following files:
* model.py containing the script to create and train the model
* drive.py for driving the car in autonomous mode
* model.h5 containing a trained convolution neural network 
* writeup_report.md summarizing the results
* video.mp4 A video recording of the vehicle driving autonomously at least one lap around the track

#### 2. Submission includes functional code
Using the Udacity provided simulator and my drive.py file, the car can be driven autonomously around the track by executing 
```sh
python drive.py model.h5
```

#### 3. Submission code is usable and readable

The model.py file contains the code for training and saving the convolution neural network. The file shows the pipeline I used for training and validating the model, and it contains comments to explain how the code works.

### Model Architecture and Training Strategy

#### 1. An appropriate model architecture has been employed

My model refers to the model developed by NVIDIA for their automouns driving (model.py lines 45-57) 

The model starts from the normalization of input data (Keras lambda layer, model.py line 46 ), followed by 5 convolutional layers (layer1: 5x5 filter size, 2x2 stride, depth:24; layer2: 5x5 filter size, 2x2 stride, depth:36; layer3: 5x5 filter size, 2x2 stride, depth:48; layer4: 3x3 filter size, depth:64; layer5: 3x3 filter size, depth:64), flattened, and followed by 4 fully-connected layers (layer1: output size 100; layer2: output size 50; layer3: output size 10; layer 4: output steering angle)

All convolution layers includes RELU layers to introduce nonlinearity (model.py lines 48-52) 

#### 2. Attempts to reduce overfitting in the model

The model in NVIDIA does not include any dropout layer. 

However, the model was trained and validated to ensure that the model was not overfitting by observing the validation loss after each epoch. The model was tested by running it through the simulator and ensuring that the vehicle could stay on the track.

#### 3. Model parameter tuning

The model used an adam optimizer, so the learning rate was not tuned manually (model.py line 59).

#### 4. Appropriate training data

Training data was chosen to keep the vehicle driving on the road. I used a combination of center lane driving, recovering from the left and right sides of the road, and some additional training drive which focuses on driving smoothly around curves. 

For details about how I created the training data, see the next section. 

### Model Architecture and Training Strategy

#### 1. Solution Design Approach

My first step was to use a convolution neural network model developed by NVIDIA. I thought this model might be appropriate because it has been proved to a powerful model to drive autonomously.

In order to gauge how well the model was working, I split my image and steering angle data into a training and validation set, on the original sample data provided by Udacity. I found that my first model had a low mean squared error on the training set but a high mean squared error on the validation set, and the error on validation set continues to increase with more training epoches. This implied that the model was overfitting, which could also be verified when the car was tested on automonous mode (hitting the wall and failed to stay in the center of the track; this video is not included in the submission). 

To combat the overfitting, I modified the model so that it was training with less number of epoches.

Then I also trained my own driving data, which includes 2 laps of normal driving along the center of the track (one in clockwise direction, and the other in counter-clockwise direction), some recovery driving attempts from the sides to the center of the track by only recording the recovery processes, and some further turning focusing on driving smoothly around curves.

The final step was to run the simulator to see how well the car was driving around track one. The car could stay on the track all the time but it seems that the car may not drive very smoothly for some of the curves. To improve the driving behavior in these cases, the better training on the curves are needed. (I also notice in the end that there exist 3 cameras to collect the training data: left, center and right; it is possible that the left and right camaera could help correct the steering angle in the curves with additional manual adjustment, as shown in section 13: using multiple cameras.)

#### 2. Final Model Architecture

The model starts from the normalization of input data (Keras lambda layer, model.py line 46), followed by 5 convolutional layers (layer1: 5x5 filter size, 2x2 stride, depth:24; layer2: 5x5 filter size, 2x2 stride, depth:36; layer3: 5x5 filter size, 2x2 stride, depth:48; layer4: 3x3 filter size, depth:64; layer5: 3x3 filter size, depth:64), flattened, and followed by 4 fully-connected layers (layer1: output size 100; layer2: output size 50; layer3: output size 10; layer 4: output steering angle)

All convolution layers includes RELU layers to introduce nonlinearity (model.py lines 48-52)

Here is a visualization of the architecture (note: visualizing the architecture is optional according to the project rubric)

[image1]: ./IMG/cnn-architecture.png "network architecture by NVIDIA"

#### 3. Creation of the Training Set & Training Process

To capture good driving behavior, I first recorded two laps on track one using center lane driving. Here is an example image of center lane driving:

[image2]: ./IMG/center.jpg "driving in the center"

I then recorded the vehicle recovering from the left side and right sides of the road back to center so that the vehicle would learn to recover when it is on the side of the track. These images show what a recovery looks like starting from the side to the center :

[image3]: ./IMG/recover_from_right_1.jpg
[image4]: ./IMG/recover_from_right_2.jpg
[image5]: ./IMG/recover_from_right_3.jpg


To augment the data sat, I also flipped images and angles thinking that this would further enhance the training. For example, here is an image that has then been flipped:

[image6]: /IMG/original.jpg
[image7]: /IMG/flipped.jpg


After the collection process, I had 14500 data points. I then preprocessed this data by normalizing the data into a range of 0 to 1, and shift by -0.5 to make them from -0.5 to 0.5 so they have a centered mean at zero, through Keras lambda layer.


I finally randomly shuffled the data set and put 20% of the data into a validation set. 

I used this training data for training the model. The validation set helped determine if the model was over or under fitting. The ideal number of epochs was 6 as evidenced by the minimum validation loss. I used an adam optimizer so that manually training the learning rate wasn't necessary.
