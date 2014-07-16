#ifndef BOX_H
#define BOX_H

//#include "sysdefs.h"
#include <limits>
#include "vector3.h"
//#include "frustum.h"
//#include "utl3d.h"

#ifdef max
#undef max
#endif
#ifdef min
#undef min
#endif

enum ECull
{
    I_CULL_NONE = 0,
    I_CULL_PARTIAL = 1,
    I_CULL_ALL = 2
};

template <typename T>
struct Box
{

    // members

    Vector3<T> vmin;
    Vector3<T> vmax;

    // constructors

    Box()
    {
        Reset();
    }

    Box(const Vector3<T>& umin, const Vector3<T>& umax)
        : vmin(umin), vmax(umax)
    {
    }

    Box(const Box& a, const Box& b)
    {
        vmin.x = a.vmin.x < b.vmin.x ? a.vmin.x : b.vmin.x;
        vmin.y = a.vmin.y < b.vmin.y ? a.vmin.y : b.vmin.y;
        vmin.z = a.vmin.z < b.vmin.z ? a.vmin.z : b.vmin.z;
        vmax.x = a.vmax.x > b.vmax.x ? a.vmax.x : b.vmax.x;
        vmax.y = a.vmax.y > b.vmax.y ? a.vmax.y : b.vmax.y;
        vmax.z = a.vmax.z > b.vmax.z ? a.vmax.z : b.vmax.z;
    }

    // methods
    void reset()
    {
        vmin.SetVector(std::numeric_limits<T>::max(), std::numeric_limits<T>::max(), std::numeric_limits<T>::max());
        vmax = (T)-1 * vmin;
    }

    void updateBox(const Vector3<T>& umin, const Vector3<T>& umax)
    {
        if ( umin.x < vmin.x) vmin.x = umin.x;
        if ( umin.y < vmin.y) vmin.y = umin.y;
        if ( umin.z < vmin.z) vmin.z = umin.z;
        if ( umax.x > vmax.x) vmax.x = umax.x;
        if ( umax.y > vmax.y) vmax.y = umax.y;
        if ( umax.z > vmax.z) vmax.z = umax.z;
    }

    void updateBox(const Vector3<T>& v)
    {
        if ( v.x < vmin.x ) vmin.x = v.x; if ( v.x > vmax.x ) vmax.x = v.x;
        if ( v.y < vmin.y ) vmin.y = v.y; if ( v.y > vmax.y ) vmax.y = v.y;
        if ( v.z < vmin.z ) vmin.z = v.z; if ( v.z > vmax.z ) vmax.z = v.z;
    }

    void updateBox(const T &x, const T &y, const T &z)
    {
        if ( x < vmin.x ) vmin.x = x;
        if ( y < vmin.y ) vmin.y = y;
        if ( z < vmin.z ) vmin.z = z;

        if ( x > vmax.x ) vmax.x = x;
        if ( y > vmax.y ) vmax.y = y;
        if ( z > vmax.z ) vmax.z = z;
    }

    Vector3<T> getCenter() const
    {
        return (vmin + vmax) * static_cast<T>(0.5);
    }

    Vector3<T> getTopLeft() const
    {
        return Vector3<T>(vmin.x, vmax.y, vmin.z);
    }

    Vector3<T> getBtmLeft() const
    {
        return Vector3<T>(vmin.x, vmin.y, vmin.z);
    }

    Vector3<T> getSize() const
    {
        return vmax - vmin;
    }

    T getWidth() const
    {
        return vmax.x - vmin.x;
    }

    T getHeight() const
    {
        return vmax.y - vmin.y;
    }

    T getDepth() const
    {
        return vmax.z - vmin.z;
    }

    bool isInside(const Vector3<T>& point) const
    {
        if ( point.x < vmin.x || point.y < vmin.y || point.z < vmin.z ) return false;
        if ( point.x > vmax.x || point.y > vmax.y || point.z > vmax.z ) return false;
        return true;
    }

    bool isInside(T x, T y, T z) const
    {
        if ( x < vmin.x || y < vmin.y || z < vmin.z ) return false;
        if ( x > vmax.x || y > vmax.y || z > vmax.z ) return false;
        return true;
    }

