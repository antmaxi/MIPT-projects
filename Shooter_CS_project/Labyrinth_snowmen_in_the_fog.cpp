#include <GL/glut.h>    
#include <iostream>    
#include <math.h>    
#include <time.h>    
#include <Windows.h> 
#define N 7//размер лабиринта(нечетный!)
#define DT 10
#define PN 100//кол-во патронов
#define eps 0.01//расст до стенок
#define rad 0.3//радиус сферы
#define pi 3.14159
#define rad1 8//радиус снеговика
#define SN 20//кол-во снеговиков
using namespace std;
float angle = 0, lx = 0, ly = 0, lz = 0, x = 0, z = 0, y = 7, dangle = 0, dmove = 0,
 walkangle = 0, walk = 0, yangle = 0, ydangle = 0, lastTime = 0, ratio, CELL = 30.0,
 dx = 0, dz = 0, FoV = 30, ax = 0, ay = 0, az = 0, velosn = 0.2, snx = -0, snz = +0;
int fv = 1, kt = 0, wid = 1280, hei = 725, yOrigin = hei/2, xOrigin = wid/2, pricel = 0,
x_start, x_end, ball = 0, patr = PN, kolsn = SN;
float color[N][N][3];
float snowangle[N][N];
char map[N][N];
int coord[3][2];
float ambient[]= { 0.5, 0.5, 0.5, 1.0 };  
float mat1_dif[]={0.8f,0.8f,0.0f};
float mat1_amb[]= {0.2f,0.2f,0.2f};
float mat1_spec[]={0.9f,0.9f,0.9f};
float mat1_shininess=0.5f*128;
float mat1_emission[]={0.2, 0.2, 0.2, 1};

struct _ball
{
	float x, y, z;	
	float vx, vy, vz;
	float color[3];
	float lifetime;
	int motion;
} set[PN];

struct _sn
{
	float x, y, z;	
	float vx, vy, vz;
	float snowangle;
	float povorot;
	float color[3];
	int motion;
	int target[2];
	int pos[2];
} sn[SN];

float dist(float x1, float y1, float z1, float x2, float y2, float z2) {
	return(sqrt((x1-x2)*(x1-x2) + (y1-y2)*(y1-y2) + (z1-z2)*(z1-z2)));
}

void proverka() {
	for (int i=0; i<(SN-kolsn); i++) {
		if ((dist(sn[i].x+snx, sn[i].y+rad1, sn[i].z+snz, x, y, z) < 3*rad1) && (sn[i].motion)) {
			printf("LOSE");
			exit(0);
		}
		for (int j=0; j<(PN-patr); j++)
			if (dist(sn[i].x+snx, sn[i].y+rad1, sn[i].z+snz, set[j].x, set[j].y, set[j].z) < rad1 && (sn[i].motion) && (set[j].lifetime > 0)) {
				sn[i].motion = 0;
				set[j].lifetime = 0;
				set[j].motion = 0;
			}
	}
}

void sn_init(_sn * sn) {
	if (kolsn > 0) {
		//printf("%d %d\n", i, )
		int i = SN-kolsn;
		sn[i].x = (N-0.49)*CELL;
		sn[i].y = 0;
		sn[i].z = (x_end-0.49)*CELL ;
		sn[i].pos[0] = N-1;
		sn[i].pos[1] = x_end-1;
		sn[i].target[0] = N-2;
		sn[i].target[1] = x_end-1;
		sn[i].vx = -velosn;
		sn[i].vy = 0;
		sn[i].vz = 0;
		sn[i].color[0] = (rand()%100)/100.0;
		sn[i].color[1] = (rand()%100)/100.0;
		sn[i].color[2] = (rand()%100)/100.0;
		sn[i].motion = 1;
		sn[i].snowangle = (rand()%360)/360.0;
		sn[i].povorot = 0;
		kolsn--;
	}
}

