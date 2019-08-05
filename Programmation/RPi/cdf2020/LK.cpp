#include <utility>   //  pair
#include <list>      //  list
#include <iostream>
#include <ctime>
#include <cstdlib>

#include "LK.h"





bool LK::choose_x1(Node *tour, int n, bool *visited, Node *t1,
        edge *x1){
    
    // conditions in LK: 
    //        - x1 in tour  

    int i1 = (*t1).rank;
    int i2 = (i1+1)%n;
    Node t2 = tour[i2];

    x1->u = *t1; // this means: 
    x1->v = t2; //     x1 = (t1,t2)

    // mark us visited
    visited[(*t1).id] = true;
    visited[t2.id] = true;
    
    // update t_i
    *t1 = t2;

    // this operation is always possible
    return true;
}

bool LK::choose_y1(Node *tour, int n, bool *visited, 
        edge x1, Node *t2, Node *t3, edge *y1, int *G1){
    
    // conditios in LK:
    //      - y1 not in tour
    //      - G1 > 0

    // not in tour
    if (((t2->rank + 1)%n == t3->rank) && (t2->rank != (t3->rank+1)%n))
        return false;

    // not visited
    if(visited[t3->id])
        return false;

    int d1 = distance(x1.u,x1.v);   // s.getDist((x1.u).id, (x1.v).id);
    int d2 = distance(*t2,*t3);  //s.getDist(t2->id,t3->id); 

    // G1 > 0
    if(d1-d2 <= 0)
        return false;


    visited[t3->id] = true;
    // update gain
    *G1 = *G1 + d1 - d2;
    // update t2
    y1->u = *t2;
    y1->v = *t3;
    *t2 = *t3;

    return true;
}

bool LK::choose_xi(Node *tour, int n, bool *visited, Node *t3,
        edge *x2){
    //Conditions LK:
    //    1. x in T
    //    2. xi != yi
    //    3. if t is joined to t1 the resulting tour is a tour. 
    
    // find x limited to one choice 
    
    Node t4 = tour[(n+(t3->rank)-1)%n];
    if(visited[t4.id])
        return false;
    
    x2->u = *t3;
    x2->v = t4;
    visited[t4.id] = true;
    *t3 = t4;
    return true;
}

bool LK::choose_yi(Node *tour,int n, bool *visited,
        Node *t4,edge x2, edge *y2,int *Gi){
    // conditions LK: 
    //     1. y not in T
    //     3. yi != xi
    //     2. G > 0

    for (int i=0;i<n;i++){
    
        Node t5 = tour[i];
        
        // if visited skip
        if (visited[t5.id])
            continue;

        // (t4,t5) must not be in the tour
        if((((*t4).rank+1)%n == t5.rank ) && ((*t4).rank == (t5.rank +1)%n))
            continue;

        
        int d1 = distance(x2.u, x2.v); // s.getDist((x2.u).id, (x2.v).id);
        int d2 = distance(*t4, t5); //s.getDist((*t4).id,t5.id); 
        // Gi > 0
        if((*Gi)+ d1-d2 <= 0)
            continue;

        // update gain
        (*Gi) = (*Gi) + d1 - d2;
        // update visited
        visited[t5.id] = true;
        // init y2
        y2->u = *t4;
        y2->v = t5;
        // update ti
        (*t4) = t5;
        return true;
    }

    return false;
   
}



// store 
void LK::store(Node *tour_, Node *tour, int n){
    for(int i=0; i<n; i++)
        tour_[i] = tour[i];
}


void LK::flip(Node *tour, int n, std::list<edge> X){
    
    Node t1 = (X.front()).u;
    //X.pop_front();

    int r = 1;
    int i, j;
    Node ti;
    //while (!X.empty()){
        ti = (X.back()).u;
        //X.pop_front();
        i = (t1.rank+1)%n;   // r = 1 => next(t1) else prev(t1) 
        j = (n+ti.rank-1)%n; // r = 1 => prev(ti) else next(ti)
    
        /*if (!( (j>i && j-i< n/2) || (j<i && i-j<n/2)) ){
            i = ti.rank;
            j = t1.rank;
            r= (-1)*r;
        }*/
        Node tmp;
        for(int k=0; k< ((n + j-i ) %n +1)/2 ; k++){
            tmp = tour[(i+k)%n];
            tour[(i+k)%n] = tour[(n+j-k)%n];
            tour[(n+j-k)%n] = tmp;

            tour[(i+k)%n].rank = (i+k)%n;
            tour[(n+j-k)%n].rank =(n+j-k)%n;
        }
    //}
}


