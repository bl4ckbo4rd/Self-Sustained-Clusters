#include "graph.h"



//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------// methods of class Node
//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//



int Node::numberOfLinks(){
  return v_link.size();
};



//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//--------------------------------------------------------------------------------------------------------------------------------------------------------------// methods of class Factor
//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//



//input variables:
//p_l : index of the link

Link::Link(int p_l, int p_J) : l ( p_l ), J (p_J) {
    v_node.reserve(2);
};



//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------// methods of class Graph
//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//



//input variables:
//p_N : # of nodes

Graph::Graph() {
    
    getGraph();
    getConfiguration();
    
};

int Graph::numberOfTotalLinks(){
    return E.size();
};


int Graph::numberOfTotalNodes(){
    return v.size();
};


void Graph::neighsOfNode(int i){
    cout << "node " << i << " has " << v[i].numberOfLinks() << " neighbours: " << endl;
    for (vector<int>::iterator it = v[i].v_neigh.begin() ; it != v[i].v_neigh.end(); ++it)
        cout << *it << endl;
};


void Graph::nodesOfLink(int l){
    cout << "link " << l << " connects the two nodes : " << endl;
    for (vector<int>::iterator it = E[l].v_node.begin() ; it != E[l].v_node.end(); ++it)
        cout << *it << endl;
};



int Graph::addLink(int p_l, vector<int> v_dl, int p_J){
    
    vector <int> v1 = v_dl;
    std::sort(v1.begin(), v1.end());
    
    int flag=1;
    
    //before adding a link we check that the link does not already exist.
    //if the link already exists, flag is set to 0.
    //to this aim it is sufficient to check that the first node of v_dl does not appear in a link with the other node of v_dl.
    
    for(vector<int>::iterator it_l = v[v_dl[0]].v_link.begin(); it_l != v[v_dl[0]].v_link.end(); it_l++){
        
        vector <int> v2 = E[*it_l].v_node;
        std::sort(v2.begin(), v2.end());
        
        if (v1 == v2){
            flag=0;
        }

    }
    
    if(flag){
        
        Link l(p_l, p_J);
        
        for (int i=0; i<2; i++)
            l.v_node.push_back(v_dl[i]);
        
        E.push_back(l);
        
        for(int i=0; i<2; i++)
            v[v_dl[i]].v_link.push_back(p_l);
        
        v[v_dl[0]].v_neigh.push_back(v_dl[1]);
        v[v_dl[1]].v_neigh.push_back(v_dl[0]);
        
        
    }
    
    return flag;
    
};



void Graph::graphStructure(){
    
    cout << endl;
    cout << endl;
    
    cout << "structure of the graph: " << endl;
    cout << endl;
    
    for (int i=0; i<N; i++)
        neighsOfNode(i);
    
    for (int l=0; l<M; l++){
        cout << "coupling on link " << l << ": " << E[l].J << endl;
        nodesOfLink(l);
    }
        
    cout << endl;
    cout << endl;

};

void Graph::printConfiguration(){
    
    cout << endl;
    cout << endl;
    
    cout << "configuration: " << endl;
    cout << endl;
    
    for (int i=0; i<N; i++)
        cout << v[i].s << endl;
    
    cout << endl;
    cout << endl;
    
    
};


//input variables:

void Graph::getGraph(){
    
    string temp;
    
    int i    = 0;
    int l    = 0;
    int flag = 0;
    vector<int> edge;
    
    //line "i" of the file graph.txt contains the list of neighbours of the node "i". near each site, there is the value of the coupling
    //e.g. :
    //1 +1 3 -1 5 +1
    //2 -1 0 +1
    //means that node 0 is connected to node 1, 3 and 5 with couplings given by +1, -1 and +1
    //and that node 1 is connected to node 2 and 0 with couplings given by -1 and +1.
    ifstream infile("graph.txt");
    
    // If we couldn't open the output file stream for reading
    if (!infile)
    {
        cerr << "Uh oh, graph.txt could not be opened for reading!" << endl;
        exit(1);
    }
    
    vector< vector <int> > neighs;

    while (std::getline(infile, temp)) {
        //for each line of the file we create a Node
        v.push_back (Node(i));
        i++;
    }
    
    i=0;
    
    //reset the getline to the beginning of the file
    infile.clear();
    infile.seekg(0, ios::beg);
    
    while (std::getline(infile, temp)) {
    
        istringstream buffer(temp);
        vector<int> line( (istream_iterator<int>(buffer)), istream_iterator<int>() );
        neighs.push_back(line);

        //c is the connectivity of the node
        //the 0.5 factor comes from the structure of the file graph.txt where line i contains
        //a number of elements which is twice the number of its neighbours. see below
        //line "i" of the file graph.txt contains the list of neighbours of the node "i". near each site, there is the value of the coupling
        //e.g. :
        //1 +1 3 -1 5 +1
        //2 -1 0 +1
        //means that node 0 is connected to node 1, 3 and 5 with couplings given by +1, -1 and +1
        //and that node 1 is connected to node 2 and 0 with couplings given by -1 and +1.
        
        int c = (int)(0.5 * neighs[i].size());
        
        for (int r = 0; r < c; r++){
            edge    = make_vector<int>() << i << neighs[i][2*r];
            int p_J = neighs[i][2*r+1];
            
            flag = addLink(l, edge, p_J);
            if (flag)
                l++;
        }
        i++;
    }
    
    N=i;
    M=l;
    
};



