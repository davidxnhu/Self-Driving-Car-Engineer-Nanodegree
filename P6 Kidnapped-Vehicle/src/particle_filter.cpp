/**
 * particle_filter.cpp
 *
 * Created on: Dec 12, 2016
 * Author: Tiffany Huang
 */

#include "particle_filter.h"

#include <math.h>
#include <algorithm>
#include <iostream>
#include <iterator>
#include <numeric>
#include <random>
#include <string>
#include <vector>

#include "helper_functions.h"

using std::string;
using std::vector;
using std::normal_distribution;
using std::default_random_engine;

void ParticleFilter::init(double x, double y, double theta, double std[]) {
  /**
   * TODO: Set the number of particles. Initialize all particles to 
   *   first position (based on estimates of x, y, theta and their uncertainties
   *   from GPS) and all weights to 1. 
   * TODO: Add random Gaussian noise to each particle.
   * NOTE: Consult particle_filter.h for more information about this method 
   *   (and others in this file).
   */
  num_particles = 500;  // TODO: Set the number of particles

  double std_x = std[0];
  double std_y = std[1];
  double std_theta = std[2];

  normal_distribution<double> dist_x(x, std_x);
  normal_distribution<double> dist_y(y, std_y);
  normal_distribution<double> dist_theta(theta, std_theta);

  default_random_engine gen;

  for (int i=0; i<num_particles; ++i){
    Particle p;
    p.x =  dist_x(gen);     
    p.y =  dist_y(gen);
    p.theta = dist_theta(gen); 
    p.weight = 1.0;
    p.id = i;
    particles.push_back(p);
  }

  is_initialized = true;
  
}

void ParticleFilter::prediction(double delta_t, double std_pos[], 
                                double velocity, double yaw_rate) {
  /**
   * TODO: Add measurements to each particle and add random Gaussian noise.
   * NOTE: When adding noise you may find std::normal_distribution 
   *   and std::default_random_engine useful.
   *  http://en.cppreference.com/w/cpp/numeric/random/normal_distribution
   *  http://www.cplusplus.com/reference/random/default_random_engine/
   */
  double std_x = std_pos[0];
  double std_y = std_pos[1];
  double std_theta = std_pos[2];

  normal_distribution<double> dist_x(0, std_x);
  normal_distribution<double> dist_y(0, std_y);
  normal_distribution<double> dist_theta(0, std_theta);

  default_random_engine gen;

  for (int i=0; i<num_particles; ++i){
      double x = particles[i].x;
      double y = particles[i].y;
      double theta = particles[i].theta;

      if (fabs(yaw_rate)>0.000001){
        x += velocity/yaw_rate *(sin(theta+yaw_rate*delta_t)-sin(theta)) + dist_x(gen);
        y += velocity/yaw_rate *(cos(theta)-cos(theta+yaw_rate*delta_t)) + dist_y(gen);
        theta += yaw_rate*delta_t + dist_theta(gen);
      }else {
        x += velocity * delta_t * cos(theta) + dist_x(gen);
        y += velocity * delta_t * sin(theta) + dist_y(gen);
        theta += dist_theta(gen);
      }

     particles[i].x = x;
     particles[i].y = y;
     particles[i].theta = theta;
  }    
}

void ParticleFilter::dataAssociation(vector<LandmarkObs> predicted, 
                                     vector<LandmarkObs>& observations) {
  /**
   * TODO: Find the predicted measurement that is closest to each 
   *   observed measurement and assign the observed measurement to this 
   *   particular landmark.
   * NOTE: this method will NOT be called by the grading code. But you will 
   *   probably find it useful to implement this method and use it as a helper 
   *   during the updateWeights phase.
   */
   for (unsigned int i=0; i<observations.size();++i){
     //observation_id = observations[i].id;
     double distMin = std::numeric_limits<double>::max();
     double obs_x = observations[i].x;
     double obs_y = observations[i].y;
     int idMin;

     for (unsigned int j=0; j<predicted.size();++j){
        int landmark_id = predicted[j].id;
        double x = predicted[j].x;
        double y = predicted[j].y;
        //calling dist function from helper_functions.h
        double cal_dist = dist(obs_x, obs_y, x, y);
        if (cal_dist<distMin){
           distMin = cal_dist;
           idMin = landmark_id;
        }
     }
     observations[i].id = idMin;
  }  
}

