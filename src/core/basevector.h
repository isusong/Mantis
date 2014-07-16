#ifndef BASEVECTOR_H
#define BASEVECTOR_H

#include <cassert>

    template <typename TYPE, int SIZE>
    struct BaseVector
    {
        const TYPE& operator [] (int index) const
        {
            assert( index >= 0 && index < SIZE );
            return reinterpret_cast<const TYPE*>(this)[index];
        }

        TYPE& operator [] (int index)
        {
            assert( index >= 0 && index < SIZE );
            return reinterpret_cast<TYPE*>(this)[index];
        }

        operator const TYPE* () const
        {
            return reinterpret_cast<const TYPE*>(this);
        }

        operator TYPE* ()
        {
            return reinterpret_cast<TYPE*>(this);
        }

        bool operator == (const BaseVector<TYPE,SIZE>& v)
        {
            for ( int i=0; i<SIZE; ++i )
            {
                if ( v[i] != (*this)[i] )
                    return false;
            }
            return true;
        }

        bool operator != (const BaseVector<TYPE,SIZE>& v)
        {
            for ( int i=0; i<SIZE; ++i )
            {
                if ( v[i] != (*this)[i] )
                    return true;
            }
            return false;
        }
    };

#endif
