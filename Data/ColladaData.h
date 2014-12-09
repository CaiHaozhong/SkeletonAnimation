#pragma once
#include "CustomizedType.h"
#include "ColladaDomHeader.h"
#include "ColladaGeometry.h"
#include "SkeletonData.h"
class ColladaData
{
public:
	ColladaData();
	~ColladaData();
	static boost::shared_ptr<ColladaData> createColladaData(const char* file);
	bool initWithFile(const char* file);
	bool initGeometries(boost::shared_ptr<daeElement> root);
	bool initSkeleton(boost::shared_ptr<daeElement> root);
	GeometriesMap getGeometries() const { return mGeometries; }
	void setGeometries(GeometriesMap val) { mGeometries = val; }

	SkeletonData* getSkeletonData() const { return mSkeletonData; }
	void setSkeletonData(SkeletonData* val) { mSkeletonData = val; }
private:
	GeometriesMap mGeometries;
	SkeletonData* mSkeletonData;
	
};