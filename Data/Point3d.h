#pragma once
class Point3d
{
public:
	Point3d()
	{
		init(0,0,0);
	}
	Point3d(double x,double y, double z)
	{
		init(x,y,z);
	}
	void init(double x,double y, double z)
	{
		this->x = x;
		this->y = y;
		this->z = z;
	}


	Point3d operator+(const Point3d& b)
	{
		return Point3d(x+b.x,y+b.y,z+b.z);
	}

	Point3d operator *= (double b)
	{
		x *= b;
		y *= b;
		z *= b;
		return *this;
	}
	Point3d operator += (const Point3d& b)
	{
		x += b.x;
		y += b.y;
		z += b.z;
		return *this;
	}
	Point3d operator -= (const Point3d& b)
	{
		x -= b.x;
		y -= b.y;
		z -= b.z;
		return *this;
	}
	Point3d operator /(const int k)
	{
		return Point3d(x/k, y/k, z/k);
	}

	Point3d operator *(const float k)
	{
		return Point3d(x*k, y*k, z*k);
	}
	Point3d operator -(const Point3d& other)
	{
		return Point3d(x-other.x, y-other.y, z-other.z);
	}
	double operator* (const Point3d& other)
	{
		return x*other.x + y*other.y + z*other.z;
	}

	Point3d operator=(const Point3d& other)
	{
		this->x = other.x;
		this->y = other.y;
		this->z = other.z;
		return *this;
	}
public:
	double x,y,z;
};