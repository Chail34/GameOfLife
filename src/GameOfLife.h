#ifndef GAMEOFLIFE_H
#define GAMEOFLIFE_H

#include <cmath>
#include <ctime>
#include <random>
#include <vector>
#include <cstdlib>
#include <float.h>
#include <iostream>
#include <algorithm>

#include "Vec3.h"
#include "Cell.h"
#include "utils.h"

class GameOfLife {

    private:
        std::vector<Cell> tabCell ;
        int generation ;
        float sizeCell ;

    public:

        // CONSTRUCTORS

        GameOfLife(){}
        GameOfLife(std::vector<Cell> cells){
            this->tabCell = cells ;
            this->generation = 0 ;
            this->sizeCell = 1.f ;
        }
        GameOfLife(std::vector<Cell> cells, float s){
            this->tabCell = cells ;
            this->generation = 0 ;
            this->sizeCell = s ;
        }
        GameOfLife(std::vector<Cell> cells, int g, float s){
            this->tabCell = cells ;
            this->generation = g ;
            this->sizeCell = s ;
        }
        GameOfLife(std::vector<Vec3> cells, float s){
            this->tabCell = convertInAliveCells(cells, s) ;
            this->generation = 0 ;
            this->sizeCell = s ;
        }
        GameOfLife(std::vector<Vec3> cells, int g, float s){
            this->tabCell = convertInAliveCells(cells, s) ;
            this->generation = g ;
            this->sizeCell = s ;
        }

        // GETTER & SETTER

        std::vector<Cell> getTabCell(){return this->tabCell ;}
        int getGeneration(){return this->generation ;}
        int getNbCells(){return this->tabCell.size() ;}

        void setTabCell(std::vector<Cell> cells){this->tabCell = cells ;}
        void setGeneration(int g){this->generation = g ;}

        // OTHER METHODS

        // Boolean

        bool isAlive(Vec3 pos, std::vector<Cell> tab){
            // Si cette position appartient à une cellule du jeu, alors c'est la position d'une cellule vivante
            for(Cell c : tab){
                if(isSamePosition(c.getPosition(), pos))
                return true ;
            }
            return false ;
        }

        void addCell(Cell c){ // Ajoute sans doublons
            std::vector<Cell> t = this->tabCell ;
            if(std::find(t.begin(), t.end(), c) == t.end()) // Si le tableau ne contient pas c 
            this->tabCell.push_back(c);
        }

        void reset(){
            std::vector<Cell> newGame ;
            this->tabCell = newGame ;
            this->generation = 0 ;
        }

        void soup2D(int res){

            this->tabCell.clear();

            std::random_device rd;
            std::mt19937 gen(rd());
            std::uniform_int_distribution<int> dist(0, 1);

            // Calcul sur le plan orthogonal à Z (ajustable)
            Vec3 p ;
            float x, y ;
            float base = (-this->sizeCell * float(res))/2.f ;

            for(float i = 0 ; i < res ; i++){
                x = base + this->sizeCell * i ;
                for(float j = 0 ; j < res ; j++){
                    y = base + this->sizeCell * j ;
                    if(dist(gen) == 0){
                        p = Vec3(x,y,0);
                        addCell(Cell(p,this->sizeCell, 1));
                    }
                }
            }
        }

        void soup3D(int res){

            this->tabCell.clear();

            std::random_device rd;
            std::mt19937 gen(rd());
            std::uniform_int_distribution<int> dist(0, 1);

            // Calcul sur le plan orthogonal à Z (ajustable)
            Vec3 p ;
            float x, y, z ;
            float sc = this->sizeCell ;
            float base = (-sc * float(res))/2.f ;

            for(float i = 0 ; i < res ; i++){
                x = base + sc * i ;
                for(float j = 0 ; j < res ; j++){
                    y = base + sc * j ;
                    for(int k = 0 ; k < res ; k++){
                        z = base + sc * k ;
                        if(dist(gen) == 0){
                            p = Vec3(x,y,z);
                            addCell(Cell(p,this->sizeCell, 1));
                        }
                    }
                }
            }
        }

        std::vector<Cell> convertInAliveCells(std::vector<Vec3> pos, float sizeCell){
            std::vector<Cell> cells ;
            Cell cell ;

            for(const Vec3 &p : pos){
                cell = Cell(p, sizeCell, 1);
                cells.push_back(cell);
            }

            return cells ;
        }

        int nextState2D(Cell c, std::vector<Cell> tab){

            int nbNeighbor = 0 ;
            std::vector<Vec3> neighborPos = c.neighborPos2D() ;
            for(Vec3 v : neighborPos){
                if(isAlive(v, tab)) nbNeighbor++ ;
            }

            int actuelState = c.getState() ;
            if((actuelState == 0) && (nbNeighbor == 3)){return 1 ;}
            if((actuelState == 1) && (nbNeighbor != 2) && (nbNeighbor != 3)){return 0 ;}
            return actuelState ;
        }

        void nextGeneration2D(){

            std::vector<Cell> currentGen = this->tabCell ;
            std::vector<Cell> allCells, finalCells ;
            bool exists ;

            std::cout << "nbCell vivante : " << currentGen.size() << std::endl ;

            // On calcul toutes les cellules à analyser
            allCells = currentGen ;
            for (Cell c : currentGen) {
                for (Vec3 pos : c.neighborPos2D()) {

                    exists = false ;
                    for (Cell c : allCells){
                        if (isSamePosition(c.getPosition(), pos)){
                            exists = true;
                            break;
                        }
                    }
                    if (!exists){
                        allCells.push_back(Cell(pos, this->sizeCell, isAlive(pos, this->tabCell) ? 1 : 0));
                    }
                }
            }

            // On analyse
            for(Cell c : allCells){
                if(nextState2D(c, this->tabCell)){
                    finalCells.push_back(Cell(c.getPosition(), c.getSize(), 1));
                }
            }

            this->tabCell = finalCells ;
        }


        void printCells(){
            for(Cell c : this->tabCell){
                c.printCell();
            }
        }
};

#endif