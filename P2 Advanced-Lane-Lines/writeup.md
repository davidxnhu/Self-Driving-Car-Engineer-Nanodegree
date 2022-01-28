## Writeup Template

### You can use this file as a template for your writeup if you want to submit it as a markdown file, but feel free to use some other method and submit a pdf if you prefer.

---

**Advanced Lane Finding Project**

The goals / steps of this project are the following:

* Compute the camera calibration matrix and distortion coefficients given a set of chessboard images.
* Apply a distortion correction to raw images.
* Use color transforms, gradients, etc., to create a thresholded binary image.
* Apply a perspective transform to rectify binary image ("birds-eye view").
* Detect lane pixels and fit to find the lane boundary.
* Determine the curvature of the lane and vehicle position with respect to center.
* Warp the detected lane boundaries back onto the original image.
* Output visual display of the lane boundaries and numerical estimation of lane curvature and vehicle position.

[//]: # (Image References)

[image1]: ./examples/undistort_output.png "Undistorted"
[image2]: ./test_images/test1.jpg "Road Transformed"
[image3]: ./examples/binary_combo_example.jpg "Binary Example"
[image4]: ./examples/warped_straight_lines.jpg "Warp Example"
[image5]: ./examples/color_fit_lines.jpg "Fit Visual"
[image6]: ./examples/example_output.jpg "Output"
[video1]: ./project_video.mp4 "Video"

## [Rubric](https://review.udacity.com/#!/rubrics/571/view) Points

### Here I will consider the rubric points individually and describe how I addressed each point in my implementation.  

---

### Writeup / README


### Camera Calibration

#### 1. Briefly state how you computed the camera matrix and distortion coefficients. Provide an example of a distortion corrected calibration image.

The code for this step is contained in cell 4 of the IPython notebook located in "./p2.ipynb" 

The calibration images are read from "./camera_cal/calibration*.jpg". I start by preparing "object points", which will be the (x, y, z) coordinates of the chessboard corners in the world. Here I am assuming the chessboard is fixed on the (x, y) plane at z=0, such that the object points are the same for each calibration image.  Thus, `objp` is just a replicated array of coordinates, and `objpoints` will be appended with a copy of it every time I successfully detect all chessboard corners in a test image.  `imgpoints` will be appended with the (x, y) pixel position of each of the corners in the image plane with each successful chessboard detection.  

I then used the output `objpoints` and `imgpoints` to compute the camera calibration and distortion coefficients using the `cv2.calibrateCamera()` function.  

I applied this distortion correction to the test image using the `cv2.undistort()` function and obtained this result (undistored "calibration1.jpg"), which is included in cell 5.

[image1]: ./output_images/distorted_free_calibration1.jpg  




### Pipeline (single images)

#### 1. Provide an example of a distortion-corrected image.


Similar to previous point, all the testing images are loaded by "./test_images/{}".format(filename) for filename in os.listdir("./test_images/")

The distortion coefficients, etc. obtained in camera calibration step are applied to these testing images to get distorted-free image.

An example:
[image2]: ./output_images/distorted_free_test1.png 

Also in "./p2.ipynb" cell 13, each test image generates 4 subplots: subplot(221) is distorted-free image (will explain others later).

#### 2. Describe how (and identify where in your code) you used color transforms, gradients or other methods to create a thresholded binary image.  Provide an example of a binary image result.

Included under `binary_warper()` function in "./p2.ipynb", at cell 8. 

With the trials on challenge_video, color transforms include: filtering for yellow and white RGB ranges, color to gray, color to RGB, HLS and HSV space.

Gradients are tested on the derivitive in x direction for both grayed image, and individual R/G/B image (but not applied in the final pipeline function). Threshold is applied to pick the strength of the gradients within the threshold.

Threshold are also applied on S channel in HLS space (also tried on HSV space but not applied in the end) to pick "S" within the range of the threshold.

Filtered "Yellow" and "White" images are also converted into binary values by dividing 255 to make it in between 0 and 1, and additional threshold is applied to convert to binary images.

Last but not least, a mask is applied to only keep pixels within the region of interest. This is not relavent to color/gradient transformations but is included in the example result and the final pipeline.

The resultant binay image combines the binary result from above, i.e. as long as it meets one of the above criteria it is "1" for the pixel; otherwise it is "0".


An example of a binary image result is as included in "./p2.ipynb" for "./test_images/test1.jpg"
[image3]: ./output_images/binary_image.png



#### 3. Describe how (and identify where in your code) you performed a perspective transform and provide an example of a transformed image.

The code for my perspective transform includes a function called `warper()`, which appears in cell 6 in the "./p2.ipynb" notebook.  The `warper()` function takes as inputs an image (`img`), and include source (`src`) and destination (`dst`) points.  I chose the source and destination points by eye-check and also check the transformation of undistorted test_image "straight_lines2.jpg":


This resulted in the following source and destination points:

| Source        | Destination   | 
|:-------------:|:-------------:| 
| 250, 700      | 300, 720      | (img.shape[0] = 720) 
| 430, 570      | 300, 620      |
| 870, 570      | 900, 620      |
| 1065, 700     | 900, 720      | (img.shape[0] = 720)

I verified that my perspective transform was working as expected by drawing the `src` and `dst` points onto a test image and its warped counterpart to verify that the lines appear parallel in the warped image. The comparison is included in cell 11 "Test perspective transformation" in "./p2.ipynb"

[image4]: ./output_images/straight_line2_warped.jpg


#### 4. Describe how (and identify where in your code) you identified lane-line pixels and fit their positions with a polynomial?

Part of the process is included under "Helper functions" named `find_lane_pixels()` function, in cell 6. The function starts from identifying "nonzero" pixels from the warped binary image, and summation of pixels over the "y" axis is applied for the lower half of the image. The histogram plot is conducted and the indice are identified for the maximum of the histogram plot for the left and right of the x-axis, respectively. These indice serve as the starting point for the next step, for the left and right line respectively.

A sliding window approach is applied along "y" axis starting from the bottom of the image to the top. All the windows have the same height in "y" axis. Within each window, the range for x-axis is within +/- margin of the starting x coordinate. For the first window, the starting x coordinate is obtained in the previous step, and only the "nonzero" pixels within this window are kept. If the number of maintained pixels exceed a minimum value, the starting x coordinate in the next window will be updated as the average x coordinate of the maintained pixels in this window. Following this approach, maintained "nonzero" pixels are all identified.

The remaining process is included under `pipeline()` function, in cell 12. Following above, a 2nd order polynomial is fit for left and right lines, respectively. And the fitted 2nd order polynomial is extrapolated with the fitting coefficients.

 
[image5]: ./output_images/pipeline_test1.jpg As an example, the identified lane pixels are as shown in the top-right figure, and the fitted polynomials are the yellow lines as shown in the bottom-left figure.

#### 5. Describe how (and identify where in your code) you calculated the radius of curvature of the lane and the position of the vehicle with respect to center.

It is included under "Helper functions" named `conversion_to_world_space()`, in cell 6. 

R = (1+(2*A*y+B)**2)**(3/2)/(abs(2*A)) (the formula is given in the class material, where A and B are second and first order fitting coefficients for the polynomial, respecitively.)
 
With the approximation that 
    ym_per_pix = 30/720 # meters per pixel in y dimension
    xm_per_pix = 3.7/700 # meters per pixel in x dimension
    
All fitting coefficient and the distance could be converted into the world space, and the radius of curvature will be given in meters, too.

The x coordinate for both fitted left and right lines at the bottom of the image could be evaluated. Assuming that the vehicle will always be at the center of the image, the middle of the above two x coordinates with respect to the center of the image will be the shift of the car.

For example, if the middle of two x coordinates is at 650 pixels, and the center of the image is at 600 pixels, then the car is 50 pixels left to the center of the line, which will also be converted into the world space.



#### 6. Provide an example image of your result plotted back down onto the road such that the lane area is identified clearly.

It is included in the function `unwarper()`, in cell 6.  The source and destination points are the opposite as in the perspective transformation. 

Here is an example of my result on a test image (and all './output_images/test*.jpg' are the results corresponding to the testing images): 

[image6]:./output_images/pipeline_test1.jpg

[image7]:./output_images/test2.jpg


### Pipeline (video)

#### 1. Provide a link to your final video output.  Your pipeline should perform reasonably well on the entire project video (wobbly lines are ok but no catastrophic failures that would cause the car to drive off the road!).

The same pipeline function is applied to the first frame of the video, but for the subsequent frames, an alternative way applied here is to identify the "nonzero" pixels within certain margin of the fitted lines in the previous frame, as included in the function `search_around_poly()`, in cell 15.  But again it depends on how to identify pixels in the warped image and that's why I call it "Key function".

Here's a [link to my video result](./output_videos/project_video.mp4)


Many trials are also tested on "challenge video", however it only works fine for 70%-80% of the frames.
Here's a [link to my video result](./output_videos/challenge_video_4.mp4)

The same pipeline is also tested on "harder challenge video" to get a sense of where the problem is.
Here's a [link to my video result](./output_videos/harder_challenge_video.mp4)

---

### Discussion

#### 1. Briefly discuss any problems / issues you faced in your implementation of this project.  Where will your pipeline likely fail?  What could you do to make it more robust?

It mostly works fine for project_video, but does not perform very well on challenge video, and even poorer in harder challenge video.

By investigating frames by frames, possible situations that could lead to a failure of identifying lane pixels are:
1. too dark (e.g. when the vehicle passes underneath the bridge in challenge video, lane pixels are hardly identified)

2. Another clear line which makes pipeline "confused" (e.g. the last few seconds in challenge video, between the yellow line and white line, there is a sudden change on the color of the road in between which makes it identified)

3. Big curvature (e.g. in harder challenge video, the sliding window fails to follow the curvature; I did not proceed further on this since I might need to completely solve challenge video first.)

4. Threshold of color channels. E.g. inappropriate values will include roads under bright conditions , and the dark "walls" on the left as in the challenge video. When too many pixels are "selected", the histogram might pick up a wrong starting x coordinate for the fitting.
