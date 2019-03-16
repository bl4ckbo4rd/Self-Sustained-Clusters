#include "problems.h"

void f_ERgraph(Graph& G){
    
    //here we give information on the structure of the graph
    G.graphStructure();
    //here we give information on the configuration studied
    G.printConfiguration();
}

void f_BPstep(Graph& G, double eps){
    
    
    Messages mess(G,eps);
    
    //here we initialize messages
    mess.initMessages();
    
    //here we update the messages, we normalize them and we compute the marginals
    mess.messUpdate();
    mess.messNormalize();
    mess.linkMarginals();
    
    //we print the state of messages and of the marginals
    mess.messState();
    mess.linkMarginalState();
    
}

void f_BPiteration(Graph& G, double th, int T, double eps){
    
    
    Messages mess(G,eps);
    
    //here we initialize messages
    mess.initMessages();
    
    //here we iterate the BP messages for T BP-time sweeps until the largest
    //difference found between marginals is smaller or equal to th.
    
    bool verbose = 1;
    mess.BPiteration(th, T, verbose);
    
}


