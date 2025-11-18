// -------------------------------------------
// gMini : a minimal OpenGL/GLUT application
// for 3D graphics.
// Copyright (C) 2006-2008 Tamy Boubekeur
// All rights reserved.
// -------------------------------------------

// -------------------------------------------
// Disclaimer: this code is dirty in the
// meaning that there is no attention paid to
// proper class attribute access, memory
// management or optimisation of any kind. It
// is designed for quick-and-dirty testing
// purpose.
// -------------------------------------------

#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <string>
#include <cstdio>
#include <cstdlib>
#include <map>
#include <cmath>
#include <random>

#include <GL/glut.h>
#include <float.h>
#include "src/Vec3.h"
#include "src/Camera.h"

#include "src/utils.h"
#include "src/Cell.h"
#include "src/GameOfLife.h"
using namespace std;


// Fonction permettant d'afficher des points avec une certaine couleur
void DrawPoints(std::vector<Vec3> TableauPoints, float R, float G, float B){
    glPointSize(4.0);
    glBegin(GL_POINTS);
    glColor3f(R,G,B); // Couleur de la courbe
    for(long unsigned int i = 0 ; i < TableauPoints.size() ; i++){
        glVertex3f(TableauPoints[i][0], TableauPoints[i][1], TableauPoints[i][2]);
    }
    glEnd();
}

void DrawCube(std::vector<Vec3> TableauPoints, float R, float G, float B){
    
    // A CORRIGER !!!!!!!!! PROBLEME D'INDICE, pas les mêmes que sur mon brouillon

    glBegin(GL_LINE_STRIP);
    glColor3f(R,G,B); // Couleur de la courbe
    for(int i = 0 ; i < 4 ; i++){
        glVertex3f(TableauPoints[i][0], TableauPoints[i][1], TableauPoints[i][2]);
    }
    glEnd();

    glBegin(GL_LINE_STRIP);
    glColor3f(R,G,B); // Couleur de la courbe
    for(int i = 4 ; i < 8 ; i++){
        glVertex3f(TableauPoints[i][0], TableauPoints[i][1], TableauPoints[i][2]);
    }
    glEnd();

    glBegin(GL_LINE_STRIP);
    glColor3f(R,G,B); // Couleur de la courbe
    glVertex3f(TableauPoints[0][0], TableauPoints[0][1], TableauPoints[0][2]);
    glVertex3f(TableauPoints[3][0], TableauPoints[3][1], TableauPoints[3][2]);
    glVertex3f(TableauPoints[7][0], TableauPoints[7][1], TableauPoints[7][2]);
    glVertex3f(TableauPoints[4][0], TableauPoints[4][1], TableauPoints[4][2]);
    glVertex3f(TableauPoints[0][0], TableauPoints[0][1], TableauPoints[0][2]);
    glEnd();

    glBegin(GL_LINE_STRIP);
    glColor3f(R,G,B); // Couleur de la courbe
    glVertex3f(TableauPoints[1][0], TableauPoints[1][1], TableauPoints[1][2]);
    glVertex3f(TableauPoints[2][0], TableauPoints[2][1], TableauPoints[2][2]);
    glVertex3f(TableauPoints[6][0], TableauPoints[6][1], TableauPoints[6][2]);
    glVertex3f(TableauPoints[5][0], TableauPoints[5][1], TableauPoints[5][2]);
    glVertex3f(TableauPoints[1][0], TableauPoints[1][1], TableauPoints[1][2]);
    glEnd();
}


enum DisplayMode{ WIRE=0, SOLID=1, LIGHTED_WIRE=2, LIGHTED=3 };

struct GridData {
    // Information to store in the grid's cells
    std::vector<Vec3> vertices ; // Sommets du maillage qui sont dans la cellule
    std::vector<Vec3> normals ; // Normales du maillage qui sont dans la cellule

    Vec3 representant ;

