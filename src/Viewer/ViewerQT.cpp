#include "QOSG/ViewerQT.h"

#include "Viewer/PickHandler.h"
#include "Viewer/CameraManipulator.h"
#include "Viewer/CoreGraph.h"
#include "Util/ApplicationConfig.h"
#include "Core/Core.h"


// Kostan test
#include <osg/Notify>
#include <osg/MatrixTransform>
#include <osg/ShapeDrawable>
#include <osg/PositionAttitudeTransform>
#include <osg/Geometry>
#include <osg/Texture2D>
#include <osg/Geode>
#include <osg/LightSource>
#include <osg/TexGenNode>
#include <osg/TexMat>
#include <osgDB/WriteFile>
#include <osgUtil/Optimizer>
#include <osgDB/Registry>
#include <osgDB/ReadFile>
#include <osgViewer/Viewer>
osg::StateSet* createProjectorState(osg::Texture2D* texture, osg::Vec3 projectorPos, osg::Vec3 projectorDirection, float projectorFOV) {

    osg::StateSet* stateset = new osg::StateSet;

    /* 1. Load the texture that will be projected */
    if (!texture){
        texture = new osg::Texture2D();
        osg::Image* foo = osgDB::readImageFile("foo.jpg");
        if (foo == NULL)
            printf("Error: Image \"foo.jpg\" has not been read!\n");
        texture->setImage(foo);
    }
    texture->setWrap(osg::Texture::WRAP_S, osg::Texture::CLAMP_TO_BORDER);
    texture->setWrap(osg::Texture::WRAP_T, osg::Texture::CLAMP_TO_BORDER);
    texture->setWrap(osg::Texture::WRAP_R, osg::Texture::CLAMP_TO_BORDER);
    stateset->setTextureAttributeAndModes(1, texture, osg::StateAttribute::ON);


    // set up tex gens
    stateset->setTextureMode(1, GL_TEXTURE_GEN_S,
        osg::StateAttribute::ON);
    stateset->setTextureMode(1, GL_TEXTURE_GEN_T,
        osg::StateAttribute::ON);
    stateset->setTextureMode(1, GL_TEXTURE_GEN_R,
        osg::StateAttribute::ON);
    stateset->setTextureMode(1, GL_TEXTURE_GEN_Q,
        osg::StateAttribute::ON);

    /* 2. Load the Shaders */
    osg::ref_ptr<osg::Program> projProg(new osg::Program);
    const std::string vertexSource =
        "varying vec4 projCoord; \n"
        "void main() \n"
        "{ \n"
        "	projCoord = gl_TextureMatrix[1] * gl_Vertex; \n"
        "	gl_Position = ftransform(); \n"
        "	gl_FrontColor = gl_Color; \n"
        "} \n";
    osg::ref_ptr<osg::Shader> projvertexShader(new osg::Shader(
        osg::Shader::VERTEX, vertexSource));

    const std::string fragmentSource =
        "uniform sampler2D projectionMap; \n"
        "varying vec4 projCoord; \n"
        "void main() \n"
        "{ \n"
        "	vec4 dividedCoord = projCoord / projCoord.w; \n"
        "	vec2 texCoord = dividedCoord.st + vec2(0.5f, 0.5f); \n"
        "	vec4 color = texture2D(projectionMap, texCoord); \n"
        "	gl_FragColor = color * gl_Color; \n"
        "} \n";
    osg::ref_ptr<osg::Shader> projfragShader(new osg::Shader(
        osg::Shader::FRAGMENT, fragmentSource));

    projProg->addShader(projvertexShader.get());
    projProg->addShader(projfragShader.get());

    /* 3. Handover the texture to the fragment shader via uniform */
    osg::Uniform* texUniform = new osg::Uniform(osg::Uniform::SAMPLER_2D,
        "projectionMap");
    texUniform->set(1);
    stateset->addUniform(texUniform);

    /* 4. set Texture matrix*/
    osg::TexMat* texMat = new osg::TexMat;
    osg::Matrix mat;
    osg::Vec3 up(0.0f, 0.0f, 1.0f);
    mat = osg::Matrixd::lookAt(projectorPos, projectorPos + projectorDirection, up)
        * osg::Matrixd::perspective(projectorFOV, 1.0, 0.1, 100);
    texMat->setMatrix(mat);
    stateset->setTextureAttributeAndModes(1, texMat, osg::StateAttribute::ON);

    stateset->setAttribute(projProg.get());
    return stateset;
}


