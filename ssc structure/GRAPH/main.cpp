#include "problems.h"

int main (int argc, char *argv[]){

    //N : # of Nodes
    //M : # of Links
    
    Graph G;
    //f_BPstep(G,0.);
    
    double  th  = 0.00001;
    int     T   = 100;
    double  eps = 0.1;
    
    f_BPiteration(G, th, T, eps);

    return 1;

}