    bool isInsideOrOn(T x, T y, T z) const
    {
        if ( x <= vmin.x || y <= vmin.y || z <= vmin.z ) return false;
        if ( x >= vmax.x || y >= vmax.y || z >= vmax.z ) return false;
        return true;
    }

    /*
    int Cull(Frustum *pFrustum)
    {
        // test all verts against each plane
        // cases:
        //        1. all dfp's (same plane) is negative - completely outside frustum
        //        2. all dfp's postive                  - completely within frustum
        //        3. not case 1 or 2                    - intersects frustum

        PlaneEq *pclPlaneEqs = pFrustum->m_paclPlaneEqs;
        float fDis[8]; //TODO: OPTIMIZE THIS
        bool bAllPos = TRUE;
        bool bAllNeg = TRUE;

        for (int j=0; j<6; j++)    //  all 6 frustum planes
        {
            fDis[0] = M_DFP2(pclPlaneEqs[j], (float)vmin.x, (float)vmin.y, (float)vmin.z);
            fDis[1] = M_DFP2(pclPlaneEqs[j], (float)vmax.x, (float)vmin.y, (float)vmin.z);
            fDis[2] = M_DFP2(pclPlaneEqs[j], (float)vmin.x, (float)vmax.y, (float)vmin.z);
            fDis[3] = M_DFP2(pclPlaneEqs[j], (float)vmin.x, (float)vmin.y, (float)vmax.z);
            fDis[4] = M_DFP2(pclPlaneEqs[j], (float)vmax.x, (float)vmax.y, (float)vmin.z);
            fDis[5] = M_DFP2(pclPlaneEqs[j], (float)vmin.x, (float)vmax.y, (float)vmax.z);
            fDis[6] = M_DFP2(pclPlaneEqs[j], (float)vmax.x, (float)vmin.y, (float)vmax.z);
            fDis[7] = M_DFP2(pclPlaneEqs[j], (float)vmax.x, (float)vmax.y, (float)vmax.z);

            bAllNeg = true;
            for (int i=0; i<8; i++)
            {
                if (fDis[i] < 0.0)
                    bAllPos = false;
                else
                    bAllNeg = false;
            }

            // fully clipped
            if (bAllNeg) return I_CULL_ALL;
        }

        // fully within
        if (bAllPos) return I_CULL_NONE;


        return I_CULL_PARTIAL;
    }
    */

    /*
    void GetVertexArray8(Vector3<T> vertex[]) const
    {
    vertex[0] = Vector3<T>(vmin.x,vmin.y,vmin.z);
    vertex[1] = Vector3<T>(vmax.x,vmin.y,vmin.z);
    vertex[2] = Vector3<T>(vmin.x,vmax.y,vmin.z);
    vertex[3] = Vector3<T>(vmax.x,vmax.y,vmin.z);
    vertex[4] = Vector3<T>(vmin.x,vmin.y,vmax.z);
    vertex[5] = Vector3<T>(vmax.x,vmin.y,vmax.z);
    vertex[6] = Vector3<T>(vmin.x,vmax.y,vmax.z);
    vertex[7] = Vector3<T>(vmax.x,vmax.y,vmax.z);
    }

    void GetVertexArray8(Vector3<T> vertex[], const Matrix4x4<T>& transform) const
    {
    vertex[0] = Vector3<T>(vmin.x,vmin.y,vmin.z) * transform;
    vertex[1] = Vector3<T>(vmax.x,vmin.y,vmin.z) * transform;
    vertex[2] = Vector3<T>(vmin.x,vmax.y,vmin.z) * transform;
    vertex[3] = Vector3<T>(vmax.x,vmax.y,vmin.z) * transform;
    vertex[4] = Vector3<T>(vmin.x,vmin.y,vmax.z) * transform;
    vertex[5] = Vector3<T>(vmax.x,vmin.y,vmax.z) * transform;
    vertex[6] = Vector3<T>(vmin.x,vmax.y,vmax.z) * transform;
    vertex[7] = Vector3<T>(vmax.x,vmax.y,vmax.z) * transform;
    }
    */
};

typedef Box<float>		box3f;
typedef Box<double>		box3d;

#endif
