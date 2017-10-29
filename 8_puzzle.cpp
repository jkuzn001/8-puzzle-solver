#include <iostream>
#include <string>
#include <queue>
#include <vector>
#include <cmath>
using namespace std;


//global constants
const int puzzlesize = 9;
const int rowsize    = 3;


//node struct

struct node {
    public:
    int uniformcost = -1;    //uniform cost so far to get to this node set to -1 to make the initialilzation of the queue easier
    int hueristic   = 0;    //hueristic "guess" for this node (calculated during the queueing function depending on the hueristic chosen
    int fn          = 0;    //will store the addition of the uniform cost and the hueursitic
    int config[puzzlesize]; //current puzzle configuration
    
};

//compare class for use in priority queue

class compare {
    public:
        bool operator() (node const & node1,node const & node2) {
            return node1.fn > node2.fn;
        }
};

//swap function for use in expand

void swap(int i1, int i2, int* arr) {
    int temp = arr[i1];
    arr[i1] = arr[i2];
    arr[i2] = temp;
    return;
}
//type definition for function pointers
typedef priority_queue<node, vector<node>, compare >(*fctptr)(priority_queue<node, vector<node>, compare >, vector<node>);

//forward declarations of major fucntions

node generalsearch(node, fctptr);
priority_queue<node, vector<node>, compare > uniform(priority_queue<node,vector<node>, compare >, vector<node>);
priority_queue<node, vector<node>, compare > mannhattan(priority_queue<node,vector<node>, compare >, vector<node>);
priority_queue<node, vector<node>, compare > misplaced(priority_queue<node,vector<node>, compare >, vector<node>);
vector<node> expand(node);

//forward declarations of helper functions that calcutlate huerisitics 
void uniformhueristic(node*);
void manhattanhueristic(node*);
void misplacedhueristic(node*);

//helper function to test if a puzzle in the goal state

bool isgoal(node test) {
    node goal;
    goal.config[0] = 1;
    goal.config[1] = 2;
    goal.config[2] = 3;
    goal.config[3] = 4;
    goal.config[4] = 5;
    goal.config[5] = 6;
    goal.config[6] = 7;
    goal.config[7] = 8;
    goal.config[8] = 0;
    for(int i = 0; i < puzzlesize; ++i) {
        if(test.config[i] != goal.config[i]) return false;
    }
    return true;
}

void print(node p) {
    cout << "Current Puzzle " << endl;
    cout << "Cost: " << p.uniformcost << " Hueristic: " << p.hueristic << endl;
    for(int i = 0; i < puzzlesize; ++i) {
        cout << p.config[i] << " ";
        if((i + 1) % 3 == 0 && i != 0) cout << endl;
    }
    cout << endl;
}
//general search algorithm given by Dr Keogh with some modifications
//made so I could implement the design in a way that made sense to me

node generalsearch(node puzzle, fctptr qngfct) {
    //set up priority queue with given puzzle
    priority_queue<node, vector<node>, compare> nodes;
    nodes.push(puzzle);

    //if nodes is empty return failure
    //if front of nodes queue is the solution return solution
    //if not expand all children and put them on the queue
    while(1) {
        if(nodes.empty()) {
            node fail;
            fail.config[0] = -1;
            return fail;
        }
        node curr = nodes.top();
        print(curr);
        nodes.pop();
        if(isgoal(curr)) return curr;
        nodes = qngfct(nodes,expand(curr));
    }
}

//expand function

vector<node> expand(node curr) {
   vector<node> ret; //vector of nodes to return
   //iterate through puzzle to find the blank
   print(curr);
   for(int i = 0; i < puzzlesize; ++i) {
       if(curr.config[i] == 0) {
            //check if node can be expanded upwards if it can
            //expand it and push onto the vector
            if(i != 0 && i != 1 && i != 2) {
                node temp = curr;
                cout << "the blank is at postion: " << i << endl;
                swap(i, i - 3, temp.config);
                ret.push_back(temp);
                //print(temp);
            }
            //check if node can be expanded downwards if it can
            //expand it and push on the vector
            if(i != 6 && i != 7 && i != 8) {
                node temp = curr;
                swap(i, i + 3, temp.config);
                ret.push_back(temp);
                //print(temp);
            }
            //check if node can be expanded left if it can
            //expand it and push on the vector
            if(i != 0 && i != 3 && i != 6) {
                node temp = curr;
                swap(i, i - 1, temp.config);
                ret.push_back(temp);
                //print(temp);
            }
            //check if node can be expanded right if it can
            //expand it and push on the vector
            if(i != 2 && i != 5 && i != 8) {
                node temp = curr;
                swap(i, i + 1, temp.config);
                ret.push_back(temp);
                //print(temp);
            }
            return ret;
       }
   }
}

//implementation of queueing functions

priority_queue<node, vector<node>, compare > uniform(priority_queue<node, vector<node>, compare > nodes, vector<node> v) {
    for(int i = 0; i < v.size(); ++i) {
        node temp = v.at(i);
        uniformhueristic(&temp);
        nodes.push(temp);
    }
    return nodes;
    
}