void Graph::getConfiguration(){
    
    ifstream infile("s.txt");
    
    // If we couldn't open the output file stream for reading
    if (!infile)
    {
        cerr << "Uh oh, s.txt could not be opened for reading!" << endl;
        exit(1);
    }
    int i=0;
    
    while(!infile.eof()) {
        
        infile >> v[i].s;
        i++;
    }
    
};



//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//------------------------------------------------------------------------------------------------------------------------------------------------------------// methods of class Messages
//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//



Messages::Messages(Graph& p_G, double p_eps) : G (p_G), eps(p_eps) {
    
    N=G.numberOfTotalNodes();
    M=G.numberOfTotalLinks();
    
    mess.resize(2*M);
    update_mess.resize(2*M);

    link_marginal.resize(M);
    
    super_marginal.resize(M);
    prev_super_marginal.resize(M);
    
    initMessages();

};



void Messages::initMessages(){

    //Init messages to the uniform distributions
    for(vector<Node>::iterator it_i = G.v.begin(); it_i != G.v.end(); ++it_i){
        int size_di = G.v[it_i->n].numberOfLinks();  //di is the set of neighbours attached to node i. size_di is the connectivity of the node
        mess[it_i->n].resize(size_di);
        update_mess[it_i->n].resize(size_di);
        
        for (vector<int>::iterator it_j = G.v[it_i->n].v_neigh.begin(); it_j != G.v[it_i->n].v_neigh.end(); ++it_j){
            int index_j = distance (G.v[it_i->n].v_neigh.begin(), it_j);
        
            //mess[i][j] will have 4 components index by t = xi + 2*xj
            //in such a way that
            //t=0 when xi=0 and xj=0;
            //t=1 when xi=1 and xj=0;
            //t=2 when xi=0 and xj=1;
            //t=3 when xi=1 and xj=1;
            
            vector <double> tmp_mess(4,1./4);
            vector <double> tmp_update_mess(4,0.);
            
            mess[it_i->n][index_j].resize(4);
            mess[it_i->n][index_j] = tmp_mess;

            update_mess[it_i->n][index_j].resize(4);
            update_mess[it_i->n][index_j] = tmp_update_mess;

        }
        
    }

    
    //init link_marginals to uniform distributions
    //init prev_marginals to zero
    //init super_marginals to zero
    for(vector<Link>::iterator it_l=G.E.begin(); it_l !=G.E.end(); ++it_l){
        link_marginal[it_l->l].resize(2);
        link_marginal[it_l->l][0].resize(2,0.5);
        link_marginal[it_l->l][1].resize(2,0.5);
        
        super_marginal[it_l->l].resize(4,0.);
        prev_super_marginal[it_l->l].resize(4,0.);
        
    }
    
};



