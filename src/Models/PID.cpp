#include "./Models/PID.h"

/**
 * Constructs the PIDController object with PID Gains and function pointers
 * for retrieving feedback (pidSource) and delivering output (pidOutput).
 * All PID gains should be positive, otherwise the system will violently diverge
 * from the target.
 * @param p The Proportional gain.
 * @param i The Integral gain.
 * @param d The Derivative gain.
 * @param (*pidSource) The function pointer for retrieving system feedback.
 * @param (*pidOutput) The function pointer for delivering system output.
 */

double get_ellapsed_ms(std::chrono::_V2::system_clock::time_point since){
    return std::chrono::duration<double, std::milli>(std::chrono::high_resolution_clock::now()-since).count() / 1000.0;
}

template <class T>
PIDController<T>::PIDController(double p, double i, double d)
{
  firstPass = true;
  _p = p;
  _i = i;
  _d = d;

  target = 0;
  output = 0;
  currentFeedback = 0;
  error = 0;
  lastError = 0;

  cumulStartFactor = 0;
  integralCumulation = 0;
  maxCumulation = 30000;

  derivativeBounded = false;
  maxDerivative = 30000;
  cycleDerivative = 0;
  
  outputBounded = false;
  outputLowerBound = 0;
  outputUpperBound = 0;
}

/**
 * This method uses the established function pointers to retrieve system
 * feedback, calculate the PID output, and deliver the correction value
 * to the parent of this PIDController.  This method should be run as
 * fast as the source of the feedback in order to provide the highest
 * resolution of control (for example, to be placed in the loop() method).
 */
template <class T>
T PIDController<T>::tick(float currentFeedback)
{
  //Calculate the error between the feedback and the target.
  error = target - currentFeedback;

  currentTime = std::chrono::high_resolution_clock::now();
  if(!firstPass){
    //Calculate time since last tick() cycle.
    double deltaTime = get_ellapsed_ms(lastTime);

    //Calculate the integral of the feedback data since last cycle.
    double cycleIntegral = ((lastError + error) / 2) * deltaTime;

    //Add this cycle's integral to the integral cumulation.
    if(error < (1-cumulStartFactor)*target){
      integralCumulation += cycleIntegral;
    }

    //Calculate the slope of the line with data from the current and last cycles.
    cycleDerivative = (error - lastError) / deltaTime;

    if(derivativeBounded){
      if(cycleDerivative > maxDerivative) cycleDerivative = maxDerivative;
      if(cycleDerivative < -maxDerivative) cycleDerivative = -maxDerivative;
    }
  }else{
    firstPass = false;
  }

  //Save time data for next iteration.
  lastTime = currentTime;

  //Prevent the integral cumulation from becoming overwhelmingly huge.
  if(integralCumulation > maxCumulation) integralCumulation = maxCumulation;
  if(integralCumulation < -maxCumulation) integralCumulation = -maxCumulation;

  //Calculate the system output based on data and PID gains.
  output = (error * _p) + (integralCumulation * _i) + (cycleDerivative * _d);
  // Serial.printf("Error: %.2f, Integral: %.2f, Derivative: %.2f \n", error, integralCumulation, cycleDerivative);

  //Save a record of this iteration's error.
  lastError = error;

  //Trim the output to the bounds if needed.
  if(outputBounded)
  {
    if(output > outputUpperBound) output = outputUpperBound;
    if(output < outputLowerBound) output = outputLowerBound;
  }

  return output;
}

/**
 * Sets the target of this PIDController.  This system will generate
 * correction outputs indended to guide the feedback variable (such
 * as position, velocity, etc.) toward the established target.
 */
template <class T>
void PIDController<T>::setTarget(T t)
{
  target = t;
}

/**
 * Returns the current target of this PIDController.
 * @return The current target of this PIDController.
 */
template <class T>
T PIDController<T>::getTarget()
{
  return target;
}

/**
 * Returns the latest output generated by this PIDController.  This value is
 * also delivered to the parent systems via the PIDOutput function pointer
 * provided in the constructor of this PIDController.
 * @return The latest output generated by this PIDController.
 */
template <class T>
T PIDController<T>::getOutput()
{
  return output;
}

/**
 * Returns the last read feedback of this PIDController.
 * @return The
 */
template <class T>
T PIDController<T>::getFeedback()
{
  return currentFeedback;
}

/**
 * Returns the last calculated error of this PIDController.
 * @return The last calculated error of this PIDController.
 */
template <class T>
T PIDController<T>::getError()
{
  return error;
}

/**
 * Returns the value that the Proportional component is contributing to the output.
 * @return The value that the Proportional component is contributing to the output.
 */
template <class T>
T PIDController<T>::getProportionalComponent()
{
  return (T) (error * _p);
}