void sn_draw(_sn * sn)
{	

	for (int i=0; i<(SN-kolsn); i++) {
		if (sn[i].motion) {
		float k = rad1/0.75;
		glPushMatrix();
		//glRotatef(sn[i].povorot, 0.0f, 1.0f, 0.0f)
		glTranslatef(sn[i].x+snx, sn[i].y, sn[i].z+snz);
		glTranslatef(0, 0.5*(sin(sn[i].snowangle)+1), 0);
		sn[i].snowangle += 0.5 + 0.25*(rand()%100)/100.0;
		GLfloat front_color[] = {sn[i].color[0], sn[i].color[1], sn[i].color[2], 1};
		glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, front_color);
		glMaterialfv (GL_FRONT_AND_BACK,GL_AMBIENT,mat1_amb);
   		glMaterialfv (GL_FRONT_AND_BACK,GL_SPECULAR,mat1_spec);
   		glMaterialf  (GL_FRONT_AND_BACK,GL_SHININESS,mat1_shininess);
		glTranslatef(0, rad1, 0);
		glutSolidSphere(rad1, 20, 20);
		glTranslatef(0, 0.95*k, 0);	
		glutSolidSphere(rad1/3, 20, 20);
		glPushMatrix();
		glRotatef(sn[i].povorot, 0, 1, 0);
		front_color[0] = 1;
    	front_color[1] = 0;
    	front_color[2] = 0;
   	 	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, front_color);
		glTranslatef(0.05f*k, 0.1f*k, 0.18f*k);	
		//glutSolidSphere(0.03f*k, 10, 10);
		glTranslatef(-0.1f*k, 0, 0);	
		//glutSolidSphere(0.03f*k, 20, 20);
		glPopMatrix();
		front_color[0] = 1;
   		front_color[1] = 0.5;
    	front_color[2] = 0.5;
    	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, front_color);
		glRotatef(0, 1.0f, 0.0f, 0.0f);
		//glutSolidCone(0.08f*k,0.5f*k,10,2);
		glPopMatrix();
		}
	}
}

int kol_st(int a, int b) {
	int kol = 0;
	if (map[a][b-1] == ' ') {
		coord[kol][0] = a;
		coord[kol][1] = b-1;
		kol++;
	}
	if (map[a][b+1] == ' ') {
		coord[kol][0] = a;
		coord[kol][1] = b+1;
		kol++;
	}
	if (map[a+1][b] == ' ') {
		coord[kol][0] = a+1;
		coord[kol][1] = b;
		kol++;
	}
	if (map[a-1][b] == ' ') {
		coord[kol][0] = a-1;
		coord[kol][1] = b;
		kol++;
	}
	return kol;
}

void sn_velocity(_sn * sn, int i) {
	sn[i].vx = (sn[i].target[0] - sn[i].pos[0])*velosn;
	sn[i].vy = 0;
	sn[i].vz = (sn[i].target[1] - sn[i].pos[1])*velosn;
}

void sn_choose(_sn * sn, int a, int b, int i) {
	int kol = kol_st(a, b);
	if (kol == 1) {
		sn[i].target[0] = coord[0][0];
		sn[i].target[1] = coord[0][1];
		sn[i].pos[0] = a;
		sn[i].pos[1] = b;
		sn_velocity(sn, i);
	}
	if (kol == 2) {
		int rnd = rand() % 2;
		for (int j = 0; j < 2; j++){
			if ((coord[j][0] != a) || (coord[j][1] != b)) {
				if (rnd) 
					rnd = 0;
				else {
					sn[i].target[0] = coord[j][0];
					sn[i].target[1] = coord[j][1];
					break;
				}
			}
		}
		sn[i].pos[0] = a;
		sn[i].pos[1] = b;
		sn_velocity(sn, i);
	}
	
	if (kol == 3) {
		int rnd = rand() % 2;
		for (int j = 0; j < 3; j++){
			if ((coord[j][0] != a) || (coord[j][1] != b)) {
				if (rnd) 
					rnd = 0;
				else {
					sn[i].target[0] = coord[j][0];
					sn[i].target[1] = coord[j][1];
					break;
				}
				
			}
		}
		/*printf("@%d %d %d\n", a, b, kol);
		for (int k = 0; k < kol; k++)
			printf("%d %d \n", coord[k][0], coord[k][1]);*/
		sn[i].pos[0] = a;
		sn[i].pos[1] = b;
		sn_velocity(sn, i);
	}
}