void Messages::messUpdate(){

    double u, v, s_j;
    double prod_mess;
    
    int theta;
    int i, j, t_ij, t_ki;
    int J_ij;
    int xk, z, k;
    
    //for each link i-j
    for (vector<Link>::iterator it_l = G.E.begin() ; it_l != G.E.end(); ++it_l){
    
        //we look at both the direction i -> j and j -> i
        for (int h=0; h<2 ; h++){
            
            vector <double> mess_ij(4,0.);
            
            i = G.E[it_l->l].v_node[h];
            j = G.E[it_l->l].v_node[(h+1)%2];
            
            vector<int>::iterator it = find(G.v[i].v_neigh.begin(), G.v[i].v_neigh.end(), j);
            int index_j = distance (G.v[i].v_neigh.begin(), it);
            
            //cout << "message from node " << i << " to node " << j << endl;
            //compute message mess_i_to_j
            vector <vector <double> > in_mess;
            //spins/magnetizations on the nodes k's
            vector <double> vec_s;
            //couplings between node i and nodes k's.
            vector <int> vec_J;
            
            k = 0; //k runs on the links between i and sites k's.
            z = 0; //z is the connectivity of node i less 1.
            
            //for each link k-i, where k is different from j
            for(vector<int>::iterator it_k = G.v[i].v_neigh.begin(); it_k != G.v[i].v_neigh.end(); ++it_k){
                if(*it_k != j){
                    //cout << "k: " << *it_k << endl;
                    vector<int>::iterator it = find(G.v[*it_k].v_neigh.begin(), G.v[*it_k].v_neigh.end(), i);
                    int index_i = distance (G.v[*it_k].v_neigh.begin(), it);
                
                    //we store the mess_k_to_i
                    in_mess.push_back(mess[*it_k][index_i]);
                    
                    //we store as well the spins on nodes k's and the J's between k's and i
                    vec_s.push_back( G.v[*it_k].s );
                    vec_J.push_back( G.E[G.v[i].v_link[k]].J );
                    z++;
                }
                k++;
            }
            
            // this is the number of combinations of the spins k's.
            int Nc = (int) pow(2.,z);
            
            //in each of these combinations, sigma_k's may assume 0 or 1 values. we store these values in the matrix x
            //e.g. let's consider the case when i has 3 other neighbours besides j: in this case z=3 and Nc=2^3=8 and x will be the matrix
            //0 0 0
            //0 0 1
            //0 1 0
            //0 1 1
            //1 0 0
            //1 0 1
            //1 1 0
            //1 1 1
            vector < vector <int> > x;
            x.resize(Nc);
            
            //cout << "Printing z and Nc: " << z << " " << Nc << endl;
            
            for (int r = 0; r < Nc; r++){
                x[r].resize(z);
                
                for (int k = 0; k < z; k++)
                    x[r][k] = ( ( r & (int)pow(2.,k) ) >> k ) ;
            }
            
            //we store s_j and the coupling between i and j
            s_j  = G.v[j].s;
            J_ij = G.E[it_l->l].J;
            
            
            //u and v field have to be computed only when xi = 1
            vector < vector <double> > u;
            vector < vector <double> > v;
            
            u.resize(Nc);
            v.resize(Nc);
            
            for (int xi = 0; xi < 2; ++xi){
                for (int xj = 0; xj < 2; ++xj){
                
                    t_ij = xi + 2 * xj;
                
                    for (int r = 0; r < Nc; r++){
                        
                        if (xi == 1){
                            u[r].resize(2,0.);
                            v[r].resize(2,0.);
                        }
                        
                        //for each value of r (i.e. for each combination of the xk's)
                        //we store the product of messages mess_k_to_i in prod_mess.
                        prod_mess = 1.;
                        
                        for (int k = 0; k < z; k++){
                            xk = x[r][k];
                            
                            t_ki = xk + 2 * xi;
                            
                            prod_mess *= in_mess[k][t_ki];
                            
                            if (xi == 1){
                                u[r][xj] += vec_J[k] * xk * vec_s[k];
                                v[r][xj] += vec_J[k] * (1 - xk) * vec_s[k];
                                
                                //cout << "************** u and v at xi, xj = " << xi << " " << xj << " : ---------------> " << u[r][xj] << " " << v[r][xj] << " for k= " << k << " and xk= " << xk << endl;
                                
                            }
                            
                        }
                    
                        theta = 1;
                        
                        //for each value of r (i.e. for each combination of the xk's)
                        //we also store the value of the fields u and v, that depends
                        //on the values of the xk's and of xj.
                        
                        if (xi == 1){
                            u[r][xj] += J_ij * xj * s_j;
                            v[r][xj] += J_ij * (1 - xj) * s_j;
                            
                            if ( u[r][xj]*u[r][xj] - v[r][xj]*v[r][xj] >= eps)
                                theta = 1;
                                //theta = u[r][xj]*u[r][xj] - v[r][xj]*v[r][xj] + 0.1;
                                //theta = 2 * exp (u[r][xj]*u[r][xj] - v[r][xj]*v[r][xj]) / (2 * cosh (u[r][xj]*u[r][xj] - v[r][xj]*v[r][xj])) - 1.;
                            else
                                theta = 0;
                            
                            //cout << "************** u and v with xj at xi, xj = " << xi << " " << xj << " : ---------------> " << u[r][xj] << " " << v[r][xj] << endl;
                        }
                        
                        mess_ij[t_ij] += theta * prod_mess;
                        
                    }
                    
                    //cout << "mess ( t_ij = " << t_ij << " ) = " << mess_ij[t_ij] << endl;
                    
                }
                
            }
            
            update_mess[i][index_j] = mess_ij;
            
        }
        
    }
    
    
    
};



