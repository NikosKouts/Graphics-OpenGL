#include <GL/glut.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

int screenWidth = 640;
int screenHeight = 480;
int selection = 1;
bool looks = true;
const int MAX_POINTS = 20;

//Coordinates for Pixels
typedef struct{
	GLint x, y;
}GLintPoint;

//Number of edges for it's polygon
int CornersPolygon = 0;
int CornersRect = 0;

void drawPoint(GLint x, GLint y);

GLintPoint clipWindow[2];
GLintPoint RectangleEdges[20];
GLintPoint clipWindowPoints[4];

//Fill the Polygon
void drayPolygon() {
    glColor3f(0.0, 0.0, 1.0);
    
    glBegin(GL_POLYGON);
    for (int i = 0; i < CornersPolygon; i++)
		glVertex2i(RectangleEdges[i].x,RectangleEdges[i].y);
    
    glEnd();
    glFlush();    
}

//Fill the Rectangle
void drawRectangle() {
	glColor3f(1.0, 0.0, 0.0);

	glRecti(clipWindow[0].x, clipWindow[0].y, clipWindow[1].x, clipWindow[1].y);
}

//---------------------------------------------Sutherland – Hodgeman clipping algorithm---------------------------------------------------------

// Returns x-value of point of intersection of two
// lines
int x_intersect(int x1, int y1, int x2, int y2, int x3, int y3, int x4, int y4) {
    int num = (x1*y2 - y1*x2) * (x3-x4) - (x1-x2) * (x3*y4 - y3*x4);
    int den = (x1-x2) * (y3-y4) - (y1-y2) * (x3-x4);
    return num/den;
}
  
// Returns y-value of point of intersection of
// two lines
int y_intersect(int x1, int y1, int x2, int y2, int x3, int y3, int x4, int y4) {
    int num = (x1*y2 - y1*x2) * (y3-y4) - (y1-y2) * (x3*y4 - y3*x4);
    int den = (x1-x2) * (y3-y4) - (y1-y2) * (x3-x4);
    return num/den;
}
  
// This functions clips all the edges w.r.t one clip
// edge of clipping area
void clip(int x1, int y1, int x2, int y2) {
    GLintPoint new_points[MAX_POINTS];
	int new_CornersPolygon = 0;
	
  
    // (ix,iy),(kx,ky) are the co-ordinate values of
    // the points
    for (int i = 0; i < CornersPolygon; i++) {
        // i and k form a line in polygon
        int k = (i+1) % CornersPolygon;
        int ix = RectangleEdges[i].x, iy = RectangleEdges[i].y;
        int kx = RectangleEdges[k].x, ky = RectangleEdges[k].y;
  
        // Calculating position of first point
        // w.r.t. clipper line
        int i_pos = (x2-x1) * (iy-y1) - (y2-y1) * (ix-x1);
  
        // Calculating position of second point
        // w.r.t. clipper line
        int k_pos = (x2-x1) * (ky-y1) - (y2-y1) * (kx-x1);
  
        // Case 1 : When both points are inside
        if (i_pos < 0  && k_pos < 0) {
            //Only second point is added
            new_points[new_CornersPolygon].x = kx;
            new_points[new_CornersPolygon].y = ky;
            new_CornersPolygon++;
        }
  
        // Case 2: When only first point is outside
        else if (i_pos >= 0  && k_pos < 0) {
            // Point of intersection with edge
            // and the second point is added
            new_points[new_CornersPolygon].x = x_intersect(x1, y1, x2, y2, ix, iy, kx, ky);
            new_points[new_CornersPolygon].y = y_intersect(x1, y1, x2, y2, ix, iy, kx, ky);
            new_CornersPolygon++;
  
            new_points[new_CornersPolygon].x = kx;
            new_points[new_CornersPolygon].y = ky;
            new_CornersPolygon++;
        }
  
        // Case 3: When only second point is outside
        else if (i_pos < 0  && k_pos >= 0) {
            //Only point of intersection with edge is added
            new_points[new_CornersPolygon].x = x_intersect(x1,
                              y1, x2, y2, ix, iy, kx, ky);
            new_points[new_CornersPolygon].y = y_intersect(x1,
                              y1, x2, y2, ix, iy, kx, ky);
            new_CornersPolygon++;
        }
  
        // Case 4: When both points are outside
        else {
            //No points are added
        }
    }
  
    // Copying new points into original array
    // and changing the no. of vertices
    CornersPolygon = new_CornersPolygon;
    for (int i = 0; i < CornersPolygon; i++) {
        RectangleEdges[i].x = new_points[i].x;
        RectangleEdges[i].y = new_points[i].y;
    }
}
  
// Implements Sutherland–Hodgman algorithm
void suthHodgClip(int clipper_size) {
    //i and k are two consecutive indexes
    for (int i=0; i<clipper_size; i++) {
        int k = (i+1) % clipper_size;
  
        // We pass the current array of vertices, it's size
        // and the end points of the selected clipper line
        clip(clipWindowPoints[i].x, clipWindowPoints[i].y, clipWindowPoints[k].x, clipWindowPoints[k].y);
    }
}

//-------------------------------------------End of Sutherland – Hodgeman clipping algorithm---------------------------------------------------------------------------

