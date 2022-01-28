#include "TWIDDLE.h"

TWIDDLE::TWIDDLE() {}

TWIDDLE::~TWIDDLE() {}

// Init
void TWIDDLE::Init(double Kp_, double Ki_, double Kd_, double Diff_Kp_, double Diff_Ki_, double Diff_Kd_) {
  Kp = Kp_;
  Ki = Ki_;
  Kd = Kd_;
  Diff_Kp = Diff_Kp_;
  Diff_Ki = Diff_Ki_;
  Diff_Kd = Diff_Kd_; 
  update_index = 0;
  state = 0;
  count = 0;
  accu_err = 0;
  best_err = 10000000;  

  // tolerance level for twiddle algorithm
  tolerance = 0.01;
  tolerance_flag = false;

  best_err_initialized = false;

  sample_period = 20;

}

// Accumulate errors before it is time to update params. 
void TWIDDLE::Increment(double cte){
  if (best_err_initialized && count == 0 && state == 0){
    if (update_index == 0){
      Kp += Diff_Kp;
    } else if (update_index == 1){
      Ki += Diff_Ki;
    } else {
      Kd += Diff_Kd;
    }
  }

  if (count>=sample_period && count<2*sample_period){
    accu_err += cte*cte;
  }
  count++;
}

// Update params based on accumurated errors  
std::vector<double> TWIDDLE::UpdateParams(){

  std::vector<double> params = {Kp, Ki, Kd};
  std::vector<double> diff_params = {Diff_Kp, Diff_Ki, Diff_Kd};
  if (accu_err < best_err){
     best_err = accu_err;
     diff_params[update_index] *=1.1;
     state = 0;
     update_index = (update_index+1)%3;
  } 
  
  else if (accu_err >= best_err && state == 0){
     params[update_index] -= 2*diff_params[update_index];
     state = 1;
  } 

  else if (accu_err >= best_err && state == 1){
     params[update_index] += diff_params[update_index];
     diff_params[update_index] *= 0.9;
     state = 0;
     update_index = (update_index+1)%3;
  }
  
  Diff_Kp = diff_params[0];
  Diff_Ki = diff_params[1];
  Diff_Kd = diff_params[2];

  Kp = params[0];
  Ki = params[1];
  Kd = params[2];

  //reset the count and accu_err
  count = 0;
  accu_err = 0;

  return params;
}

// Return True if tolerance is reached; otherwise False.
bool TWIDDLE::ReachTolerance(){
  if (tolerance_flag ==true){
    return true;
  } else if (Diff_Kp + Diff_Ki + Diff_Kd < tolerance){
    // set the flag once it is reached
    tolerance_flag = true;
    return true;
  }

  return false;
}

// Set tolerance;
void TWIDDLE::SetTolerance(bool tolerance_){
  tolerance_flag = tolerance_;
}

bool TWIDDLE::TimeToUpdate(){
  if (count< 2* sample_period){
    return false;
  } else {
    //reset the count
    count = 0;
    return true;
  }
}

bool TWIDDLE::BestErrorInitialized(){
  return best_err_initialized;
}

void TWIDDLE::SetBestError(){
  best_err = accu_err;
  //reset the count and accu_err
  count = 0;
  accu_err = 0;

  // set as true after initialization
  best_err_initialized = true;
}

double TWIDDLE::GetBestError(){
  return best_err;
}

int TWIDDLE::GetCount(){
  return count;
}

std::vector<double> TWIDDLE::GetParams(){
  std::vector<double> params = {Kp, Ki, Kd, Diff_Kp, Diff_Ki, Diff_Kp};
  return params;
}