void sn_step(_sn * sn) {
	for (int i=0; i<(SN-kolsn); i++) {
			float a = 0+sn[i].x/CELL,
			b = 0+sn[i].z/CELL;
			int c = clock();
			if (!(c%200))
			//	printf("%d %d %d %d %d %d\n", (int)floor(a), (int)floor(b), sn[i].target[0], sn[i].target[1], N, x_end);
			if (((int)floor(a) == sn[i].target[0]) && ((int)floor(b) == sn[i].target[1])) {
				sn_choose(sn, (int)floor(a), (int)floor(b), i);
			//	printf("!!\n");
			}
					
			sn[i].x += sn[i].vx;
			sn[i].y += sn[i].vy;
			sn[i].z += sn[i].vz;	
		
	}
}

void ball_init(_ball *s)
{	
	if (patr > 0) {
		float velocity = 2;
		int i = PN - patr;
		s[i].x = x; 
		s[i].y = y; 
		s[i].z = z;
		s[i].vx = velocity*lx;
		s[i].vy = velocity*ly;
		s[i].vz = velocity*lz;
		s[i].color[0] = 1;//(rand()%100)/100.0;
		s[i].color[1] = 1;//(rand()%100)/100.0;
		s[i].color[2] = 1;//(rand()%100)/100.0;
		s[i].lifetime = 5.0;
		s[i].motion = 1;
		patr--;
	}
	
}

void ball_step(_ball *s)
{
	for (int i=0; i<(PN-patr); i++)
	{	s[i].lifetime -= 0.01;
		if (s[i].motion) {
		float a = (s[i].x + s[i].vx)/CELL,
		b = (s[i].z + s[i].vz)/CELL,
		c = (s[i].y + s[i].vy)/CELL;
		
		//printf("%.1f %.1f %.1f\n", a, b, c);
		if ((c*CELL < 0) /*|| (c*CELL > CELL)*/ || (a*CELL < 0) || (b*CELL < 0) || (a*CELL > N*CELL) || (b*CELL > N*CELL) || (map[(int)floor(a)][(int)floor(b)] == '#') /*|| (map[(int)floor(a+eps)][(int)floor(b+eps)] == '#') || (map[(int)floor(a-eps)][(int)floor(b-eps)] == '#') || (map[(int)floor(a+eps)][(int)floor(b-eps)] == '#')*/ ) {
			s[i].motion = 0;
			continue;	
		}
			
		s[i].x = a*CELL;
		s[i].y = c*CELL;
		s[i].z = b*CELL;
		s[i].vy -=0.01;
		}
		
	}
}

void ball_draw(_ball *s)
{	
	for (int i=0; i<(PN-patr); i++) {
		int f = 1;
		if (s[i].lifetime > 0) {
			f = 0;
			glPushMatrix();
			glTranslatef(s[i].x, s[i].y, s[i].z);	
			GLfloat front_color[] = {s[i].color[0], s[i].color[1], s[i].color[2], 1};
			glMaterialfv(GL_FRONT, GL_DIFFUSE, front_color);
			glMaterialfv(GL_BACK, GL_DIFFUSE, front_color);	
			glutSolidSphere(rad, 10, 10);	
			glPopMatrix();
		}
		if (f)
			ball = 0;
	}
}

