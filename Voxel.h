#ifndef VOXEL_H
#define VOXEL_H

#include <vector>

#include <iostream>
#include "src/Vec3.h"
using namespace std;

class Voxel{

private:
    int id ;

    Vec3 centre ;
    double length ;

public:

    // Constructor
    Voxel(){}
    Voxel(Vec3 c, double l){
        this->centre = c ;
        this->length = l ;
    }

    // Getter et Setter
    Vec3 getCentre(){return this->centre ;}
    double getLength(){return this->length ;}
    int getId(){return this->id ;}

    void setCentre(Vec3 c){this->centre = c ;}
    void setLength(double l){this->length = l ;}
    void setId(int i){this->id = i ;}

    // Autres méthodes
    bool contains(Vec3 p){
        double demi = this->length / 2.f ;
        Vec3 centre = this->centre ;

        return ((abs(p[0] - centre[0]) <= demi) &&
                (abs(p[1] - centre[1]) <= demi) &&
                (abs(p[2] - centre[2]) <= demi));
    }

    bool intersectTriangle(Vec3 p0, Vec3 p1, Vec3 p2 , Vec3 normal_t){
        // Separating Axis Theorem
        // Deux volumes ne s'intersecte pa s'il existe un axe sur lequel leurs projection ne se recouvrent pas
        // On cherche à vérifier qu'il s'il y a un axe séparateur
        // Si axe : pas d'intersection
        // si pas d'axe : intersection

        Vec3 c = this->centre;
        double l = this->length / 2.f ;

        // On utilise notre voxel comme espace référence
        Vec3 t0 = p0 - c ; // Point 0 du triangle dans le repère du cube
        Vec3 t1 = p1 - c ; // Point 1 du triangle dans le repère du cube
        Vec3 t2 = p2 - c ; // Point 2 du triangle dans le repère du cube

        // Arêtes du triangles
        Vec3 a0 = t1 - t0 ;
        Vec3 a1 = t2 - t1 ;
        Vec3 a2 = t0 - t2 ;

        // 1ere étape :on teste si les arêtes sont dans le cube
        // Lambda fonction retournant le min et le max parmis 3 floats
        auto minmax = [](float a, float b, float c){
            float mini = min({a,b,c});
            float maxi = max({a,b,c});
            return make_pair(mini,maxi);
        };

        // Pour chacun des axes X,Y et Z, on regarde si les sommets du triangles sont dans ou hors de notre cube
        auto [minX, maxX] = minmax(t0[0], t1[0], t2[0]);
        if (maxX < -l || minX > l) return false;
        // Axe Y
        auto [minY, maxY] = minmax(t0[1], t1[1], t2[1]);
        if (maxY < -l || minY > l) return false;
        // Axe Z
        auto [minZ, maxZ] = minmax(t0[2], t1[2], t2[2]);
        if (maxZ < -l || minZ > l) return false;

        // === Étape 3 : Test du plan du triangle ===
    // On utilise la normale fournie
    Vec3 n = normal_t;
    float d = Vec3::dot(-1*n, t0);

    // Rayon projeté du cube sur la normale
    double r = l * (std::abs(n[0]) + std::abs(n[1]) + std::abs(n[2]));
    double s = Vec3::dot(n, Vec3(0,0,0)) + d; // cube centré à l’origine ⇒ c = 0
    if (std::abs(s) > r) return false;

    // === Étape 4 : Tests des 9 axes croisés (cube × triangle) ===
    // Arêtes du cube (unitaires)
    Vec3 axisX(1, 0, 0);
    Vec3 axisY(0, 1, 0);
    Vec3 axisZ(0, 0, 1);

    Vec3 triEdges[3] = { a0, a1, a2 };
    Vec3 boxAxes[3]  = { axisX, axisY, axisZ };

    // Fonction lambda : projeter un triangle sur un axe
    auto projectTriangle = [&](const Vec3& axis) {
        double p0 = Vec3::dot(t0, axis);
        double p1 = Vec3::dot(t1, axis);
        double p2 = Vec3::dot(t2, axis);
        return std::pair<double,double>(
            std::min({p0, p1, p2}),
            std::max({p0, p1, p2})
        );
    };

    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 3; ++j) {
            Vec3 axis = Vec3::cross(boxAxes[i],triEdges[j]);

            // Axe nul -> pas de test (arêtes parallèles)
            if (axis.squareLength() < 1e-8) continue;

            axis.normalize();

            // Projeter triangle et cube sur cet axe
            auto [minT, maxT] = projectTriangle(axis);

            // Projection du cube : rayon = h * (|axis·X| + |axis·Y| + |axis·Z|)
            double r = l * (std::abs(Vec3::dot(axis, axisX)) +
                            std::abs(Vec3::dot(axis, axisY)) +
                            std::abs(Vec3::dot(axis, axisZ)));

            // Si pas de recouvrement sur cet axe -> pas d’intersection
            if (maxT < -r || minT > r)
                return false;
        }
    }

        return true ;
    }

};

#endif