priority_queue<node, vector<node>, compare > manhattan(priority_queue<node, vector<node>, compare > nodes, vector<node> v) {
    for(int i = 0; i < v.size(); ++i) {
        node temp = v.at(i);
        manhattanhueristic(&temp);
        nodes.push(temp);
    }
    return nodes;
    
}

priority_queue<node, vector<node>, compare > misplaced(priority_queue<node, vector<node>, compare > nodes, vector<node> v) {
    for(int i = 0; i < v.size(); ++i) {
        node temp = v.at(i);
        misplacedhueristic(&temp);
        nodes.push(temp);
    }
    return nodes;
}

//implementation of hueristic calculating functions
//increments uniform cost by 1 calculates new hueristic
//and calculates f(n) for the node

void uniformhueristic(node* n) {
    ++n->uniformcost;
    n->hueristic = 0; //this value is always 0 since we are doing uniform cost search
    n->fn = n->uniformcost + n->hueristic;
    return;
}

void manhattanhueristic(node* n) {
    ++n->uniformcost;
    int temph = 0;      //temporary variable for the hueristic
    for(int i = 0; i < puzzlesize; ++i) {
        if(n->config[i] != 0) {
            int dist = abs(n->config[i] - i);
            //calculate the manhattan distance by adding the number of columns (dist % rowsize)
            //to the number of rows (dist / rowsize) each tile is away from its propper place
            temph += (dist % rowsize) + (dist / rowsize);
        }
        else {
            //same calculation as above except it takes special measures for the blank which
            //is represented by a 0
            int dist = abs(puzzlesize - i);
            temph += (dist % rowsize) + (dist / rowsize);
        }
    }
    n->hueristic = temph;
    n->fn = n->hueristic + n->uniformcost;
    return;
}

void misplacedhueristic(node* n) {
    ++n->uniformcost;
    int temph = 0;      //temporary variable for the hueristic

    //loop through the current puzzle configuration if the index does not
    //match the value at that index the tile is misplaced and we should
    //add one the the hueristic guess
    //this is not true for the last entry in the array which should be the 
    //blank which is represented by 0 so we will perform this check on its 
    //own
    for(int i = 0; i < puzzlesize - 1; ++i) { 
        if(n->config[i] != i + 1) {
            ++temph;
        }
    }
    if(n->config[puzzlesize - 1] != 0) {
        ++temph;
    }
    n->hueristic = temph;
    n->fn = n->hueristic + n->uniformcost;
    return;
}
int main() {

    //output prompts and collect user input
    cout << "Welcome to Jacob Kuznicki's 8-puzzle solver" << endl;
   
    int input = 0;
    node puzzle;

    cout << "type \'1\' to use a default puzzle or \'2\' to enter a custom puzzle: "; 
    cin  >> input; cout << endl;
    cin.ignore();

    //populate with a premade puzzle
    if(input == 1) {
        puzzle.config[0] = 1;
        puzzle.config[1] = 2;
        puzzle.config[2] = 3;
        puzzle.config[3] = 4;
        puzzle.config[4] = 5;
        puzzle.config[5] = 6;
        puzzle.config[6] = 7;
        puzzle.config[7] = 0;
        puzzle.config[8] = 8;
    }
    //get user generated puzzle
    else if(input == 2) {
        string in;
        cout << "enter row 1 (type \'0\' for blank, separate numbers with a space)";
        getline(cin, in);
        cin.ignore();
        puzzle.config[0] = in.at(0) - 48;
        puzzle.config[1] = in.at(2) - 48;
        puzzle.config[2] = in.at(4) - 48;
        cout << "enter row 2 (type \'0\' for blank, separate numbers with a space)";
        getline(cin, in);
        cin.ignore();
        puzzle.config[3] = in.at(0) - 48;
        puzzle.config[4] = in.at(2) - 48;
        puzzle.config[5] = in.at(4) - 48;
        cout << "enter row 3 (type \'0\' for blank, separate numbers with a space)";
        getline(cin, in);
        cin.ignore();
        puzzle.config[6] = in.at(0) - 48;
        puzzle.config[7] = in.at(2) - 48;
        puzzle.config[8] = in.at(4) - 48;
    }
    cout << "Type \'1\' for uniform cost search, \'2\' for misplaced tile, \'3\' for manhanttan distance: ";
    cin  >> input; cout << endl;
    if(input == 1) {
        uniformhueristic(&puzzle);
        puzzle = generalsearch(puzzle,uniform);
        if(isgoal(puzzle)) {
            cout << "success! cost: " << puzzle.uniformcost  << endl;
        }
        else {
            cout << "failure" << endl;
        }
    }
    else if(input == 2) {
        misplacedhueristic(&puzzle);
        puzzle = generalsearch(puzzle,misplaced);
        if(isgoal(puzzle)) {
            cout << "success! cost: " << puzzle.uniformcost  << endl;
        }
        else {
            cout << "failure" << endl;
        }
    }
    else {
        manhattanhueristic(&puzzle);
        puzzle = generalsearch(puzzle,manhattan);
        if(isgoal(puzzle)) {
            cout << "success! cost: " << puzzle.uniformcost  << endl;
        }
        else {
            cout << "failure" << endl;
        }
    }

    return 0;
}