void LK::optimize(std::vector<Node> initialTour){

    int n = initialTour.size(); // s.getSize();// size of instance
    Node T[n];       // tour of Nodes
    bool visited[n];    // visited Nodes
    std::cout << "Number of Nodes : " << n << std::endl; 

    for(int i=0; i<n; i++){
        //Node u = {i,initialTour.at(i).id, initialTour.at(i).coord};  //;{i,s.getVal(i)}; // Node (rank, ID)
        Node u; 
        u.rank = i; 
        u.id = initialTour.at(i).id; 
        u.coord = initialTour.at(i).coord; 
        T[i] = u;
        visited[T[i].id] = false;
    }

//    std::cout << "-----Greedy solution \n";
/*    for(int i=0;i<n; i++)
        std::cout << T[i].rank <<  ";" << T[i].id << "\n";
    std::cout << "-------------------- \n"; 
  */  
    int nb_moves = 0;
    edge x1,y1,x2,y2;
    std::list<edge> X,Y;
    Node tour[n];
    Node tour_b[n];
    Node t,t_next;
    int G = 0;
    int g = 0;
    bool stop = false;
    while (nb_moves < m_count){
        
        store(tour,T,n);
        /*if(nb_moves == 0){
            for(int i=0;i<n; i++)
                std::cout << tour[i].rank <<  ";" << tour[i].id << "\n";
        }*/
        for(int i=0; i<n; i++){
            stop = false;

            t = tour[i];
            
            // 1. choose x1 in Tour
            if (!choose_x1(tour,n,visited,&t,&x1))
                continue;
            X.push_back(x1);
            
            G = 0; 
            g = 0;            
            for(int j=0; j<n; j++){
                
                stop = false;
                t_next = tour[j];
                
                // 2. choose y1 such that y1 not in Tour & G > 0
                if ( !choose_y1(tour,n,visited,x1,&t,&t_next,&y1,&G) )
                    continue;
                Y.push_back(y1);

                // 3. loop
                
                for(int l = 2; l<n; l++){
                    // 4. choose xi
                    if( !choose_xi(tour,n,visited,&t,  &x2)){
                        //store(tour,tour_,n); // undo changes
                        break;
                    }
                    // 4.a resulting tour
                    // flip((t1,t4) (t2,t3))
                    int d1 = distance(x2.u, x2.v); //s.getDist((x2.u).id, (x2.v).id);
                    int d2 = distance(t,x1.u); //s.getDist(t.id,(x1.u).id);
                    X.push_back(x2);
                    // resulting tour better
                    
                    //std::cout<< nb_moves << ".";
                    if( G + d1-d2 > 0){
                        /*std::cout << "----------------\n";
                        std::list<edge>::iterator it_ = Y.begin();
                        for(std::list<edge>::iterator it = X.begin(); it != X.end(); ++it){
                            std::cout << "("<<(*it).u.id <<";"<<(*it).v.id <<")   ";  
                            if(it_ != Y.end()){
                                std::cout << "("<<(*it_).u.id <<";"<<(*it_).v.id <<")   ";  
                                ++it_; 
                            }
                        }
                        std::cout << "\n";*/
                        G= G + d1-d2;
                            
                        //std::cout << "  gain " << G <<"\n before :";
                        //for(int r=0;r<n;r++)
                          //  std::cout << tour[r].rank << ";"<<tour[r].id << " __ ";
                        flip(tour,n,X);
                        //std::cout << " \n after :";
                        /*for(int r=0;r<n;r++)
                            std::cout << tour[r].id << " : ";
                        std::cout << "\n";*/
                        store(T,tour,n);
                        /*if(G > g){
                            store(tour_b,tour,n);
                            g = G;
                        }*/
                        // stop search for move
                        stop = true;
                        break;
                    }
                    

                    flip(tour,n,X); 
                    
                    // 5. choose yi
                    if( !choose_yi(tour,n, visited, &t,x2,&y2,&G)){
                        //store(tour,tour_,n); // undo changes
                        break;
                    }
                    Y.push_back(y2);
                
                } // end loop 3
       
                // if LK-move done!! stop!!
                if(stop){
                    nb_moves++;
                    //store(T,tour_b,n); 
                    //g = 0;
                    break;
                // else jump to loop y1 & undo changes
                } else {
                    store(tour,T,n);
                    X.clear();
                    Y.clear();
                    X.push_back(x1);
                    for(int l=0; l<n; l++)
                        visited[l] = false;
                    visited[(x1.u).id] = true;
                    visited[(x1.v).id] = true;
                    G = 0; 
                    t = x1.v;
                }                   
            } // end loop y1        
            
            // if LK-move done stop
            if(stop)
                break;
            else {
                X.clear();
                Y.clear();
                for(int l=0; l<n; l++)
                    visited[l] = false;
                G = 0;
                g = 0;
                store(tour,T,n);
            }
        } //end loop x1

        //std::cout << "nb moves: " << nb_moves << "\n";
        // no possible LK-move
        if(!stop)
            break;
    } // end while            

    int tour_[n];
    for(int i=0; i<n; i++){
        tour_[i] = T[i].id;
        m_solution.push_back(T[i]); 
    }
    std::cout << "nb of moves : " << nb_moves << "\n";
    //s.setPath(tour_);
}

