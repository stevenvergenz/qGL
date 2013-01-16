/***************************************
  Name: Steven Vergenz (s2129096)
  Date: September 27, 2011
  Comment: Draws the bench model in 3d, use arrow keys to rotate
  *************************************/
#include "glwidget.h"

GLWidget::GLWidget(QWidget *parent)
  : QGLWidget(parent)
{
	// initialize window attributes
	setWindowTitle("Steven Vergenz s2129096");
	setFocusPolicy(Qt::StrongFocus);
	resize(500, 500);

	// initialize GL context
	QGLFormat fmt;
	fmt.setDoubleBuffer(true);
	fmt.setAlpha(true);
	fmt.setDepth(true);
	fmt.setStencil(true);
	fmt.setProfile(QGLFormat::CoreProfile);
	fmt.setVersion(3, 3);
	QGLFormat::setDefaultFormat(fmt);
	
	// initialize class variables
	moveSpeed = 100.0f;
	lookSpeed = 6.0f;
	lightSpeed = M_PI/2;
}

GLWidget::~GLWidget()
{
	delete shadow;
	delete lighting;
	sceneDeInit(scene);
}

// comparable to the init() function in tut01
void GLWidget::initializeGL()
{
	// initialize the GL extensions
	GLenum err = glewInit();
	if( err != GLEW_OK ){
		cerr << "Error: " << glewGetErrorString(err) << endl;
		exit(1);
		QApplication::exit(1);
	}
	cout << "GLEW ok!" << endl;
		
	// other initialization
	glClearColor(.5,.5,.5,1);
	glPointSize(5.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	
	// load the model
	scene = new Scene();
	if( !initScene("../qgl/SaintPeter_04/models/", "warehouse_model.dae", scene) ){
		cerr << "Could not load the model." << endl;
		exit(1);
		QApplication::exit(1);
	}
	sceneCenter = (scene->scene_max + scene->scene_min)*0.5f;
	sceneDiagLength = glm::length( scene->scene_max - scene->scene_min );
	
	// initialize the perspective and view matrices
	projectionMat = glm::perspective(60.0f, (float)width()/height(), 
	                                 0.01f*sceneDiagLength,
	                                 10.0f*sceneDiagLength
	                                 );
	cameraPosition = sceneCenter - glm::vec3(0, 0, 1.0*sceneDiagLength);
	cameraDirection = glm::vec3(0, 0, 1);
	
	// initialize the light information
	lightAngle = 0.0f;
	lightDir = 0;
	lightIntensity = glm::vec4(1,1,1,1);
	
	// initialize the lighting model
	lighting = new LightingModel(scene);
	if( !lighting->initializeLightingModel() ){
		cerr<< "Lighting initialization failure!" << endl;
		exit(1);
		QApplication::exit(1);
	}
	
	// initialize the shadow receiver
	shadow = new ShadowModel(scene);
	if( !shadow->initializeShadowReceiver() ){
		cerr << "Shadow initialization failure!" << endl;
		exit(1);
		QApplication::exit(1);
	}
	
	// start the draw timer
	startTimer(1000/DRAW_FPS);
}



void GLWidget::resizeGL(int w, int h)
{
	glViewport(0, 0, (GLsizei)w, (GLsizei)h);
	
	// initialize the perspective and view matrices
	projectionMat = glm::perspective(60.0f, (float)w/h, 
	                                 0.01f*sceneDiagLength,
	                                 10.0f*sceneDiagLength
	                                 );
}

// comparable to the display() function in tut01
void GLWidget::paintGL()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	// update the global information
	viewMat = glm::lookAt(cameraPosition, cameraPosition+cameraDirection, glm::vec3(0, 1, 0));
	lightPosition = glm::vec4(sceneCenter + 
	                          glm::vec3( -0.5*sceneDiagLength*glm::sin(lightAngle), 
	                                     0.9*sceneDiagLength, 
	                                     +0.5*sceneDiagLength*glm::cos(lightAngle))
	                          , 1);
	
	// set global information
	shadow->setViewTransforms(projectionMat, viewMat);
	shadow->setLightingInfo(lightPosition, lightIntensity);
	lighting->setViewTransforms(projectionMat, viewMat);
	lighting->setLightingInfo(lightPosition, lightIntensity);
	shadow->calculateShadowVolume();
	
	// draw the scene with ambient lighting only
	lighting->drawAmbient();
	
	// prepare the stencil buffer for writing
	glEnable(GL_STENCIL_TEST);
	glClear(GL_STENCIL_BUFFER_BIT);
	glStencilFunc(GL_ALWAYS, 0, ~0);
	
	// disable writing to the color and depth buffers
	glColorMask( GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE );
	glDepthMask( GL_FALSE );
	
	// write to the stencil
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glStencilOp(GL_KEEP, GL_KEEP, GL_INCR);
	shadow->drawShadowVolume();
	glCullFace(GL_FRONT);
	glStencilOp(GL_KEEP, GL_KEEP, GL_DECR);
	shadow->drawShadowVolume();
	
	// re-enable color rendering, set stencil rendering
	glCullFace(GL_BACK);
	glColorMask( GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE );
	glStencilFunc( GL_EQUAL, 0, ~0 );
	glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
	glDepthFunc(GL_EQUAL);
	
	// enable blending and draw remainder of scene
	glEnable(GL_BLEND); glBlendFunc(GL_ONE, GL_ONE);
	lighting->drawLit();
	
	// reset gl fields to normal
	glDisable(GL_BLEND);
	glBlendFunc(GL_ONE, GL_ZERO);
	glDisable(GL_STENCIL_TEST);
	glStencilFunc(GL_ALWAYS, 0, ~0);
	glDepthMask(GL_TRUE);
	glDepthFunc(GL_LESS);
	
	// draw the lighting and shadow volume indicators
	shadow->drawIndicators();
	
	// not necessary, automatically called after paintGL() returns
	//swapBuffers();
}