void init (void) {
	glClear(GL_COLOR_BUFFER_BIT);//The buffers currently enabled for color writing 
	glClearColor (0.8, 0.8, 0.8, 0.0);	// select clearing (background) color
   	glColor3f(1.0, 0.0, 0.0); 			
			
	glMatrixMode(GL_PROJECTION);		
	glLoadIdentity(); 
	gluOrtho2D(0.0,(GLdouble)screenWidth,0.0,(GLdouble)screenHeight);	//define a 2-D orthographic projection matrix 
}


void display(void){
	
	glClear(GL_COLOR_BUFFER_BIT);	//The buffers currently enabled for color writing
	glColor3f(1.0, 0.0, 0.0); 
    if(selection == 2) {
        if (CornersRect == 2) {
		    drawRectangle();
        }
    }
    drayPolygon();
	glFlush();
}


void myMouse (int button, int state, int x1, int y1) {   

	//static int numCorners = 0;
	if (button == GLUT_LEFT_BUTTON && state ==GLUT_DOWN) {
        //First option: create the polygon
        if(selection == 1) {
            if(CornersPolygon > 10)
            return;

            RectangleEdges[CornersPolygon].x = x1;
            RectangleEdges[CornersPolygon].y = screenHeight-y1;
            drawPoint(RectangleEdges[CornersPolygon].x,RectangleEdges[CornersPolygon].y);
            CornersPolygon++;  
        }
        //Second option: create the rectangle
        else if(selection == 2) {
				
		    clipWindow[0].x= x1;
		    clipWindow[0].y =screenHeight - y1;
		    CornersRect = 1;
		    glutPostRedisplay();
			looks = true;	
        }
    }
    //Thrird option: clean the screen
	else if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN){
		glClear(GL_COLOR_BUFFER_BIT);
        CornersPolygon = 0;
        CornersRect = 0;
        selection = 1;

    }
		
	glFlush();
}

//To create the rectangle
void moveMouse(int xnew, int ynew){
    if(selection == 2) {
	    clipWindow[1].x= xnew;
	    clipWindow[1].y =screenHeight - ynew;
	    CornersRect = 2;
	    glutPostRedisplay();
    }
 }

// To use the F1 key
static void SpecialKeyFunc( int Key, int x, int y ) {
	switch (Key) {
        case GLUT_KEY_F1:
            drayPolygon();
    		selection = 2;
            break;
	}
}

//Find all the edges of the rectangle
void createClipperEdges() {
    //Find x
	if (clipWindow[0].x < clipWindow[1].x) {
		clipWindowPoints[0].x = clipWindow[0].x;
		clipWindowPoints[1].x = clipWindow[0].x;
		clipWindowPoints[2].x = clipWindow[1].x;
		clipWindowPoints[3].x = clipWindow[1].x;
	}
	else {
		clipWindowPoints[0].x = clipWindow[1].x;
		clipWindowPoints[1].x = clipWindow[1].x;
		clipWindowPoints[2].x = clipWindow[0].x;
		clipWindowPoints[3].x = clipWindow[0].x;
	}

    //Find y
	if (clipWindow[0].y < clipWindow[1].y) {
		clipWindowPoints[0].y = clipWindow[0].y;
		clipWindowPoints[1].y = clipWindow[1].y;
		clipWindowPoints[2].y = clipWindow[1].y;
		clipWindowPoints[3].y = clipWindow[0].y;
	}
	else {
		clipWindowPoints[0].y = clipWindow[1].y;
		clipWindowPoints[1].y = clipWindow[0].y;
		clipWindowPoints[2].y = clipWindow[0].y;
		clipWindowPoints[3].y = clipWindow[1].y;
	}

}

void keyEvent(unsigned char key,int x,int y){

    switch (key) {
        //Clipping the polygon
        case 'C':
		case 'c':
			createClipperEdges();
			suthHodgClip(4);
			glClear(GL_COLOR_BUFFER_BIT);
            if (looks) {
			    drawRectangle();
            }
			drayPolygon();
			break;
        //Visible rectangle
		case ' ':
			if (looks) {
				glClear(GL_COLOR_BUFFER_BIT);
				drayPolygon();
				looks = false;
			}
			else{
				glClear(GL_COLOR_BUFFER_BIT);
				drawRectangle();
				drayPolygon();
				looks = true;
			}
		break;
        //Exit
        case 'Q':
        case 'q':
	        exit(0);
	        break;	      
        default:	
	    break;
    }
}


int main(int argc, char** argv) 
{
	//initialize toolkit
	glutInit(&argc, argv);
	//set display mode 
	glutInitDisplayMode (GLUT_SINGLE | GLUT_RGB); 
	//set window size
	glutInitWindowSize (640, 480);
	//set window initial position
	glutInitWindowPosition (1, 1);
	//creates a window 
	glutCreateWindow ("Exersice 2_6");
			
	glutDisplayFunc(display);
	
	//registers a keyboard handler
	glutKeyboardFunc(keyEvent);
	glutSpecialFunc(SpecialKeyFunc);
	glutMouseFunc(myMouse);	
	glutMotionFunc(moveMouse);
	//additional initializatons
	init ();
	
	//perpetual ever processing loop
	glutMainLoop(); 
	return 0;	
}      

void drawPoint(GLint x, GLint y)
{
	glColor3f(0.0, 0.0, 1.0);
		
	glBegin(GL_POINTS);
		glVertex2i(x,y);
	glEnd();
	glFlush();
}