    Vec3 getRepresentant(){
        return representant/vertices.size() ;
    }
    Vec3 getNormal(){
        Vec3 norm = Vec3(0.0,0.0,0.0) ;
        int nbN = normals.size() ;
        for(int i = 0 ; i < nbN ; i++){
            norm += normals[i] ;
        }
        norm.normalize() ;
        return norm ;
    }

};
struct Grid {
    std::vector<GridData> cells;

    Vec3 minPos, maxPos;
    int resolution;

    int getCellX(Vec3 pos) { return resolution * (pos[0] - minPos[0]) / (maxPos[0] - minPos[0]); }
    int getCellY(Vec3 pos) { return resolution * (pos[1] - minPos[1]) / (maxPos[1] - minPos[1]); }
    int getCellZ(Vec3 pos) { return resolution * (pos[2] - minPos[2]) / (maxPos[2] - minPos[2]); }

    int getIndex(int x, int y, int z) { return x * resolution * resolution + y * resolution + z; }
    int getIndex(Vec3 pos) { return getCellX(pos) * resolution * resolution + getCellY(pos) * resolution + getCellZ(pos); }
};



//Transformation made of a rotation and translation
struct Transformation {
    Mat3 rotation;
    Vec3 translation;
};

void collect_one_ring (std::vector<Vec3> const & i_vertices,
                       std::vector< Triangle > const & i_triangles,
                       std::vector<std::vector<unsigned int> > & o_one_ring) {//one-ring of each vertex, i.e. a list of vertices with which it shares an edge
    //Initialiser le vecetur de o_one_ring de la taille du vecteur vertices
    o_one_ring = std::vector<std::vector<unsigned int>>(i_vertices.size());
    //Parcourir les triangles et ajouter les voisins dans le 1-voisinage
    for (auto& t : i_triangles) {
        unsigned int i0 = t[0];
        unsigned int i1 = t[1];
        unsigned int i2 = t[2];
        if (std::find(o_one_ring[i0].begin(), o_one_ring[i0].end(), i1) == o_one_ring[i0].end()) o_one_ring[i0].push_back(i1);
        if (std::find(o_one_ring[i0].begin(), o_one_ring[i0].end(), i2) == o_one_ring[i0].end()) o_one_ring[i0].push_back(i2);
        
        if (std::find(o_one_ring[i1].begin(), o_one_ring[i1].end(), i0) == o_one_ring[i1].end()) o_one_ring[i1].push_back(i0);
        if (std::find(o_one_ring[i1].begin(), o_one_ring[i1].end(), i2) == o_one_ring[i1].end()) o_one_ring[i1].push_back(i2);
        
        if (std::find(o_one_ring[i2].begin(), o_one_ring[i2].end(), i0) == o_one_ring[i2].end()) o_one_ring[i2].push_back(i0);
        if (std::find(o_one_ring[i2].begin(), o_one_ring[i2].end(), i1) == o_one_ring[i2].end()) o_one_ring[i2].push_back(i1);
    }

}

// -------------------------------------------
// OpenGL/GLUT application code.
// -------------------------------------------

static GLint window;
static unsigned int SCREENWIDTH = 1600;
static unsigned int SCREENHEIGHT = 900;
static Camera camera;
static bool mouseRotatePressed = false;
static bool mouseMovePressed = false;
static bool mouseZoomPressed = false;
static int lastX=0, lastY=0, lastZoom=0;
static bool fullScreen = false;

