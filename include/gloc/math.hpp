#ifndef MATH_HPP
#define MATH_HPP

#include <vector>
#include <cmath>

#define PI 3.14159265358

namespace gloc
{
	

  /**
   * Perform a apodized sinc for a limited kernel
   * @param x: f(x)
   * @param alpha: apodization factor. The larger the value
   * the stronger the apodization
   */
  template<class real>
    real
    apod_sinc (real x, real alpha)
    {
      const real pi = static_cast<real> (PI);

      if (x == static_cast<real> (0))
	{
	  return static_cast<real> (1);
	}
      const real p = x / alpha;
      const real xpi = pi * x;

      const real value = (sin (xpi) / xpi) * exp (-0.5 * p * p);
      return static_cast<real> (value);
    }

  /**
   * The sin method evaluate a sinc function for a given
   * for any real type. Handles devision through 0
   */
  template<class real>
    real
    sinc (const real& x)
    {
      const real pi = static_cast<real> (PI);
      if (x == static_cast<real> (0))
	{
	  return static_cast<real> (1);
	}
      const real xpi = pi * x;
      return static_cast<real> (sin (xpi) / xpi);
    }

  /**
   * The Polynome class is a naive implementation
   * to evaluate polynomes.
   * the parameter set is with a descending order such
   * as; $f(x) = p_0 * xˆ{n} + p_1 * xˆ{n - 1} + ... + p_n$
   */
  template<class real>
    class Polynome
    {
    protected:
      std::vector<real> parameter_;
    public:
      Polynome () = delete;

      Polynome (std::vector<real> parameter) :
	  parameter_ (parameter)
      {
      }

      real
      operator() (real x)
      {
	real val = parameter_[0];
	for (size_t i = 0; i < parameter_.size () - 1; ++i)
	  {
	    val = val * x + parameter_[i + 1];
	  }
	return val;
      }
    };

  /**
   * the polyval class is a metatemplate function
   * which gets a various number of parameters
   * f(x, p0, ..., pn) = x^n * p_0 + ... + x * p_{n - 1} + p_n
   * @param: x is the value where f i evaluated at
   * @param params...: a list of values
   */
  template<class real, class first>
    real
    polyval (real x, first pi)
    {
      return pi;
    }

  template<class real, class first, class ... types>
    real
    polyval (real x, first pi, types ... params)
    {
      return x * pi + polyval (x, params...);
    }

} // end namespace gloc
#endif // MATH_HPP
