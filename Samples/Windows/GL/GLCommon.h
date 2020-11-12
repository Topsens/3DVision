#pragma once

#include <gl/glew.h>
#include <initializer_list>
#include <cmath>

template<typename Scalar>
inline Scalar sqrtx(Scalar s)
{
    return sqrt(s);
}

template<>
inline float sqrtx(float s)
{
    return sqrtf(s);
}

template<>
inline long double sqrtx(long double s)
{
    return sqrtl(s);
}

template<typename Scalar>
inline Scalar ToRadian(Scalar degree)
{
    return degree * (Scalar)3.14159265358979323846264338327950288419716939937510l / (Scalar)180.0;
}

template<typename Scalar>
inline Scalar ToDegree(Scalar radian)
{
    return radian / (Scalar)3.14159265358979323846264338327950288419716939937510l * (Scalar)180.0;
}

template<typename Scalar, int Dimensions>
struct Vector
{
    Scalar v[Dimensions];

    Vector() = default;
    Vector(const Scalar* v)
    {
        for (auto i = 0; i < Dimensions; i++)
        {
            this->v[i] = v[i];
        }
    }
    Vector(const std::initializer_list<Scalar>& v)
    {
        int i = 0;
        for (auto& val : v)
        {
            this->v[i++] = val;

            if (i == Dimensions)
            {
                break;
            }
        }
    }

    inline Scalar& operator[](int index)
    {
        return this->v[index];
    }
    inline const Scalar& operator[](int index) const
    {
        return this->v[index];
    }
    inline operator Scalar*()
    {
        return this->v;
    }
    inline operator const Scalar*() const
    {
        return this->v;
    }
};

template<typename Scalar, int Dimensions>
inline Vector<Scalar, Dimensions> operator+(const Vector<Scalar, Dimensions>& first, const Vector<Scalar, Dimensions>& second)
{
    Vector<Scalar, Dimensions> result;

    for (auto i = 0; i < Dimensions; i++)
    {
        result.v[i] = first.v[i] + second.v[i];
    }

    return result;
}

template<typename Scalar, int Dimensions>
inline Vector<Scalar, Dimensions> operator-(const Vector<Scalar, Dimensions>& first, const Vector<Scalar, Dimensions>& second)
{
    Vector<Scalar, Dimensions> result;

    for (auto i = 0; i < Dimensions; i++)
    {
        result.v[i] = first.v[i] - second.v[i];
    }

    return result;
}

template<typename Scalar, int Dimensions>
inline Vector<Scalar, Dimensions> operator*(const Vector<Scalar, Dimensions>& vector, Scalar scale)
{
    Vector<Scalar, Dimensions> result;

    for (auto i = 0; i < Dimensions; i++)
    {
        result.v[i] = vector.v[i] * scale;
    }

    return result;
}

template<typename Scalar, int Dimensions>
inline Vector<Scalar, Dimensions> operator*(Scalar scale, const Vector<Scalar, Dimensions>& vector)
{
    return vector * scale;
}

template<typename Scalar, int Dimensions>
inline Vector<Scalar, Dimensions> operator/(const Vector<Scalar, Dimensions>& vector, Scalar scale)
{
    Vector<Scalar, Dimensions> result;

    for (auto i = 0; i < Dimensions; i++)
    {
        result.v[i] = vector.v[i] / scale;
    }

    return result;
}

template<typename Scalar, int Dimensions>
inline Vector<Scalar, Dimensions>& operator+=(Vector<Scalar, Dimensions>& first, const Vector<Scalar, Dimensions>& second)
{
    for (auto i = 0; i < Dimensions; i++)
    {
        first.v[i] += second.v[i];
    }

    return first;
}

template<typename Scalar, int Dimensions>
inline Vector<Scalar, Dimensions>& operator-=(Vector<Scalar, Dimensions>& first, const Vector<Scalar, Dimensions>& second)
{
    for (auto i = 0; i < Dimensions; i++)
    {
        first.v[i] -= second.v[i];
    }

    return first;
}

template<typename Scalar, int Dimensions>
inline Vector<Scalar, Dimensions>& operator+=(Vector<Scalar, Dimensions>& first, Scalar second)
{
    for (auto i = 0; i < Dimensions; i++)
    {
        first.v[i] += second;
    }

    return first;
}

template<typename Scalar, int Dimensions>
inline Vector<Scalar, Dimensions>& operator-=(Vector<Scalar, Dimensions>& first, Scalar second)
{
    for (auto i = 0; i < Dimensions; i++)
    {
        first.v[i] -= second;
    }

    return first;
}

