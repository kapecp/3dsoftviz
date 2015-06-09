/**
*  DataHelper.h
*  Projekt 3DVisual
*/
#ifndef VIEWER_DATA_HELPER_DEF
#define VIEWER_DATA_HELPER_DEF 1

#include "Data/Node.h"
#include "Data/Edge.h"
#include "Data/Type.h"


#include <osg/Geode>
#include "osg/Texture2D"

#include <QString>

namespace Vwr {
////////////////////////////////////////////////////////////////////////////////////////////////////
/// \class  DataHelper
///
/// \brief  Trieda generuj rozne testovacie grafy
///
/// \author Michal Paprcka, Michal Pavlik, Pavol Perdik
/// \version 1.0a
/// \date   13. 12. 2009
////////////////////////////////////////////////////////////////////////////////////////////////////
class DataHelper
{
public:
	/**
	             * \param center stred gule
	             * \param radius polomer gule
	             * \returns objekt gula
	             * \brief
	             * Vytvori objekt gulu.
	             *
	             **/
	osg::ref_ptr<osg::Geode> static getSphereGeode( osg::Vec3 center, float radius );
	/**
	             * \param count pocet vektorov
	             * \returns pole vygenerovanych vektorov
	             **/
	osg::ref_ptr<osg::Vec3Array> static getInitialVectors( int count );
	/**
	             * \brief Vygeneruje jednoduhcy graf - Kocku
	             *
	             * \deprecated
	             **/
	//static void generateCube(std::vector<Data::Node*> *nodes,std::vector<Data::Edge*> *edges,std::vector<Data::Type*> *types);

	/**
	             * \brief Vygeneruje jednoduhcy graf - Kocku
	             *
	             **/
	//static void generateCube2(std::vector<Data::Node*> *nodes,std::vector<Data::Edge*> *edges,std::vector<Data::Type*> *types);

	/**
	             * \brief Vygeneruje jednoduhcy graf - Stvorsten
	             *
	             **/
	//static void generatePyramid(std::vector<Data::Node*> *nodes,std::vector<Data::Edge*> *edges,std::vector<Data::Type*> *types);

	/**
	             * \brief Vygeneruje jednoduhcy graf - Kocku 2x2
	             *
	             * \deprecated
	             **/
	//static void generateMultiCube(std::vector<Data::Node*> *nodes,std::vector<Data::Edge*> *edges,std::vector<Data::Type*> *types);

	/**
	             * \brief Vygeneruje jednoduhcy graf - Valec so specifikovanymi rozmermi
	             *
	             * \param pocetUzlovNaPodstave  pocet uzlov na podstave
	             * \param pocetUzlovNaVysku     pocet uzlov na vysku valca
	             **/
	static void generateCylinder( QMap<qlonglong, osg::ref_ptr<Data::Node> >* nodes, QMap<qlonglong, osg::ref_ptr<Data::Edge> >* edges,QMap<qlonglong, Data::Type*>* types, int pocetUzlovNaPodstave, int pocetUzlovNaVysku );

	/**
	             * \brief Vygeneruje testovaciu scenu pozostavajucu z niekolkych samostatnych grafov
	             *
	             **/
	//static void generateTestingGraph(std::vector<Data::Node*> *nodes,std::vector<Data::Edge*> *edges,std::vector<Data::Type*> *types);
	/**
	             * Vypocita krajne pozicie bodov hrany
	             *
	             * \param inNode                 pozicia zdrojoveho uzla
	             * \param outNode                pozicia cieloveho uzla
	             **/
	static osg::ref_ptr<osg::Vec3Array> getEdgeVectors( osg::ref_ptr<Data::Node> inNode, osg::ref_ptr<Data::Node> outNode );
	/**
	             * Nacita textury vo formate PNG.
	             *
	             **/
	static osg::ref_ptr<osg::Texture2D> readTextureFromFile( QString path );

	/**
	    *  \fn public static  createTexture
	    *  \brief Creates texture from image
	    *  \param      image    texture image
	    *  \return osg::ref_ptr texture
	    */
	static osg::ref_ptr<osg::Texture2D> createTexture( osg::ref_ptr<osg::Image> image );

	/**
	    *  \fn public static  getRandomNumber(int lowest, int highest)
	    *  \brief returns random number
	    *  \param      lowest     lowest random number
	    *  \param      highest     highest random number
	    *  \return int random number
	    */
	static int getRandomNumber( int lowest, int highest );


	/**
	    *  \fn public static  getMassCenter(osg::ref_ptr<osg::Vec3Array> coordinates
	    *  \brief Returns mass center of given points in space
	    *  \param    coordinates    points in space
	    *  \return osg::Vec3f position of mass center
	    */
	static osg::Vec3f getMassCenter( osg::ref_ptr<osg::Vec3Array> coordinates );
};
}

#endif
