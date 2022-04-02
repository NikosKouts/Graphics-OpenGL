/*********************************
Ergasia 1  Askhsh 2  2021-2022
Koutsoukis Nikolaos - 2907 
*********************************/

#include <GL/glut.h>
#include <stdlib.h>
#include <math.h>
#include <stdio.h>

#define LINE 1
#define ELLIPSE 2
#define CLEAN 3
#define EXIT 0

GLint screenWidth = 640;
GLint screenHeight = 480;

int menu; // for menu creation
int selection; // for menu choices
int sxhma; // for drawing line or eclipse
int numCorners = 0;
double t = 0; 

  typedef struct { 
	 GLint x,y;
  }GLintPoint;

void drawPoint( int x, int y, int e) {

	glBegin(GL_POINTS);
		glVertex2i(x,y);
	glEnd();

	glFlush();
}

void drawPointLine( int x, int y, int dx, double e) {
	GLdouble R = 1 - t, G = 0, B = t;

	glColor3d(R*1,G*1,B*1);
	t += (double) 1/dx;
	

	glBegin(GL_POINTS);
		glVertex2i(x,y);
	glEnd();

	glFlush();
}



void drawLine(int x1, int y1, int x2, int y2) {
	int M, temp, dx, dy, p, x, y, w;
	double e = 0.5;

	if ((x2 - x1) == 0)
		M = (y2 - y1);
	else 
		M = (y2 - y1) / (x2 - x1);

	w = 1 - M;

	if(fabs(M) < 1) {

		if(x1 > x2) {
			temp = x1;
			x1 = x2;
			x2 = temp;

			temp = y1;
			y1 = y2;
			y2 = temp;
		}

		dx = fabs(x2 - x1);
		dy = fabs(y2 - y1);

		p = 2*dy - dx;

		x = x1;
		y = y1;

		drawPointLine(x, y, dx, e);	
		

		while (x <= x2) {
			x++;

			if(p >= 0){
				if(M < 1 && y1 < y2)
					y++;
				else
					y--;
				
				p = p + 2*dy - 2*dx;
				e += w;
			}
			else {
				p = p + 2*dy;
				e -= w;
			}

			drawPointLine(x, y, dx, e);	
		}
	}

	if(fabs(M) >= 1) {

		if(y1 > y2) {
			temp = x1;
			x1 = x2;
			x2 = temp;

			temp = y1;
			y1 = y2;
			y2 = temp;
		}

		dx = fabs(x2 - x1);
		dy = fabs(y2 - y1);
		p = 2*dx - dy;

		x = x1;
		y = y1;

		drawPointLine(x, y, dy, e);

		while (y <= y2) {
			y++;

			if(p >= 0){
				if(M >= 1)
					x++;
				else
					x--;
				
				p = p + 2*dx - 2*dy;
				e -= M;
			}
			else {
				p = p + 2*dx;
				e += M;
			}
		
			drawPointLine(x, y, dy, e);
		}
	}

	glFlush();
}

void drawEclipse(int cx, int cy, int ax, int by){
    int rx = fabs(ax - cx);
    int ry = fabs(by - cy);

    //plotting for 1st region of 1st quardant and the slope will be < -1
     //----------------------Region-1------------------------//
    float x = 0;
    float y = ry;
    float p1 = ry * ry - (rx * rx)* ry + (rx * rx) * (0.25) ;
    //slope
    float dx = 2 * (ry * ry) * x;
    float dy = 2 * (rx * rx) * y;
    while(dx < dy) {

        //plot (x,y)
         drawPoint(cx + x  ,  cy+y, 1);
         drawPoint( cx - x,   cy + y, 1);
         drawPoint( cx + x  , cy - y, 1);
         drawPoint( cx - x  , cy - y, 1);
        if(p1 < 0) {
            x = x + 1;
            dx = 2 * (ry * ry) * x;
            p1 = p1 + dx + (ry * ry);
        }
        else {
            x = x + 1;
            y = y - 1;
            dx =  2 * (ry * ry) * x;
            dy =  2 * (rx * rx) * y;
             p1 = p1 + dx - dy +(ry * ry);
        }
    }
    //ploting for 2nd region of 1st quardant and the slope will be > -1
     //----------------------Region-2------------------------//
    float p2 = (ry * ry )* ( x +  0.5) * ( x +  0.5) +  ( rx * rx) * ( y - 1) * ( y - 1) - (rx * rx )* (ry * ry);


    while(y > 0) {
        //plot (x,y)
         drawPoint(cx + x  ,  cy+y, 1);
         drawPoint( cx - x,   cy + y, 1);
         drawPoint( cx + x  , cy - y, 1);
         drawPoint( cx - x  , cy - y, 1);    
        if(p2 > 0) {
            x = x;
            y = y - 1;
            dy = 2 * (rx * rx) * y;
            p2 = p2 - dy + (rx * rx);
        }
        else {
            x = x + 1;
            y = y - 1;
            dy = dy - 2 * (rx * rx) ;
            dx = dx + 2 * (ry * ry) ;
            p2 = p2 + dx -
            dy + (rx * rx);
        }
    }

	glFlush();
}