osg::Geode* createBase()
{
    osg::Geode* baseGeode = new osg::Geode();
    osg::Geometry* baseGeometry = new osg::Geometry();

    baseGeode->addDrawable(baseGeometry);

    //base
    osg::Vec3Array* vertices = new osg::Vec3Array;
    vertices->push_back(osg::Vec3(-1.5, -1.345, 0)); // lb
    vertices->push_back(osg::Vec3(0, -1.345, 0)); // rb
    vertices->push_back(osg::Vec3(0, 0, 0)); // rt
    vertices->push_back(osg::Vec3(-1.5, 0, 0)); // lt
    vertices->push_back(osg::Vec3(0, 0, 2.1)); // rt1
    vertices->push_back(osg::Vec3(-1.5, 0, 2.1)); // lt1
    baseGeometry->setVertexArray(vertices);

    osg::DrawElementsUInt* base = new osg::DrawElementsUInt(osg::PrimitiveSet::QUADS, 0);
    base->push_back(3);
    base->push_back(2);
    base->push_back(1);
    base->push_back(0);

    baseGeometry->addPrimitiveSet(base);

    osg::DrawElementsUInt* baseBehind = new osg::DrawElementsUInt(osg::PrimitiveSet::QUADS, 0);
    baseBehind->push_back(5);
    baseBehind->push_back(4);
    baseBehind->push_back(2);
    baseBehind->push_back(3);

    baseGeometry->addPrimitiveSet(baseBehind);

    return baseGeode;
}