//<------------------------ Handle keyboard input ------------------------->
void GLWidget::keyPressEvent(QKeyEvent *e)
{
	// ignore events generated by auto-repeats
	if( e->isAutoRepeat() ) return;
	
	switch( e->key() ){
	
	// close the program on escape
	case Qt::Key_Escape:
		cout << "Exiting..." << endl;
		exit(1);
		QApplication::exit(1);
		break;
		
	// reset view on space
	case Qt::Key_Space:
		cout << "Resetting viewport" << endl;
		cameraPosition = sceneCenter - glm::vec3(0, 0, 1.0*sceneDiagLength);
		cameraDirection = glm::vec3(0, 0, 1);
		break;
		
	// increase/decrease translation rate
	case Qt::Key_Plus:
		moveSpeed *= 2.0f;
		cout << "Move speed now " << moveSpeed << " U/s" << endl;
		break;
	case Qt::Key_Minus:
		moveSpeed /= 2.0f;
		cout << "Move speed now " << moveSpeed << " U/s" << endl;
		break;
		
	// rotate the light source
	case Qt::Key_Left:
		lightDir -= 1;
		break;
	case Qt::Key_Right:
		lightDir += 1;
		break;
		
	// translate camera location by WADS
	case Qt::Key_W:
		moveDir.z += 1.0f;
		break;
	case Qt::Key_S:
		moveDir.z -= 1.0f;
		break;
	case Qt::Key_A:
		moveDir.x -= 1.0f;
		break;
	case Qt::Key_D:
		moveDir.x += 1.0f;
		break;
	case Qt::Key_Q:
		moveDir.y += 1.0f;
		break;
	case Qt::Key_E:
		moveDir.y -= 1.0f;
		break;
	
	};	
	
}
void GLWidget::keyReleaseEvent(QKeyEvent* e)
{
	if( e->isAutoRepeat() ) return;
	
	switch( e->key() ){
	
	// undo light translations
	case Qt::Key_Left:
		lightDir += 1;
		break;
	case Qt::Key_Right:
		lightDir -= 1;
		break;
		
	// undo camera translations once the key is released
	case Qt::Key_W:
		moveDir.z -= 1.0f;
		break;
	case Qt::Key_S:
		moveDir.z += 1.0f;
		break;
	case Qt::Key_A:
		moveDir.x += 1.0f;
		break;
	case Qt::Key_D:
		moveDir.x -= 1.0f;
		break;
	case Qt::Key_Q:
		moveDir.y -= 1.0f;
		break;
	case Qt::Key_E:
		moveDir.y += 1.0f;
		break;
	
	};	
}

//<-------------------------- Handle Mouse Input ---------------------------->
void GLWidget::mousePressEvent(QMouseEvent* e)
{
	savedPoint = e->pos();
}

void GLWidget::mouseMoveEvent(QMouseEvent* e)
{
	// project 2d mouse vector into the 3d space
	glm::vec2 moveDir = glm::vec2(lookSpeed/width(), lookSpeed/height())
	                * glm::vec2(e->x()-savedPoint.x(), savedPoint.y()-e->y());
	glm::vec4 move = glm::inverse(viewMat) * glm::vec4(moveDir, 0, 0);
	
	// add the 3d vector to the current direction and renormalize
	cameraDirection = glm::normalize( cameraDirection + glm::vec3(move) );
	
	savedPoint = e->pos();
}

//<------------------------- Trigger redraw events --------------------------->
void GLWidget::timerEvent(QTimerEvent* e)
{
	cameraPosition += (moveSpeed/DRAW_FPS) * (
	                moveDir.z*cameraDirection
	                + moveDir.x*glm::cross(cameraDirection, glm::vec3(0, 1, 0))
	                + moveDir.y*glm::cross( glm::cross(cameraDirection, glm::vec3(0, 1, 0)), cameraDirection )
	                );
	lightAngle = fmod((lightSpeed/DRAW_FPS)*lightDir + lightAngle, 2*M_PI);
	
	updateGL();
}