double LK::distance(Node n1, Node n2){

  double x = n2.coord.first - n1.coord.first ; 
  double y = n2.coord.second - n1.coord.second ; 
  /*
  std::cout << " N1 " << n1.coord.first << ","<< n1.coord.second << " N2 " << n2.coord.first << "," << n2.coord.second << std::endl; 
  std::cout << "Distance " << sqrt(x*x+y*y) << std::endl; */
  return sqrt(x*x+y*y); 
}

std::vector<Node> LK::getSolution() {
  return m_solution; 
}



void LK::printSolution(){
  for(uint i =0; i<m_solution.size(); i++){
    std::cout << m_solution.at(i).id  << std::endl; 
  }
}


double LK::getDistance(){

  double tourDistance=0;  
  for(uint i =0; i<m_solution.size()-1; i++){
    tourDistance = tourDistance + distance(m_solution.at(i), m_solution.at(i+1)); 
  }
  return tourDistance; 
}

double LK::calculateDistance(std::vector<Node> solution){

  double tourDistance=0;  
  for(uint i =0; i<solution.size()-1; i++){
    tourDistance = tourDistance + distance(solution.at(i), solution.at(i+1)); 
  }
  return tourDistance; 
}

void LK::readNodes(std::vector<Node>& initialTour){
  std::ifstream data("entreeDroite.txt"); 

  if(!data){
    std::cerr << "Problem opening file" << std::endl; 
    exit(1); 
  }

  uint counter {0}; 
  int rank, id, coordx, coordy, info , coeff; 
  while (data)
  {
      std::string strInput;
      data >> strInput; 
      counter++; 
      if(strInput.length() != 0 ){
        if(counter == 5){
          info = std::stod(strInput); 
        }
        else 
        {
          info  = std::stoi(strInput) ; 
        }
      }
      else 
        return; 
      switch(counter){
        case 1: 
           rank=info; 
           break;
        case 2: 
           id=info;   
           break;
        case 3: 
           coordx=info;   
           break;
        case 4: 
          coordy = info;  
          break; 

        case 5: 
           coeff=info;   
           Node n ; //{rank ,id, std::make_pair(coordx, coordy) }; 
           n.rank = rank; 
           n.id = id; 
           n.coord = std::make_pair(coordx, coordy); 
           n.actionCoeff = coeff; 
           initialTour.push_back(n); 
           counter = 0; 
           break;
    }
  }
}

void LK::writeSolution(std::vector<Node> solution){

  std::ofstream outf("solution.txt"); 
  
  if(!outf)
  {
    std::cerr<< "Problem opening the file" << std::endl; 
    exit(1); 
  }

  for(uint i =0; i<solution.size(); i++)
  {
    outf << solution.at(i).id << "\t" << solution.at(i).coord.first << "\t" << solution.at(i).coord.second << std::endl; 
  }

}