void QOSG::ViewerQT::createProjectorScene() {

    unsigned int tex_width = 2048;
    unsigned int tex_height = 2048;
    unsigned int samples = 0;
    unsigned int colorSamples = 0;

    osg::Camera::RenderTargetImplementation renderImplementation = osg::Camera::FRAME_BUFFER_OBJECT;

    osg::Node* loadedModel = this->cg->getScene();
    //osg::Node* loadedModel = osgDB::readNodeFile("cessna.osgt");

    if (!loadedModel)
    {
        return ;
    }

    // create a transform to spin the model.
    osg::MatrixTransform* loadedModelTransform = new osg::MatrixTransform;
    loadedModelTransform->addChild(loadedModel);

    osg::NodeCallback* nc = new osg::AnimationPathCallback(loadedModelTransform->getBound().center(), osg::Vec3(0.0f, 0.0f, 1.0f), osg::inDegrees(45.0f));
    //loadedModelTransform->setUpdateCallback(nc);

    if (!loadedModelTransform) return;

    // create a group to contain the flag and the pre rendering camera.
    osg::Group* parent = new osg::Group;

    // texture to render to and to use for rendering of flag.
    osg::Texture2D* texture = new osg::Texture2D;
    texture->setTextureSize(tex_width, tex_height);
    texture->setInternalFormat(GL_RGBA);
    texture->setFilter(osg::Texture2D::MIN_FILTER, osg::Texture2D::LINEAR);
    texture->setFilter(osg::Texture2D::MAG_FILTER, osg::Texture2D::LINEAR);



    // first create the geometry of the flag of which to view.

    renderCamera = new osg::Camera;
    // then create the camera node to do the render to texture
    {
        //osg::Camera* camera = new osg::Camera;

        // set up the background color and clear mask.
        renderCamera->setClearColor(osg::Vec4(0.0f, 0.0f, 0.0f, 1.0f));
        renderCamera->setClearMask(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        const osg::BoundingSphere& bs = loadedModelTransform->getBound();
        if (!bs.valid())
        {
            return ;
        }

        float znear = 1.0f*bs.radius();
        float zfar = 3.0f*bs.radius();

        // 1:1 aspect ratio as per flag geometry below.
        float proj_top = 0.5f*znear;
        float proj_right = 0.5f*znear;

        znear *= 0.9f;
        zfar *= 1.1f;

        // set up projection.
        //renderCamera->setProjectionMatrixAsPerspective( 60, static_cast<double>( width() )/static_cast<double>( height() ), 0.01, 100/*appConf->getValue( "Viewer.Display.ViewDistance" ).toFloat()*/ );
        renderCamera->setProjectionMatrixAsFrustum(-proj_right, proj_right, -proj_top, proj_top, znear, zfar);
        //camera->setProjectionMatrix(osg::Matrixd::perspective(110.0f, 1.0, 0.1, 100)/*identity()*/);

        // set view
        renderCamera->setReferenceFrame(osg::Transform::ABSOLUTE_RF);
        renderCamera->setViewMatrixAsLookAt(bs.center() - osg::Vec3(0.0f, 2.0f, 0.0f)*bs.radius(), bs.center(), osg::Vec3(0.0f, 0.0f, 1.0f));
        //camera->setViewMatrixAsLookAt(osg::Vec3(0.0f, -3.0f, 3.0f), osg::Vec3(0.0f, 0.0f, 0.0f), osg::Vec3(0.0f, 0.0f, 1.0f));

        // set viewport
        renderCamera->setViewport(0, 0, tex_width, tex_height);

        // set the camera to render before the main camera.
        renderCamera->setRenderOrder(osg::Camera::PRE_RENDER);

        // tell the camera to use OpenGL frame buffer object where supported.
        renderCamera->setRenderTargetImplementation(renderImplementation);

        {
            // attach the texture and use it as the color buffer.
            renderCamera->attach(osg::Camera::COLOR_BUFFER, texture,
                0, 0, false,
                samples, colorSamples);
        }


        // add subgraph to render
        renderCamera->addChild(loadedModelTransform);
    }


    projectorScene = new osg::Group;
    osg::Geode* base = createBase();

    osg::Vec3 projectorPos = osg::Vec3(-1.88, -0.95f, 1.72f);//osg::Vec3(0, -1.345f, 1.72f);
    osg::Vec3 projectorDirection = osg::Vec3d( -0.75, 0, 0.238 )-projectorPos;
    float projectorAngle =90;
    /* Enable projective texturing for all objects of this node */
    //root->setStateSet(createProjectorState(texture, projectorPos, projectorDirection, projectorAngle));

    base->setStateSet(createProjectorState(texture, projectorPos, projectorDirection, projectorAngle));

    projectorScene->addChild(base);
    projectorScene->addChild(renderCamera);

}

void QOSG::ViewerQT::toggleProjectorView()
{
    if(projectorViewEnabled)
    {
        this->setSceneData(cg->getScene());
        cg->setCamera( this->getCamera() );
        projectorViewEnabled = false;
    }
    else
    {
        if(projectorScene == nullptr)
        {
            createProjectorScene();
            this->setSceneData(projectorScene);
        }
        cg->setCamera( renderCamera );
        projectorViewEnabled = true;
    }
}

//************************

QOSG::ViewerQT::ViewerQT( QWidget* parent , const char* name , const QGLWidget* shareWidget , WindowFlags f , Vwr::CoreGraph* cg ):
	AdapterWidget( parent, name, shareWidget, f )
{
    // (Kostan) Projector view init
    projectorViewEnabled = true;
    renderCamera = nullptr;
    projectorScene = nullptr;

	this->cg = cg;
    if(projectorViewEnabled)
    {
        createProjectorScene();
        this->setSceneData(projectorScene);
        cg->setCamera( renderCamera );
    }
    else{
        cg->setCamera( this->getCamera() );
        this->setSceneData(cg->getScene());
    }
	this->mIsClicAruco=false;

	appConf = Util::ApplicationConfig::get();

	osg::DisplaySettings::instance()->setStereo( ( appConf->getValue( "Viewer.Display.Stereoscopic" ).toInt() ? true : false ) );
	osg::DisplaySettings::instance()->setStereoMode( osg::DisplaySettings::ANAGLYPHIC );

	getCamera()->setViewport( new osg::Viewport( 0,0,width(),height() ) );
    getCamera()->setProjectionMatrixAsPerspective( 30, static_cast<double>( width() )/static_cast<double>( height() ), 0.01, appConf->getValue( "Viewer.Display.ViewDistance" ).toFloat() );
	getCamera()->setGraphicsContext( getGraphicsWindow() );
	getCamera()->setComputeNearFarMode( osg::CullSettings::DO_NOT_COMPUTE_NEAR_FAR );

	manipulator = new Vwr::CameraManipulator( cg );
	QObject::connect( manipulator, SIGNAL( sendMouseRotation( osg::Quat ) ),
					  cg, SLOT( updateGraphRotByMouse( osg::Quat ) ) );
	QObject::connect( manipulator, SIGNAL( sendFaceDetRotation( osg::Quat ) ),
					  cg, SLOT( updateGraphRotByFaceDet( osg::Quat ) ) );

	pickHandler = new Vwr::PickHandler( manipulator, cg );

	addEventHandler( new osgViewer::StatsHandler );
	addEventHandler( pickHandler );
    //setCameraManipulator( manipulator );

	// Gloger: background to white color after disabling skybox
    getCamera()->setClearColor( osg::Vec4( 0, 1, 0, 1 ) );
    getCamera()->setViewMatrixAsLookAt( osg::Vec3d( -0.665, -1.345, 0.825f), osg::Vec3d( -0.75, 0, 0.238 ), osg::Vec3d( 0, 0, 1 ) );

	setThreadingModel( osgViewer::ViewerBase::SingleThreaded );

	connect( &_timer, SIGNAL( timeout() ), this, SLOT( updateGL() ) );

	_timer.start( 10 );
}

Vwr::PickHandler* QOSG::ViewerQT::getPickHandler() const
{
	return pickHandler;
}

Vwr::CameraManipulator* QOSG::ViewerQT::getCameraManipulator() const
{
	return manipulator;
}

void QOSG::ViewerQT::reloadConfig()
{
	manipulator->setMaxSpeed( appConf->getValue( "Viewer.CameraManipulator.MaxSpeed" ).toFloat() );
	manipulator->setTrackballSize( appConf->getValue( "Viewer.CameraManipulator.Sensitivity" ).toFloat() );

	osg::DisplaySettings::instance()->setStereo( ( appConf->getValue( "Viewer.Display.Stereoscopic" ).toInt() ? true : false ) );
}

void QOSG::ViewerQT::paintGL()
{
	frame();
	cg->update();
}

void QOSG::ViewerQT::moveMouseAruco( double positionX,double positionY,bool isClick,int windowX,int windowY ,Qt::MouseButton button )
{
	//qDebug() << positionX << "  " << positionY << "         " << isClick;

	float wieverX = static_cast<float>( positionX * static_cast<float>( this->width() ) );
	float wieverY = static_cast<float>( positionY * static_cast<float>( this->height() ) );

	int screenX = static_cast<int>( positionX * this->width()  + this->x() + windowX + 8 );
	int screenY = static_cast<int>( positionY * this->height() + this->y() + windowY + 28 );

	this->cursor().setPos( screenX, screenY );

	qDebug() << screenX << "  " << screenY ;
	wieverY = ( static_cast<float>( this->height() ) - wieverY );

	if ( isClick != mIsClicAruco ) {
		mIsClicAruco = isClick;

		if ( isClick ) {
			this->getEventQueue()->mouseButtonPress( wieverX, wieverY,button );
			this->getEventQueue()->mouseMotion( wieverX, wieverY );
		}
		else {
			this->getEventQueue()->mouseButtonRelease( wieverX, wieverY, button );
			return;
		}
	}
	this->getEventQueue()->mouseMotion( wieverX, wieverY );
}

void QOSG::ViewerQT::moveMouseKinect( double positionX,double positionY,double speed,bool isClick,int windowX,int windowY ,Qt::MouseButton button )
{
	//qDebug() << positionX << "  " << positionY << "         " << isClick;
	positionX /=640.0;
	positionY/=480.0;

	float wieverX = static_cast<float>( positionX * static_cast<float>( this->width() ) );
	float wieverY = static_cast<float>( positionY * static_cast<float>( this->height() ) );

	int screenX = static_cast<int>( positionX * this->width()  + this->x() + windowX + 8 );
	int screenY = static_cast<int>( positionY * this->height() + this->y() + windowY + 28 );

	this->cursor().setPos( screenX, screenY );

	qDebug() << screenX << "  " << screenY ;
	wieverY = ( static_cast<float>( this->height() ) - wieverY );

	if ( isClick != mIsClicAruco ) {
		mIsClicAruco = isClick;

		if ( isClick ) {
			this->getEventQueue()->mouseButtonPress( wieverX, wieverY,button );
			this->getEventQueue()->mouseMotion( wieverX, wieverY );
		}
		else {
			this->getEventQueue()->mouseButtonRelease( wieverX, wieverY, button );
			return;
		}
	}
	this->getEventQueue()->mouseMotion( wieverX, wieverY );
}
