# **Finding Lane Lines on the Road** 

## Writeup Template

### You can use this file as a template for your writeup if you want to submit it as a markdown file. But feel free to use some other method and submit a pdf if you prefer.

---

**Finding Lane Lines on the Road**

The goals / steps of this project are the following:
* Make a pipeline that finds lane lines on the road
* Reflect on your work in a written report


### Reflection

### 1. Describe your pipeline. As part of the description, explain how you modified the draw_lines() function.

My pipeline consisted of 6 steps. First, I converted the images to grayscale, then I apply Gaussian smoothing on the grayed image to reduce the noise. The image is then passed into "Canny edge detection" and the upper and lower threshold are obtained via tries and errors. Then a polygon is identified in the figure to select the lines within the region of interest, and transformed via Hough transformation to detect the line segments, where the transformation parameters are also tried out. Finally, the detected lines are drawn on top of the original image.

In order to draw a single line on the left and right lanes, I modified the draw_lines() function by separating the line segments into two groups: left line and right line, based on the slope of the line segments. Then the average starting point and ending point within each group is taken in order to find the averaged slope for left line and right line, respectively. In the end, some calculations are conducted to derive the coordinates of starting point and ending point for averaged left line and right line, if both lines are extended to start from the boundary of the original image and end with the boundary of the region of interest. 

After trying on the challenge video, it is found that a lot of "fake" line segments will be identified at the bottom of the image (it looks like that the camara is blocked a little bit so the closest lines etc. could not be captured.) Therefore, a further selection of the original line segments are conducted based on the slope (those fake line segments have a slope quite close to zero/horizontal), before the averaging is conducted.

If you'd like to include images to show how the pipeline works, here is how to include an image: 


### 2. Identify potential shortcomings with your current pipeline

One potential shortcoming is that the transformation parameters of hough transformation is fixed, which might affect the detection of line segments when the images change substantially. E.g. the distance between dashed lines, the shape of the line (from straight to curve or vice versa). 

Another shortcomming might come with the change of the image size, which indicates that the vertices of region of interest might change. Although we could use the relative coordinates of the vertices (e.g. y coordinate is set as 0.6 times the size of the image in y-axis), it would definitely not fit all cases.

In addition, the problem could also come with the circled/curved lines. Although the function cv.HoughCircles() was tried on challenge video, it did not help in the end. However, when it comes to the time when this function is needed, further trials of parameters needs to be conducted.

Last but not least, it might not work well when there is a change of overall intensity of the image, e.g. the sunshine is blocked by the tree at certain time in the challenge video; further work is needed in order to improve the detection then.

So it seems that the pipeline works mostly OK for test videos and most of the challenge video, but does not work well at certain frames of the challenge video.


### 3. Suggest possible improvements to your pipeline

A possible improvement would be to include function cv.HoughCircles() to detect circles.

Another one might be the averaging of the image frames in the video, in order to overcome the change of the intensity among frames.

Finally, it might predict the next frame based on the current frame and several previous frames, with machine learning approach.
