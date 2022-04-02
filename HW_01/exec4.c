/*********************************
Ergasia 1  Askhsh 4  2021-2022
Koutsoukis Nikolaos - 2907 
*********************************/

#include <GL/glut.h>
#include <stdlib.h>
#include <stdio.h>

#define RECTANGLE_POINTS 1
#define RECTANGLE 2
#define RECT_RED_COLOR 3
#define RECT_GREEN_COLOR 4
#define RECT_BLUE_COLOR 5
#define CLEAR_SCREEN 6

#define maxHt 480
#define maxWd 600
#define maxVer 10000

GLint screenWidth= 640;
GLint screenHeight =480;

int menu; // for menu creation
int selection; // for menu choices
int sxhma; // for drawing line or rectangle
int numCorners = 0;

 typedef struct { 
	 GLint x,y;
 }GLintPoint;

 static GLintPoint corner[9];//max edges = 10


// Start from lower left corner
typedef struct edgebucket {
	int ymax; //max y-coordinate of edge
	float xofymin; //x-coordinate of lowest edge point updated only in aet
	float slopeinverse;
}EdgeBucket;

typedef struct edgetabletup {
	// the array will give the scanline number
	// The edge table (ET) with edges entries sorted
	// in increasing y and x of the lower end
	int countEdgeBucket; //no. of edgebuckets
	EdgeBucket buckets[maxVer];

}EdgeTableTuple;

EdgeTableTuple EdgeTable[maxHt], ActiveEdgeTuple;

void drawPoint( int ax, int ay) {
	glBegin(GL_POINTS);
		glVertex2i(ax,ay);
	glEnd();
	glFlush();
}

void drawLine(int ax, int ay, int bx, int by) {
	glBegin(GL_LINES);
		glVertex2i(ax,ay);
		glVertex2i(bx, by);
	glEnd();
	glFlush();
}

// Scanline Function
void initEdgeTable() {
	int i;

	for (i=0; i<maxHt; i++)
		EdgeTable[i].countEdgeBucket = 0;
	
	ActiveEdgeTuple.countEdgeBucket = 0;
}

/* Function to sort an array using insertion sort*/
void insertionSort(EdgeTableTuple *ett) {
	int i,j;
	EdgeBucket temp;

	for (i = 1; i < ett->countEdgeBucket; i++) {
		temp.ymax = ett->buckets[i].ymax;
		temp.xofymin = ett->buckets[i].xofymin;
		temp.slopeinverse = ett->buckets[i].slopeinverse;
		j = i - 1;

		while ((temp.xofymin < ett->buckets[j].xofymin) && (j >= 0)) {
			ett->buckets[j + 1].ymax = ett->buckets[j].ymax;
			ett->buckets[j + 1].xofymin = ett->buckets[j].xofymin;
			ett->buckets[j + 1].slopeinverse = ett->buckets[j].slopeinverse;
			j = j - 1;
		}

		ett->buckets[j + 1].ymax = temp.ymax;
		ett->buckets[j + 1].xofymin = temp.xofymin;
		ett->buckets[j + 1].slopeinverse = temp.slopeinverse;
	}
}


void storeEdgeInTuple (EdgeTableTuple *receiver,int ym,int xm,float slopInv) {
	// both used for edgetable and active edge table..
	// The edge tuple sorted in increasing ymax and x of the lower end.
	(receiver->buckets[(receiver)->countEdgeBucket]).ymax = ym;
	(receiver->buckets[(receiver)->countEdgeBucket]).xofymin = (float)xm;
	(receiver->buckets[(receiver)->countEdgeBucket]).slopeinverse = slopInv;
			
	// sort the buckets
	insertionSort(receiver);
		
	(receiver->countEdgeBucket)++;
	
	
}

void storeEdgeInTable (int x1,int y1, int x2, int y2) {
	float m,minv;
	int ymaxTS,xwithyminTS, scanline; //ts stands for to store
	
	if (x2==x1)
		minv=0.000000;
	
	else {
		m = ((float)(y2-y1))/((float)(x2-x1));
		
		// horizontal lines are not stored in edge table
		if (y2==y1)
			return;
			
		minv = (float)1.0/m;
	}
	
	if (y1>y2) {
		scanline=y2;
		ymaxTS=y1;
		xwithyminTS=x2;
	}
	else {
		scanline=y1;
		ymaxTS=y2;
		xwithyminTS=x1;	
	}
	// the assignment part is done..now storage..
	storeEdgeInTuple(&EdgeTable[scanline],ymaxTS,xwithyminTS,minv);
	
	
}