void generate_lab(int kol) {
    int ii = kol, oo = kol;
    char b[10000],t='p';
    int a = 1, c[10000], c1 = 2, n, k = 0;
    ii += 2;
    oo += 2;
    for (int x = 0; x < ii*oo; x++){
        b[x] = ' ';
        if (x < oo) b[x] = '#';
        if (x>ii*oo-oo-1) b[x] = '#';
        if (x % oo == 0) b[x] = '#';
        if (x % oo == oo-1) b[x] = '#';
    }
    while (a%2==1)
       a = rand() % (oo-4)+oo*ii-oo+2;
    b[a] = '.';
    b[a-oo] = '.';
    a = 1;
    while (a%2==1)
       a = rand() % (oo-4)+2;
    b[a] = '.';
    b[a+oo] = '.';
    a += oo*2;
    while (c1>1){
        int a1 = a;
        if (k < 15){
            c[c1] = a;
            c1++;
        }
        b[a] = '.';
        k = 0;
        while (a1 == a && k<15){
            n = rand() % 4 + 1;
            if (n == 1 && b[a + 2] ==' ')a++;
            if (n == 2 && b[a + oo*2] ==' ')a += oo;
            if (n == 3 && b[a - oo*2] ==' ' )a -= oo;
            if (n == 4 && b[a - 2] ==' ')a--;
            k++;
        }
        if (k == 15){
            c[c1 - 1] = 0;
            c1 -= 2;
            a = c[c1];
            c1++;
        }
        if (a>0)
            b[a] = '.';
        if (a < 0)
            c1 = 0;
        if (k < 15){
            if (n == 1)a++;
            if (n == 2)a += oo;
            if (n == 3)a -= oo;
            if (n == 4)a--;
            b[a] = '.';
        }
    }
    b[oo*2] = '#';
    for (int x = 0; x < oo*ii; x++){
        if (b[x] == '#')b[x] = 'x';
        if (b[x] == ' ')b[x] = '#';
        if (x>oo * 2||x<oo)
             if (b[x] == '.') b[x] = ' ';
    }
    for (int x = 0; x < oo*ii; x++){
    	if(b[x] == '.')
    		b[x] = ' ';
    	if ((b[x] ==  ' ') && (x - oo < oo)) {
    		b[x] = 's';
    		x_start = x % oo;
    	}
    	if ((b[x] ==  ' ') && ((ii-1)*oo - x < oo)) {
    		b[x] = 'e';
    		x_end = x % oo;
    	}
    		
    }
	for (int i = 0; i < kol; i++) {
		for (int j = 0; j < kol; j++) {
			map[i][j] = b[(i+1)*oo + j + 1];
		}
	}
	printf("\n");
	for (int i = 0; i < kol; i++) {
		for (int j = 0; j < kol; j++)
			printf("%c", map[i][j]);
		printf("\n");
	}
}
 
