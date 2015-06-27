#include "UtlMath.h"

//=======================================================================
//=======================================================================
double UtlMath::clip(double v, double min, double max)
{
    if (v > max) return max;
    if (v < min) return min;
    return v;
}


//=======================================================================
//=======================================================================
double UtlMath::getNormalizedDeg(double deg)
{
    double div = deg / 360.0;
    if (div <= 1) return deg;

    int imul = (int)div;
    return deg - 360*imul;
}

//=======================================================================
//=======================================================================
double UtlMath::getNormalizedDeg180(double deg)
{
    deg = getNormalizedDeg(deg);

    if (deg > 180) deg = deg - 360;

    return deg;
}

//=======================================================================
//=======================================================================
double UtlMath::getNormalizedDeg360(double deg)
{
    deg = getNormalizedDeg(deg);
    if (deg >= 0) return deg;

    deg = 180 + -1*deg;
    return deg;
}
