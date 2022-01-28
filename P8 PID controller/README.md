# CarND-Controls-PID
Self-Driving Car Engineer Nanodegree Program


This project implements PID controllers to make the car stay on the track. Two PID controllers are employed, where the first one is to control the steering angle and the second one is to control the throttle. Twiddle algorithm is also used to tune the parameters.

## PID controller
PID controller is a kind of control loop feedback mechanisms. It calculates an error (which is the cross track error, cte, in this project) and applies the correction based on the error. A PID controller consists of three terms:

P(proportional): P linnearly relates how large the change in the output to a given change in the error, based on P coefficient. If P coefficient is too large, the system will become unstable.

I(integral): I links both the magnitude and accumulation of the error to the output. It helps to deal with the systematic bias (i.e, if the system is away from the desired point for a long time, the accumulation of the error will be large and the system will be compensated by I coefficient).

D(differential): D links how much the rate of change in the error to the output. It helps to smooth the reponse, i.e. if the error is becoming smaller, D will help to prevent the overshoot and stablize the system.

In short:
control (steering angle in this project) = -tau_p * cte - tau_d * diff_cte - tau_i * int_cte

The effect of PID could be seen from this plot:
[image1]: ./image/PID.png  

## Parameters of PID controller for  steering angle

Both manual tuning and twiddle algorithm are applied here. 

Manual tuning is to preset the parameters and see the Car performance.

Twiddle algorithm is a kind of iterative methods to find out the optimum parameters. In short words, it starts with the initializing parameters and iteratively update the parameters by certain margin to check the accumulated error after the parameters are updated. If the accumulated error is less than the best error, this set of parameters will be adopted where the margin will be increased; otherwise the margin will be reduced for the parameters to iterate again. Once the tolerance is reached (here is defined as if the sum of the margin for all parameters is less than a predefined value), the parameters will not be updated any longer.

Manual tuning also helps here for initializing the parameters for Twiddle algorithm. It is found that if I is initialized with a large value, the car will oscillate a lot or even leave the track because the parameters are not closed to the optimum values yet. Another important parameter here is the time period that how long the error is accumulated. Currently it is set as 20 steps in case the car will leave the track before the optimum parameters are found; probably manual tuning could be applied first to make sure that the car will stay in the track for at least one lap, then the accumulated error is calculated within the lap; and several laps are needed in order to find the best parameters; this might nee further investigation.


## Parameters of PID controller for throttle
Manual tuning is applied here. The main purpose is to adjust the throttle such that the throttle should be smaller given larger error (or even slow down when the error is high enough; this could be seen when the car is about to leave the track at the turnings).

An example image when car is slowing down (break light is on):
[image2]: ./image/break.png

---

## Dependencies

* cmake >= 3.5
 * All OSes: [click here for installation instructions](https://cmake.org/install/)
* make >= 4.1(mac, linux), 3.81(Windows)
  * Linux: make is installed by default on most Linux distros
  * Mac: [install Xcode command line tools to get make](https://developer.apple.com/xcode/features/)
  * Windows: [Click here for installation instructions](http://gnuwin32.sourceforge.net/packages/make.htm)
* gcc/g++ >= 5.4
  * Linux: gcc / g++ is installed by default on most Linux distros
  * Mac: same deal as make - [install Xcode command line tools]((https://developer.apple.com/xcode/features/)
  * Windows: recommend using [MinGW](http://www.mingw.org/)
* [uWebSockets](https://github.com/uWebSockets/uWebSockets)
  * Run either `./install-mac.sh` or `./install-ubuntu.sh`.
  * If you install from source, checkout to commit `e94b6e1`, i.e.
    ```
    git clone https://github.com/uWebSockets/uWebSockets 
    cd uWebSockets
    git checkout e94b6e1
    ```
    Some function signatures have changed in v0.14.x. See [this PR](https://github.com/udacity/CarND-MPC-Project/pull/3) for more details.
* Simulator. You can download these from the [project intro page](https://github.com/udacity/self-driving-car-sim/releases) in the classroom.

Fellow students have put together a guide to Windows set-up for the project [here](https://s3-us-west-1.amazonaws.com/udacity-selfdrivingcar/files/Kidnapped_Vehicle_Windows_Setup.pdf) if the environment you have set up for the Sensor Fusion projects does not work for this project. There's also an experimental patch for windows in this [PR](https://github.com/udacity/CarND-PID-Control-Project/pull/3).

## Basic Build Instructions

1. Clone this repo.
2. Make a build directory: `mkdir build && cd build`
3. Compile: `cmake .. && make`
4. Run it: `./pid`. 

Tips for setting up your environment can be found [here](https://classroom.udacity.com/nanodegrees/nd013/parts/40f38239-66b6-46ec-ae68-03afd8a601c8/modules/0949fca6-b379-42af-a919-ee50aa304e6a/lessons/f758c44c-5e40-4e01-93b5-1a82aa4e044f/concepts/23d376c7-0195-4276-bdf0-e02f1f3c665d)

## Editor Settings

We've purposefully kept editor configuration files out of this repo in order to
keep it as simple and environment agnostic as possible. However, we recommend
using the following settings:

* indent using spaces
* set tab width to 2 spaces (keeps the matrices in source code aligned)

## Code Style

Please (do your best to) stick to [Google's C++ style guide](https://google.github.io/styleguide/cppguide.html).

## Project Instructions and Rubric

Note: regardless of the changes you make, your project must be buildable using
cmake and make!

More information is only accessible by people who are already enrolled in Term 2
of CarND. If you are enrolled, see [the project page](https://classroom.udacity.com/nanodegrees/nd013/parts/40f38239-66b6-46ec-ae68-03afd8a601c8/modules/f1820894-8322-4bb3-81aa-b26b3c6dcbaf/lessons/e8235395-22dd-4b87-88e0-d108c5e5bbf4/concepts/6a4d8d42-6a04-4aa6-b284-1697c0fd6562)
for instructions and the project rubric.

## Hints!

* You don't have to follow this directory structure, but if you do, your work
  will span all of the .cpp files here. Keep an eye out for TODOs.

## Call for IDE Profiles Pull Requests

Help your fellow students!

We decided to create Makefiles with cmake to keep this project as platform
agnostic as possible. Similarly, we omitted IDE profiles in order to we ensure
that students don't feel pressured to use one IDE or another.

However! I'd love to help people get up and running with their IDEs of choice.
If you've created a profile for an IDE that you think other students would
appreciate, we'd love to have you add the requisite profile files and
instructions to ide_profiles/. For example if you wanted to add a VS Code
profile, you'd add:

* /ide_profiles/vscode/.vscode
* /ide_profiles/vscode/README.md

The README should explain what the profile does, how to take advantage of it,
and how to install it.

Frankly, I've never been involved in a project with multiple IDE profiles
before. I believe the best way to handle this would be to keep them out of the
repo root to avoid clutter. My expectation is that most profiles will include
instructions to copy files to a new location to get picked up by the IDE, but
that's just a guess.

One last note here: regardless of the IDE used, every submitted project must
still be compilable with cmake and make./

## How to write a README
A well written README file can enhance your project and portfolio.  Develop your abilities to create professional README files by completing [this free course](https://www.udacity.com/course/writing-readmes--ud777).

