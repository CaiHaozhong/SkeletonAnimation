#include "Joint.h"


Joint::Joint()
{
	mType = NODE;
	keyframes.resize(36);
	mHasAnimation = false;
}

Joint::~Joint()
{
}