// ------------------------------------
// File I/O
// ------------------------------------
bool saveOFF( const std::string & filename ,
              std::vector< Vec3 > const & i_vertices ,
              std::vector< Vec3 > const & i_normals ,
              std::vector< Triangle > const & i_triangles,
              std::vector< Vec3 > const & i_triangle_normals ,
              bool save_normals = true ) {
    std::ofstream myfile;
    myfile.open(filename.c_str());
    if (!myfile.is_open()) {
        std::cout << filename << " cannot be opened" << std::endl;
        return false;
    }

    myfile << "OFF" << std::endl ;

    unsigned int n_vertices = i_vertices.size() , n_triangles = i_triangles.size();
    myfile << n_vertices << " " << n_triangles << " 0" << std::endl;

    for( unsigned int v = 0 ; v < n_vertices ; ++v ) {
        myfile << i_vertices[v][0] << " " << i_vertices[v][1] << " " << i_vertices[v][2] << " ";
        if (save_normals) myfile << i_normals[v][0] << " " << i_normals[v][1] << " " << i_normals[v][2] << std::endl;
        else myfile << std::endl;
    }
    for( unsigned int f = 0 ; f < n_triangles ; ++f ) {
        myfile << 3 << " " << i_triangles[f][0] << " " << i_triangles[f][1] << " " << i_triangles[f][2]<< " ";
        if (save_normals) myfile << i_triangle_normals[f][0] << " " << i_triangle_normals[f][1] << " " << i_triangle_normals[f][2];
        myfile << std::endl;
    }
    myfile.close();
    return true;
}

void openOFF( std::string const & filename,
              std::vector<Vec3> & o_vertices,
              std::vector<Vec3> & o_normals,
              std::vector< Triangle > & o_triangles,
              std::vector< Vec3 > & o_triangle_normals,
              bool load_normals = true )
{
    std::ifstream myfile;
    myfile.open(filename.c_str());
    if (!myfile.is_open())
    {
        std::cout << filename << " cannot be opened" << std::endl;
        return;
    }

    std::string magic_s;
    int iLine = 0;

    myfile >> magic_s;
    iLine++;

    if( magic_s != "OFF" )
    {
        std::cout << magic_s << " != OFF :   We handle ONLY *.off files." << std::endl;
        myfile.close();
        exit(1);
    }

    int n_vertices , n_faces , dummy_int;
    myfile >> n_vertices >> n_faces >> dummy_int;
    iLine++;

    o_vertices.clear();
    o_normals.clear();

    for( int v = 0 ; v < n_vertices ; ++v )
    {
        float x , y , z ;

        myfile >> x >> y >> z ;
        o_vertices.push_back( Vec3( x , y , z ) );

        if( load_normals ) {
            myfile >> x >> y >> z;
            o_normals.push_back( Vec3( x , y , z ) );
        }
        iLine++;
    }

    o_triangles.clear();
    o_triangle_normals.clear();
    for( int f = 0 ; f < n_faces ; ++f )
    {
        int n_vertices_on_face;
        myfile >> n_vertices_on_face;

        if( n_vertices_on_face == 3 )
        {
            unsigned int _v1 , _v2 , _v3;
            myfile >> _v1 >> _v2 >> _v3;

            o_triangles.push_back(Triangle( _v1, _v2, _v3 ));

            if( load_normals ) {
                float x , y , z ;
                myfile >> x >> y >> z;
                o_triangle_normals.push_back( Vec3( x , y , z ) );
            }
        }
        else if( n_vertices_on_face == 4 )
        {
            unsigned int _v1 , _v2 , _v3 , _v4;
            myfile >> _v1 >> _v2 >> _v3 >> _v4;

            o_triangles.push_back(Triangle(_v1, _v2, _v3 ));
            o_triangles.push_back(Triangle(_v1, _v3, _v4));
            if( load_normals ) {
                float x , y , z ;
                myfile >> x >> y >> z;
                o_triangle_normals.push_back( Vec3( x , y , z ) );
            }

        }
        else
        {
            std::cout << "We handle ONLY *.off files with 3 or 4 vertices per face (here = " << n_vertices_on_face << " on line " << iLine << ")" << std::endl;
            myfile.close();
            exit(1);
        }
        iLine++;
    }

}