template<typename Scalar, int Dimensions>
inline Vector<Scalar, Dimensions>& operator*=(Vector<Scalar, Dimensions>& vector, Scalar scale)
{
    for (auto i = 0; i < Dimensions; i++)
    {
        vector.v[i] *= scale;
    }

    return vector;
}

template<typename Scalar, int Dimensions>
inline Vector<Scalar, Dimensions>& operator/=(Vector<Scalar, Dimensions>& vector, Scalar scale)
{
    for (auto i = 0; i < Dimensions; i++)
    {
        vector.v[i] /= scale;
    }

    return vector;
}

template<typename Scalar>
inline Scalar Dot(const Vector<Scalar, 3>& v0, const Vector<Scalar, 3>& v1)
{
    return v0.v[0] * v1.v[0] + v0.v[1] * v1.v[1] + v0.v[2] * v1.v[2];
}

template<typename Scalar>
inline Vector<Scalar, 3> Cross(const Vector<Scalar, 3>& v0, const Vector<Scalar, 3>& v1)
{
    return Vector<Scalar, 3>{ v0.v[1] * v1.v[2] - v0.v[2] * v1.v[1],
                              v0.v[2] * v1.v[0] - v0.v[0] * v1.v[2],
                              v0.v[0] * v1.v[1] - v0.v[1] * v1.v[0] };
}

template<typename Scalar>
inline Vector<Scalar, 3> Normalize(const Vector<Scalar, 3>& v)
{
    return v / sqrtx(Dot(v, v));
}

template<typename Scalar>
inline Scalar Length(const Vector<Scalar, 3>& v)
{
    return sqrtx(Dot(v, v));
}

template<typename Scalar>
inline Scalar CosOfVectors(const Vector<Scalar, 3>& v0, const Vector<Scalar, 3>& v1)
{
    return Dot(Normalize(v0), Normalize(v1));
}

template<typename Scalar>
struct Vector3 : public Vector<Scalar, 3>
{
    Vector3() = default;
    Vector3(const Scalar* v) : Vector<Scalar, 3>(v)
    {
    }
    Vector3(const std::initializer_list<Scalar>& v) : Vector<Scalar, 3>(v)
    {
    }

    inline Scalar Dot() const
    {
        return ::Dot(*this, *this);
    }
    inline Scalar Dot(const Vector3<Scalar>& other) const
    {
        return ::Dot(*this, other);
    }
    inline Scalar Length() const
    {
        return sqrtx(this->Dot());
    }
    inline Vector3<Scalar> Cross(const Vector3<Scalar>& other) const
    {
        return (Vector3<Scalar>&)::Cross(*this, other);
    }
    inline Vector3<Scalar> Normalize() const
    {
        return (Vector3<Scalar>&)::Normalize(*this);
    }

    inline operator Vector<Scalar, 3>&()
    {
        return *this;
    }
    inline operator const Vector<Scalar, 3>&() const
    {
        return *this;
    }

    static Vector3<Scalar> XAxis, YAxis, ZAxis;
};

template<typename Scalar>
Vector3<Scalar> Vector3<Scalar>::XAxis = { 1, 0, 0 };
template<typename Scalar>
Vector3<Scalar> Vector3<Scalar>::YAxis = { 0, 1, 0 };
template<typename Scalar>
Vector3<Scalar> Vector3<Scalar>::ZAxis = { 0, 0, 1 };

template<typename Scalar>
inline Vector3<Scalar> operator+(const Vector3<Scalar>& first, const Vector3<Scalar>& second)
{
    return (Vector3<Scalar>&)((Vector<Scalar, 3>&)first + (Vector<Scalar, 3>&)second);
}

template<typename Scalar>
inline Vector3<Scalar> operator-(const Vector3<Scalar>& first, const Vector3<Scalar>& second)
{
    return (Vector3<Scalar>&)((Vector<Scalar, 3>&)first - (Vector<Scalar, 3>&)second);
}

template<typename Scalar>
inline Vector3<Scalar> operator*(const Vector3<Scalar>& vector, Scalar scale)
{
    return (Vector3<Scalar>&)((Vector<Scalar, 3>&)vector * scale);
}

template<typename Scalar>
inline Vector3<Scalar> operator*(Scalar scale, const Vector3<Scalar>& vector)
{
    return vector * scale;
}

template<typename Scalar>
inline Vector3<Scalar> operator/(const Vector3<Scalar>& vector, Scalar scale)
{
    return (Vector3<Scalar>&)((Vector<Scalar, 3>&)vector / scale);
}