void drawMap()
{	
	GLfloat front_color[] = {0.75, 0.1, 0.5, 1};
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, front_color);
	glMaterialfv (GL_FRONT_AND_BACK,GL_AMBIENT,mat1_amb);
   	glMaterialfv (GL_FRONT_AND_BACK,GL_SPECULAR,mat1_spec);
   	glMaterialf  (GL_FRONT_AND_BACK,GL_SHININESS,mat1_shininess);
        glBegin(GL_QUADS);
        	glVertex3f(0, 0, 0);
        	glVertex3f(0, 0,  N*CELL);
        	glVertex3f( N*CELL, 0,  N*CELL);
        	glVertex3f( N*CELL, 0, 0);
        glEnd();
    front_color[0] = 0.2;
    front_color[1] = 0.3;
    front_color[2] = 0.8;
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, front_color);
	glMaterialfv (GL_FRONT_AND_BACK,GL_AMBIENT,front_color);
   	glMaterialfv (GL_FRONT_AND_BACK,GL_SPECULAR,front_color);
   	glMaterialf  (GL_FRONT_AND_BACK,GL_SHININESS,mat1_shininess);
        glBegin(GL_QUADS);
       		glVertex3f(0, CELL, 0);
       		glVertex3f( N*CELL, CELL, 0);
        	glVertex3f( N*CELL, CELL,  N*CELL);
        	glVertex3f(0, CELL,  N*CELL);
    	glEnd();
    front_color[0] = 1;
    front_color[1] = 1;
    front_color[2] = 1;
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, front_color);
	glMaterialfv (GL_FRONT_AND_BACK,GL_AMBIENT,front_color);
   	glMaterialfv (GL_FRONT_AND_BACK,GL_SPECULAR,front_color);
   	glMaterialf  (GL_FRONT_AND_BACK,GL_SHININESS,mat1_shininess);
   	glMaterialfv  (GL_FRONT_AND_BACK, GL_EMISSION, mat1_emission);
    	glBegin(GL_QUADS);
       		glVertex3f(N*CELL, 0, x_end*CELL);
       		glVertex3f(N*CELL, CELL, x_end*CELL);
        	glVertex3f(N*CELL, CELL,  (x_end-1)*CELL);
        	glVertex3f(N*CELL, 0,  (x_end-1)*CELL);       	
    	glEnd();
    front_color[0] = 0;
    front_color[1] = 0;
    front_color[2] = 0;
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, front_color);
	glMaterialfv (GL_FRONT_AND_BACK,GL_AMBIENT,front_color);
   	glMaterialfv (GL_FRONT_AND_BACK,GL_SPECULAR,front_color);
   	glMaterialf  (GL_FRONT_AND_BACK,GL_SHININESS,mat1_shininess);
    	glBegin(GL_QUADS);
       		glVertex3f(0, 0, x_start*CELL);
       		glVertex3f(0, 0,  (x_start-1)*CELL);
        	glVertex3f( 0, CELL,  (x_start-1)*CELL);
        	glVertex3f( 0, CELL, x_start*CELL);
    	glEnd();	
	for (int i=0; i<N; i++)
		for (int j=0; j<N; j++)
			if (map[i][j] == '#')
			{	
				GLfloat front_color[] = {color[i][j][0]+0.1, color[i][j][1]+0.1, color[i][j][2]+0.1, 1}; 
				glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, front_color);
				glMaterialfv (GL_FRONT_AND_BACK,GL_AMBIENT,mat1_amb);
   				glMaterialfv (GL_FRONT_AND_BACK,GL_SPECULAR,mat1_spec);
   				glMaterialf  (GL_FRONT_AND_BACK,GL_SHININESS,mat1_shininess);

				//glColor3f(color[i][j][0]+0.1, color[i][j][1]+0.1, color[i][j][2]+0.1);	
				glBegin(GL_QUADS);
				glVertex3f( i*CELL, 0, (j+1)*CELL);
				glVertex3f( i*CELL, CELL,  (j+1)*CELL);
				glVertex3f( i*CELL, CELL,  j*CELL);
				glVertex3f( i*CELL, 0,  j*CELL);

				glVertex3f( i*CELL, 0, j*CELL);
				glVertex3f( i*CELL, CELL,  j*CELL);
				glVertex3f( (i+1)*CELL, CELL,  j*CELL);
				glVertex3f( (i+1)*CELL, 0,  j*CELL);

				glVertex3f( (i+1)*CELL, 0, j*CELL);
				glVertex3f( (i+1)*CELL, CELL,  j*CELL);
				glVertex3f( (i+1)*CELL, CELL,  (j+1)*CELL);
				glVertex3f( (i+1)*CELL, 0,  (j+1)*CELL);

				glVertex3f( (i+1)*CELL, 0, (j+1)*CELL);
				glVertex3f( (i+1)*CELL, CELL,  (j+1)*CELL);
				glVertex3f( i*CELL, CELL,  (j+1)*CELL);
				glVertex3f( i*CELL, 0,  (j+1)*CELL);
				glEnd();				
			}
}

