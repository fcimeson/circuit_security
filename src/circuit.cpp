
/*************************************************************************//**
 *****************************************************************************
 * @file        circuit.cpp
 * @brief       
 * @author      Frank Imeson
 * @date        
 *****************************************************************************
 ****************************************************************************/

// http://www.ros.org/wiki/CppStyleGuide

#include "circuit.hpp"

//#define DEBUG




    
/*************************************************************************//**
 * @brief	
 * @version						v0.01b \n
 ****************************************************************************/
void sis_convert(string infile, string tech_lib, string outfile) {

    if (infile == outfile)
        return;

    string sis_cmds = "";
    sis_cmds += "read_library " + tech_lib + "\n";
    sis_cmds += "read_blif "    + infile + "\n";
    sis_cmds += "decomp \n";
    sis_cmds += "map -Wm 0.0 \n";
    sis_cmds += "write_blif -n " + outfile + "\n";
    sis_cmds += "echo test \n";
    sis_cmds += "quit \n";

    // Setup Pipes
    int pc[2], cp[2];
    
    if (pipe(pc) == -1 | pipe(cp) == -1) {
        perror("Error: No Pipe \n");
        exit(1);
    }
    
    pid_t pid = fork();

    // Error (PID == -1)    
    if (pid == -1) {
        perror("Error: No PID \n");
        exit(1);
        
    // Child (PID == 0)
    } else if (pid == 0) {
        dup2 (pc[0],0);   // make 0 same as read-from end of pipe 
        close(pc[1]);     // close write end of pipe              
        close(cp[0]);     // close read end of pipe              
        dup2 (cp[1],1);   // write to pipe
        string sis = get_home("/bin/sis");
        execlp(sis.c_str(),"sis",NULL);
        perror("error exec");
        _exit(1);        // no flush                             

    // Parent (PID > 0)
    } else {
        close(pc[0]);    // close read end of pipe               
        write(pc[1], sis_cmds.c_str(), sis_cmds.size());
        close(pc[1]);
      
        wait(NULL);
        close(cp[0]);    // close excess fildes
        close(cp[1]);
    }

}



/*************************************************************************//**
 * @brief	
 * @version						v0.01b \n
 ****************************************************************************/
void load_circuit(Circuit *circuit, string infile, bool nand=false) {


//    printf("\n");
//    printf("Circuit: %s \n", infile.c_str());

    /******************************
     * Create Graph
     ******************************/
    circuit->erase();
    string circuit_name = infile;
    circuit_name = circuit_name.substr(circuit_name.rfind('/')+1);
    circuit_name = circuit_name.substr(0, circuit_name.find('.'));
    SETGAS(circuit, "name", circuit_name.c_str());

    /******************************
     * Parse input file
     ******************************/
    ifstream circuit_file;
    circuit_file.open(infile.c_str());
    
    boost::regex token_rx("([\\w!+():]+)");
    boost::regex gate_rx(".gate");
    boost::regex wire_rx("wire");
    boost::regex io_rx("=\\S+");
    

    string line;
    boost::smatch matches;
    int node_index(0);
    map<string,VertexAttribs> vertex_map;
    
    while (circuit_file.good()) {
        getline(circuit_file, line);
        
        if (boost::regex_search(line, gate_rx) && !boost::regex_search(line,wire_rx)) {
        
            boost::sregex_token_iterator iter(line.begin(),  line.end(), token_rx, 0);
            boost::sregex_token_iterator io_it(line.begin(), line.end(), io_rx, 0);
            boost::sregex_token_iterator end;

            vector<string> io;
            for (; io_it != end; io_it++) {
                string in_out = *io_it;
                // strip '='
                in_out = in_out.substr(1,in_out.size()-1);
                if (in_out[0] == '[') {
                    // strip brackets []
                    in_out = in_out.substr(1,in_out.size()-1);
                    in_out = in_out.substr(0,in_out.size()-1);
                }

                /******************************
                 * Populate vertex_map
                 ******************************/
                map<string,VertexAttribs>::iterator it = vertex_map.find(in_out);
                if (it == vertex_map.end()) {
                    VertexAttribs attribs(node_index, "input");
                    vertex_map[in_out] = attribs;
                    node_index++;
                }
                io.push_back(in_out);
            }
            
            string gate;
            iter++;
            if (!nand)  gate = *iter;
            else        gate = "nand";
            
            // set output vertex type
            vertex_map[io.back()].type = gate;

        
        
            /******************************
             * Add nodes/edges to graph
             ******************************/

            // add vertices
            if (igraph_vcount(circuit) < vertex_map.size()) {
                int delta = vertex_map.size()-igraph_vcount(circuit);
                igraph_add_vertices(circuit, delta, 0);
            }

            // add edges
            string output = io.back();

            SETVAS(circuit, "label", vertex_map[output].index, output.c_str());
            SETVAS(circuit, "type", vertex_map[output].index, vertex_map[output].type.c_str());
            
            for (int i=0; i<io.size()-1; i++) {
                igraph_add_edge(circuit, vertex_map[io[i]].index, vertex_map[output].index);
                SETVAS(circuit, "label", vertex_map[io[i]].index, io[i].c_str());
                SETVAS(circuit, "type", vertex_map[io[i]].index, vertex_map[io[i]].type.c_str());
                SETVAN(circuit, "colour", vertex_map[io[i]].index, 5);
            }
                                 
        }
   
    }

    printf("\n");   
    circuit_file.close();
    circuit->update();
}