void removeEdgeByYmax(EdgeTableTuple *Tup,int yy) {
	int i,j;

	for (i=0; i< Tup->countEdgeBucket; i++) {
		if (Tup->buckets[i].ymax == yy) {		
			for ( j = i ; j < Tup->countEdgeBucket -1 ; j++ ) {
				Tup->buckets[j].ymax =Tup->buckets[j+1].ymax;
				Tup->buckets[j].xofymin =Tup->buckets[j+1].xofymin;
				Tup->buckets[j].slopeinverse = Tup->buckets[j+1].slopeinverse;
			}
			Tup->countEdgeBucket--;
			i--;
		}
	}
}	


void updatexbyslopeinv(EdgeTableTuple *Tup) {

	for (int i = 0; i<Tup->countEdgeBucket; i++)
		(Tup->buckets[i]).xofymin =(Tup->buckets[i]).xofymin + (Tup->buckets[i]).slopeinverse;	
}


void ScanlineFill() {
    /* Follow the following rules:
    1. Horizontal edges: Do not include in edge table
    2. Horizontal edges: Drawn either on the bottom or on the top.
    3. Vertices: If local max or min, then count twice, else count
        once.
    4. Either vertices at local minima or at local maxima are drawn.*/
    int i, j, x1, ymax1, x2, ymax2, FillFlag = 0, coordCount;
     
    // we will start from scanline 0;
    // Repeat until last scanline:
    for (i=0; i<maxHt; i++) {   //4. Increment y by 1 (next scan line)
         
        // 1. Move from ET bucket y to the
        // AET those edges whose ymin = y (entering edges)
        for (j=0; j<EdgeTable[i].countEdgeBucket; j++) {
            storeEdgeInTuple(&ActiveEdgeTuple,EdgeTable[i].buckets[j].
                    			ymax,EdgeTable[i].buckets[j].xofymin,
                    			EdgeTable[i].buckets[j].slopeinverse);
        }
         
        // 2. Remove from AET those edges for
        // which y=ymax (not involved in next scan line)
        removeEdgeByYmax(&ActiveEdgeTuple, i);
         
        //sort AET (remember: ET is presorted)
        insertionSort(&ActiveEdgeTuple);
         
        //3. Fill lines on scan line y by using pairs of x-coords from AET
        j = 0;
        FillFlag = 0;
        coordCount = 0;
        x1 = 0;
        x2 = 0;
        ymax1 = 0;
        ymax2 = 0;
        while (j<ActiveEdgeTuple.countEdgeBucket) {
            if (coordCount%2==0) {
                x1 = (int)(ActiveEdgeTuple.buckets[j].xofymin);
                ymax1 = ActiveEdgeTuple.buckets[j].ymax;
                if (x1==x2) {
                /* three cases can arrive-
                    1. lines are towards top of the intersection
                    2. lines are towards bottom
                    3. one line is towards top and other is towards bottom
                */
                    if (((x1==ymax1)&&(x2!=ymax2))||((x1!=ymax1)&&(x2==ymax2))) {
                        x2 = x1;
                        ymax2 = ymax1;
                    }
                    else
                        coordCount++;
                    
                }
                else
                    coordCount++;
                
            }
            else {
                x2 = (int)ActiveEdgeTuple.buckets[j].xofymin;
                ymax2 = ActiveEdgeTuple.buckets[j].ymax;
             
                FillFlag = 0;
                 
                // checking for intersection...
                if (x1==x2) {
                /*three cases can arrive-
                    1. lines are towards top of the intersection
                    2. lines are towards bottom
                    3. one line is towards top and other is towards bottom
                */
                    if (((x1==ymax1)&&(x2!=ymax2))||((x1!=ymax1)&&(x2==ymax2))) {
                        x1 = x2;
                        ymax1 = ymax2;
                    }
                    else {
                        coordCount++;
                        FillFlag = 1;
                    }
                }
                else {
                        coordCount++;
                        FillFlag = 1;
                }
             
             
				if(FillFlag)    
					drawLine(x1, i, x2, i);      
        	}
             
        	j++;
    	}     
    	// 5. For each nonvertical edge remaining in AET, update x for new y
    	updatexbyslopeinv(&ActiveEdgeTuple);
	}
}

void init_corner() {
	for (int i = 0; i < 10; i++) {
		corner[i].x = 0;
		corner[i].y = 0;
	}       
}

void draw_rectangle()
{
	initEdgeTable();
	
	//draw the sides of the rectangle
	for (int i = 0; i < numCorners - 1; i++) 
        drawLine(corner[i].x, corner[i].y, corner[i+1].x, corner[i+1].y);
    
    drawLine(corner[numCorners-1].x, corner[numCorners-1].y, corner[0].x, corner[0].y);	

	//store the edges in the table
	for (int i = 0; i < numCorners - 1; i++) 
        storeEdgeInTable(corner[i].x, corner[i].y, corner[i+1].x, corner[i+1].y);
	
    storeEdgeInTable(corner[numCorners-1].x, corner[numCorners-1].y, corner[0].x, corner[0].y);	


	ScanlineFill();
}


