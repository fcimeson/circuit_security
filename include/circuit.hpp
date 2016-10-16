
/*************************************************************************//**
 *****************************************************************************
 * @file        circuit.hpp
 * @brief       
 * @author      Frank Imeson
 * @date        
 *****************************************************************************
 ****************************************************************************/

// http://www.parashift.com/c++-faq-lite/
// http://www.emacswiki.org/emacs/KeyboardMacrosTricks

#ifndef CIRCUIT_H		// guard
#define CIRCUIT_H

/*****************************************************************************
 * INCLUDE
 ****************************************************************************/


#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <queue>
#include <limits>
#include <string>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <igraph/igraph.h>
#include <boost/regex.hpp>

#include "general.hpp"

using namespace std;


/*****************************************************************************
 * Defs
 ****************************************************************************/
 
typedef pair<int,int> Edge; 

/*****************************************************************************
 * Prototypes
 ****************************************************************************/
 
struct VeretxAttribs;
class Circuit;

void sis_convert(string infile, string tech_lib, string outfile);
void load_circuit(string infile);
void load_circuit(Circuit *circuit, string infile, bool nand);



/*****************************************************************************
 * Classes
 ****************************************************************************/

struct VertexAttribs{
    private:
    public:
    unsigned int index;
    string type;

    VertexAttribs (unsigned int _index, string _type)
        : index (_index)
        , type  (_type)
    {};

    VertexAttribs ()
        : index(0)
        , type("")    
    {};

    VertexAttribs & operator= (const VertexAttribs &rhs) {
        index = rhs.index;
        type  = rhs.type;
    }
};


class Circuit : public igraph_t {
    private:
        vector< vector<int> > solutions;
    public:
       
        Circuit () {
            igraph_i_set_attribute_table(&igraph_cattribute_table);
            igraph_empty(this, 0, 1);
        };
        
        void erase () {
            igraph_i_set_attribute_table(&igraph_cattribute_table);
            igraph_empty(this, 0, 1);
        };

        void update ();
        int  max_L1 ();
        void print  (ostream &out=cout);
        
        void rand_del_edges (float percent);
        void rand_del_edges (int rcount);
        void remove_io ();

        Edge get_edge  (int index);
        void add_edge  (Edge edge);
        void del_edge  (Edge edge);
        bool test_edge (Edge edge);
        
        void copy (Circuit* circuit) {
//            igraph_destroy(circuit);
            igraph_copy(this,circuit);
        };
        
        string get_name (int vertex);
        string get_name ();
        
        void save (string filename);
        void load (string filename);
};




#endif
