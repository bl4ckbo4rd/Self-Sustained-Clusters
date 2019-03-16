#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <algorithm>
#include <cmath>
#include <stdlib.h>
#include <cassert>

using namespace std;



//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//------------------------------------------------------------------------------------------------------------------------------------------------------------// class representing a node
//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//



class Node{
public:

    Node(int p_n) : n (p_n) {};
  
    int n;                                                //n is the index of the Node
    vector <int> v_link;                                  //v_link contains the indices of links attached to the node
    vector <int> v_neigh;                                 //v_neigh contains the indices of the nodes attached to the node
    
    int numberOfLinks();                                  //this method returns the number of links to which the Node is attached
  
    double s;                                             //value of the spin on the node

};



//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//----------------------------------------------------------------------------------------------------------------------------------------------------------// class representing a factor
//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//



class Link{
public:
    
    Link(int, int);

    int l;                                                      //l is the index of the link
    vector <int> v_node;                                        //v_node contains the indices of nodes connected by l

    int J;                                                      //value of the coupling on the link
    
};



//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------// class representing the graph
//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//



class Graph{
public:
    
    Graph();
    
    int N;
    int M;
    
    vector <Node> v;                                        //v containss all the Nodes of the Graph.
                                                            //this vector is filled by the method getGraph.
    
    vector <Link> E;                                        //E contains all the Links of the Graph.
    
    int addLink(int, vector<int>, int);                     //the addLink method implements the operations needed when adding a link in the graph
                                                            //namely one needs to create a Link object l, store its neighour variables and for each of them
                                                            //add the index of l as a neghbouring link.
                                                            //input variables: link index, vector of nodes attached to the link (connected to the link).
                                                            //output: 1 if the operation can be done, 0 if not (the link already exists).
    
    int numberOfTotalLinks();                               //this method returns the total number of Links in the Graph.
    
    int numberOfTotalNodes();                               //this method returns the total number or nodes in the graph.

    void neighsOfNode(int);                                 //this method returns the links attached to the input node
    void nodesOfLink(int);                                  //this method returns the nodes connected by the link
    
    void getGraph();                                        //this method reads the structure of the graph from the file graph.txt and it is called from the constructor
    
    void getConfiguration();                                //this method gets the s-configuration
    
    void graphStructure();                                  //this method prints the structure of the graph
    void printConfiguration();                              //this method prints the configuration studied
    
};



//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------// class representing the BP messages
//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//



class Messages{
public:

    Messages();
    
    Messages(Graph &, double);

    double eps;                                                 //eps is defined by the inequality u^2 - v^2 > eps
    
    int N;
    int M;
    Graph G;

    vector < vector < vector <double> > > mess;                 //mess contains the messages from nodes to nodes
                                                                //Loosely speaking
                                                                //mess[i][j][t] is the t-th component of the message from node i to node j
                                                                //t = x_i + 2 x_j, i.e. t = 0,1,2,3 while (x1 xj) = [(0,0);(1,0);(0,1);(1,1)].
    
    vector < vector < vector <double> > > update_mess;          //Update_mess contains the messages from nodes to nodes after a BP-sweep

    vector < vector < vector <double> > > link_marginal;        //link_marginal probability
                                                                //given the particular form of the problem, where we have a super-variable for each link of the original graph
                                                                //we have the same variables in multiple super-variables and
                                                                //at link l, marginal[l] contains two marginals, one for each of the spins that are connected from l.

    vector < vector <double> > node_marginal;                   //node_marginal probability
                                                                //here we store in a less redundant way the information contained in link_marginal
                                                                //considering for each site i the marginal on that site.
                                                                //IMPORTANT: to improve performaces, it is RECOMMENDED to call this method only after the convergence of BP equations
                                                                //otherwise, if we want to call it after every BP sweep, for instance to study nodeMarginalState(),
                                                                //it is necessary to free the vector with marginal_node.swap( vector < vector<double> >() );

    
    vector < vector <double> > super_marginal;                  //super_marginal contains the marginal of the super variables
    vector < vector <double> > prev_super_marginal;             //prev_super_marginal is used to store the values of the super_marginals at the previous BP time step
    
    void initMessages();                                        //this method initializes messages to umbiased values 1/4.
                                                                //the marginals are set to the uniform distribution as well.
                                                                //it is called inside the constructor
    
    void messUpdate();                                          //messUpdate implements a BP-sweep on the whole graph
                                                                //updated messages are stored in update_mess
    
    bool messNormalize();                                       //this method normalizes the messagess. it retuns 0 if the probability is not normalizable (all entries equal to 0)
    
    void superMarginals();                                      //this method computes (do not print) super_marginals
    void linkMarginals();                                       //this method computes (do not print) link_marginals
    void nodeMarginals();                                       //this method computes (do not print) node_marginals
    
    void messState();                                           //this method prints the state of the messages
    void linkMarginalState();                                   //this method prints the state of the link_marginals
    void nodeMarginalState();                                   //this method prints the state of the node_marginals

    
    double compareSuperMarginals();                             //this method compare super_marginals at time step t-1 and t during BPiteration

    void BPiteration(double, int, bool);                        //this method iterates BP equations by calling BP_sweep until convergence
                                                                //input variables:
                                                                //th      : this value sets the convergence quality. set it to ~10^-3.
                                                                //T       : maximum iteration time. set it to ~ N.
                                                                //verbose : set to one to have a verbose version
    
    void updateAndStore();                                      //this method sets the update_mess in mess for the next BP-sweep
                                                                //and store the current super_marginals in prev_super_marginals
    
};



//---------------------------------------------------------------------------------------------------------------------------------------------------------------------// useful functions

//this function defines allows to fill a vector in one single line
//it has been downloaded from https://gist.github.com/pablomtz/5577626
//examples:
//vector<int> v = make_vector<int>() << 0 << 0 << 1 << 1 << 1 << 0 << 0 << 1 << 1;

template <typename T>
class make_vector {
 public:
  typedef make_vector<T> my_type;
  my_type& operator<< (const T& val) {
    data_.push_back(val);
    return *this;
  }
  operator std::vector<T>() const {
    return data_;
  }
 private:
  std::vector<T> data_;
};

//this function print all the elements of a vector.

void vec_print(vector<int>& vec);


