#ifndef CELL_H
#define CELL_H

#include <cmath>
#include <string>
#include <float.h>
#include <iostream>

#include "Vec3.h"
#include "utils.h"

class Cell {

    private:
        Vec3 position ;
        float size ;
        int state ;
        Vec3 color ;

    public:

        // CONSTRUCTORS

        Cell(){}
        Cell(Vec3 p, float si, int st){
            this->position = p ;
            this->size = si ;
            this->state = st ;
            this->color = Vec3(1,1,1);
        }
        Cell(Vec3 p, float si, int st, Vec3 c){
            this->position = p ;
            this->size = si ;
            this->state = st ;
            this->color = c;
        }

        // GETTER & SETTER

        Vec3 getPosition(){return this->position ;}
        float getSize(){return this->size ;}
        int getState(){return this->state ;}
        Vec3 getColor(){return this->color ;}

        void setPosition(Vec3 p){this->position = p ;}
        void setSize(float s){this->size = s ;}
        void setState(int s){this->state = s ;}
        void setColor(Vec3 c){this->color = c ;}

        // OTHER METHODS

        bool operator==(const Cell& other) const {
            return std::fabs(this->position[0] - other.position[0] ) < 1e-4f 
                && std::fabs(this->position[1] - other.position[1] ) < 1e-4f 
                && std::fabs(this->position[2] - other.position[2] ) < 1e-4f ;
        }

        std::vector<Vec3> neighborPos2D(){

            // Calcul sur le plan orthogonal à Z (ajustable : il faut aussi modifier soup2D() dans GameOfLife.h)

            float dist = this->size ;
            return {Vec3(this->position[0] + dist , this->position[1] + dist , 0.0f),
                    Vec3(this->position[0] + dist , this->position[1] , 0.0f),
                    Vec3(this->position[0] + dist , this->position[1] - dist , 0.0f),
                    Vec3(this->position[0] , this->position[1] + dist , 0.0f),
                    Vec3(this->position[0] , this->position[1] - dist , 0.0f),
                    Vec3(this->position[0] - dist , this->position[1] + dist , 0.0f),
                    Vec3(this->position[0] - dist , this->position[1] , 0.0f),
                    Vec3(this->position[0] - dist , this->position[1] - dist , 0.0f)} ;
        }

        std::vector<Vec3> neighborPos3D(){
            
            float dist = this->size/2.f ;
            return {Vec3(this->position[0] - dist , this->position[1] - dist, this->position[2] - dist)
                   ,Vec3(this->position[0] - dist , this->position[1] - dist, this->position[2])
                   ,Vec3(this->position[0] - dist , this->position[1] - dist, this->position[2] + dist)
                   ,Vec3(this->position[0] - dist , this->position[1], this->position[2] - dist)
                   ,Vec3(this->position[0] - dist , this->position[1], this->position[2])
                   ,Vec3(this->position[0] - dist , this->position[1], this->position[2] + dist)
                   ,Vec3(this->position[0] - dist , this->position[1] + dist, this->position[2] - dist)
                   ,Vec3(this->position[0] - dist , this->position[1] + dist, this->position[2])
                   ,Vec3(this->position[0] - dist , this->position[1] + dist, this->position[2] + dist)
                   ,Vec3(this->position[0] , this->position[1] + dist, this->position[2] - dist)
                   ,Vec3(this->position[0] , this->position[1] + dist, this->position[2])
                   ,Vec3(this->position[0] , this->position[1] + dist, this->position[2] + dist)
                   ,Vec3(this->position[0] , this->position[1], this->position[2] - dist)
                   ,Vec3(this->position[0] , this->position[1], this->position[2] + dist)
                   ,Vec3(this->position[0] , this->position[1] - dist, this->position[2] - dist)
                   ,Vec3(this->position[0] , this->position[1] - dist, this->position[2])
                   ,Vec3(this->position[0] , this->position[1] - dist, this->position[2] + dist)
                   ,Vec3(this->position[0] + dist , this->position[1] - dist, this->position[2] - dist)
                   ,Vec3(this->position[0] + dist , this->position[1] - dist, this->position[2])
                   ,Vec3(this->position[0] + dist , this->position[1] - dist, this->position[2] + dist)
                   ,Vec3(this->position[0] + dist , this->position[1], this->position[2] - dist)
                   ,Vec3(this->position[0] + dist , this->position[1], this->position[2])
                   ,Vec3(this->position[0] + dist , this->position[1], this->position[2] + dist)
                   ,Vec3(this->position[0] + dist , this->position[1] + dist, this->position[2] - dist)
                   ,Vec3(this->position[0] + dist , this->position[1] + dist, this->position[2])
                   ,Vec3(this->position[0] + dist , this->position[1] + dist, this->position[2] + dist)} ;
        }

        void printCell(){
            std::string str ;
            if(this->state == 0){
                str = "dead" ;
            }else if(this->state == 1){
                str = "alive" ;
            }else{
                str = "other" ;
            }
            std::cout << "Cell("<<this->position[0]<<","<<this->position[1]<<","<<this->position[2]<<") : " << str << ", size : " << this->size << std::endl ;
        }

};

#endif