/*************************************************************************//**
 * @brief	
 * @version						v0.01b \n
 ****************************************************************************/
bool Circuit::test_edge (Edge edge) {
    int eid1(-1);
    igraph_get_eid(this, &eid1, edge.first, edge.second, IGRAPH_DIRECTED, 0);
    return eid1 != -1;
}



/*************************************************************************//**
 * @brief	
 * @version						v0.01b \n
 ****************************************************************************/
void Circuit::update () {

    map<string,int> type_map;
    vector<int> colour_count;
    unsigned int colour_index(0);
    for (unsigned int i=0; i<igraph_vcount(this); i++) {
        map<string,int>::iterator it01 = type_map.find(VAS(this, "type", i));
        if (it01 == type_map.end()) {
            type_map[VAS(this, "type", i)] = colour_index;
            colour_count.push_back(0);
            colour_index++;
        }
        colour_count[type_map[VAS(this, "type", i)]]++;
        SETVAN(this, "colour", i, type_map[VAS(this, "type", i)]);
    }

    map<string,int>::iterator it = type_map.begin();
    int min_count = colour_count[(*it).second];
    string min_gate;
    min_gate = (*it).first;
    //cout << min_gate << " " << min_count << endl;

    for (unsigned int i=0; i<colour_count.size(); i++) {

        it = type_map.begin();
        for (unsigned int j=0; j<type_map.size(); j++)
            if ( (*it).second == i )
                break;
            else
                it++;

        if (colour_count[i] < min_count) {
            min_count = colour_count[i];
            min_gate = (*it).first;
            //cout << min_gate << " " << min_count << endl;
        }
        
        //cout << (*it).first << " colour " << i << endl;
    }
    SETGAN(this, "max_L1", min_count);
    SETGAN(this, "gate_count", type_map.size());
    SETGAS(this, "min_gate", min_gate.c_str());
}



/*************************************************************************//**
 * @brief	
 * @version						v0.01b \n
 ****************************************************************************/