bool Messages::messNormalize(){
    
    double sum;
    
    int f = 1;

    int i,j;
    
    for (vector<Link>::iterator it_l = G.E.begin() ; it_l != G.E.end(); ++it_l){
        
        //we look at both the direction i -> j and j -> i
        for (int h=0; h<2 ; h++){
            i = G.E[it_l->l].v_node[h];
            j = G.E[it_l->l].v_node[(h+1)%2];

            vector<int>::iterator it = find(G.v[i].v_neigh.begin(), G.v[i].v_neigh.end(), j);
            int index_j = distance (G.v[i].v_neigh.begin(), it);
            
            sum = 0.;
            for (int t=0; t<4; ++t){
                sum += update_mess[i][index_j][t];
            }
            
            if (sum !=0 ){
                for (int t=0; t<4; ++t){
                    update_mess[i][index_j][t]/=sum;
                }
            }
            else
                f=0;
            
        }
    }
    
    return f;
    
};



void Messages::superMarginals(){
    
    int i,j;
    double x0, x1;
    
    for (vector<Link>::iterator it_l = G.E.begin() ; it_l != G.E.end(); ++it_l){

        vector <double> marg(4,0.);
        
        i = G.E[it_l->l].v_node[0];
        j = G.E[it_l->l].v_node[1];
        
        vector<int>::iterator it_j = find(G.v[i].v_neigh.begin(), G.v[i].v_neigh.end(), j);
        int index_j = distance (G.v[i].v_neigh.begin(), it_j);

        vector<int>::iterator it_i = find(G.v[j].v_neigh.begin(), G.v[j].v_neigh.end(), i);
        int index_i = distance (G.v[j].v_neigh.begin(), it_i);

        //cout << "marginal of the SuperNode at link " << it_l->l << ":" << endl;
        
        marg[0] = update_mess[i][index_j][0] * update_mess[j][index_i][0];
        marg[1] = update_mess[i][index_j][1] * update_mess[j][index_i][2];
        marg[2] = update_mess[i][index_j][2] * update_mess[j][index_i][1];
        marg[3] = update_mess[i][index_j][3] * update_mess[j][index_i][3];
        
        super_marginal[it_l->l] = marg;
        
    }

};


void Messages::linkMarginals(){

    double x0, x1;
    
    for (vector<Link>::iterator it_l = G.E.begin() ; it_l != G.E.end(); ++it_l){

        //at link l, marginal[l] contains two distributions, one for each of the spins that are connected from l.
        //for the first spin, i, the marginal in contained in marginal[l][0]
        //the first component,  marginal[l][0][0] is the probability that sigma_i = 0, obtained summing over sigma_j
        //the second component, marginal[l][0][1] is the probability that sigma_i = 1, obtained summing over sigma_j
        
        x0 = super_marginal[it_l->l][0] + super_marginal[it_l->l][2];
        x1 = super_marginal[it_l->l][1] + super_marginal[it_l->l][3];
        
        link_marginal[it_l->l][0][0] = x0 / (x0 + x1);
        link_marginal[it_l->l][0][1] = x1 / (x0 + x1);
        
        //for the second spin, j, the marginal in contained in marginal[l][1]
        //the first component,  marginal[l][1][0] is the probability that sigma_j = 0, obtained summing over sigma_i
        //the second component, marginal[l][1][1] is the probability that sigma_j = 1, obtained summing over sigma_i
        
        x0 = super_marginal[it_l->l][0] + super_marginal[it_l->l][1];
        x1 = super_marginal[it_l->l][2] + super_marginal[it_l->l][3];
        
        link_marginal[it_l->l][1][0] = x0 / (x0 + x1);
        link_marginal[it_l->l][1][1] = x1 / (x0 + x1);
        
    }

};



