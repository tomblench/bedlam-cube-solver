#include <bitset>
#include <set>
#include <iostream>
#include <vector>

#include "Shapes.h"

void print3d(std::bitset<64> in);

std::bitset<64> permute(std::bitset<64> in, int which);

bool move1(std::bitset<64> &in, std::bitset<64> &out, int which, int dir);
bool move(std::bitset<64> &in, std::bitset<64> &out, int which, int howMuch);

std::bitset<64>* walls[6];
std::vector<std::bitset<64> > shapes;
std::vector<std::set<unsigned long>* > allShapesAllRotsAndOffsets;

unsigned int maxBits = 0;

void placePiece(unsigned long board, std::vector<std::set<unsigned long>*>::iterator allPieces, std::vector<std::set<unsigned long>*>::iterator end, std::vector<unsigned long> pieceHistory);


int main(int argc, char **argv) {

    // setup walls so we know how much we can move things about
    std::bitset<64> side1("1000100010001000100010001000100010001000100010001000100010001000");
    std::bitset<64> side2("0001000100010001000100010001000100010001000100010001000100010001");
    std::bitset<64> side3("1111000000000000111100000000000011110000000000001111000000000000");
    std::bitset<64> side4("0000000000001111000000000000111100000000000011110000000000001111");
    std::bitset<64> side5("1111111111111111000000000000000000000000000000000000000000000000");
    std::bitset<64> side6("0000000000000000000000000000000000000000000000001111111111111111");

    walls[0] = &side1;
    walls[1] = &side2;
    walls[2] = &side3;
    walls[3] = &side4;
    walls[4] = &side5;
    walls[5] = &side6;

    shapes.push_back(std::bitset<64>(shape_a));
    shapes.push_back(std::bitset<64>(shape_b));
    shapes.push_back(std::bitset<64>(shape_c));
    shapes.push_back(std::bitset<64>(shape_d));
    shapes.push_back(std::bitset<64>(shape_e));
    shapes.push_back(std::bitset<64>(shape_f));
    shapes.push_back(std::bitset<64>(shape_g));
    shapes.push_back(std::bitset<64>(shape_h));
    shapes.push_back(std::bitset<64>(shape_i));
    shapes.push_back(std::bitset<64>(shape_j));
    shapes.push_back(std::bitset<64>(shape_k));
    shapes.push_back(std::bitset<64>(shape_l));
    shapes.push_back(std::bitset<64>(shape_m));

    for (std::vector<std::bitset<64> >::iterator it = shapes.begin(); it != shapes.end(); it++) {

        //printf("shape has %lu bits\n", it->count());

        std::set<unsigned long> allRots;
        std::set<unsigned long> *allRotsAndOffsets = new std::set<unsigned long>;
        
        // the unrotated shape
        allRots.insert(it->to_ulong());
        // now do all the rotations (there will be some repeats)
        std::bitset<64> rot1 = *it;
        for (int i=0;i<4;i++) {
            rot1 = (permute(rot1,1));
            allRots.insert(rot1.to_ulong());
            std::bitset<64> rot2 = rot1;
            for (int j=0;j<4;j++) {
                rot2 = (permute(rot2,2));
                allRots.insert(rot2.to_ulong());
                std::bitset<64> rot3 = rot2;
                for (int k=0;k<4;k++) {
                    rot3 = (permute(rot3,3));
                    allRots.insert(rot3.to_ulong());
                }
            }
        }
        //printf("number of rotations %lu\n", allRots.size());
        
        // now do all the moves (not all will be valid, only add valid ones)
        for (std::set<unsigned long>::iterator it = allRots.begin(); it != allRots.end(); it++) {
            // first stick in the unmoved one
            allRotsAndOffsets->insert(*it);
            std::bitset<64> orig(*it);
            std::bitset<64> moved;
            std::bitset<64> moved2;
            std::bitset<64> moved3;
            
            for (int i=-3;i<4;i++) {
                bool ok = move(orig,moved,0,i);
                if (ok) {
                    for (int j=-3;j<4;j++) {
                        ok = move(moved,moved2,1,j);
                        if (ok) {
                            for (int k=-3;k<4;k++) {
                                ok = move(moved2,moved3,2,k);
                                if (ok) {
                                    allRotsAndOffsets->insert(moved3.to_ulong());
                                }
                            }
                        }
                    }
                }
            }
        }

        printf("number of rotations & moves %lu \n", allRotsAndOffsets->size());
        allShapesAllRotsAndOffsets.push_back(allRotsAndOffsets);
    }
    
    for (std::vector<std::set<unsigned long>*>::iterator it = allShapesAllRotsAndOffsets.begin(); it != allShapesAllRotsAndOffsets.end(); it++) {
        std::set<unsigned long> *theseRotsAndOffsets = *it;
        for (std::set<unsigned long>::iterator it2 = theseRotsAndOffsets->begin(); it2 != theseRotsAndOffsets->end(); it2++) {
            
        }
    }

    unsigned long board = 0;
    placePiece(board, allShapesAllRotsAndOffsets.begin(), allShapesAllRotsAndOffsets.end(), std::vector<unsigned long>());


}

