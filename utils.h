#ifndef UTILS_H
#define UTILS_H

#include "src/Vec3.h"
#include <vector>
using namespace std;

// ---------- STRUCTURES ----------

struct Triangle {
    inline Triangle () {
        v[0] = v[1] = v[2] = 0;
    }
    inline Triangle (const Triangle & t) {
        v[0] = t.v[0];   v[1] = t.v[1];   v[2] = t.v[2];
    }
    inline Triangle (unsigned int v0, unsigned int v1, unsigned int v2) {
        v[0] = v0;   v[1] = v1;   v[2] = v2;
    }
    unsigned int & operator [] (unsigned int iv) { return v[iv]; }
    unsigned int operator [] (unsigned int iv) const { return v[iv]; }
    inline virtual ~Triangle () {}
    inline Triangle & operator = (const Triangle & t) {
        v[0] = t.v[0];   v[1] = t.v[1];   v[2] = t.v[2];
        return (*this);
    }
    // membres indices des sommets du triangle:
    unsigned int v[3];
};

struct Mesh {
    std::vector< Vec3 > vertices; //array of mesh vertices positions
    std::vector< Vec3 > normals; //array of vertices normals useful for the display
    std::vector< Triangle > triangles; //array of mesh triangles
    std::vector< Vec3 > triangle_normals; //triangle normals to display face normals
    
    std::vector<Vec3> C ; // Cube englobant

    void simplify(unsigned int resolution);
    void adaptativeSimplify (unsigned int numOfPerLeafVertices);
};

// ---------- CODE DÉJÀ ECRIT AUX TPs PRÉCÉDENTS ----------

void setSphere(Mesh & mesh, Vec3 centre, double rayon){

    // On vide tout
    mesh.vertices.clear() ;
    mesh.triangles.clear() ;
    mesh.normals.clear() ;
    mesh.triangle_normals.clear() ;

    float x,y,z, phi, theta ;

    unsigned int nX = 20 ;
    unsigned int nY = 20 ;

    unsigned int nXb = nX+1 ;
    unsigned int nYb = nY+1 ;
    unsigned int nb_points = nX*nY ;
    unsigned int nb_points_bis = nXb*nYb ;
    unsigned int nb_triangles = nb_points*2 ;

    float base_theta = .0f ;
    float pas_theta = (2*M_PI)/nX ;

    float base_phi = .0f ;
    float pas_phi = M_PI/nY ;

    mesh.vertices.resize(nb_points_bis);
    mesh.triangles.resize(nb_triangles);
    mesh.normals.resize(nb_points_bis);
    mesh.triangle_normals.resize(nb_triangles);

    // Création des sommets
    unsigned int nbN = 0 ;
    for(unsigned int i = 0 ; i < nYb ; i++){
        phi = base_phi + (float)i * pas_phi ;
        for(unsigned int j = 0 ; j < nXb ; j++){
            theta = base_theta + (float)j * pas_theta ;

            x = centre[0] + rayon * sin(phi) * cos(theta) ;
            y = centre[1] + rayon * cos(phi) ;
            z = centre[2] + rayon * sin(phi) * sin(theta) ;

            mesh.vertices[j+i*nYb] = Vec3{x,y,z} ;

            // Calcul de la normal par sommets
            Vec3 normal = Vec3(
                (x/sqrt(x*x + y*y + z*z)) ,
                (y/sqrt(x*x + y*y + z*z)) , 
                (z/sqrt(x*x + y*y + z*z))
            );
            mesh.normals[nbN++] = normal ;
        }
    }

    // Création des triangles
    for(unsigned int i = 0 ; i < nY ; i++){
        for(unsigned int j = 0 ; j < nX ; j++){
            int hg = j+i*nXb ; // Sommet courant
            int hd = hg+1 ;
            int bg = hg+nXb ;
            int bd = hg+nXb+1 ;

            // On va créer 2 triangles par carré (en imaginant un maillage plan)
            mesh.triangles[(i*nX+j)*2] = Triangle(bg,hg,hd) ; // Triangle du haut
            mesh.triangles[(i*nX+j)*2+1] = Triangle(bd,bg,hd) ; // Triangle du bas

            // On calcul les normales des triangles
            Vec3 p0 = mesh.vertices[bg];
            Vec3 p1 = mesh.vertices[hg];
            Vec3 p2 = mesh.vertices[hd];
            Vec3 p3 = mesh.vertices[bd];

            Vec3 c0 = Vec3::cross((p1 - p0), (p2 - p0)) ;
            Vec3 c1 = Vec3::cross((p2 - p1), (p3 - p1)) ;

            mesh.triangle_normals[(i*nX+j)*2] = c0 / (c0.squareLength()) ;
            mesh.triangle_normals[(i*nX+j)*2 + 1] = c1 / (c1.squareLength()) ;
        }
    }
}