template<typename Scalar>
inline Vector3<Scalar>& operator+=(Vector3<Scalar>& first, const Vector3<Scalar>& second)
{
    return (Vector3<Scalar>&)((Vector<Scalar, 3>&)first += (Vector<Scalar, 3>&)second);
}

template<typename Scalar>
inline Vector3<Scalar>& operator-=(Vector3<Scalar>& first, const Vector3<Scalar>& second)
{
    return (Vector3<Scalar>&)((Vector<Scalar, 3>&)first -= (Vector<Scalar, 3>&)second);
}

template<typename Scalar>
inline Vector3<Scalar>& operator+=(Vector3<Scalar>& first, Scalar second)
{
    return (Vector3<Scalar>&)((Vector<Scalar, 3>&)first += second);
}

template<typename Scalar>
inline Vector3<Scalar>& operator-=(Vector3<Scalar>& first, Scalar second)
{
    return (Vecotr3<Scalar>&)((Vector<Scalar, 3>&)first -= second);
}

template<typename Scalar>
inline Vector3<Scalar>& operator*=(Vector3<Scalar>& vector, Scalar scale)
{
    return (Vector3<Scalar>&)((Vector<Scalar, 3>&)vector *= scale);
}

template<typename Scalar>
inline Vector3<Scalar>& operator/=(Vector3<Scalar>& vector, Scalar scale)
{
    return (Vector3<Scalar>&)((Vector<Scalar, 3>&)vector /= scale);
}

template<typename Scalar>
struct Quaternion : public Vector<Scalar, 4>
{
    Quaternion() = default;
    Quaternion(const Scalar* v) : Vector<Scalar, 4>(v)
    {
    }
    Quaternion(const std::initializer_list<Scalar>& v) : Vector<Scalar, 4>(v)
    {
    }

    inline Vector<Scalar, 3> Rotate(const Vector<Scalar, 3>& v)
    {
        auto uv = Cross((Vector<Scalar, 3>&)*this, v);
        uv += uv;
        return uv * this->v[3] + Cross((Vector<Scalar, 3>&)*this, uv) + v;
    }

    inline Quaternion<Scalar> operator*(const Quaternion& other)
    {
        auto& t = (Vector<Scalar, 3>&)*this;
        auto& o = (Vector<Scalar, 3>&)other;

        auto w = t.v[3] * o.v[3] - Dot(t, o);
        auto p = Cross(t, o) + o * this->v[3] + t * other.v[3];

        return Quaternion<Scalar>{ p.v[0], p.v[1], p.v[2], w };
    }

    inline Vector<Scalar, 4> ToRotation()
    {
        auto a = acos(this->v[3]);
        auto s = sin(a);

        if (abs(s) < FLT_EPSILON)
        {
            return Vector<Scalar, 4>{ 0, 0, 0, 0 };
        }

        return Vector<Scalar, 4>{ ToDegree(a * 2), this->v[0] / s, this->v[1] / s, this->v[2] / s };
    }

    inline static Quaternion<Scalar> FromRotation(const Vector<Scalar, 4>& rotation)
    {
        auto& axis = *(Vector<Scalar, 3>*)&rotation.v[1];
        auto angle = rotation.v[0];

        if (Dot(axis, axis) < FLT_EPSILON)
        {
            return Identity;
        }

        return FromAxisAngle(axis, ToRadian(angle));
    }

    inline static Quaternion<Scalar> From2Vectors(const Vector<Scalar, 3>& v0, const Vector<Scalar, 3>& v1)
    {
        auto n0 = Normalize(v0);
        auto n1 = Normalize(v1);

        if (n0 == n1)
        {
            return Identity;
        }

        auto h = Normalize(n0 + n1);
        auto q = Cross(n0, h);

        return Quaternion<Scalar>{ q.v[0], q.v[1], q.v[2], Dot(n0, h) };
    }

    inline static Quaternion<Scalar> FromAxisAngle(const Vector<Scalar, 3>& axis, float radian)
    {
        auto a = radian / 2;
        auto c = cos(a);
        auto s = sin(a);
        auto n = Normalize(axis);

        return Quaternion<Scalar>{ s * n.v[0], s * n.v[1], s * n.v[2], c };
    }

    static Quaternion<Scalar> Identity;
};

template<typename Scalar>
Quaternion<Scalar> Quaternion<Scalar>::Identity = { 0, 0, 0, 1 };

typedef Vector3<float>   Vertex;
typedef Vector3<float>   Normal;
typedef Vector<float, 2> Coordinate;

template <typename T, size_t N>
char(&_ArraySizeHelper(T(&array)[N]))[N];
#define COUNTOF(array)(sizeof( _ArraySizeHelper(array)))