// ------------------------------------
// Application initialization
// ------------------------------------
void initLight () {
    GLfloat light_position1[4] = {22.0f, 16.0f, 50.0f, 0.0f};
    GLfloat direction1[3] = {-52.0f,-16.0f,-50.0f};
    GLfloat color1[4] = {1.0f, 1.0f, 1.0f, 1.0f};
    GLfloat ambient[4] = {0.3f, 0.3f, 0.3f, 0.5f};

    glLightfv (GL_LIGHT1, GL_POSITION, light_position1);
    glLightfv (GL_LIGHT1, GL_SPOT_DIRECTION, direction1);
    glLightfv (GL_LIGHT1, GL_DIFFUSE, color1);
    glLightfv (GL_LIGHT1, GL_SPECULAR, color1);
    glLightModelfv (GL_LIGHT_MODEL_AMBIENT, ambient);
    glEnable (GL_LIGHT1);
    glEnable (GL_LIGHTING);
}

void init () {
    camera.resize (SCREENWIDTH, SCREENHEIGHT);
    // initLight ();
    glCullFace (GL_BACK);
    glDisable (GL_CULL_FACE);
    glDepthFunc (GL_LESS);
    glEnable (GL_DEPTH_TEST);
    glClearColor (0.2f, 0.2f, 0.3f, 1.0f);
    glEnable(GL_COLOR_MATERIAL);
    glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);
}


// ------------------------------------
// Rendering.
// ------------------------------------

void drawVector( Vec3 const & i_from, Vec3 const & i_to ) {

    glBegin(GL_LINES);
    glVertex3f( i_from[0] , i_from[1] , i_from[2] );
    glVertex3f( i_to[0] , i_to[1] , i_to[2] );
    glEnd();
}

void drawAxis( Vec3 const & i_origin, Vec3 const & i_direction ) {

    glLineWidth(4); // for example...
    drawVector(i_origin, i_origin + i_direction);
}

void drawReferenceFrame( Vec3 const & origin, Vec3 const & i, Vec3 const & j, Vec3 const & k ) {

    // glDisable(GL_LIGHTING);
    glColor3f( 0.8, 0.2, 0.2 );
    drawAxis( origin, i );
    glColor3f( 0.2, 0.8, 0.2 );
    drawAxis( origin, j );
    glColor3f( 0.2, 0.2, 0.8 );
    drawAxis( origin, k );
    // glEnable(GL_LIGHTING);

}

typedef struct {
    float r;       // ∈ [0, 1]
    float g;       // ∈ [0, 1]
    float b;       // ∈ [0, 1]
} RGB;

RGB scalarToRGB( float scalar_value ) //Scalar_value ∈ [0, 1]
{
    RGB rgb;
    float H = scalar_value*360., S = 1., V = 0.85,
            P, Q, T,
            fract;

    (H == 360.)?(H = 0.):(H /= 60.);
    fract = H - floor(H);

    P = V*(1. - S);
    Q = V*(1. - S*fract);
    T = V*(1. - S*(1. - fract));

    if      (0. <= H && H < 1.)
        rgb = (RGB){.r = V, .g = T, .b = P};
    else if (1. <= H && H < 2.)
        rgb = (RGB){.r = Q, .g = V, .b = P};
    else if (2. <= H && H < 3.)
        rgb = (RGB){.r = P, .g = V, .b = T};
    else if (3. <= H && H < 4.)
        rgb = (RGB){.r = P, .g = Q, .b = V};
    else if (4. <= H && H < 5.)
        rgb = (RGB){.r = T, .g = P, .b = V};
    else if (5. <= H && H < 6.)
        rgb = (RGB){.r = V, .g = P, .b = Q};
    else
        rgb = (RGB){.r = 0., .g = 0., .b = 0.};

    return rgb;
}

