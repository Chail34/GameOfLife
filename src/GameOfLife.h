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
#include <unordered_set>
#include <unordered_map>

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
            // Complexité : O(n) où n le nombre de cellules vivantes

            // Si cette position appartient à une cellule du jeu, alors c'est la position d'une cellule vivante
            for(Cell c : tab){
                if(isSamePosition(c.getPosition(), pos))
                return true ;
            }
            return false ;
        }

        void addCell(Cell c){ // Ajoute sans doublons
            // Complexité : O(n) où n le nombre de cellules vivantes
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

            // Calcul sur le plan orthogonal à Z (ajustable : il faut aussi changer neighborPos2D() dans Cell.h)

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

        int nextState3D(Cell c, std::vector<Cell> tab){

            int nbNeighbor = 0 ;
            std::vector<Vec3> neighborPos = c.neighborPos3D() ;
            for(Vec3 v : neighborPos){
                if(isAlive(v, tab)) nbNeighbor++ ;
            }

            int actuelState = c.getState() ;
            if((actuelState == 0) && (nbNeighbor == 4)){return 1 ;}
            if((actuelState == 1) && (nbNeighbor != 3) && (nbNeighbor != 4)){return 0 ;}
            return actuelState ;
        }
        void nextGeneration2D(){
            // Hash encode/decode helpers
            auto encode = [](int x, int y)->int64_t{
                return ( (int64_t)x << 32 ) ^ (uint32_t)y;
            };
            auto decode = [](int64_t key)->std::pair<int,int>{
                int x = int(key >> 32);
                int y = int(uint32_t(key & 0xffffffff));
                return {x,y};
            };

            const float sc = this->sizeCell;

            // Build alive set (grid coordinates)
            std::unordered_set<int64_t> alive;
            for(Cell c : this->tabCell){
                int ix = lround(c.getPosition()[0] / sc);
                int iy = lround(c.getPosition()[1] / sc);
                alive.insert(encode(ix, iy));
            }

            // Count neighbors for every neighbor of every alive cell
            std::unordered_map<int64_t, int> neighborCount;
            for(const int64_t key : alive){
                auto [ix, iy] = decode(key);
                for(int dx = -1; dx <= 1; ++dx){
                    for(int dy = -1; dy <= 1; ++dy){
                        if(dx == 0 && dy == 0) continue;
                        neighborCount[ encode(ix + dx, iy + dy) ]++;
                    }
                }
            }

            // Determine next generation in linear time
            std::vector<Cell> next;
            next.reserve(neighborCount.size()); // heuristic

            // Also include currently alive cells that had zero neighbor entries (isolated),
            // need to consider them: they have 0 neighbors and should die, so no add.
            for(const auto &kv : neighborCount){
                int64_t key = kv.first;
                int cnt = kv.second;
                bool isCurrAlive = (alive.find(key) != alive.end());
                if((!isCurrAlive && cnt == 3) || (isCurrAlive && (cnt == 2 || cnt == 3))){
                    auto [ix, iy] = decode(key);
                    Vec3 p( ix * sc, iy * sc, 0.f );
                    next.emplace_back( Cell(p, sc, 1) );
                }
            }

            this->tabCell = std::move(next);
        }

        void nextGeneration3D(){
            // Hash key for 3D integer grid coords
            struct Key {
                int x, y, z;
                Key(int a=0,int b=0,int c=0):x(a),y(b),z(c){}
                bool operator==(Key const& o) const noexcept { return x==o.x && y==o.y && z==o.z; }
            };
            struct KeyHash {
                size_t operator()(Key const& k) const noexcept {
                    // simple spatial hash (large prime multipliers)
                    return (static_cast<size_t>(k.x) * 73856093u)
                         ^ (static_cast<size_t>(k.y) * 19349663u)
                         ^ (static_cast<size_t>(k.z) * 83492791u);
                }
            };

            const float sc = this->sizeCell;

            // Build alive set (grid coordinates)
            std::unordered_set<Key, KeyHash> alive;
            alive.reserve(this->tabCell.size()*2);
            for(Cell c : this->tabCell){
                int ix = lround(c.getPosition()[0] / sc);
                int iy = lround(c.getPosition()[1] / sc);
                int iz = lround(c.getPosition()[2] / sc);
                alive.insert(Key(ix, iy, iz));
            }

            // Count neighbors for every neighbor of every alive cell (26 neighbors)
            std::unordered_map<Key, int, KeyHash> neighborCount;
            neighborCount.reserve(alive.size()*27);
            for(auto const &key : alive){
                for(int dx=-1; dx<=1; ++dx){
                    for(int dy=-1; dy<=1; ++dy){
                        for(int dz=-1; dz<=1; ++dz){
                            if(dx==0 && dy==0 && dz==0) continue;
                            Key nk(key.x + dx, key.y + dy, key.z + dz);
                            ++neighborCount[nk];
                        }
                    }
                }
            }

            // Determine next generation
            std::vector<Cell> next;
            next.reserve(neighborCount.size());

            for(auto const &kv : neighborCount){
                const Key &k = kv.first;
                int cnt = kv.second;
                bool isCurrAlive = (alive.find(k) != alive.end());
                // 3D rules in your code: birth at 4, survival at 3 or 4
                if((!isCurrAlive && cnt == 4) || (isCurrAlive && (cnt == 3 || cnt == 4))){
                    Vec3 p( k.x * sc, k.y * sc, k.z * sc );
                    next.emplace_back(Cell(p, sc, 1));
                }
            }

            // Note: isolated currently-alive cells (0 neighbors) will die (no entry in neighborCount)
            this->tabCell = std::move(next);
        }

        void printCells(){
            for(Cell c : this->tabCell){
                c.printCell();
            }
        }
};

#endif