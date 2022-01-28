#ifndef VEHICLE_H
#define VEHICLE_H


#include <string>
#include <vector>

using std::map;
using std::string;
using std::vector;

// functions related to car behaviour planning
vector<string> successor_states(string &current_state, int &lane, int &lanes_available, vector<vector<double>> sensor_fusion, double &ref_vel, double car_s);
vector<double> choose_next_state(string &current_state, int &lane, int &lanes_available, double &ref_vel, vector<vector<double>> sensor_fusion, double car_s, double car_speed);
vector<double> keep_lane(int &lane, vector<vector<double>> sensor_fusion, double car_s, double &ref_vel);
vector<double> prepare_lane_change(string state, int &lane, vector<vector<double>> sensor_fusion, double car_s, double &ref_vel);
vector<double> lane_change(string state, int &lane, vector<vector<double>> sensor_fusion, double car_s, double &ref_vel);
vector<double> generate_action(string state, double &ref_vel, int &lane, vector<vector<double>> sensor_fusion, double car_s, double car_speed);
int whichLine(float d);
vector<double> get_car_front(vector<double> action, vector<vector<double>> sensor_fusion, double &ref_vel, double car_s, int &lane);
double calculate_cost(string state, vector<double> action, vector<vector<double>> sensor_fusion, double &ref_vel, double car_s, int &lane);


vector<string> successor_states(string &current_state, int &lane, int &lanes_available, vector<vector<double>> sensor_fusion, double &ref_vel, double car_s){
  vector<string> states;
  states.push_back("KL");
  string state = current_state;
 
  vector<double> results = get_car_front({0.0, 0.0},  sensor_fusion, ref_vel, car_s, lane);
  double min_dist_lane = results[1];
 
/*
  if(state.compare("KL") == 0 && min_dist_lane <=30) {
    states.push_back("PLCL");
    states.push_back("PLCR");
  } else if (state.compare("PLCR") == 0 && min_dist_lane <=30) {
    if (lane != lanes_available-1) {
      states.push_back("PLCR");
      states.push_back("LCR");
    }
  } else if (state.compare("PLCL") == 0 && min_dist_lane <=30) {
    if (lane != 0) {
      states.push_back("PLCL");
      states.push_back("LCL");
    }
  }
*/

  // only KL, LCL and LCR, KL could change to LCL and LCR, but LCL and LCR could only change back to KL    
  if (state.compare("KL")== 0 && min_dist_lane <=30){
   if (lane != 0){
     states.push_back("LCL");
   }

   if (lane != lanes_available-1){
     states.push_back("LCR");
   }
  } 
 
 if (lane ==0){
  vector<double> results = get_car_front({0,1},  sensor_fusion, ref_vel, car_s, lane);

  double min_dist_lane = results[0];
  double min_speed_lane = results[1];
  double min_dist_new_lane = results[2];
  double min_speed_new_lane = results[3];

  if (min_dist_lane>=50 && min_dist_new_lane >=50){
    states.push_back("LCR");
  }

 } 

  if (lane == lanes_available - 1){
  vector<double> results = get_car_front({0,-1},  sensor_fusion, ref_vel, car_s, lane);

  double min_dist_lane = results[0];
  double min_speed_lane = results[1];
  double min_dist_new_lane = results[2];
  double min_speed_new_lane = results[3];

  if (min_dist_lane>=50 && min_dist_new_lane >=50){
    states.push_back("LCL");
  }

 }

   return states;
}