void drawVectorField( std::vector<Vec3> const & i_positions, std::vector<Vec3> const & i_directions ) {
    glLineWidth(1.);
    for(unsigned int pIt = 0 ; pIt < i_directions.size() ; ++pIt) {
        Vec3 to = i_positions[pIt] + 0.02*i_directions[pIt];
        drawVector(i_positions[pIt], to);
    }
}
void drawNormals(Mesh const& i_mesh){
    drawVectorField( i_mesh.vertices, i_mesh.normals );
}

// ------------------------------------------------------------------------------------------------------------------------

// Variables globales
bool playTime = false ;
bool displayModification = false ;
bool displayReferenceFrame = false ;
bool display2D = true ;
bool display3D = false ;

float R = 255.f, G = 0.f, B = 0.f ;
float t = 0.0f; // "temps" simulé
int tailleSoupe = 50 ;

double tailleVoxel = 0.02f;
float mX = 0, mY = 0, mZ = 0 ;

unsigned int GlobalTime = 0 ;
unsigned int vitesse = 1 ;
int generation2D = 0 ;
int generation3D = 0 ;

std::vector<Cell> CELLS_2D ;
GameOfLife GAME_2D = GameOfLife(CELLS_2D, tailleVoxel) ;

std::vector<Cell> CELLS_3D ;
GameOfLife GAME_3D = GameOfLife(CELLS_3D, tailleVoxel) ;

// Appelée à chaque frame
void updateColor() {
    t += 0.1f; // vitesse du changement

    R = (0.5f + 0.5f * sin(t));
    G = (0.5f + 0.5f * sin(t + 2.0f * M_PI / 3.0f)); // déphasé de 120°
    B = (0.5f + 0.5f * sin(t + 4.0f * M_PI / 3.0f)); // déphasé de 240°
}

// Affichages du jeu de la vie
void DisplayCell(Cell &c){
    
    Vec3 color = c.getColor() ;

    // Compute cube vertices on the stack (avoids allocating a std::vector each draw)
    Vec3 p = c.getPosition();
    float h = c.getSize() * 0.5f;
    Vec3 vv[8];
    vv[0] = Vec3(p[0]-h, p[1]-h, p[2]-h);
    vv[1] = Vec3(p[0]+h, p[1]-h, p[2]-h);
    vv[2] = Vec3(p[0]-h, p[1]+h, p[2]-h);
    vv[3] = Vec3(p[0]+h, p[1]+h, p[2]-h);
    vv[4] = Vec3(p[0]-h, p[1]-h, p[2]+h);
    vv[5] = Vec3(p[0]+h, p[1]-h, p[2]+h);
    vv[6] = Vec3(p[0]-h, p[1]+h, p[2]+h);
    vv[7] = Vec3(p[0]+h, p[1]+h, p[2]+h);
    
    //On créer chaque face manuellement :
    glBegin(GL_QUAD_STRIP);
    glColor3f(color[0],color[1],color[2]);
    
    // Face du bas
    glVertex3f(vv[0][0],vv[0][1],vv[0][2]);
    glVertex3f(vv[1][0],vv[1][1],vv[1][2]);
    glVertex3f(vv[4][0],vv[4][1],vv[4][2]);
    glVertex3f(vv[5][0],vv[5][1],vv[5][2]);
    
    // Face droite
    glVertex3f(vv[6][0],vv[6][1],vv[6][2]);
    glVertex3f(vv[7][0],vv[7][1],vv[7][2]);
    
    // Face du haut
    glVertex3f(vv[2][0],vv[2][1],vv[2][2]);
    glVertex3f(vv[3][0],vv[3][1],vv[3][2]);
    
    // Face gauche
    glVertex3f(vv[0][0],vv[0][1],vv[0][2]);
    glVertex3f(vv[1][0],vv[1][1],vv[1][2]);
    glEnd();

    glBegin(GL_QUADS);
    // Face arrière
    glVertex3f(vv[0][0],vv[0][1],vv[0][2]);
    glVertex3f(vv[2][0],vv[2][1],vv[2][2]);
    glVertex3f(vv[6][0],vv[6][1],vv[6][2]);
    glVertex3f(vv[4][0],vv[4][1],vv[4][2]);         

    // Face avant
    glVertex3f(vv[1][0],vv[1][1],vv[1][2]);
    glVertex3f(vv[3][0],vv[3][1],vv[3][2]);
    glVertex3f(vv[7][0],vv[7][1],vv[7][2]);
    glVertex3f(vv[5][0],vv[5][1],vv[5][2]);         
    glEnd();

    // affichage des aretes
    glLineWidth(2.f);
    glBegin(GL_LINE_STRIP);
    glColor3f(0,0,0);
    glVertex3f(vv[0][0], vv[0][1], vv[0][2]);
    glVertex3f(vv[1][0], vv[1][1], vv[1][2]);
    glVertex3f(vv[3][0], vv[3][1], vv[3][2]);
    glVertex3f(vv[2][0], vv[2][1], vv[2][2]);
    glVertex3f(vv[0][0], vv[0][1], vv[0][2]);
    glVertex3f(vv[4][0], vv[4][1], vv[4][2]);
    glVertex3f(vv[5][0], vv[5][1], vv[5][2]);
    glVertex3f(vv[1][0], vv[1][1], vv[1][2]);
    glEnd();

    glBegin(GL_LINE_STRIP);
    glColor3f(0,0,0);
    glVertex3f(vv[2][0], vv[2][1], vv[2][2]);
    glVertex3f(vv[6][0], vv[6][1], vv[6][2]);
    glVertex3f(vv[4][0], vv[4][1], vv[4][2]);
    glEnd();

    glBegin(GL_LINE_STRIP);
    glColor3f(0,0,0);
    glVertex3f(vv[3][0], vv[3][1], vv[3][2]);
    glVertex3f(vv[7][0], vv[7][1], vv[7][2]);
    glVertex3f(vv[5][0], vv[5][1], vv[5][2]);
    glEnd();

    glBegin(GL_LINE_STRIP);
    glColor3f(0,0,0);
    glVertex3f(vv[6][0], vv[6][1], vv[6][2]);
    glVertex3f(vv[7][0], vv[7][1], vv[7][2]);
    glEnd();
}