void reshape(int w, int h)
{
	ratio = w/(1.0*h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
	glViewport(0, 0, w, h);
	gluPerspective(45, ratio, 0.1, 1000);      
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();           
}
 
void comPos()
{	
	float a = (x + dx)/CELL,
		b = (z + dz)/CELL;
	if ((a*CELL - eps < 0) || (b*CELL - eps < 0) || (map[(int)floor(a)][(int)floor(b)] == '#') || (map[(int)floor(a+eps)][(int)floor(b+eps)] == '#') || (map[(int)floor(a-eps)][(int)floor(b-eps)] == '#') || (map[(int)floor(a+eps)][(int)floor(b-eps)] == '#') )
		return;
	if ((a*CELL + 1500*eps > N*CELL) || (b*CELL + 1500*eps > N*CELL)) {
		printf("WIN");
		exit(0);
	}
		
	x += dx;
	z += dz;
	//wprintf("%.1f %.1f\n", dx, dz);
	dx = 0;
	dz = 0;
	
}
 
 void display()
{
	comPos();
	lx = sin(angle);
	lz = -cos(angle);
	ly = -sin(yangle);
    
	glutFullScreen();
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    int i = 0;
    //printf("%.2f  %.2f %.2f\n", ax, ay, az);
    glEnable(GL_LIGHT0);   
	GLfloat light_position[] = {ax, 20+ay, -0+az, 1};
    GLfloat light_ambient[] = { 1, 1, 1, 1.0 };
	GLfloat light_diffuse[] = { 0.7, 0.7, 0.7, 1.0 };
	GLfloat light_specular[] = { 1.0, 0.5, 0.5, 1.0 };
	GLfloat light_spot_direction[] = { lx, ly, lz, 0 };

   glLightfv (GL_LIGHT0+i, GL_AMBIENT, light_ambient);
   glLightfv (GL_LIGHT0+i, GL_DIFFUSE, light_diffuse);
   glLightfv (GL_LIGHT0+i, GL_SPECULAR, light_specular);
    glLightfv(GL_LIGHT0+i, GL_POSITION, light_position);
    glLightf(GL_LIGHT0+i, GL_CONSTANT_ATTENUATION, 1 );
    glLightf(GL_LIGHT0+i, GL_LINEAR_ATTENUATION, 0.0001);
    glLightf(GL_LIGHT0+i, GL_QUADRATIC_ATTENUATION, 0.001);
    //glLightf(GL_LIGHT0, GL_SPOT_CUTOFF, 178);
	//glLightfv(GL_LIGHT0+i, GL_SPOT_DIRECTION, light_spot_direction);
    
    /*GLfloat FogColor[4]={0.5,0.5,0.5,0.3};
	glEnable(GL_FOG);
	glFogi(GL_FOG_MODE,GL_EXP);
	glFogf(GL_FOG_DENSITY, 0.015);
	glFogf(GL_FOG_START,20.0);
	glFogf(GL_FOG_END,100.0);
	glFogfv(GL_FOG_COLOR,FogColor);*/
    

//	}
    /*
    */
    gluLookAt( x, y, z,
			x+lx, y+ly, z+lz,
			0.0f, 1.0f, 0.0f);
	//printf("%.2f %.2f %.2f\n", x, y, z);
    i++;
    //glEnable(GL_LIGHT1); 
    GLfloat light_position1[] = {(N-0.1)*CELL, CELL/2, (x_end-0.5)*CELL, 1};
    GLfloat light_ambient1[] = { 1, 1, 1, 1.0 };
	GLfloat light_diffuse1[] = { 1, 0.1, 0.1, 1.0 };
	GLfloat light_specular1[] = { 1.0, 0.1, 0.1, 1.0 };
	GLfloat light_spot_direction1[] = { 1, 0, 1, 1 };
    glLightfv(GL_LIGHT1, GL_AMBIENT, light_ambient1);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, light_diffuse1);
	glLightfv(GL_LIGHT1, GL_SPECULAR, light_specular1);
    glLightfv(GL_LIGHT1, GL_POSITION, light_position1);
    glLightf(GL_LIGHT1, GL_CONSTANT_ATTENUATION, 0.1 );
    glLightf(GL_LIGHT1, GL_LINEAR_ATTENUATION, 0.01);
    glLightf(GL_LIGHT1, GL_QUADRATIC_ATTENUATION, 0.001);
    //glLightf(GL_LIGHT1, GL_SPOT_CUTOFF, 150);
    //glLightfv(GL_LIGHT1, GL_SPOT_DIRECTION, light_spot_direction1);
    
    /*glPushMatrix();
		glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, light_diffuse);
		glMaterialfv (GL_FRONT,GL_AMBIENT,mat1_amb);
   		glMaterialfv (GL_FRONT,GL_SPECULAR,mat1_spec);
   		glMaterialf  (GL_FRONT,GL_SHININESS,mat1_shininess);
		glTranslatef(light_position1[0], light_position1[1], light_position1[2]);		
		glutSolidSphere(3*rad, 100, 100);	
	glPopMatrix();*/
	
	
	if (pricel) {
		glMatrixMode(GL_PROJECTION);
    	glLoadIdentity();
		gluPerspective(FoV, ratio, 0.1, 1000);      
    	glMatrixMode(GL_MODELVIEW);  
		SetCursorPos(wid/2, hei/2);
		
		GLfloat light4_diffuse[] = {1, 1, 1};
    	GLfloat light4_position[] = {0+ax, 6+ay, 0+az, 1.0};
    	GLfloat light4_spot_direction[] = {lx, ly, lz};
    	glEnable(GL_LIGHT4);
    	glLightfv(GL_LIGHT4, GL_AMBIENT, light_ambient1);
		glLightfv(GL_LIGHT4, GL_SPECULAR, light_specular1);
   	 	glLightfv(GL_LIGHT4, GL_DIFFUSE, light4_diffuse);
   		glLightfv(GL_LIGHT4, GL_POSITION, light4_position);
   		glLightf(GL_LIGHT4, GL_SPOT_CUTOFF, 50);
		glLightfv(GL_LIGHT4, GL_SPOT_DIRECTION, light4_spot_direction);
		glLightf(GL_LIGHT4, GL_SPOT_EXPONENT, 1);
		//printf("%.2f  %.2f %.2f\n", ax, ay, az);
	}
	else {
		glMatrixMode(GL_PROJECTION);
    	glLoadIdentity();
		gluPerspective(45, ratio, 0.1, 1000);      
    	glMatrixMode(GL_MODELVIEW);
		glDisable(GL_LIGHT4); 
	}
	drawMap();
	
   /**/
	
	//if (ball) {	
		ball_draw(set);
		ball_step(set);
	//}
	int cl = clock();
	if (!(cl%5000))
		sn_init(sn);
	sn_draw(sn);
	sn_step(sn);
	proverka();
	
    glutSwapBuffers();
}