void Messages::nodeMarginals(){
  
    node_marginal.resize(N);
    int n = 0;
    int i,j;
    
    for (vector<Link>::iterator it_l = G.E.begin() ; it_l != G.E.end(); ++it_l){
        if (n<N){
        
            i = G.E[it_l->l].v_node[0];
            j = G.E[it_l->l].v_node[1];
        
            if(!node_marginal[i].size()){
                node_marginal[i]=link_marginal[it_l->l][0];
                n++;
            }
        
            if(!node_marginal[j].size()){
                node_marginal[j]=link_marginal[it_l->l][1];
                n++;
            }
    
        }
        else
            break;
        
    }
    
};



void Messages::updateAndStore(){

    mess = update_mess;
    prev_super_marginal = super_marginal;

};



void Messages::messState(){

    cout << endl;
    cout << "---*---*---*---printing messages---*---*---*---" << endl;

    int i,j;
    
    for (vector<Link>::iterator it_l = G.E.begin() ; it_l != G.E.end(); ++it_l){
        //we look at both the direction i -> j and j -> i
        for (int h=0; h<2 ; h++){
            i = G.E[it_l->l].v_node[h];
            j = G.E[it_l->l].v_node[(h+1)%2];
            
            vector<int>::iterator it = find(G.v[i].v_neigh.begin(), G.v[i].v_neigh.end(), j);
            int index_j = distance (G.v[i].v_neigh.begin(), it);
        
            cout << "message from node " << i << " to node " << j << endl;
            cout << mess[i][index_j][0] << " " << mess[i][index_j][1] << " " << mess[i][index_j][2] << " " << mess[i][index_j][3] << endl;
        }
    }

};


 
void Messages::linkMarginalState(){

    cout << endl;
    cout << "---*---*---*---printing link marginals---*---*---*---" << endl;
    
    for (vector<Link>::iterator it_l = G.E.begin() ; it_l != G.E.end(); ++it_l){
        
        cout << "marginal computed at link " << it_l->l << " of node " << G.E[it_l->l].v_node[0] << ":" << endl;
        cout <<  link_marginal[it_l->l][0][0] << " " << link_marginal[it_l->l][0][1] << endl;

        cout << "marginal computed at link " << it_l->l << " of node " << G.E[it_l->l].v_node[1] << ":" << endl;
        cout <<  link_marginal[it_l->l][1][0] << " " << link_marginal[it_l->l][1][1] << endl;

    }
    
};



void Messages::nodeMarginalState(){
    
    cout << endl;
    cout << "---*---*---*---printing node marginals---*---*---*---" << endl;
    
    for(vector<Node>::iterator it_i = G.v.begin(); it_i != G.v.end(); ++it_i){
        
        cout << "marginal of node " << it_i-> n << ":" << endl;
        cout <<  node_marginal[it_i->n][0] << " " << node_marginal[it_i->n][1] << endl;
        
    }
    
    cout << "---*---*---*---printing node marginals of sigma = 1---*---*---*---" << endl;
    for(vector<Node>::iterator it_i = G.v.begin(); it_i != G.v.end(); ++it_i){
        cout << node_marginal[it_i->n][1] << endl;
    }
    
    
};


    


double Messages::compareSuperMarginals(){
    
    double tmp, max = 0.;
    
    for (vector<Link>::iterator it_l = G.E.begin() ; it_l != G.E.end(); ++it_l){
        
        for (int k = 0; k < 3; k++){
            tmp = abs(super_marginal[it_l->l][k] - prev_super_marginal[it_l->l][k]);
            if (tmp > max)
                max = tmp;
        }
    }
    
    return max;
    
};



void Messages::BPiteration(double th, int T, bool verbose){
    
    int    t = 0;
    double tmp_th = 1;
    
    
    while (tmp_th > th && t < T){

        messUpdate();
        messNormalize();
        superMarginals();
        
        tmp_th = compareSuperMarginals();

        updateAndStore();
        
        if(verbose){
            messState();
            linkMarginals();
            linkMarginalState();
            cout << endl;
            cout << "BP iteration: at time t=" << t << " the maximum error between current and previous super_marginals is " << tmp_th << endl;
        }

        t++;
    
    }
    
    cout << endl;
    cout << "BP iteration stopped with an error equal to " << tmp_th << " and the final node_marginals are " << endl;

    linkMarginals();
    nodeMarginals();
    nodeMarginalState();
    
};



void vec_print(vector<int>& vec){
    for (int i=0; i<vec.size(); i++)
        cout << vec[i] << ' ';
    cout << endl;
}


