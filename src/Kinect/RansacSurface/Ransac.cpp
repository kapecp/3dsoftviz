
#include "Kinect/RansacSurface/Ransac.h"
#include "Kinect/RansacSurface/PlaneAlgorithm.h"
#include "Util/ApplicationConfig.h"

#include<fstream>
#include <string>
#include <time.h>

#include "QDebug"

Kinect::Ransac::Ransac()
{
	calc= new Kinect::PlaneAlgorithm();
}

Kinect::Ransac::~Ransac()
{
}

void Kinect::Ransac::findPoints(int *vectorDepths)
{

	int x = rand() % 3000;
	int y = rand() % 3000;
	int z = rand() % 3000;
	calc->setRandomPoint(0,((x / 50)+1), ((x % 50)+1), vectorDepths[x]);
	calc->setRandomPoint(1, ((y / 50) + 1), ((y % 50) + 1), vectorDepths[y]);
	calc->setRandomPoint(2, ((z / 50) + 1), ((z % 50) + 1), vectorDepths[z]);

	if (calc->calculatePlane() == 0)
	{
		int count = 0;
		for (int i = 0; i < 3000; i++)
		{
			if (calc->findIntersection(((i / 50) + 1), ((i % 50) + 1), vectorDepths[i]) == 0)
				count++;
		}
		if (count > 10)
		{
			calc->logfile << "Pocet najdenych bodov: " << count << endl << endl;
		}
	}
}

void Kinect::Ransac::calculate()
{

	string line;
	srand(time(NULL));
	char cline[1000];
	int numbers[3000];
	int i = 0;
	char *pch;
	char *context = NULL;
	std::string data= Util::ApplicationConfig::get()->getValue("Kinect.Surface.DATA").toStdString();

	ifstream myfile(data);
	if(!myfile.is_open()){
		qDebug() << "FILE dont find." << endl;
	}

	if (myfile.is_open())
	{
		qDebug() << "OPEN." << endl;
		qDebug() << "Calculate start";
		while (getline(myfile, line))
		{
			strcpy_s(cline, line.c_str());
			pch = strtok_s(cline, " ",&context);
			while (pch != NULL)
			{

				numbers[i] = atoi(pch);
				i++;
				pch = strtok_s(NULL, " ", &context);
			}
		}

		for (int j = 0; j < 1000; j++)
		{
			findPoints(numbers);
		}
		myfile.close();
		qDebug() << "Calculate end";
	}
}