void setCylinder(Mesh & mesh, Vec3 axisOrigin, Vec3 axisVector, double r){

    mesh.vertices.clear();
    mesh.triangles.clear();

    static unsigned int nC = 20 ; // Nombre de méridien du cylindre

    mesh.vertices.resize((nC*2)+2);
    mesh.triangles.resize(nC*4);

    double pasC = (2*M_PI)/nC ;
    double h = axisVector.length();
    axisVector.normalize() ;
    // Pour définir le plan du cercle
    Vec3 axeY = axisVector ;
    Vec3 randomVect = (fabs(axeY[0]) < 0.9) ? Vec3{1,0,0} : Vec3{0,0,1}; // vecteur arbitraire non parallèle
    Vec3 axeX = Vec3::cross(randomVect, axeY) ; axeX.normalize();
    Vec3 axeZ = Vec3::cross(axeX, axeY) ; axeZ.normalize();
    Vec3 centreCercleBas = axisOrigin ;
    Vec3 centreCercleHaut = axisOrigin + (h*axeY) ;

    // On initialise les 2 premiers points comme les 2 prremiers centres
    mesh.vertices[0] = centreCercleBas ;
    mesh.vertices[1] = centreCercleHaut ;

    // Création de tous les sommets
    for(double i = 1 ; i < nC+1 ; i++){ // on créer les 2 cercles en même temps

        Vec3 radius = r*(cos(pasC*i)*axeX + sin(pasC*i)*axeZ) ;

        mesh.vertices[i*2] = centreCercleBas + radius ;
        mesh.vertices[(i*2)+1] = centreCercleHaut + radius ;
    }

    // Création des triangles
    for(double i = 0 ; i < nC-1 ; i++){ // A chaque itération on créer 4 triangles
        int bg = (i+1)*2 ; int bd = bg+2 ; int hg = bg+1 ; int hd = bg+3 ;
        mesh.triangles[i*4] = Triangle(bd, bg, hd); // Triangle du bas
        mesh.triangles[(i*4)+1] = Triangle(hd, bg, hg); // Triangle du haut
        mesh.triangles[(i*4)+2] = Triangle(bg, bd, 0); // Triangle du cercle inférieur
        mesh.triangles[(i*4)+3] = Triangle(hd, hg, 1); // Triangle du cercle supérieur
    }
    mesh.triangles[(nC-1)*4] = Triangle(2, nC*2, 3); // Triangle du bas
    mesh.triangles[((nC-1)*4)+1] = Triangle(3, nC*2, (nC*2)+1); // Triangle du haut
    mesh.triangles[((nC-1)*4)+2] = Triangle(nC*2, 2, 0); // Triangle du cercle inférieur
    mesh.triangles[((nC-1)*4)+3] = Triangle(3, (nC*2)+1, 1); // Triangle du cercle supérieur
}

// ---------- CODE POUR LE TP3 ----------