void displayGame2D(){
    std::vector<Cell> cells = GAME_2D.getTabCell() ;
    for(Cell cell : cells){
        DisplayCell(cell) ;
    }
}

void displayGame3D(){
    
    std::vector<Cell> cells = GAME_3D.getTabCell() ;
    for(Cell cell : cells){
        DisplayCell(cell) ;
    }
    glEnd();
}

void displayModif(){

    float R,G,B ;
    R = 1.f ; G = 0.f ; B = 0.f ;
    Vec3 centreVoxel = Vec3(mX, mY, mZ) ;
    
    std::vector<Vec3> vv ; // On va créer un tableau pour stocker les sommets de notre voxel (voxel_vertice)
    vv = sommetsCube(centreVoxel, tailleVoxel*1.05f) ;

    glLineWidth(1.5f);
    glBegin(GL_LINE_STRIP);
    glColor3f(R,G,B);
    glVertex3f(vv[0][0], vv[0][1], vv[0][2]);
    glVertex3f(vv[1][0], vv[1][1], vv[1][2]);
    glVertex3f(vv[3][0], vv[3][1], vv[3][2]);
    glVertex3f(vv[2][0], vv[2][1], vv[2][2]);
    glVertex3f(vv[0][0], vv[0][1], vv[0][2]);
    glVertex3f(vv[4][0], vv[4][1], vv[4][2]);
    glVertex3f(vv[5][0], vv[5][1], vv[5][2]);
    glVertex3f(vv[1][0], vv[1][1], vv[1][2]);
    glEnd();

    glBegin(GL_LINE_STRIP);
    glColor3f(R,G,B);
    glVertex3f(vv[2][0], vv[2][1], vv[2][2]);
    glVertex3f(vv[6][0], vv[6][1], vv[6][2]);
    glVertex3f(vv[4][0], vv[4][1], vv[4][2]);
    glEnd();

    glBegin(GL_LINE_STRIP);
    glColor3f(R,G,B);
    glVertex3f(vv[3][0], vv[3][1], vv[3][2]);
    glVertex3f(vv[7][0], vv[7][1], vv[7][2]);
    glVertex3f(vv[5][0], vv[5][1], vv[5][2]);
    glEnd();

    glBegin(GL_LINE_STRIP);
    glColor3f(R,G,B);
    glVertex3f(vv[6][0], vv[6][1], vv[6][2]);
    glVertex3f(vv[7][0], vv[7][1], vv[7][2]);
    glEnd();

}

