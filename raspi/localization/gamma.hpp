#ifndef GAMMA_HPP
#define GAMMA_HPP

#include <algorithm>
#include <cassert>
#include <cfloat>
#include <climits>
#include <cmath>

#ifndef M_PI
#define M_PI 3.141592653589793238462643383280
#endif
#define M_LN_SQRT_2PI 0.918938533204672741780329736406
#define M_LN_SQRT_PId2  0.225791352644727432363097614947
#define M_1_SQRT_2PI  0.39894228040143267793994605993
#define M_2PI   6.28318530717958647692528676655
#define M_SQRT_32 5.656854249492380195206754896838

// Chi-sqaured density computation functions (from Scythe, Berkeley)

#define SIXTEN 16
#define do_del(X)              \
    xsq = trunc(X * SIXTEN) / SIXTEN;        \
    del = (X - xsq) * (X + xsq);          \
    if(log_p) {              \
        *cum = (-xsq * xsq * 0.5) + (-del * 0.5) + std::log(temp);  \
        if((lower && x > 0.) || (upper && x <= 0.))      \
        *ccum = ::log1p(-std::exp(-xsq * xsq * 0.5) *     \
          std::exp(-del * 0.5) * temp);    \
    }                \
    else {                \
        *cum = std::exp(-xsq * xsq * 0.5) * std::exp(-del * 0.5) * temp;  \
        *ccum = 1.0 - *cum;            \
    }

#define swap_tail            \
    if (x > 0.) {/* swap  ccum <--> cum */      \
        temp = *cum; if(lower) *cum = *ccum; *ccum = temp;  \
    }
inline void
pnorm_both(double x, double *cum, double *ccum, int i_tail,
            bool log_p)
{
  const double a[5] = {
    2.2352520354606839287,
    161.02823106855587881,
    1067.6894854603709582,
    18154.981253343561249,
    0.065682337918207449113
  };
  const double b[4] = {
    47.20258190468824187,
    976.09855173777669322,
    10260.932208618978205,
    45507.789335026729956
  };
  const double c[9] = {
    0.39894151208813466764,
    8.8831497943883759412,
    93.506656132177855979,
    597.27027639480026226,
    2494.5375852903726711,
    6848.1904505362823326,
    11602.651437647350124,
    9842.7148383839780218,
    1.0765576773720192317e-8
  };
  const double d[8] = {
    22.266688044328115691,
    235.38790178262499861,
    1519.377599407554805,
    6485.558298266760755,
    18615.571640885098091,
    34900.952721145977266,
    38912.003286093271411,
    19685.429676859990727
  };
  const double p[6] = {
    0.21589853405795699,
    0.1274011611602473639,
    0.022235277870649807,
    0.001421619193227893466,
    2.9112874951168792e-5,
    0.02307344176494017303
  };
  const double q[5] = {
    1.28426009614491121,
    0.468238212480865118,
    0.0659881378689285515,
    0.00378239633202758244,
    7.29751555083966205e-5
  };

  double xden, xnum, temp, del, eps, xsq, y;
  int i, lower, upper;

  /* Consider changing these : */
  eps = DBL_EPSILON * 0.5;

  /* i_tail in {0,1,2} =^= {lower, upper, both} */
  lower = i_tail != 1;
  upper = i_tail != 0;

  y = std::fabs(x);
  if (y <= 0.67448975) {
    /* qnorm(3/4) = .6744.... -- earlier had 0.66291 */
    if (y > eps) {
      xsq = x * x;
      xnum = a[4] * xsq;
      xden = xsq;
      for (i = 0; i < 3; ++i) {
        xnum = (xnum + a[i]) * xsq;
        xden = (xden + b[i]) * xsq;
      }
    } else xnum = xden = 0.0;

    temp = x * (xnum + a[3]) / (xden + b[3]);
    if(lower)  *cum = 0.5 + temp;
    if(upper) *ccum = 0.5 - temp;
    if(log_p) {
      if(lower)  *cum = std::log(*cum);
      if(upper) *ccum = std::log(*ccum);
    }
  } else if (y <= M_SQRT_32) {
    /* Evaluate pnorm for 0.674.. = qnorm(3/4) < |x| <= sqrt(32)
     * ~= 5.657 */

    xnum = c[8] * y;
    xden = y;
    for (i = 0; i < 7; ++i) {
      xnum = (xnum + c[i]) * y;
      xden = (xden + d[i]) * y;
    }
    temp = (xnum + c[7]) / (xden + d[7]);
    do_del(y);
    swap_tail;
  } else if (log_p
            || (lower && -37.5193 < x && x < 8.2924)
            || (upper && -8.2929 < x && x < 37.5193)
      ) {
    /* Evaluate pnorm for x in (-37.5, -5.657) union (5.657, 37.5) */
    xsq = 1.0 / (x * x);
    xnum = p[5] * xsq;
    xden = xsq;
    for (i = 0; i < 4; ++i) {
      xnum = (xnum + p[i]) * xsq;
      xden = (xden + q[i]) * xsq;
    }
    temp = xsq * (xnum + p[4]) / (xden + q[4]);
    temp = (M_1_SQRT_2PI - temp) / y;
    do_del(x);
    swap_tail;
  } else {
    if (x > 0) {
      *cum = 1.;
      *ccum = 0.;
    } else {
      *cum = 0.;
      *ccum = 1.;
    }

    assert(false && "Did not converge");
  }

  return;
}