/**
 * Returns the value that the Integral component is contributing to the output.
 * @return The value that the Integral component is contributing to the output.
 */
template <class T>
T PIDController<T>::getIntegralComponent()
{
  return (T) (integralCumulation * _i);
}


/**
 * Returns the value that the Derivative component is contributing to the output.
 * @return The value that the Derivative component is contributing to the output.
 */
template <class T>
T PIDController<T>::getDerivativeComponent()
{
  return (T) (cycleDerivative * _d);
}

/**
 * Sets proportion of target at which to start accumulating.
 * @param factor Percentage of the target at which you start accumulating.
 */
template <class T>
void PIDController<T>::setCumulStartFactor(T factor)
{
  cumulStartFactor = factor;
}

/**
 * Set to True if the derivative has a maximum value.
 * @param bounded Whether it has a maximum value or not.
 */
template <class T>
void PIDController<T>::setDerivativeBounded(bool bounded)
{
  derivativeBounded = bounded;
}

/**
 * Sets the maximum value that the derivative can reach.
 * @param max The maximum value of the derivative.
 */
template <class T>
void PIDController<T>::setMaxDerivative(T max)
{
  maxDerivative = max;
}

/**
 * Sets the maximum value that the integral cumulation can reach.
 * @param max The maximum value of the integral cumulation.
 */
template <class T>
void PIDController<T>::setMaxIntegralCumulation(T max)
{
  maxCumulation = max;
}

/**
 * Returns the maximum value that the integral value can cumulate to.
 * @return The maximum value that the integral value can cumulate to.
 */
template <class T>
T PIDController<T>::getMaxIntegralCumulation()
{
  return maxCumulation;
}

/**
 * Returns the current cumulative integral value in this PIDController.
 * @return The current cumulative integral value in this PIDController.
 */
template <class T>
T PIDController<T>::getIntegralCumulation()
{
  return integralCumulation;
}

/**
 * Enables or disables bounds on the output.  Bounds limit the upper and lower
 * values that this PIDController will ever generate as output.
 * @param bounded True to enable output bounds, False to disable.
 */
template <class T>
void PIDController<T>::setOutputBounded(bool bounded)
{
  outputBounded = bounded;
}

/**
 * Returns whether the output of this PIDController is being bounded.
 * @return True if the output of this PIDController is being bounded.
 */
template <class T>
bool PIDController<T>::isOutputBounded()
{
  return outputBounded;
}

/**
 * Sets bounds which limit the lower and upper extremes that this PIDController
 * will ever generate as output.  Setting output bounds automatically enables
 * output bounds.
 * @param lower The lower output bound.
 * @param upper The upper output bound.
 */
template <class T>
void PIDController<T>::setOutputBounds(T lower, T upper)
{
  if(upper > lower)
  {
    outputBounded = true;
    outputLowerBound = lower;
    outputUpperBound = upper;
  }
}

/**
 * Returns the lower output bound of this PIDController.
 * @return The lower output bound of this PIDController.
 */
template <class T>
T PIDController<T>::getOutputLowerBound()
{
  return outputLowerBound;
}

/**
 * Returns the upper output bound of this PIDController.
 * @return The upper output bound of this PIDController.
 */
template <class T>
T PIDController<T>::getOutputUpperBound()
{
  return outputUpperBound;
}

/**
 * Sets new values for all PID Gains.
 * @param p The new proportional gain.
 * @param i The new integral gain.
 * @param d The new derivative gain.
 */
template <class T>
void PIDController<T>::setPID(double p, double i, double d)
{
  _p = p;
  _i = i;
  _d = d;
}

/**
 * Sets a new value for the proportional gain.
 * @param p The new proportional gain.
 */
template <class T>
void PIDController<T>::setP(double p)
{
  _p = p;
}

/**
 * Sets a new value for the integral gain.
 * @param i The new integral gain.
 */
template <class T>
void PIDController<T>::setI(double i)
{
  _i = i;
}

/**
 * Sets a new value for the derivative gain.
 * @param d The new derivative gain.
 */
template <class T>
void PIDController<T>::setD(double d)
{
  _d = d;
}

/**
 * Returns the proportional gain.
 * @return The proportional gain.
 */
template <class T>
double PIDController<T>::getP()
{
  return _p;
}

/**
 * Returns the integral gain.
 * @return The integral gain.
 */
template <class T>
double PIDController<T>::getI()
{
  return _i;
}

/**
 * Returns the derivative gain.
 * @return The derivative gain.
 */
template <class T>
double PIDController<T>::getD()
{
  return _d;
}

/*
 * Lets the compiler/linker know what types of templates we are expecting to
 * have this class instantiated with.  Basically, it prepares the program to
 * make a PIDController of any of these defined types.
 */
template class PIDController<int>;
template class PIDController<long>;
template class PIDController<float>;
template class PIDController<double>;
