#ifndef TWIDDLE_H
#define TwIDDLE_H

#include <vector>

class TWIDDLE {
 public:
  /**
   * Constructor
   */
  TWIDDLE();

  /**
   * Destructor.
   */
  virtual ~TWIDDLE();

  /**
   * Initialize TWIDDLE.
   * @diff_param (Kp_, Ki_, Kd_) The initial PID coefficients
   */
  void Init(double Kp_, double Ki_, double Kd_, double Diff_Kp_, double Diff_Ki_, double Diff_Kd_);

  // Continue to accumulate errors before it is time to update params. 
  void Increment(double cte);

  // Update params based on accumurated errors 
  std::vector<double> UpdateParams();

  // Return True if tolerance is reached; otherwise False.
  bool ReachTolerance();

  // Set tolerance;
  void SetTolerance(bool tolerance_);

  // Check if the stps are enough to update the params, return true when steps equal to two sample period (where errors are only accumulated in the second sample period)
  bool TimeToUpdate();

  // Check if it is the first iteration
  bool BestErrorInitialized();

  // set the best error if it is the first iteration
  void SetBestError();
 
  // for debug
  double GetBestError();

  // for debug
  int GetCount();

  // for debug
  std::vector<double> GetParams();


 private:
  /**
   * TWIDDLE Coefficients
   */
  double Kp;
  double Ki;
  double Kd;
  double Diff_Kp;
  double Diff_Ki;
  double Diff_Kd;

  // store accumulated error
  double accu_err;

  // tolerance level when the algorithm will stop
  double tolerance;
  // flag to indicate if tolerance is reached
  bool tolerance_flag;
  // store the best error so far
  double best_err;

  // monitor which coefficieint needs to be updated, 0->P coefficient, 1->I coefficient, 2 -> D coefficient
  int update_index;

  // monitor state in the if else relationship for twiddle algorithm, 0 is for p[i]+= dp[i], 1 is after p[i]+=dp[i] where the accumulated error is larger than the best error, then p[i]-= 2*dp[i] should be applied. 
  int state;

  // monitor steps in the twiddle algorithm before it is time to check for update
  int count;

  // flag to indicate if it is the first iteration
  bool best_err_initialized;

  // how long that  errors are accumulated
  int sample_period;
};

#endif  // TWIDDLE_H
    