void light_init ()
{ 
    glClearColor (0.4, 0.4, 0.4, 0.4);
    glEnable(GL_LIGHTING);
    glLightModelf(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);
    //glEnable(GL_NORMALIZE);
}

void key_n(unsigned char key, int xx, int yy)
{
	if (key == 27)
		exit(0);
	int mod = glutGetModifiers();
	float fraction = 2;
	int t = 0;
	if ((key == 'w') || (key == 'W') || (key == 'ц') || (key == 'Ц'))
		t = 119;
	if ((key == 's') || (key == 'S') || (key == 'ы') || (key == 'Ы'))
		t = 115;
	if ((key == 'd') || (key == 'D') || (key == 'в') || (key == 'В'))
		t = 100;
	if ((key == 'a') || (key == 'A') || (key == 'ф') || (key == 'Ф'))
		t = 97;
	switch (t){
		case 97:
			angle-= 0.1;
			break;
		case 100:
			angle+= 0.1;
			break;
		case 119:
				if (mod == GLUT_ACTIVE_SHIFT) {
					dx = lx*fraction*2;
					dz = lz*fraction*2;
					if (walkangle <= 1.0f) {
						walkangle = 359.0f;
					}
      				else {
						walkangle-= 40;
    					yangle += (float)sin(walkangle * pi/180)/50;
    				}
				}
				else {
					dx = lx*fraction;
					dz = lz*fraction;
				}
				break;
		case 115:
			if (mod == GLUT_ACTIVE_SHIFT) {
				dx = -lx*fraction*1.5;
				dz = -lz*fraction*1.5;
				if (walkangle <= 1.0f) {
						walkangle = 359.0f;
					}
      				else {
						walkangle-= 40;
    					yangle += (float)sin(walkangle * pi/180)/50;
    				}
			}
			else {
				dx = -lx*fraction;
				dz = -lz*fraction;
			}
			
			
			break;
	}
	/*
	if (key == 'r')
		ax++;
	if (key == 'f')
		ax--;
	if (key == 't')
		ay++;
	if (key == 'g')
		ay--;
	if (key == 'y')
		az++;
	if (key == 'h')
		az--;		*/
}