void ParticleFilter::updateWeights(double sensor_range, double std_landmark[], 
                                   const vector<LandmarkObs> &observations, 
                                   const Map &map_landmarks) {
  /**
   * TODO: Update the weights of each particle using a mult-variate Gaussian 
   *   distribution. You can read more about this distribution here: 
   *   https://en.wikipedia.org/wiki/Multivariate_normal_distribution
   * NOTE: The observations are given in the VEHICLE'S coordinate system. 
   *   Your particles are located according to the MAP'S coordinate system. 
   *   You will need to transform between the two systems. Keep in mind that
   *   this transformation requires both rotation AND translation (but no scaling).
   *   The following is a good resource for the theory:
   *   https://www.willamette.edu/~gorr/classes/GeneralGraphics/Transforms/transforms2d.htm
   *   and the following is a good resource for the actual equation to implement
   *   (look at equation 3.33) http://planning.cs.uiuc.edu/node99.html
   */
   
   double sig_x = std_landmark[0];
   double sig_y = std_landmark[1];

   for (int i=0; i<num_particles; ++i){
     double x_part = particles[i].x;
     double y_part = particles[i].y;
     double theta = particles[i].theta;
    
     vector<LandmarkObs> in_range_landmarks;
     vector<LandmarkObs> converted_observations;
     
     for (unsigned int j=0; j<map_landmarks.landmark_list.size(); ++j){
	double x = map_landmarks.landmark_list[j].x_f;
        double y = map_landmarks.landmark_list[j].y_f;
        int landmark_id = map_landmarks.landmark_list[j].id_i;

        double dist_cal = dist(x_part, y_part, x, y);
        if (dist_cal <= sensor_range){
          in_range_landmarks.push_back(LandmarkObs{landmark_id, x ,y});
        }
     }

     for (unsigned int j=0; j<observations.size(); ++j){
        double x_obs = observations[j].x;
        double y_obs = observations[j].y;
        int landmark_id = observations[j].id;
        
        //convert from vecihle's coor.system to map system
  	double x_map;
  	x_map = x_part + (cos(theta) * x_obs) - (sin(theta) * y_obs);        

  	double y_map;
  	y_map = y_part + (sin(theta) * x_obs) + (cos(theta) * y_obs);        

        converted_observations.push_back(LandmarkObs{landmark_id, x_map, y_map});
     }

     // associate the observations to the landmarks 
     dataAssociation(in_range_landmarks, converted_observations);

     double prob = 1.0;
     // update weight
     for (unsigned int j=0; j<=converted_observations.size(); ++j){
	//obtain landmark's position
        int landmark_id = converted_observations[j].id;
        //double mu_x = in_range_landmarks[landmark_id].x;
        //double mu_y = in_range_landmarks[landmark_id].y;
        double x_obs = converted_observations[j].x;
        double y_obs = converted_observations[j].y; 
        
        unsigned int k = 0;
        bool found = false;
        double mu_x, mu_y;
        while(!found && k < in_range_landmarks.size()) {
          if (in_range_landmarks[k].id == landmark_id) {
          found = true;
          mu_x = in_range_landmarks[k].x;
          mu_y = in_range_landmarks[k].y;
          }
          k++;
       }

        //calculate multivariable-gaussian prob.
        double gauss_norm;
        gauss_norm = 1 / (2 * M_PI * sig_x * sig_y);
  
	// calculate exponent
  	double exponent;
  	exponent = (pow(x_obs - mu_x, 2) / (2 * pow(sig_x, 2)))
               + (pow(y_obs - mu_y, 2) / (2 * pow(sig_y, 2)));
    
  	// calculate weight using normalization terms and exponent
  	double weight;
  	weight = gauss_norm * exp(-exponent);
    
        if (weight>0.000001)
           prob *= weight;
        else
           prob *= 0.000001;
     }

     //weights[i] = prob;
     particles[i].weight = prob;
   }
}   
 

void ParticleFilter::resample() {
  /**
   * TODO: Resample particles with replacement with probability proportional 
   *   to their weight. 
   * NOTE: You may find std::discrete_distribution helpful here.
   *   http://en.cppreference.com/w/cpp/numeric/random/discrete_distribution
   */
   vector<Particle> newparticles;
   vector<double> weights;
   double max_w = 0;
   for (int i=0; i<num_particles; ++i){
      weights.push_back(particles[i].weight);
      if (particles[i].weight>max_w){
         max_w = particles[i].weight;
      }
   }

  int index = rand() % particles.size();
  

   double beta = 0.0;
   for (int i=0; i<num_particles; ++i){
      beta += (double)rand()/(double)(RAND_MAX/(2.0*max_w));
      while (beta>weights[index]){
          beta -= weights[index];
          index = (index+1)%num_particles;
      }
      newparticles.push_back(particles[index]); 
   }

   particles = newparticles;
}

void ParticleFilter::SetAssociations(Particle& particle, 
                                     const vector<int>& associations, 
                                     const vector<double>& sense_x, 
                                     const vector<double>& sense_y) {
  // particle: the particle to which assign each listed association, 
  //   and association's (x,y) world coordinates mapping
  // associations: The landmark id that goes along with each listed association
  // sense_x: the associations x mapping already converted to world coordinates
  // sense_y: the associations y mapping already converted to world coordinates
  particle.associations= associations;
  particle.sense_x = sense_x;
  particle.sense_y = sense_y;
}

string ParticleFilter::getAssociations(Particle best) {
  vector<int> v = best.associations;
  std::stringstream ss;
  copy(v.begin(), v.end(), std::ostream_iterator<int>(ss, " "));
  string s = ss.str();
  s = s.substr(0, s.length()-1);  // get rid of the trailing space
  return s;
}

string ParticleFilter::getSenseCoord(Particle best, string coord) {
  vector<double> v;

  if (coord == "X") {
    v = best.sense_x;
  } else {
    v = best.sense_y;
  }

  std::stringstream ss;
  copy(v.begin(), v.end(), std::ostream_iterator<float>(ss, " "));
  string s = ss.str();
  s = s.substr(0, s.length()-1);  // get rid of the trailing space
  return s;
}
