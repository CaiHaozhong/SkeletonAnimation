#pragma once
#include "Matrix.h"
class KeyFrame
{
public:
	KeyFrame();
	~KeyFrame();
	double mTime;
	Matrix* mTransformMatrix;
	void set(double time, Matrix* trans)
	{
		this->mTime= time;
		this->mTransformMatrix = trans;
	}
private:

};