/* The standard normal distribution function */
inline double
pnorm1 (double x, bool lower_tail, bool log_p)
{
  assert (std::isfinite(x));

  double p, cp;
  pnorm_both(x, &p, &cp, (lower_tail ? 0 : 1), log_p);

  return (lower_tail ? p : cp);
}

/* Computes the value of the normal cumulative distribution
 * function with given \a mean and standard deviation \a sd, at the
 * desired quantile \a x.
 */
inline double
pnorm (double x, double mean, double sd)

{
  assert (sd > 0);

  return pnorm1((x - mean) / sd, true, false);
}

/* Computes the value of the normal probability density
 * function with given \a mean and standard deviation \a sd, at the
 * desired quantile \a x.
 */
inline double
dnorm(double x, double mean, double sd)
{
  assert(sd > 0);

  double X = (x - mean) / sd;

  return (M_1_SQRT_2PI * std::exp(-0.5 * X * X) / sd);
}


/* Evaluates the "deviance part" */
inline double
bd0(double x, double np)
{

  if(std::fabs(x - np) < 0.1 * (x + np)) {
    double v = (x - np) / (x + np);
    double s = (x - np) * v;
    double ej = 2 * x * v;
    v = v * v;
    for (int j = 1; ; j++) {
      ej *= v;
      double s1 = s + ej / ((j << 1) + 1);
      if (s1 == s)
        return s1;
      s = s1;
    }
  }

  return x * std::log(x / np) + np - x;
}

/* Computes the log of the error term in Stirling's formula */
inline double
stirlerr(double n)
{
#define S0 0.083333333333333333333       /* 1/12 */
#define S1 0.00277777777777777777778     /* 1/360 */
#define S2 0.00079365079365079365079365  /* 1/1260 */
#define S3 0.000595238095238095238095238 /* 1/1680 */
#define S4 0.0008417508417508417508417508/* 1/1188 */

  /* error for 0, 0.5, 1.0, 1.5, ..., 14.5, 15.0 */
  const double sferr_halves[31] = {
    0.0, /* n=0 - wrong, place holder only */
    0.1534264097200273452913848,  /* 0.5 */
    0.0810614667953272582196702,  /* 1.0 */
    0.0548141210519176538961390,  /* 1.5 */
    0.0413406959554092940938221,  /* 2.0 */
    0.03316287351993628748511048, /* 2.5 */
    0.02767792568499833914878929, /* 3.0 */
    0.02374616365629749597132920, /* 3.5 */
    0.02079067210376509311152277, /* 4.0 */
    0.01848845053267318523077934, /* 4.5 */
    0.01664469118982119216319487, /* 5.0 */
    0.01513497322191737887351255, /* 5.5 */
    0.01387612882307074799874573, /* 6.0 */
    0.01281046524292022692424986, /* 6.5 */
    0.01189670994589177009505572, /* 7.0 */
    0.01110455975820691732662991, /* 7.5 */
    0.010411265261972096497478567, /* 8.0 */
    0.009799416126158803298389475, /* 8.5 */
    0.009255462182712732917728637, /* 9.0 */
    0.008768700134139385462952823, /* 9.5 */
    0.008330563433362871256469318, /* 10.0 */
    0.007934114564314020547248100, /* 10.5 */
    0.007573675487951840794972024, /* 11.0 */
    0.007244554301320383179543912, /* 11.5 */
    0.006942840107209529865664152, /* 12.0 */
    0.006665247032707682442354394, /* 12.5 */
    0.006408994188004207068439631, /* 13.0 */
    0.006171712263039457647532867, /* 13.5 */
    0.005951370112758847735624416, /* 14.0 */
    0.005746216513010115682023589, /* 14.5 */
    0.005554733551962801371038690  /* 15.0 */
  };
  double nn;

  if (n <= 15.0) {
    nn = n + n;
    if (nn == (int)nn)
      return(sferr_halves[(int)nn]);
    return (std::lgamma(n + 1.) - (n + 0.5) * std::log(n) + n -
        std::log(std::sqrt(2 * M_PI)));
  }

  nn = n*n;
  if (n > 500)
    return((S0 - S1 / nn) / n);
  if (n > 80)
    return((S0 - (S1 - S2 / nn) / nn) / n);
  if (n > 35)
    return((S0 - (S1 - (S2 - S3 / nn) / nn) / nn) / n);
  /* 15 < n <= 35 : */
  return((S0 - (S1 - (S2 - (S3 - S4 / nn) / nn) / nn) / nn) / n);
#undef S1
#undef S2
#undef S3
#undef S4
}

