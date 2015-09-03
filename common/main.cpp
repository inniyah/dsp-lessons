#include "OGLGraph.hpp"

#include <cstdint>
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <cmath>
#include <unistd.h>

IBaseApp * IBaseApp::MainApp = NULL;

void init(void) {
	glClearColor(0.0f, 0.0f, 0.0f, 0.f); // Black Background
	//glClearDepth(1.0f); // Depth Buffer Setup
	IBaseApp::MainApp->setup();
}

void display(void) {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60.0, (float) 800 / (float) 600, 0.1, 100.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	//float time = glutGet(GLUT_ELAPSED_TIME) / 1000.0;

	IBaseApp::MainApp->update();
	IBaseApp::MainApp->draw();

	glutSwapBuffers ( );
}

void reshape(int w, int h) {
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60.0, (float) w / (float) h, 0.1, 100.0);
	glMatrixMode(GL_MODELVIEW);
	glViewport(0, 0, w, h);
}

void keyboard(unsigned char key, int x, int y) {
	switch (key)  {
	case 27:
		exit(EXIT_SUCCESS);
		break;
	default:
		break;
	}
}

void idle(void) {
	glutPostRedisplay();
}

int main(int argc, char * argv[]) {
	assert(NULL != IBaseApp::MainApp);

	glutInit(&argc, argv);
	init();

	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
	glutInitWindowSize(500, 250);
	glutCreateWindow("OpenGL Graph");
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(keyboard);
	glutIdleFunc(idle);
	glutMainLoop();

	return EXIT_SUCCESS;
}