Vec3 centre = Vec3(0,0,0) ;
Vec3 axisX = Vec3(1,0,0);
Vec3 axisY = Vec3(0,1,0);
Vec3 axisZ = Vec3(0,0,1);

//Draw fonction
void draw () {

    updateColor();
    
    if(displayReferenceFrame)
    drawReferenceFrame(centre,axisX, axisY, axisZ);

    if(display2D)
    displayGame2D();

    if(display3D)
    displayGame3D() ;

    if(playTime){
        GAME_2D.nextGeneration2D();
        GAME_3D.nextGeneration3D();
    }
    if(displayModification){
        displayModif();
    }

    glutSetWindowTitle("Jeu de la vie");
}

void display () {
    glLoadIdentity ();
    glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    camera.apply ();
    draw ();
    glFlush ();
    glutSwapBuffers ();
}

void idle () {
    glutPostRedisplay ();
}

// ------------------------------------
// User inputs
// ------------------------------------
//Keyboard event
void key (unsigned char keyPressed, int x, int y) {
    switch (keyPressed) {
    case 'f':
        if (fullScreen == true) {
            glutReshapeWindow (SCREENWIDTH, SCREENHEIGHT);
            fullScreen = false;
        } else {
            glutFullScreen ();
            fullScreen = true;
        }
        break;

    case 'n':
        if(display2D){
            GAME_2D.nextGeneration2D();
            generation2D++ ;
        }
        if(display3D){
            GAME_3D.nextGeneration3D();
            generation3D++ ;
        }
        std::cout << "Génération n°" << generation2D << std::endl ;
        break;

    case 's':
        if(display2D){
            GAME_2D.soup2D(tailleSoupe);
            generation2D = 0 ;
        }
        if(display3D){
            GAME_3D.soup3D(tailleSoupe);
            generation3D = 0 ;
        }
        std::cout << "Soupe miam !" << std::endl ;
        break;
    
    case 'r':
        GAME_2D.reset();
        GAME_3D.reset();
        generation2D = 0 ;
        generation3D = 0 ;
        std::cout << "Reset !" << std::endl ;
        break;

    case '/':
        if(display2D)
        GAME_2D.split() ;
        if(display3D)
        GAME_3D.split() ;
        std::cout << "Split !" << std::endl ;
        break;

    case 'p':
        playTime = !playTime ;
        std::cout << "Temps !" << std::endl ;
        break;

    case 'd':
        displayModification = !displayModification ;
        std::cout << "Modification !" << std::endl ;
        break ;

    case '*':
        displayReferenceFrame = !displayReferenceFrame ;
        std::cout << "Référence !" << std::endl ;
        break;
    
    case '0':
        if(display2D){
            // GAME_2D.printCells();
            std::cout << "Nb cellules vivantes : " << GAME_2D.getNbCells() << std::endl ;
        }
        if(display3D){
            // GAME_3D.printCells();
            std::cout << "Nb cellules vivantes : " << GAME_3D.getNbCells() << std::endl ;
        }
        break;
    
    case ' ':
        if(display2D)
        GAME_2D.addCell(Cell(Vec3(mX,mY, 0), tailleVoxel, 1));
        if(display3D)
        GAME_3D.addCell(Cell(Vec3(mX,mY, mZ), tailleVoxel, 1));
        std::cout << "Cellule créer !" << std::endl ;
        break;

    case '+':
        tailleSoupe++ ;
        std::cout << "tailleSoupe = " << tailleSoupe << std::endl ;
        break;

    case '-':
        if(tailleSoupe > 1)
        tailleSoupe-- ;
        std::cout << "tailleSoupe = " << tailleSoupe << std::endl ;
        break;

    case '2':
        display2D = !display2D ;
        std::cout << "2D !" << std::endl ;
        break;
    
    case '3':
        display3D = !display3D ;
        std::cout << "3D !" << std::endl ;
        break;

    case 'v':
        mZ += tailleVoxel ;
        break;
    
    case 'b':
        mZ -= tailleVoxel ;
        break;

    default:
        break;
    }
    idle ();
}