//color edges with the selection color
void color_edges() {
	for (int i = 0; i < numCorners; i++) 
        drawPoint(corner[i].x, corner[i].y);
}

void myInit (void) {
	glClearColor(0.8, 0.8, 0.8, 0.0);   // select clearing (background) color
	glClear(GL_COLOR_BUFFER_BIT);		// clear the screen          
	glColor3f(0.2,0.2,0.2);             // initialize viewing color values
	glPointSize(4);						// set point size
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0.0,(GLfloat)screenWidth, 0.0,(GLfloat)screenHeight);
	glFlush();
}

void clear_screen() {
	glClearColor(0.8, 0.8, 0.8, 0.0);   // select clearing (background) color
	glColor3f(0.2,0.2,0.2);    
	glClear(GL_COLOR_BUFFER_BIT);		// clear the screen
	glFlush();							// send all output to display
}									


//Specifying a rectangle with the mouse
void myMouse (int button, int state, int x1, int y1) {   
   
	//static int numCorners = 0;
	if (button == GLUT_LEFT_BUTTON && state ==GLUT_DOWN) {	
		//draw points
	    drawPoint(x1, screenHeight-y1);

		//draw points and store the edges of the rectangle
        if (sxhma == 1) {
			if (numCorners == 10)
				return;

		    corner[numCorners].x = x1;
		    corner[numCorners].y = screenHeight-y1;
		    drawPoint(corner[numCorners].x,corner[numCorners].y);
		    numCorners++;
        }		
	}
	else if (button == GLUT_MIDDLE_BUTTON && state == GLUT_DOWN)
		clear_screen();  // clear the window
		
	glFlush();
	
}

	// triggered when a Keyboard key is pressed
void keyEvent(unsigned char key, int x, int y) {
	switch (key) {
		//Quit
		case 'Q':
		case 'q':
		exit(-1);
		break;
		default:
		break;
	}
}


void menu_color_selection() {
	color_edges();
	draw_rectangle();
	numCorners = 0;
	sxhma = 0;
	init_corner();
	glColor3f(0.2,0.2,0.2);
}

// Menu choices
void myMenu(int selection)
{
	switch(selection) {
		//select rectangle edges
		case RECTANGLE_POINTS:
			init_corner();
			sxhma = 1;
			numCorners = 0;
			break;

		//select red rectangle
		case RECT_RED_COLOR:
			glColor3f(1.0,0.0,0.0);
			menu_color_selection();
			break;

		//select green rectangle
		case RECT_GREEN_COLOR:
			glColor3f(0.0,1.0,0.0);
			menu_color_selection();
			break;

		//select blue rectangle
		case RECT_BLUE_COLOR:
			glColor3f(0.0,0.0,1.0);
			menu_color_selection();
			break;

		//select to clear the screen
		case CLEAR_SCREEN:
			clear_screen();
			sxhma = 0;
			break;

		default:
			printf("Invalid menu selection\n");
			break;
	}

}



int main(int argc, char **argv) {
	int menu, rectangle_sub_menu;
	
	// initialize toolkit
	glutInit(&argc, argv);
	// set display mode
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
	// set window size
	glutInitWindowSize (screenWidth, screenHeight);
	// set window initial position
	glutInitWindowPosition(1,1);
	// creates a window
	glutCreateWindow ("Exersice 4");
	// registers a keyboard handler
	glutKeyboardFunc(keyEvent);
	glutMouseFunc(myMouse);
	
	// Create a two-level pop-up menu
	rectangle_sub_menu = glutCreateMenu(myMenu);
	glutAddMenuEntry("Red", RECT_RED_COLOR);
	glutAddMenuEntry("Green", RECT_GREEN_COLOR);
	glutAddMenuEntry("Blue", RECT_BLUE_COLOR);

	// menu creation
	menu = glutCreateMenu(myMenu);
	glutAddMenuEntry("Rectangle Edges", RECTANGLE_POINTS);
	glutAddSubMenu("Rectangle", rectangle_sub_menu);
	glutAddMenuEntry("Clear Screen", CLEAR_SCREEN);

	// register menu to right mouse button
	glutAttachMenu(GLUT_RIGHT_BUTTON);

	//Initializations
	myInit();

	// perpetual ever processing loop
	glutMainLoop();

	glutDestroyMenu(rectangle_sub_menu);
	glutDestroyMenu(menu);

	return 0;
}