/* Helper for dpois and dgamma */
inline double
dpois_raw (double x, double lambda)
{
  if (lambda == 0)
    return ( (x == 0) ? 1.0 : 0.0);

  if (x == 0)
    return std::exp(-lambda);

  if (x < 0)
    return 0.0;

  return std::exp(-stirlerr(x) - bd0(x, lambda))
    / std::sqrt(2 * M_PI * x);
}

/* Computes the value of the gamma cumulative distribution
 * function with given \a shape and \a scale, at the desired quantile
 * \a x.
 */
inline double
pgamma (double x, double shape, double scale)
{
  const double xbig = 1.0e+8, xlarge = 1.0e+37,
    alphlimit = 1000.;/* normal approx. for shape > alphlimit */

  int lower_tail = 1;

  double pn1, pn2, pn3, pn4, pn5, pn6, arg, a, b, c, an, osum, sum;
  long n;
  int pearson;

  /* check that we have valid values for x and shape */
  assert (shape > 0 && scale > 0);

  x /= scale;

  if (x <= 0.)
    return 0.0;

  /* use a normal approximation if shape > alphlimit */

  if (shape > alphlimit) {
    pn1 = std::sqrt(shape) * 3. * (std::pow(x/shape, 1./3.) + 1.
          / (9. * shape) - 1.);
    return pnorm(pn1, 0., 1.);
  }

  /* if x is extremely large __compared to shape__ then return 1 */

  if (x > xbig * shape)
    return 1.0;

  if (x <= 1. || x < shape) {
    pearson = 1;/* use pearson's series expansion. */
    arg = shape * std::log(x) - x - std::lgamma(shape + 1.);
    c = 1.;
    sum = 1.;
    a = shape;
    do {
      a += 1.;
      c *= x / a;
      sum += c;
    } while (c > DBL_EPSILON);
    arg += std::log(sum);
  }
  else { /* x >= max( 1, shape) */
    pearson = 0;/* use a continued fraction expansion */
    arg = shape * std::log(x) - x - std::lgamma(shape);
    a = 1. - shape;
    b = a + x + 1.;
    pn1 = 1.;
    pn2 = x;
    pn3 = x + 1.;
    pn4 = x * b;
    sum = pn3 / pn4;
    for (n = 1; ; n++) {
      a += 1.;/* =   n+1 -shape */
      b += 2.;/* = 2(n+1)-shape+x */
      an = a * n;
      pn5 = b * pn3 - an * pn1;
      pn6 = b * pn4 - an * pn2;
      if (std::fabs(pn6) > 0.) {
        osum = sum;
        sum = pn5 / pn6;
        if (std::fabs(osum - sum) <= DBL_EPSILON * std::min(1., sum))
          break;
      }
      pn1 = pn3;
      pn2 = pn4;
      pn3 = pn5;
      pn4 = pn6;
      if (std::fabs(pn5) >= xlarge) {
        /* re-scale terms in continued fraction if they are large */
        pn1 /= xlarge;
        pn2 /= xlarge;
        pn3 /= xlarge;
        pn4 /= xlarge;
      }
    }
    arg += std::log(sum);
  }

  lower_tail = (lower_tail == pearson);

  sum = std::exp(arg);

  return (lower_tail) ? sum : 1 - sum;
}


/* Computes the value of the gamma probability density
 * function with given \a shape and \a scale, at the desired quantile
 * \a x.
 */
inline double
dgamma(double x, double shape, double scale)
{
  assert(shape > 0 && scale > 0);
  if (x < 0)
    return 0.0;

  if (x == 0) {
    assert(shape >= 1);
    if (shape > 1)
      return 0.0;

    return 1 / scale;
  }

  if (shape < 1) {
    double pr = dpois_raw(shape, x/scale);
    return pr * shape / x;
  }

  /* else  shape >= 1 */
  double pr = dpois_raw(shape - 1, x / scale);
  return pr / scale;
}

/* Computes the value of the \f$\chi^2\f$ probability density
 * function with \a df degrees of freedom, at the desired quantile
 * \a x.
 */
inline double
dchisq(double x, double df)
{
  return dgamma(x, df / 2.0, 2.0);
}

/* Computes the value of the \f$\chi^2\f$ cumulative distribution
 * function with \a df degrees of freedom, at the desired quantile
 * \a x.
 */
inline double
pchisq(double x, double df)
{
  return pgamma(x, df/2.0, 2.0);
}

inline
double ChiSquaredProbability(double x, int df) {
  //return pow(x, (df / 2) - 1) * exp (-x / 2) /
  //    (pow(2, df / 2) * tgamma(df / 2));
  return dchisq(x, df);
}

#endif