vector<double> choose_next_state(string &current_state, int &lane, int &lanes_available, double &ref_vel, vector<vector<double>> sensor_fusion, double car_s, double car_speed){
   vector<string> states = successor_states(current_state, lane, lanes_available, sensor_fusion, ref_vel, car_s);

   double cost;
   vector<double> costs;
   vector<vector<double>> final_actions;   

   for (vector<string>::iterator it = states.begin(); it != states.end(); ++it){
       vector<double> action = generate_action(*it, ref_vel, lane, sensor_fusion, car_s, car_speed);
       if (action.size() != 0) {
          cost = calculate_cost(*it, action, sensor_fusion, ref_vel, car_s, lane);
          costs.push_back(cost);
          final_actions.push_back(action);
       }
   }

  vector<double>::iterator best_cost = min_element(begin(costs), end(costs));
  int best_idx = distance(begin(costs), best_cost);

  // do the action with the best idx
  vector<double> action = final_actions[best_idx];
  current_state = states[best_idx];

  // consider if the best action is to change line whereas there is car in front/behind in the new lane, then it is not allowed to change lane
  // only when state is LCL or LCR, this is the time where real LC happens
  // if (states[best_idx].compare("PLCL")==0 || states[best_idx].compare("PLCR")==0){
  //  action[1] = 0;
  //}


  if (states[best_idx].compare("LCL")==0 || states[best_idx].compare("LCR")==0){
     // could not change lane with too low/high speed
     if (ref_vel <20 || ref_vel >47){
       action[1] = 0;
     } else {
    int new_lane = lane + action[1]; 
      for (int i = 0; i < sensor_fusion.size(); i++) {
       double d = sensor_fusion[i][6];
       double vx = sensor_fusion[i][3];
       double vy = sensor_fusion[i][4];
       double check_speed = sqrt(vx*vx+vy*vy);
       double check_car_s = sensor_fusion[i][5];

       double check_car_line = whichLine(d);

       // if there is vehicle in the new_lane and its distance is within 15m behind, change line is not allowed
       if (check_car_line == new_lane && check_car_s <= car_s && car_s - check_car_s <=15 ){
        action[1] = 0;
        break;
       }
     }
    }
  } 
  return action;
}

void realize_next_state(vector<double> action, double &ref_vel, int &lane){
  ref_vel += action[0];
  lane += action[1];
}

vector<double> generate_action(string state, double &ref_vel, int &lane, vector<vector<double>> sensor_fusion, double car_s, double car_speed ){
  vector<double> action;

  if (state.compare("KL") == 0){
    action = keep_lane(lane, sensor_fusion, car_s, ref_vel);    
  } else if (state.compare("LCL") == 0 || state.compare("LCR") == 0){
    action = lane_change(state, lane, sensor_fusion, car_s, ref_vel);
  } //else if (state.compare("PLCL") == 0 || state.compare("PLCR") == 0){
    //action = prepare_lane_change(state, lane, sensor_fusion, car_s, ref_vel);
 // }

  return action;

}

vector<double> keep_lane(int &lane, vector<vector<double>> sensor_fusion, double car_s, double &ref_vel){
  vector<double> action={0.0, 0.0};
  if (ref_vel<49.5){
    action[0] = 0.224;
  }

  // if there is a car in front us and within 30m,  we need to lower the speed to that of the front car

  vector<double> results = get_car_front(action,  sensor_fusion, ref_vel, car_s, lane);

  double min_dist_lane = results[0];
  double min_speed_lane = results[1];
  double min_dist_new_lane = results[2];
  double min_speed_new_lane = results[3];

  if (min_dist_lane <30 && ref_vel >= min_speed_lane - 5 ){
    action[0] = -0.224;
  }

  return action;
}

vector<double> lane_change(string state, int &lane, vector<vector<double>> sensor_fusion, double car_s, double &ref_vel){
  int new_lane;
  vector<double> action={0.0, 0.0}; 
  if (state.compare("LCL")==0){
    action[1] = -1; 
  } else if (state.compare("LCR")==0){
    action[1] = 1;
  }  
  
  return action; 
}


vector<double> prepare_lane_change(string state, int &lane, vector<vector<double>> sensor_fusion, double car_s, double &ref_vel){
  vector<double> action={0.0, 0.0};
  if (ref_vel<49.5){
      action[0] = 0.224;
  }

  if (state.compare("PLCL")==0){
    action[1] = -1;
  } else if (state.compare("PLCR")==0){   
    action[1] = 1;
  }

  // if there is a car in front us and within 30m,  we need to lower the speed to that of the front car
  for (int i = 0; i < sensor_fusion.size(); i++) {
     float d = sensor_fusion[i][6];
     double vx = sensor_fusion[i][3];
     double vy = sensor_fusion[i][4];
     double check_speed = sqrt(vx*vx+vy*vy);
     double check_car_s = sensor_fusion[i][5];

     double check_car_line = whichLine(d);
     
     if (check_car_line == lane && check_car_s > car_s  && check_car_s - car_s <= 30 ){
       action[0] = -0.224;
       break;
     }
  }

  return action;
}

int whichLine(float d){
   if (d>=0 && d<4 ) {
     return 0;
   } else if (d<8){
     return 1;
   } else {
     return 2;
   }
}

