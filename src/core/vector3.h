#ifndef VECTOR3_H
#define VECTOR3_H

#include <cassert>
#include <cmath>
#include "basevector.h"
//#include "Point2d.h"

    template <typename T>
    struct Vector3 : BaseVector<T,3>
    {

        // members

        union { T x,r; };
        union { T y,g; };
        union { T z,b; };

        // constructors

        Vector3()
        {
            x = 0;
            y = 0;
            z = 0;
        }

        Vector3(const T& vx, const T& vy, const T& vz)
        : x(vx), y(vy), z(vz)
        {
        }

        Vector3(const T v[])
        : x(v[0]), y(v[1]), z(v[2])
        {
        }

        Vector3(const Vector3& v)
        : x(v.x), y(v.y), z(v.z)
        {
        }

        Vector3(const T& v)
        : x(v),y(v),z(v)
        {
        }

        //Point2d get2d() const { return Point2d(x, y); }

        // operators

        Vector3 operator + () const
        {
            return *this;
        }

        Vector3 operator - () const
        {
            return Vector3(-x, -y, -z);
        }

        Vector3 operator + (const Vector3& v) const
        {
            return Vector3(x + v.x, y + v.y, z + v.z);
        }

        Vector3 operator - (const Vector3& v) const
        {
            return Vector3(x - v.x, y - v.y, z - v.z);
        }

        Vector3 operator * (const Vector3& v) const
        {
            return Vector3(x * v.x, y * v.y, z * v.z);
        }

        Vector3 operator * (const T& s) const
        {
            return Vector3(x * s, y * s, z * s);
        }

        Vector3 operator / (const T& s) const
        {
            assert( s != 0 );
            T is = 1 / s;
            return Vector3(x * is, y * is, z * is);
        }

        Vector3& operator += (const Vector3& v)
        {
            x += v.x;
            y += v.y;
            z += v.z;
            return *this;
        }

        Vector3& operator -= (const Vector3& v)
        {
            x -= v.x;
            y -= v.y;
            z -= v.z;
            return *this;
        }

        Vector3& operator *= (const Vector3& v)
        {
            x *= v.x;
            y *= v.y;
            z *= v.z;
            return *this;
        }

        Vector3& operator *= (const T& s)
        {
            x *= s;
            y *= s;
            z *= s;
            return *this;
        }

        Vector3& operator /= (const T& s)
        {
            assert( s != 0 );
            T is = 1 / s;
            x *= is;
            y *= is;
            z *= is;
            return *this;
        }

        Vector3& operator = (const Vector3& v)
        {
            x = v.x;
            y = v.y;
            z = v.z;
            return *this;
        }

        Vector3& operator = (const T& v)
        {
            x = v;
            y = v;
            z = v;
            return *this;
        }

        // methods

        void Normalize()
        {
            T s = x*x + y*y + z*z;
            if ( s != 0 )
            {
                s = 1 / static_cast<T>(sqrt(s));
                x *= s;
                y *= s;
                z *= s;
            }
        }

        void SetVector(const T& vx, const T& vy, const T& vz)
        {
            x = vx;
            y = vy;
            z = vz;
        }

        void SetVector(const T& v)
        {
            x = v;
            y = v;
            z = v;
        }
    };

    // inline functions

    template <typename T>
    inline T LengthSquared(const Vector3<T>& v)
    {
        return v.x*v.x + v.y*v.y + v.z*v.z;
    }

    template <typename T>
    inline T Length(const Vector3<T>& v)
    {
        return static_cast<T>(sqrt(v.x*v.x + v.y*v.y + v.z*v.z));
    }

    template <typename T>
    inline T DotProduct(const Vector3<T>& a, const Vector3<T>& b)
    {
        return a.x * b.x + a.y * b.y + a.z * b.z;
    }

    template <typename T>
    inline Vector3<T> CrossProduct(const Vector3<T>& a, const Vector3<T>& b)
    {
        return Vector3<T>(
            (a.y * b.z) - (a.z * b.y),
            (a.z * b.x) - (a.x * b.z),
            (a.x * b.y) - (a.y * b.x));
    }

    template <typename T>
    inline Vector3<T> Normalize(const Vector3<T>& v)
    {
        T s = v.x*v.x + v.y*v.y + v.z*v.z;
        if ( s == 0 )
            return v;

        s = 1 / static_cast<T>(sqrt(s));
        return Vector3<T>(v.x * s, v.y * s, v.z * s);
    }

    template <typename T>
    inline Vector3<T> Reflect(const Vector3<T>& v, const Vector3<T>& normal)
    {
        T s = DotProduct(v,normal) * 2;
        return Vector3<T>(
            v.x + normal.x * s,
            v.y + normal.y * s,
            v.z + normal.z * s);
    }

    template <typename T>
    inline Vector3<T> Lerp(const Vector3<T>& a, const Vector3<T>& b, const T& time)
    {
        return Vector3<T>(
            a.x + (b.x - a.x) * time,
            a.y + (b.y - a.y) * time,
            a.z + (b.z - a.z) * time);
    }

    template <typename T>
    inline Vector3<T> operator * (const T& s, const Vector3<T>& v)
    {
        return Vector3<T>(s * v.x, s * v.y, s * v.z);
    }

    template <typename T>
    inline Vector3<T> ComputeNormal (const Vector3<T>& v1, const Vector3<T>& v2)
    {
        Vector3<T> n;
        n = CrossProduct(v1, v2);
        n.Normalize();
        return n;
    }

    template <typename T>
    inline Vector3<T> ComputeNormal (const T pv1[], const T pv2[])
    {
        Vector3<T> n, v1(pv1), v2(pv2);
        n = CrossProduct(v1, v2);
        n.Normalize();
        return n;
    }

    template <typename T>
    inline Vector3<T> ComputeNormal (const T pp1[], const T pp2[], const T pp3[])
    {
        Vector3<T> v1, v2;
        Vector3<T>  p1(pp1), p2(pp2), p3(pp3);

        v2 = p3 - p2;
        v1 = p2 - p1;

        return ComputeNormal(v1, v2);
    }

typedef Vector3<float>	vec3f;
typedef Vector3<double>	vec3d;

#endif