void Circuit::print (ostream &out) {
    out << "V = { \n";
    for (unsigned int i=0; i<igraph_vcount(this); i++) {
        out << setw(3) << i << ": " << setw(4) << VAS(this, "label", i) << ", " << setw(6) << VAS(this, "type", i);
        out << ", colour = " << setw(2) << (int) VAN(this, "colour", i) << endl;
    }
    out << "} \n";

    out << "E = { \n";
    for (unsigned int i=0; i<igraph_ecount(this); i++) {
        int edge01_a, edge01_b;
        igraph_edge(this, i, &edge01_a, &edge01_b);
        out << " <" << edge01_a << "," << edge01_b << "> \n";
    }
    out << "} \n\n";

    out << GAS(this, "name") << endl;
    out << "|V| = " << igraph_vcount(this) << ", |E| = " << igraph_ecount(this) << endl;
    out << "Max L1 = " << GAN(this, "max_L1") << endl;
    out << "Gate Count = " << GAN(this, "gate_count") << endl;
    out << "Min Gate = " << GAS(this, "min_gate") << endl;
}


/*************************************************************************//**
 * @brief	
 * @version						v0.01b \n
 ****************************************************************************/
int Circuit::max_L1 () {
    return GAN(this, "max_L1");
}


/*************************************************************************//**
 * @brief	
 * @version						v0.01b \n
 ****************************************************************************/
string Circuit::get_name(int vertex) {
    return string(VAS(this, "label", vertex));
}


/*************************************************************************//**
 * @brief	
 * @version						v0.01b \n
 ****************************************************************************/
string Circuit::get_name() {
    return string(GAS(this, "name"));
}


/*************************************************************************//**
 * @brief	
 * @version						v0.01b \n
 ****************************************************************************/
Edge Circuit::get_edge(int index) {
    int va, vb;
    igraph_edge(this, index, &va, &vb);
    return Edge(va, vb);
}


/*************************************************************************//**
 * @brief	
 * @version						v0.01b \n
 ****************************************************************************/
void Circuit::add_edge(Edge edge) {
    igraph_add_edge(this, edge.first, edge.second);
}

/*************************************************************************//**
 * @brief	
 * @version						v0.01b \n
 ****************************************************************************/
void Circuit::del_edge(Edge edge) {
    int index(-1);
    for (unsigned int i=0; i<igraph_ecount(this); i++) {
        if (edge == get_edge(i)) {
            index = i;
            break;
        }
    }
        
    if (index >= 0)
        igraph_delete_edges(this, igraph_ess_1(index));
}




/*************************************************************************//**
 * @brief	
 * @version						v0.01b \n
 ****************************************************************************/
void Circuit::remove_io () {

    for (int i=igraph_vcount(this)-1; i>=0; i--) {
        if ( string("input").compare(VAS(this, "type", i)) == 0) {
            igraph_delete_vertices(this, igraph_vss_1(i));
        }
    }
    update();
}



/*************************************************************************//**
 * @brief	
 * @version						v0.01b \n
 ****************************************************************************/
void Circuit::rand_del_edges (float percent) {
    srand( time(NULL) );
    int rcount = (float) percent*igraph_ecount(this);
    rand_del_edges(rcount);
}



/*************************************************************************//**
 * @brief	
 * @version						v0.01b \n
 ****************************************************************************/
void Circuit::rand_del_edges (int rcount) {
    for (unsigned int i=0; i<rcount; i++) {
        int rnum = (float) urand()*igraph_ecount(this);
        igraph_delete_edges(this, igraph_ess_1(rnum));
    }
}



/*************************************************************************//**
 * @brief	
 * @version						v0.01b \n
 ****************************************************************************/
void Circuit::save (string filename) {
    FILE *file = fopen(filename.c_str(), "w");
    igraph_write_graph_gml(this, file, NULL, NULL);
    fclose(file);
}



/*************************************************************************//**
 * @brief	
 * @version						v0.01b \n
 ****************************************************************************/
void Circuit::load (string filename) {
    erase();
    FILE *file = fopen(filename.c_str(), "r");
    igraph_read_graph_gml(this, file);
    fclose(file);
}