vector<double> get_car_front(vector<double> action, vector<vector<double>> sensor_fusion, double &ref_vel, double car_s, int &lane){
  int new_lane = lane + action[1];

  double min_speed_new_lane = ref_vel;
  double min_dist_new_lane = 1000000; // a big number here

  double min_speed_lane = ref_vel;
  double min_dist_lane = 1000000;

  for (int i = 0; i < sensor_fusion.size(); i++) {
     float d = sensor_fusion[i][6];
     double vx = sensor_fusion[i][3];
     double vy = sensor_fusion[i][4];
     double check_speed = sqrt(vx*vx+vy*vy);
     double check_car_s = sensor_fusion[i][5];

     double check_car_line = whichLine(d);

     if (check_car_line == new_lane && check_car_s > car_s  && check_car_s - car_s < min_dist_new_lane){
       min_dist_new_lane = check_car_s - car_s;
       min_speed_new_lane = check_speed;

     }
  
     if (new_lane != lane){
       if (check_car_line == lane && check_car_s > car_s  && check_car_s - car_s < min_dist_lane){
         min_dist_lane = check_car_s - car_s;
         min_speed_lane = check_speed;

       }
     }
  }

  // if there is no car in front in the new_lane or the car in front in the new_lane has speed faster than current speed
  // that car will not affect any state transtion (cost should be zero) 

  if (new_lane == lane){
    min_dist_lane = min_dist_new_lane;
    min_speed_lane = min_speed_new_lane;
  }
 
  vector<double> results = {min_dist_lane, min_speed_lane, min_dist_new_lane, min_speed_new_lane};
 
  return results;

}

double inefficiency_cost(string state, vector<double> action, vector<vector<double>> sensor_fusion, double &ref_vel, double car_s, int &lane){
// Cost becomes higher for new line 
// where the nearest vehicle in front has velocity lower than vehicle's target speed in both new_lane and lane

  // no inefficiency cost if current speed is less than 20
  //if (ref_vel < 20){
  //    return 0;
 // }
 
  int new_lane = lane + action[1];

  vector<double> results = get_car_front(action,  sensor_fusion, ref_vel, car_s, lane);
  
  double min_dist_lane = results[0];
  double min_speed_lane = results[1];
  double min_dist_new_lane = results[2];
  double min_speed_new_lane = results[3];

  if (state.compare("KL")==0){
     return (ref_vel - min_speed_lane)/ref_vel;
  }
   
  /*if (state.compare("PLCL")==0 || state.compare("PLCR")==0){
    return 0.5* + (ref_vel - min_speed_new_lane)/ref_vel + 0.5 * (ref_vel - min_speed_lane)/ref_vel;
  }
  */

  return  (ref_vel - min_speed_new_lane)/ref_vel;
 
}

double goal_distance_cost(string state, vector<double> action, vector<vector<double>> sensor_fusion, double &ref_vel, double car_s, int &lane){
  int new_lane = lane + action[1];
  vector<double> results = get_car_front(action,  sensor_fusion, ref_vel, car_s, lane);

  double min_dist_lane = results[0];
  double min_speed_lane = results[1];
  double min_dist_new_lane = results[2];
  double min_speed_new_lane = results[3];

  //  cost becomes larger when the distance to the front car in the same lane becomes smaller, the magic number is chosen
  //  by comparing the inefficiency cost if staying at the same lane
  // everything being the same, you should stay at the original lane
 
  if (state.compare("KL")==0){
   return 1 - exp(-3.0/min_dist_lane);
  }

  /*if (state.compare("PLCL")== 0 || state.compare("PLCR")== 0){
    return 0.5*(1 - exp(-3.0/min_dist_lane))+0.5*(1- exp(-4.0/min_dist_new_lane));
  } 
  */

  return 1- exp(-4.0/min_dist_new_lane);
}

double calculate_cost(string state, vector<double> action, vector<vector<double>> sensor_fusion, double &ref_vel, double car_s, int &lane){
  double cost_1 = inefficiency_cost(state, action, sensor_fusion,ref_vel, car_s,lane);
  double cost_2 = goal_distance_cost(state, action, sensor_fusion,ref_vel, car_s,lane);

  int new_lane = lane + action[1];
  //also add a fixed cost if current lane is not in the center, adjust the weight for cost_1 and cost_2
  return 0.0 * cost_1 + 1.0* cost_2 + 0.05 * (new_lane!=1);
}

#endif  // VEHICLE_H