void specialKey(int key, int x, int y) {
    switch (key) {
        case GLUT_KEY_UP:
                mY += tailleVoxel ;
            break;

        case GLUT_KEY_DOWN:
                mY -= tailleVoxel ;
            break;

        case GLUT_KEY_RIGHT:
                mX += tailleVoxel ;
            break;

        case GLUT_KEY_LEFT:
                mX -= tailleVoxel ;
            break;
    }
}


//Mouse events
void mouse (int button, int state, int x, int y) {
    if (state == GLUT_UP) {
        mouseMovePressed = false;
        mouseRotatePressed = false;
        mouseZoomPressed = false;
    } else {
        if (button == GLUT_LEFT_BUTTON) {
            camera.beginRotate (x, y);
            mouseMovePressed = false;
            mouseRotatePressed = true;
            mouseZoomPressed = false;
        } else if (button == GLUT_RIGHT_BUTTON) {
            lastX = x;
            lastY = y;
            mouseMovePressed = true;
            mouseRotatePressed = false;
            mouseZoomPressed = false;
        } else if (button == GLUT_MIDDLE_BUTTON) {
            if (mouseZoomPressed == false) {
                lastZoom = y;
                mouseMovePressed = false;
                mouseRotatePressed = false;
                mouseZoomPressed = true;
            }
        }
    }

    idle ();
}

//Mouse motion, update camera
void motion (int x, int y) {
    if (mouseRotatePressed == true) {
        camera.rotate (x, y);
    }
    else if (mouseMovePressed == true) {
        camera.move ((x-lastX)/static_cast<float>(SCREENWIDTH), (lastY-y)/static_cast<float>(SCREENHEIGHT), 0.0);
        lastX = x;
        lastY = y;
    }
    else if (mouseZoomPressed == true) {
        camera.zoom (float (y-lastZoom)/SCREENHEIGHT);
        lastZoom = y;
    }
}


void reshape(int w, int h) {
    camera.resize (w, h);
}

// ------------------------------------
// Start of graphical application
// ------------------------------------
int main (int argc, char ** argv) {
    if (argc > 2) {
        exit (EXIT_FAILURE);
    }

    glutInit (&argc, argv);
    glutInitDisplayMode (GLUT_RGBA | GLUT_DEPTH | GLUT_DOUBLE);
    glutInitWindowSize (SCREENWIDTH, SCREENHEIGHT);
    window = glutCreateWindow ("TP HAI714I");

    init ();
    glutIdleFunc (idle);
    glutDisplayFunc (display);
    glutKeyboardFunc (key);
    glutSpecialFunc(specialKey); // Pour pouvoir gérer les flèches directionnels
    glutReshapeFunc (reshape);
    glutMotionFunc (motion);
    glutMouseFunc (mouse);
    key ('?', 0, 0);

    glutMainLoop ();
    return EXIT_SUCCESS;
}

