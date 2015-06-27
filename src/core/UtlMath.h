#ifndef UTLMATH_H
#define UTLMATH_H

class UtlMath
{
public:

    static double clip(double v, double min=0, double max=1);

    static double getNormalizedDeg(double deg);
    static double getNormalizedDeg180(double deg);
    static double getNormalizedDeg360(double deg);
};

#endif // UTLMATH_H