void placePiece(unsigned long board, std::vector<std::set<unsigned long>*>::iterator allPieces, std::vector<std::set<unsigned long>*>::iterator end, std::vector<unsigned long> pieceHistory) {
    // got through all the pieces?
    if (allPieces == end) {
        printf("*** ALL PIECES***\n");
        for (std::vector<unsigned long>::iterator it2 = pieceHistory.begin(); it2 != pieceHistory.end(); it2++) {
            print3d(std::bitset<64>(*it2));
            printf("\n===\n");
        }
        return;
    }
    // try each rotation & offset of this piece
    std::set<unsigned long> *theseRotsAndOffsets = *allPieces;

    for (std::set<unsigned long>::iterator it2 = theseRotsAndOffsets->begin(); it2 != theseRotsAndOffsets->end(); it2++) {        
        // success? recurse with next piece
        unsigned long tryBoard = board;
        if ((board & *it2) == 0) {
            tryBoard = board | *it2;
            unsigned int bits = __builtin_popcountl(tryBoard);
            std::vector<unsigned long> newPieceHistory = pieceHistory;
            newPieceHistory.push_back(*it2);
            if (bits > maxBits) {
                maxBits = bits; printf("best so far: %d bits, %lu pieces\n", bits, newPieceHistory.size()); }
            placePiece(tryBoard, allPieces+1, end, newPieceHistory);
        }
        // failure? carry on
    }
    return;
}

void print3d(std::bitset<64> in) {
    int i,j,k;
    for (i=0;i<4;i++) {
        for (j=0;j<4;j++) {
            for (k=0;k<4;k++) {
                if (in[k+j*4+i*16]) {
                     printf("#");
                } else {
                    printf("-");
                }
            }
            printf("\n");
        }
        printf("\n\n\n");
    }
}

bool move(std::bitset<64> &in, std::bitset<64> &out, int which, int howMuch) {
    if (howMuch==0) {
        out = in;
        return true;
    }

    int sign = 0;
    if (howMuch<0) {
        howMuch = -howMuch;
        sign = 1;
    }
     
    std::bitset<64> toMove = in;
    std::bitset<64> moved;
    bool ok = true;
    for(int i=0;i<howMuch;i++) {
        ok = ok && move1(toMove, moved, which, sign);
        toMove = moved;
    }
    out = moved;
    return ok;
}

bool move1(std::bitset<64> &in, std::bitset<64> &out, int which, int dir) {
    int shift = 1 << which*2;
    int wallNum = which*2+dir;
    std::bitset<64> *wall = walls[wallNum];
    if ((*wall & in).count() > 0) {
        return false;
    }
    if (dir == 0) {
        out = in << shift;
    } else {
        out = in >> shift;
    }
    return true;
}

// permute and reflect, to rotate
std::bitset<64> permute(std::bitset<64> in, int which) {
    int i,j,k;
    std::bitset<64> out;
    for (i=0;i<4;i++) {
        for (j=0;j<4;j++) {
            for (k=0;k<4;k++) {
                int _i,_j,_k;
                switch (which) {
                    // no permutation - optimise by returning in?
                case 0:
                    _i = i;
                    _j = j;
                    _k = k;
                    break;
                case 1:
                    _i = -j+3;
                    _j = i;
                    _k = k;
                    break;
                case 2:
                    _i = -k+3;
                    _j = j;
                    _k = i;
                    break;
                case 3:
                    _i = i;
                    _j = k;
                    _k = -j+3;
                    break;
                }
                out[i*16+j*4+k] = in[_i*16+_j*4+_k]; 
            }
        }
    }
    return out;
}
