#ifndef PID_H
#define PID_H
#endif

#include <chrono>
#include "./Models/Controller.h"

template <class T>
class PIDController
{
public:
  PIDController(double p, double i, double d);
  T tick(float currentFeedback);
  void setTarget(T t);
  T getTarget();
  T getOutput();
  T getFeedback();
  T getError();
  T getProportionalComponent();
  T getIntegralComponent();
  T getDerivativeComponent();
  void setMaxIntegralCumulation(T max);
  void setCumulStartFactor(T factor);
  void setMaxDerivative(T max);
  void setDerivativeBounded(bool bounded);
  T getMaxIntegralCumulation();
  T getIntegralCumulation();

  void setOutputBounded(bool bounded);
  bool isOutputBounded();
  void setOutputBounds(T lower, T upper);
  T getOutputLowerBound();
  T getOutputUpperBound();

  void setPID(double p, double i, double d);
  void setP(double p);
  void setI(double i);
  void setD(double d);
  double getP();
  double getI();
  double getD();
  void setPIDSource(T (*pidSource)());
  void setPIDOutput(void (*pidOutput)(T output));
private:
  bool firstPass;
  double _p;
  double _i;
  double _d;
  T target;
  T output;
  T currentFeedback;
  T error;
  T lastError;
  std::chrono::_V2::system_clock::time_point currentTime;
  std::chrono::_V2::system_clock::time_point lastTime;
  T cumulStartFactor;
  T integralCumulation;
  T maxCumulation;
  T maxDerivative;
  T cycleDerivative;
  bool derivativeBounded;

  bool outputBounded;
  T outputLowerBound;
  T outputUpperBound;

  T (*_pidSource)();
  void (*_pidOutput)(T output);
};
