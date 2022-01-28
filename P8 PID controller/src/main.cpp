#include <math.h>
#include <uWS/uWS.h>
#include <iostream>
#include <string>
#include "json.hpp"
#include "PID.h"
#include "TWIDDLE.h"

// for convenience
using nlohmann::json;
using std::string;

// For converting back and forth between radians and degrees.
constexpr double pi() { return M_PI; }
double deg2rad(double x) { return x * pi() / 180; }
double rad2deg(double x) { return x * 180 / pi(); }

// Checks if the SocketIO event has JSON data.
// If there is data the JSON object in string format will be returned,
// else the empty string "" will be returned.
string hasData(string s) {
  auto found_null = s.find("null");
  auto b1 = s.find_first_of("[");
  auto b2 = s.find_last_of("]");
  if (found_null != string::npos) {
    return "";
  }
  else if (b1 != string::npos && b2 != string::npos) {
    return s.substr(b1, b2 - b1 + 1);
  }
  return "";
}

int main() {
  uWS::Hub h;

  PID pid;
  /**
   * TODO: Initialize the pid variable.
   */
  //pid.Init(0.1, 0.0, 1.0);
  pid.Init(0.1, 0.0001, 2.0);  

  PID throttle_pid;
  throttle_pid.Init(0.0, 0.0, 5.0);

  TWIDDLE twiddle;
  twiddle.Init(0.1, 0.0001, 2.0, 0.1/10, 0.0001/10, 2.0/10);  
  h.onMessage([&pid,&twiddle, &throttle_pid](uWS::WebSocket<uWS::SERVER> ws, char *data, size_t length, 
                     uWS::OpCode opCode) {
    // "42" at the start of the message means there's a websocket message event.
    // The 4 signifies a websocket message
    // The 2 signifies a websocket event
    if (length && length > 2 && data[0] == '4' && data[1] == '2') {
      auto s = hasData(string(data).substr(0, length));

      if (s != "") {
        auto j = json::parse(s);

        string event = j[0].get<string>();

        if (event == "telemetry") {
          // j[1] is the data JSON object
          double cte = std::stod(j[1]["cte"].get<string>());
          double speed = std::stod(j[1]["speed"].get<string>());
          double angle = std::stod(j[1]["steering_angle"].get<string>());
          double steer_value;
          
          /**
           * TODO: Calculate steering value here, remember the steering value is
           *   [-1, 1].
           * NOTE: Feel free to play around with the throttle and speed.
           *   Maybe use another PID controller to control the speed!
           */

           // compensate cte with velocity perpendicular to edge
          //cte += 0.05*speed * sin(angle * M_PI / 180);

          if (!twiddle.ReachTolerance()){
            if (!twiddle.TimeToUpdate()){
              twiddle.Increment(cte);
            } else {
              if (!twiddle.BestErrorInitialized()){
                twiddle.SetBestError();
              } else {
                std::vector<double> params = twiddle.UpdateParams();
                pid.Init(params[0], params[1], params[2]);
              }
            }
          }

          

          pid.UpdateError(cte);
          throttle_pid.UpdateError(cte);
          steer_value = std::min(std::max(-pid.TotalError(),-1.0), 1.0);
          
          // DEBUG
         if (twiddle.GetCount()==0) {
           std::vector<double> ps = twiddle.GetParams();
           std::cout << "CTE: " << cte << " Steering Value: " << steer_value << " Best Error: " << twiddle.GetBestError()
	            << " P: " << ps[0] << " I: " << ps[1] << " D: " << ps[2]  
                    << " Count: " << twiddle.GetCount() << " Reach Tolerance: " << twiddle.ReachTolerance()
                    << " Best Error Initialized: " << twiddle.BestErrorInitialized() << std::endl;


         } else {
           std::cout << "CTE: " << cte << " Steering Value: " << steer_value << " Best Error: " << twiddle.GetBestError()
                    //<< " P: " << ps[0] << " I: " << ps[1] << " D: " << ps[2] << " Count: " 
                    << " Count: " << twiddle.GetCount() << " Reach Tolerance: " << twiddle.ReachTolerance()
                    << " Best Error Initialized: " << twiddle.BestErrorInitialized() << std::endl;
         }

          json msgJson;
          msgJson["steering_angle"] = steer_value;
          if (speed<0){
            msgJson["throttle"] = 0.3;
          } else{
            msgJson["throttle"] = 0.4 - throttle_pid.TotalError();
          } 
          auto msg = "42[\"steer\"," + msgJson.dump() + "]";
          std::cout << msg << std::endl;
          ws.send(msg.data(), msg.length(), uWS::OpCode::TEXT);
        }  // end "telemetry" if
      } else {
        // Manual driving
        string msg = "42[\"manual\",{}]";
        ws.send(msg.data(), msg.length(), uWS::OpCode::TEXT);
      }
    }  // end websocket message if
  }); // end h.onMessage

  h.onConnection([&h](uWS::WebSocket<uWS::SERVER> ws, uWS::HttpRequest req) {
    std::cout << "Connected!!!" << std::endl;
  });

  h.onDisconnection([&h](uWS::WebSocket<uWS::SERVER> ws, int code, 
                         char *message, size_t length) {
    ws.close();
    std::cout << "Disconnected" << std::endl;
  });

  int port = 4567;
  if (h.listen(port)) {
    std::cout << "Listening to port " << port << std::endl;
  } else {
    std::cerr << "Failed to listen to port" << std::endl;
    return -1;
  }
  
  h.run();
}
