#ifndef CAMERA_HELPER_DEF
#define CAMERA_HELPER_DEF 1

#include <osg/Camera>
#include <QList>
#include <QDebug>

namespace Util {

/// Utility class for camera operations and coordinate transformations.
/// Source: DP - 2011 - Ukrop Jakub
class CameraHelper
{
public:
	/// Camera must be set in this method.
	static void setCamera( osg::Camera* camera );
	static osg::ref_ptr<osg::Camera> getCamera()
	{
		return camera;
	}

	static osg::Vec3f getEye()
	{
		return getViewCoords()[0];
	}

	static osg::Vec3f getCenter()
	{
		return getViewCoords()[1];
	}

	static osg::Vec3f getUp()
	{
		return getViewCoords()[2];
	}

	static osg::Vec3f byView( osg::Vec3f v, bool rotOnly = false )
	{
		if ( !rotOnly ) {
			return v * camera->getViewMatrix();
		}
		else {
			return camera->getViewMatrix().getRotate() * v;
		}
	}
	static osg::Vec3f byProjection( osg::Vec3f v )
	{
		return v * camera->getProjectionMatrix();
	}
	static osg::Vec3f byWindow( osg::Vec3f v )
	{
		return v * camera->getViewport()->computeWindowMatrix();
	}
	static osg::Vec3f byFull( osg::Vec3f v )
	{
		return byWindow( byProjection( byView( v ) ) );
	}

	static osg::Vec3f byViewInv( osg::Vec3f v, bool rotOnly = false )
	{
		if ( !rotOnly ) {
			return v * osg::Matrixd::inverse( camera->getViewMatrix() );
		}
		else {
			return camera->getViewMatrix().getRotate().inverse() * v;
		}
	}
	static osg::Vec3f byProjectionInv( osg::Vec3f v )
	{
		return v * osg::Matrixd::inverse( camera->getProjectionMatrix() );
	}
	static osg::Vec3f byWindowInv( osg::Vec3f v )
	{
		return v * osg::Matrixd::inverse( camera->getViewport()->computeWindowMatrix() );
	}
	static osg::Vec3f byFullInv( osg::Vec3f v )
	{
		return byViewInv( byProjectionInv( byWindowInv( v ) ) );
	}

	static osg::Vec2f worldToScreen( osg::Vec3f scenePos )
	{
		scenePos = byFull( scenePos );
		return osg::Vec2f( scenePos.x(), scenePos.y() );
	}

	static osg::Vec3f screenToWorld( osg::Vec2f screenPos, float projDepth )
	{
		osg::Vec3f newPos( screenPos.x(), screenPos.x(), 0 );
		newPos = byWindowInv( newPos );
		newPos.z() = projDepth;
		newPos = byViewInv( byProjectionInv( newPos ) );
	}

	static osg::Vec3f moveByScreenVector( osg::Vec3f scenePos, osg::Vec2f screenVect )
	{
		osg::Vec3f screenPos = Util::CameraHelper::byFull( scenePos );
		screenPos = osg::Vec3f( screenPos.x() + screenVect.x(),
								screenPos.y() + screenVect.y(), screenPos.z() + 0 );
		return Util::CameraHelper::byFullInv( screenPos );
	}

	static float getProjectiveDepth( osg::Vec3f scenePos )
	{
		return byProjection( byView( scenePos ) ).z();
	}

	static int windowWidth()
	{
		return camera->getViewport()->width();
	}
	static int windowHeight()
	{
		return camera->getViewport()->height();
	}
	static int windowX()
	{
		return camera->getViewport()->x();
	}
	static int windowY()
	{
		return camera->getViewport()->y();
	}
	static osg::Matrix getProjectionMatrix()
	{
		return osg::Matrix::ortho2D( windowX(), windowWidth(), windowY(), windowHeight() );
	}

	static void printVec( osg::Vec4f vec, QString str = "" )
	{
		qDebug() << str << " [" << vec.x() << "," << vec.y() << "," << vec.z() << "," << vec.w() << "]";
	}
	static void printVec( osg::Vec3f vec, QString str = "" )
	{
		qDebug() << str << " [" << vec.x() << "," << vec.y() << "," << vec.z() << "]";
	}
	static void printVec( osg::Vec2f vec, QString str = "" )
	{
		qDebug() << str << " [" << vec.x() << "," << vec.y() << "]";
	}
private:
	CameraHelper();
	~CameraHelper();

	static QList<osg::Vec3f> getViewCoords()
	{
		osg::Vec3f eye, center, up;
		camera->getViewMatrixAsLookAt( eye, center, up );
		QList<osg::Vec3f> vects;
		vects << eye << center << up;
		return vects;
	}

	static osg::Camera* camera;
};
}

#endif
