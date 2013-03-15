/**
*  SkyTransform.h
*  Projekt 3DVisual
*/
#ifndef VIEWER_SKY_TRANSFORM_DEF
#define VIEWER_SKY_TRANSFORM_DEF 1

namespace Vwr
{
	/**
	*  \class SkyTransform
	*  \brief Handles sky transformations
	*  \author Unknown
	*  \date 29. 4. 2010
	*/
	class SkyTransform : public osg::Transform
	{
	public:

		/**
		*  \fn inline public virtual constant  computeLocalToWorldMatrix(osg::Matrix& matrix,osg::NodeVisitor* nv)
		*  \brief computes local to world transformation matrix 
		*  \param  matrix local matrix
		*  \param  nv     node visitor
		*  \return bool always true
		*/
		virtual bool computeLocalToWorldMatrix(osg::Matrix& matrix,osg::NodeVisitor* nv) const 
		{
			osgUtil::CullVisitor* cv = dynamic_cast<osgUtil::CullVisitor*>(nv);
			if (cv)
			{
				osg::Vec3 eyePointLocal = cv->getEyeLocal();
				matrix.preMultTranslate(eyePointLocal);
			}
			return true;
		}


		/**
		*  \fn inline public virtual constant  computeWorldToLocalMatrix(osg::Matrix& matrix,osg::NodeVisitor* nv)
		*  \brief computes world to local transformation matrix 
		*  \param  matrix world matrix
		*  \param  nv     node visitor
		*  \return bool always true
		*/
		virtual bool computeWorldToLocalMatrix(osg::Matrix& matrix,osg::NodeVisitor* nv) const
		{
			osgUtil::CullVisitor* cv = dynamic_cast<osgUtil::CullVisitor*>(nv);
			if (cv)
			{
				osg::Vec3 eyePointLocal = cv->getEyeLocal();
				matrix.postMultTranslate(-eyePointLocal);
			}
			return true;
		}
	};
}

#endif