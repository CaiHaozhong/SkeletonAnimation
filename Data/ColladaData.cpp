#include "ColladaData.h"
ColladaData::ColladaData()
{
}

ColladaData::~ColladaData()
{
}

boost::shared_ptr<ColladaData> ColladaData::createColladaData( const char* file )
{
	boost::shared_ptr<ColladaData> ret(new ColladaData);
	if(ret->initWithFile(file))
		return ret;
	ret.reset();
	return ret;
}

bool ColladaData::initWithFile( const char* file )
{
	bool ret = false;
	do 
	{
		DAE* dae = new DAE;
		boost::shared_ptr<daeElement> root((daeElement*)(dae->open(file)));
		if(root.get() == NULL)
			break;
		if(!initGeometries(root))
			break;
		if(!initSkeleton(root))
			break;
		ret = true;
	} while (0);
	return ret;
}

bool ColladaData::initGeometries( boost::shared_ptr<daeElement> root )
{
	bool ret = false;
	do 
	{
		std::vector<daeElement*> geometries = root->getDAE()->getDatabase()->typeLookup(domGeometry::ID());
		for (int i = 0; i < geometries.size(); i++)
		{
			String id;
			boost::shared_ptr<ColladaGeometry> geo(new ColladaGeometry);
			if(!geo->initWithGeometry(geometries.at(i)))
				return false;
			if(!geo->initSkinInformation(root->getDAE()->getDatabase()->typeLookup<domController>().at(0)))
				return false;
			id = geometries.at(i)->getID();
			mGeometries.insert(GeometriesMap::value_type(id,geo));
		}
		ret = true;
	} while (0);
	return ret;
}

bool ColladaData::initSkeleton( boost::shared_ptr<daeElement> root )
{
	bool ret = false;
	do 
	{
		mSkeletonData = new SkeletonData;

		/*初始化树形结构，包括局部变换矩阵和父子结构*/
		daeElement* rootNode = root->getDAE()->getDatabase()->idLookup("astroBoy_newSkeleton_deformation_rig",root->getDocument());		
		if(! mSkeletonData->initWithRootNode(rootNode))
			break;

		/*读取InverseBindMatrix和BindShapeMatrix*/
		vector<domController*> controllerNode = root->getDAE()->getDatabase()->typeLookup<domController>();		
		if(! mSkeletonData->setUpInvertBindMatrixAndBindShapeMatrix(controllerNode.at(0)))
			break;

		mSkeletonData->setUpWorldMatrix();

		mSkeletonData->computeSkinMatrix();

		mSkeletonData->initKeyFrames(root->getDAE()->getDatabase()->typeLookup<domLibrary_animations>().at(0));

		ret = true;
	} while (0);
	return ret;
}