std::vector<Vec3> sommetsCube(Vec3 centre, double length){

    std::vector<Vec3> sommets ;
    double demi = length/2.f ; // Distance entre le centre et le centre de chaque face
    
    // Pour recréer les sommets, on va partir du centre pour aller sur le plan d'une face, puis avoir ses points, on n'a qu'a prendre 2 faces opposés
    double pos[] = {-demi, demi} ; // Pour chaque axes X,Y ou Z on va aller devant et derriere le centre
    for(double x : pos){
        for(double y : pos){
            for(double z : pos){
                sommets.push_back(Vec3(centre[0]+x,centre[1]+y,centre[2]+z)); // Position de chaque sommets du cube
            }
        }
    }

    return sommets ;
}

std::vector<Vec3> sommetsPave(Vec3 centre, double lx, double ly, double lz) {

    std::vector<Vec3> sommets;
    
    double demiX = lx / 2.0;
    double demiY = ly / 2.0;
    double demiZ = lz / 2.0;

    double posX[] = {-demiX, demiX};
    double posY[] = {-demiY, demiY};
    double posZ[] = {-demiZ, demiZ};

    for (double x : posX) {
        for (double y : posY) {
            for (double z : posZ) {
                sommets.push_back(Vec3(centre[0] + x,
                                       centre[1] + y,
                                       centre[2] + z));
            }
        }
    }

    return sommets;
}


double dist(Vec3 a, Vec3 b){
    double x = b[0] - a[0] ;
    double y = b[1] - a[1] ;
    double z = b[2] - a[2] ;
    return sqrt(x*x + y*y + z*z) ;
}

bool isPointInCylinder(Vec3 axisOrigin, Vec3 axisVector, double rayon, Vec3 p){

    // axisOrigin : point d'origine du cylindre
    // axisVector : direction du cylindre
    // rayon : rayon du cylindre
    // p : point à analyser

    float h = axisVector.length() ;
    axisVector.normalize() ;
   
    // On calcul si notre point est au dessous ou en dessous du cylindre
    Vec3 dist = p - axisOrigin ;
    float hauteurP = Vec3::dot(dist, axisVector) ;
    if(hauteurP < 0 || hauteurP > h){ return false ;}

    // On projette le point sur l'axe axisVector
    float distProj = Vec3::dot(axisVector, dist) / axisVector.length() ;
    Vec3 proj = Vec3( axisOrigin[0] + axisVector[0]*distProj,
                      axisOrigin[1] + axisVector[1]*distProj,
                      axisOrigin[2] + axisVector[2]*distProj );
    // On peut maintenant calculer la distance entre le point projeté et le point de base, voir s'il est dans le rayon du cercle
    Vec3 vecRadial = p - proj ;
    float distRadial = vecRadial.length();
    
    return distRadial <= rayon ; 
}

bool isEquals(Vec3 a, Vec3 b){
    return a[0]==b[0] && a[1]==b[1] && a[2]==b[2] ;
}

std::vector<Vec3> minMaxVec3(std::vector<Vec3> grid_vertices){

    int n = grid_vertices.size() ;

    double minX = grid_vertices[0][0], maxX = grid_vertices[0][0] ; 
    double minY = grid_vertices[0][1], maxY = grid_vertices[0][1] ; 
    double minZ = grid_vertices[0][2], maxZ = grid_vertices[0][2] ;  
    for(int i = 0 ; i < n ; i++){
        minX = min(minX, (double)grid_vertices[i][0]) ; maxX = max(maxX, (double)grid_vertices[i][0]) ;             
        minY = min(minY, (double)grid_vertices[i][1]) ; maxY = max(maxY, (double)grid_vertices[i][1]) ;             
        minZ = min(minZ, (double)grid_vertices[i][2]) ; maxZ = max(maxZ, (double)grid_vertices[i][2]) ;         
    }

    std::vector<Vec3> res ;
    res.push_back(Vec3(minX,minY,minZ));
    res.push_back(Vec3(maxX,maxY,maxZ));
    return res ;
}


#endif