void myInit (void) {
	glClearColor(0.8, 0.8, 0.8, 0.0);
	glColor3f(0.2,0.2,0.2);    // select clearing (background) color
	glClear(GL_COLOR_BUFFER_BIT);		// clear the screen	
	glPointSize(4);						
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0.0,(GLfloat)screenWidth, 0.0,(GLfloat)screenHeight);
	glFlush();
}
										


void myMouse (int button, int state, int x1, int y1) {
	static GLintPoint corner[3];

	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
		corner[numCorners].x =x1;
		corner[numCorners].y = screenHeight-y1;
		drawPoint(corner[numCorners].x,corner[numCorners].y, 1);
		numCorners++;


		if (numCorners == 2) {
			if (sxhma == 1) {
				t = 0;
				drawLine(corner[0].x, corner[0].y, corner[1].x, corner[1].y);
				numCorners = 0;
				glColor3d(0.2,0.2,0.2);
			}
		}

		if (numCorners == 3) {
			if(sxhma == 2) {
				drawEclipse(corner[0].x, corner[0].y, corner[1].x, corner[2].y);
				numCorners = 0;
				glColor3d(0.2,0.2,0.2);
				}
		
		}

	} 
	else if ((button == GLUT_MIDDLE_BUTTON && state == GLUT_DOWN)){
		glColor3d(0.2,0.2,0.2);
		glClear(GL_COLOR_BUFFER_BIT);  // clear the window
	}
	glFlush();	
}

void keyEvent(unsigned char key, int x, int y) {
	switch (key)
	{
		//Quit
		case 'Q':
		case 'q':
		exit(-1);
		break;
		default:
		break;
	}
}

// Menu choices
void myMenu(int selection) {
	switch(selection) {
		case LINE:
			sxhma = 1;
			numCorners = 0; // set drawing color to grey
			break;

		case ELLIPSE:
			sxhma = 2;
			numCorners = 0; // set drawing color to grey
			break;

		case CLEAN:
			sxhma = 3;
			glClear(GL_COLOR_BUFFER_BIT); // set drawing color to grey
			break;

		case EXIT:
			exit(-1);
	}
}



int main(int argc, char **argv)
{
	// initialize toolkit
	glutInit(&argc, argv);
	// set display mode
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
	// set window size
	glutInitWindowSize (screenWidth, screenHeight);
	// set window initial position
	glutInitWindowPosition(1,1);
	// creates a window
	glutCreateWindow ("EXERCISE 2");
	// registers a keyboard handler
	glutKeyboardFunc(keyEvent);
	glutMouseFunc(myMouse);
	// menu creation
	menu = glutCreateMenu(myMenu);
	glutAddMenuEntry("Line",1);
	glutAddMenuEntry("Ellipse",2);
	glutAddMenuEntry("Clear", 3);
	glutAddMenuEntry("Quit", 0);
	
	// register menu to right mouse button
	glutAttachMenu(GLUT_RIGHT_BUTTON);

	//Initializations
	myInit();

	// perpetual ever processing loop
	glutMainLoop();

	glutDestroyMenu(menu);
	return 0;
}
