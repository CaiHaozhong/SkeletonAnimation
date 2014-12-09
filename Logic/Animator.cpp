#include "Animator.h"


Animator::Animator()
{
}

Animator::Animator( ColladaGeometry* geo, SkeletonData* ske )
{
	this->geometry = geo;
	this->skeleton = ske;
}

Animator::~Animator()
{
}