#ifndef OCTREE_H
#define OCTREE_H

#include <vector>

#include <iostream>
#include "src/Vec3.h" 
#include "Voxel.h"
using namespace std;

class Octree{

private:
    // Information des voxel
    Voxel voxel ;

    // Information position du voxel dans l'arbre
    int profondeur ;
    std::vector<Octree> childs ;

public:
    // Constructeur
    Octree(){}
    Octree(Vec3 centre, double length){
        Voxel v = Voxel(centre, length);
        this->voxel = v ;
        this->profondeur = 0 ;
    };
    Octree(Voxel v){
        this->voxel = v ;
        this->profondeur = 0 ;
    }
    Octree(Vec3 centre, double length, int p){
        Voxel v = Voxel(centre, length);
        this->voxel = v ;
        this->profondeur = p ;
    };
    Octree(Voxel v, int p){
        this->voxel = v ;
        this->profondeur = p ;
    }

    // Getter et Setter
    int getProfondeur(){ return this->profondeur ;}
    int getNbChilds(){return this->childs.size() ;}
    std::vector<Octree> getChilds(){return this->childs ;}
    Octree getChild(int n){return this->childs[n] ;}

    
    void setProfondeur(int p){this->profondeur = p ;}
    void setChilds(std::vector<Octree> c){this->childs = c ;}
    void setChild(int n, Octree o){this->childs[n] = o ;}

    // Autres méthodes
    void addChilds(){ // On découpe un Octree en 8
        
        Vec3 centre = this->voxel.getCentre();
        double length = this->voxel.getLength();
        double d = length/2.f ;
        double pos[] = {-d, d} ;
        for(double x : pos){
            for(double y : pos){
                for(double z : pos){
                    // Calcul de la position du voxel enfant
                    Vec3 s = Vec3(centre[0]+x,centre[1]+y,centre[2]+z) ; // Position du sommets du voxel parent
                    Vec3 newCentre = (centre+s)/2.f ; // Centre du voxel enfant
                    
                    // Création du voxel enfant
                    Voxel v = Voxel(newCentre, length/2.f);
                    Octree o = Octree(v, this->profondeur + 1);

                    // Ajout du voxel enfant dans l'arborescence
                    this->childs.push_back(o);          
                }
            }
        }
    }
    void addChildsToAll(){ // Méthode Octree régulier

        // Même logique que pour getLeafs()

        if(this->isLeaf()){ // Si on est une feuille, on ajoute décompose
            this->addChilds();
        }
        else{ // Sinon on parcours chacun de nos enfants et on regarde si ce sont des feuilles
            for(auto& c : this->childs){ // On parcours tous les enfants
                c.addChildsToAll();
            }
        }
    }
    void addChildsTo(int n){ // Méthode Octree
        this->childs.resize(8);
        this->childs[n].addChilds();
    }
    bool isLeaf(){return (this->childs.size() == 0) ;}
    bool isRacine(){return (profondeur==0) ;}

    std::vector<Voxel> getLeafs(){

        std::vector<Voxel> voxels ;

        if(this->isLeaf()){ // Si on est une feuille, on ajoute
            voxels.push_back(this->voxel);
        }
        else{ // Sinon on parcours chacun de nos enfants et on regarde si ce sont des feuilles
            for(auto& c : this->childs){ // On parcours tous les enfants
                std::vector<Voxel> vox_child = c.getLeafs() ;
                int nb_child = vox_child.size() ;
                for(int i = 0 ; i < nb_child ; i++){
                    voxels.push_back(vox_child[i]);
                }
            }
        }
        // On indice les voxels ;
        int nbV = voxels.size();
        for(int i = 0 ; i < nbV ; i++){
            voxels[i].setId(i);
        }

        return voxels ;
    }
};

#endif