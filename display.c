// C program to demonstrate 
// drawing a circle using 
// OpenGL 
#include<stdio.h> 
#include<GL/glut.h> 
#include<math.h> 
#include<string.h>
#define pi 3.142857 

// function to initialize 
void myInit (void) 
{ 
	// making background color black as first 
	// 3 arguments all are 0.0 
	glClearColor(0.0, 0.0, 0.0, 1.0); 
	
	// making picture color green (in RGB mode), as middle argument is 1.0 
	glColor3f(0.0, 1.0, 0.0); 
	
	// breadth of picture boundary is 1 pixel 
	glPointSize(1.0); 
	glMatrixMode(GL_PROJECTION); 
	glLoadIdentity(); 
	
	// setting window dimension in X- and Y- direction 
	gluOrtho2D(-780, 780, -420, 420); 
} 

void idle_task(void){
	do_math(); // Honestly I really just want to use OpenGL since that way we get to create visualisers and shit
	return;
}
void display (void) 
{ 
		glClear(GL_COLOR_BUFFER_BIT);
       	glColor3f(1.0, 1.0, 1.0); // Set text color to white
       	glRasterPos2f(10, 10); // Set the position where the text will be displayed
       	char text[] = "Hello, GLUT! This \n is really small tho SMH"; // Text to be displayed
       	for (int i = 0; i < strlen(text); i++) {
           glutBitmapCharacter(GLUT_BITMAP_9_BY_15, text[i]); // Render each character
       	}
       glutSwapBuffers();;
} 

int main (int argc, char** argv) 
{ 
	glutInit(&argc, argv); 
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE); 
	
	// giving window size in X- and Y- direction 
	glutInitWindowSize(1366, 768); 
	glutInitWindowPosition(0, 0); 
	
	// Giving name to window 
	glutCreateWindow("Text Test"); 
	myInit(); 
	
	glutIdleFunc()
	glutDisplayFunc(display); 
	glutMainLoop(); 
} 
