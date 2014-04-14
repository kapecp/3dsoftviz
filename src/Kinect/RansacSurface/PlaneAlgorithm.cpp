#include "Kinect/RansacSurface/PlaneAlgorithm.h"
#include "Util/ApplicationConfig.h"

#include "QDebug"

Kinect::PlaneAlgorithm::PlaneAlgorithm()
{
	points[0] = Vector3( -1, -1, -1 );
	points[1] = Vector3( -1, -1, -1 );
	points[2] = Vector3( -1, -1, -1 );

	std::string log= Util::ApplicationConfig::get()->getValue("Kinect.Surface.LOG").toStdString();

	logfile.open(log, std::ios::app);

	if (!logfile.is_open())
	{
		qDebug() << "Error opening file!";
	}
}

Kinect::PlaneAlgorithm::~PlaneAlgorithm()
{
	logfile.close();
}

int Kinect::PlaneAlgorithm::setRandomPoint(int i, int x, int y, int z)
{
	if (i > 3)
	{
		qDebug() << "Maximum bodov je 3!\n";
		return -1;
	}
	else
	{
		points[i] = Vector3( x, y, z );
	}

	return 0;
}

int Kinect::PlaneAlgorithm::calculatePlane()
{
	Vector3 u = u.subtract(points[1], points[0]);
	Vector3 v = v.subtract(points[2], points[0]);
	//qDebug() << "vektor: [" << u.x << "," << u.y << "," << u.z << "]\n";
	//qDebug() << "vektor: [" << v.x << "," << v.y << "," << v.z << "]\n";
	int w1 = (u.mY*v.mZ) - (u.mZ*v.mY);
	int w2 = (u.mZ*v.mX) - (u.mX*v.mZ);
	int w3 = (u.mX*v.mY) - (u.mY*v.mX);
	w = Vector3( w1, w2, w3 );
	//qDebug() << "vektor: [" << w.x << "," << w.y << "," << w.z << "]\n";
	d = (points[1].mX*w1) + (points[1].mY*w2) + (points[1].mZ*w3);
	d *= -1;
	//qDebug() << "d: " << d ;
	logfile << w1 << "x + " << w2 << "y + " << w3 << "z + " << d << " = 0\n";

	if ((w1 == 0) && (w2 == 0) && (w3 == 0) && (d == 0))
	{
		return -1;
	}

	return 0;

}

int Kinect::PlaneAlgorithm::findIntersection(int x, int y, int z)
{

	int result = (w.mX * x) + (w.mY * y) + (w.mX *z) + d;
	//qDebug() << vysledok ;
	if ((result < 5) && (result > -5))
	{
		return 0;
	}
	else
	{
		return -1;
	}
	return 0;
}