void mouseMove(int xx, int yy) {
    if ((xOrigin >= 0) && (kt) && (fabs(xx - xOrigin)>0.001)) {
		angle += (xx - xOrigin) * 0.001f;
	}                                              
	
	if ((yOrigin >= 0) && (kt) && (fabs(yy - yOrigin)>0.001) && ((fabs(yangle) < 0,2) || ((yy - yOrigin)*yangle < 0))) {
		yangle += (yy - yOrigin) * 0.0005f;
	}
	
	if ((yOrigin >= 0) || (xOrigin >= 0))
		kt++;
	if (!kt)
		kt++;
		
	float currentTime = clock();
	if (currentTime - lastTime > 10) {
		SetCursorPos(wid/2, hei/2); 
		kt = 0;
		lastTime = currentTime;
	}
	xOrigin = xx;
	yOrigin = yy;
	
}

void mouseButton(int button, int state, int xx, int yy) {
 
	if (button == GLUT_RIGHT_BUTTON) {
		if (state == GLUT_DOWN) {
			xOrigin = -1;
 			pricel = 1;
 			ShowCursor(1);
 			SetCursorPos(wid/2, hei/2);
 			
		}
 		if (state == GLUT_UP) {
 			ShowCursor(0);
			pricel = 0;	
		}
}
	if (button == GLUT_LEFT_BUTTON) {
		if (state == GLUT_DOWN) {
			ball_init(set);
			ball = 1;
		}
 		else {
		}
 		
	}
	/*if ( button == GLUT_WHEEL_UP )
        {
            printf("Wheel Up\n");
            FoV++;
        }
        else if( button == GLUT_WHEEL_DOWN )
        {
            printf("Wheel Down\n");
            FoV--;
        }
	/*else {
		ShowCursor(0);
	}*/
	
	//yOrigin = yy;
}

void timf(int value) { 
	glutPostRedisplay(); // Redraw windows 
    glutTimerFunc(DT, timf, 0); // Setup next timer 
}

int main (int argc, char * argv[])
{
	srand(time(NULL));
	setlocale(0, "rus");
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DEPTH| GLUT_DOUBLE|GLUT_RGBA);
    
    generate_lab(N);
    z = (x_start-0.5) * CELL;
    x = 0.5*CELL;
    angle = 1.6;
    //glutInitWindowSize(wid, hei);
    glutCreateWindow("PROJECT");
    
    glutReshapeFunc(reshape);
    glutDisplayFunc(display);
	//glutIdleFunc(display);
	glEnable(GL_CULL_FACE);
	glutKeyboardFunc(key_n);

	glutMouseFunc(mouseButton);
	glutPassiveMotionFunc(mouseMove);
	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_TRUE);
	
        for (int i=0; i<N; i++)
		for (int j=0; j<N; j++)	{
			for (int k=0; k<3; k++)
				color[i][j][k] = (rand()%10000)/10000.0+0.05;
		}
		
		for (int i=0; i<SN; i++) {
			sn[i].motion = 0;	
		}
						
		glutSetCursor (GLUT_CURSOR_CROSSHAIR);
		ShowCursor(0);
		glutTimerFunc(DT, timf, 0);
	    light_init();
	    //glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambient);
	    glutSolidSphere(rad, 10, 10);
	    sn_init(sn);
	    //printf("%d\n", kol_st(N-2, x_end-1));
	    //printf("%c", map[6][2]);
        glutMainLoop();
        system("pause");
        return 0;
}                   
					 
