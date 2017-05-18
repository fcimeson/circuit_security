/********************************************************************************
  Copyright 2017 Frank Imeson, Siddharth Garg, and Mahesh V. Tripunitara

  Licensed under the Apache License, Version 2.0 (the "License");
  you may not use this file except in compliance with the License.
  You may obtain a copy of the License at

     http://www.apache.org/licenses/LICENSE-2.0

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.
*********************************************************************************/


// http://www.ros.org/wiki/CppStyleGuide

#include "security.hpp"
//#define DEBUG
//#define PRINT_SOLUTION
//#define MEASURE_TIME
//#define MEASURE_TIME_S1
#define USE_SOLNS
//#define NRAND
//#define VF2



/************************************************************//**
 * @brief	
 * @return            string representation of connective	
 * @version						v0.01b
 ****************************************************************/
igraph_bool_t check_map (
    const igraph_t *graph1,
    const igraph_t *graph2, 
    const igraph_integer_t vid1,
    const igraph_integer_t vid2,
    void *arg)
{
    L1_struct *mapped = (L1_struct*) arg;
    if (vid2 == mapped->vid2) {
//        if (mapped->mapped[vid2][vid1])
//            cout << "reject vid2(" << vid2 << ") -> vid1(" << vid1 << ")" << endl;
        return !mapped->mapped[vid2][vid1];
    }
    return true;
};


/*************************************************************************//**
 * @brief	
 * @version						v0.01b
 ****************************************************************************/
bool l1_edge_lt (const L1_Edge* rhs, const L1_Edge* lhs) {
    return rhs->L1_prev < lhs->L1_prev;
}



/*************************************************************************//**
 * @brief	
 * @version						v0.01b
 ****************************************************************************/
bool l1_edge_info_lt (const EdgeInfo &rhs, const EdgeInfo &lhs) {
    return rhs.max_degree < lhs.max_degree;
}



/*************************************************************************//**
 * @brief	
 * @version						v0.01b
 ****************************************************************************/
string report (string prefix, Circuit *G, Circuit *H, int L1, int L0, Edge edge) {

    stringstream out;
    out << prefix
        << ": |V(G)| = " << (int) igraph_vcount(G)
        << ", |E(G)| = " << (int) igraph_ecount(G)
        << ", |V(H)| = " << (int) igraph_vcount(H)
        << ", |E(H)| = " << (int) igraph_ecount(H);
    if (L0 > 0) out << ", #L0 = " << L0;
    if (L1 > 0) out << ", L1 = "  << L1;
    if (edge.first >= 0) out << ", +<" << edge.first << "," << edge.second << ">";
    out << endl;  

    return out.str();
}


/*************************************************************************//**
 * @brief	
 * @version						v0.01b
 ****************************************************************************/
bool parse (string line, Circuit *G, int &L1, int &L0, Edge &edge) {

    boost::regex report_rx ("\\|V\\(G\\)\\|");
    
    if ( regex_search(line, report_rx) ) {

        boost::regex num_rx   ("\\d+");
        boost::regex VG_rx    ("\\|V\\(G\\)\\| = \\d+");
        boost::regex EG_rx    ("\\|E\\(G\\)\\| = \\d+");
        boost::regex L0_rx    ("L0 = \\d+");
        boost::regex L1_rx    ("L1 = \\d+");
        boost::regex edge_rx  ("<\\d+,\\d+>");

        boost::sregex_token_iterator VG_token   (line.begin(), line.end(), VG_rx, 0);
        boost::sregex_token_iterator EG_token   (line.begin(), line.end(), EG_rx, 0);
        boost::sregex_token_iterator L0_token   (line.begin(), line.end(), L0_rx, 0);
        boost::sregex_token_iterator L1_token   (line.begin(), line.end(), L1_rx, 0);
        boost::sregex_token_iterator Edge_token (line.begin(), line.end(), edge_rx, 0);
        boost::sregex_token_iterator end;
        string token;

        assert (VG_token != end);
        {
            token = *VG_token;
            boost::sregex_token_iterator num(token.begin(), token.end(), num_rx, 0);
            assert (num != end);
            if ((int) igraph_vcount(G) != atoi(string(*num).c_str())) {
                cout << "|V(G)| = " << (int) igraph_vcount(G) << ", |V(G)| = " << atoi(string(*num).c_str()) << endl;
            }
            assert ( (int) igraph_vcount(G) == atoi(string(*num).c_str()) );
        }        

        assert (EG_token != end);
        {
            token = *EG_token;
            boost::sregex_token_iterator num(token.begin(), token.end(), num_rx, 0);
            assert (num != end);
            assert ( (int) igraph_ecount(G) == atoi(string(*num).c_str()) );
        }       

        if (L0_token != end) {
            token = *L0_token;
            boost::sregex_token_iterator num(token.begin(), token.end(), num_rx, 0);
            num++; // L0 yeilds a num
            assert (num != end);
            L0 = atoi(string(*num).c_str());
        }

        if (L1_token != end) {
            token = *L1_token;
            boost::sregex_token_iterator num(token.begin(), token.end(), num_rx, 0);
            num++; // L1 yeilds a num
            assert (num != end);
            L1 = atoi(string(*num).c_str());
        } else {
            return false;
        }

        if (Edge_token != end) {
            token = *Edge_token;
            boost::sregex_token_iterator num(token.begin(), token.end(), num_rx, 0);
            assert (num != end);
            edge.first = atoi(string(*num).c_str());
            num++;
            assert (num != end);
            edge.second = atoi(string(*num).c_str());
        } else {
            return false;
        }

        return true;
    } else {
        return false;
    }
}



/*************************************************************************//**
 * @brief	
 * @version						v0.01b
 ****************************************************************************/
string report (igraph_vector_t *soln) {

    string output;
    stringstream out;

    out << "map21: ";
    for (unsigned int i = 0; i < igraph_vector_size(soln); i++)
        out << VECTOR(*soln)[i] << ", ";
    output = out.str();
    output = output.substr(0, output.size()-2) + "\n";

    return output;
}


/*************************************************************************//**
 * @brief	
 * @version						v0.01b
 ****************************************************************************/
bool parse (string line, igraph_vector_t *soln) {

    boost::regex map_rx ("map21: ");
    
    if ( regex_search(line, map_rx, boost::match_continuous) ) {

        boost::regex num_rx ("\\d+"); 
        boost::sregex_token_iterator end;
        boost::sregex_token_iterator num(line.begin(), line.end(), num_rx, 0);
        num++; // map21 yeilds a num
        
        for (unsigned int i = 0; i < igraph_vector_size(soln); i++, num++) {
            assert (num != end);
            VECTOR(*soln)[i] = atoi(string(*num).c_str());
        }
        assert (num == end);
        return true;
    } else {
        return false;
    }
}



/*************************************************************************//**
 * @brief	
 * @version						v0.01b
 ****************************************************************************/
Security::Security (Circuit *_G, Circuit *_H)
{
    G = _G;
    H = _H;

    igraph_vector_int_init(&colour_G, igraph_vcount(G));
    igraph_vector_int_init(&colour_H, igraph_vcount(H));

    for (unsigned int i=0; i<igraph_vcount(G); i++)
        VECTOR(colour_G)[i] = (int) VAN(G, "colour", i);
    
    for (unsigned int i=0; i<igraph_vcount(H); i++)
        VECTOR(colour_H)[i] = (int) VAN(H, "colour", i);

    isosat = new Isosat(G, H, &colour_G, &colour_H, 0, 0, &igraph_compare_transitives, 0, 0);
}



/*************************************************************************//**
 * @brief	
 * @version						v0.01b
 ****************************************************************************/
void Security::clean_solutions () {
    for (int i = solutions.size()-1; i >= 0; i--) {

        igraph_bool_t iso(false);
        igraph_test_isomorphic_map (G, H, &colour_G, &colour_H, 0, 0, &iso, NULL, solutions[i],
                                    &igraph_compare_transitives, 0, 0);

        if (!iso) {
            igraph_vector_destroy(solutions[i]);
            solutions.erase(solutions.begin()+i);    
        }

    }
}




/*************************************************************************//**
 * @brief	
 * @version						v0.01b
 ****************************************************************************/
void Security::print_solutions () {
    cout << endl;
    for (int i = 0; i < solutions.size(); i++) {
        cout << "map21: ";
        igraph_vector_print(solutions[i]);
    }
}



/*************************************************************************//**
 * @brief	
 * @version						v0.01b
 ****************************************************************************/
void Security::add_edge (int eid) {
    H->add_edge(G->get_edge(eid));

    int from, to;
    igraph_edge(G, eid, &from, &to);
    igraph_get_eid(H, &eid, from, to, IGRAPH_DIRECTED, 1 /* error */);
    isosat->add_edge(G, H, eid, &colour_G, &colour_H, 0, 0, &igraph_compare_transitives, 0, 0);
}



/*************************************************************************//**
 * @brief	
 * @version						v0.01b
 ****************************************************************************/
int set_L1 (const Circuit *G, const vector<EdgeInfo> &edge_set) {

    vector<bool> from, to;
    for (unsigned int i=0; i<igraph_vcount(G); i++) {
        from.push_back(false);
        to.push_back(false);
    }

    int from_L1(0), to_L1(0);
    for (unsigned int i=0; i<edge_set.size(); i++) {
        Edge edge = edge_set[i].edge;
        if (!from[edge.first]) {
            from_L1++;
            from[edge.first] = true;
        }
        if (!to[edge.second]) {
            to_L1++;
            to[edge.second] = true;
        }
    }

    return min(from_L1, to_L1);
}



/*************************************************************************//**
 * @brief	
 * @version						v0.01b
 * precondition H is empty
 ****************************************************************************/
void Security::add_free_edges (int L1) {

    /******************************
     * Catorize edges
     ******************************/        
    map<Edge, EdgeStats> edges;
    map<Edge, EdgeStats>::iterator it;

    for (unsigned int eid=0; eid<igraph_ecount(G); eid++) {
        Edge edge, colour;
        
        igraph_edge(G, eid, &edge.first, &edge.second);
        colour.first  = (int) VAN(G, "colour", edge.first);
        colour.second = (int) VAN(G, "colour", edge.second);

        it = edges.find(colour);
        if (it == edges.end()) edges[colour] = EdgeStats();
        
        edges[colour].unplaced.push_back( EdgeInfo(eid, edge, max(igraph_vertex_degree(G,edge.first), igraph_vertex_degree(G,edge.second)) ));
    }

    vector<bool> placed;
    for (unsigned int vid1 = 0; vid1 < igraph_vcount(G); vid1++)
        placed.push_back(false);



    /******************************
     * Add edges
     ******************************/ 
    it = edges.begin();
    while ( it != edges.end() ) {
    
        // removed partialy placed edges
        int i=0;
        while (i < (*it).second.unplaced.size() && (*it).second.unplaced.size() > 0) {
            Edge edge = (*it).second.unplaced[i].edge;
            if (placed[edge.first] || placed[edge.second]) {
                (*it).second.unplaced.erase((*it).second.unplaced.begin()+i);
                i = 0;
                continue;
            }
            i++;
        }
        sort((*it).second.unplaced.begin(), (*it).second.unplaced.end(), l1_edge_info_lt);
        reverse((*it).second.unplaced.begin(), (*it).second.unplaced.end());
        
        // pick edges to add
        for (unsigned int index = 0; index < (*it).second.unplaced.size(); index++) {

            if ( set_L1(G, (*it).second.unplaced) + (*it).second.placed.size() < L1 ) break;
            
            vector<EdgeInfo> test_set = (*it).second.unplaced;
            EdgeInfo test_edge = test_set[index];
            test_set.erase(test_set.begin()+index);
            int from = test_edge.edge.first; int to = test_edge.edge.second;

            for (unsigned int i = 0; i < test_set.size(); i++) {
                if (test_set[i].edge.first   == from || test_set[i].edge.first   == to || 
                    test_set[i].edge.second  == from || test_set[i].edge.second  == to) {
                    test_set.erase(test_set.begin()+i);
                    i = -1;
                }
            }
            
            if ((*it).second.placed.size() + set_L1(G, test_set) >= L1) {
                (*it).second.placed.push_back(test_edge);
                (*it).second.unplaced = test_set;
            }

        }
//cout << "L1 = " << L1 << ", L1_set = " << set_L1(G, (*it).second.unplaced) << ", placed.size() = " << (*it).second.placed.size() << endl;
        // add edges
        for (unsigned int i = 0;  i < (*it).second.placed.size(); i++) {
            add_edge((*it).second.placed[i].eid);
            placed[(*it).second.placed[i].edge.first]  = true;
            placed[(*it).second.placed[i].edge.second] = true;
            string output;
            output = "S1_4free ("  + G->get_name() + ")";
            output = report(output, G, H, L1, 0, (*it).second.placed[i].edge);
            cout << output;
        }
        it++;
    }

}



/*************************************************************************//**
 * @brief	
 * @version						v0.01b
 run circuits/c17.blif tech_lib/minimal.genlib -w tmp -t -1
 ****************************************************************************/
bool Security::L0 (int max_count, bool quite) {
    if (!quite) {
      cout << " L0(" << max_count << "): ";
      cout.flush();
    }

    int count = 0;
    igraph_bool_t iso(true);
    while (iso && count < max_count) {
        igraph_vector_t map21;
        igraph_vector_init(&map21, igraph_vcount(H));
        isosat->solve(&iso, NULL, &map21);
        if (iso) {
            isosat->negate(NULL, &map21);
            count++;
            if (!quite) {
                cout << '*';
                cout.flush();
            }
        } else {
            return false;
        }
        igraph_vector_destroy(&map21);
    }
    return true;
}


/*************************************************************************//**
 * @brief	
 * @version						v0.01b
 ****************************************************************************/
void Security::L1 (string label) {
//cout << "L1(label): " << label << endl;
    int index(-1);
    for (unsigned int i=0; i<igraph_vcount(G); i++) {
//cout << VAS(G, "label", i) << endl;
        if ( VAS(G, "label", i) == label ) {
            index = i;
//cout << "index found: " << index << endl;
        }
    }

    vec<Lit> reject;
    igraph_bool_t iso(true);
    while (iso) {

        igraph_vector_t map21;
        igraph_vector_init(&map21, igraph_vcount(H));

        iso = false;
        isosat->solve(&iso, NULL, &map21, &reject);

        if (iso) {
            reject.push( isosat->translate(M21(index, VECTOR(map21)[index], true)) );
            cout << label << " -> " << VAS(G, "label", VECTOR(map21)[index]) << endl;
        }

        igraph_vector_destroy(&map21);
    }
}



/*************************************************************************//**
 * @brief	
 * @version						v0.01b
 ****************************************************************************/
int Security::L1 (bool quite, bool vf2) {

    /******************************
     * Setup
     ******************************/
    L1_struct L1_state;
    for (unsigned int i = 0; i < igraph_vcount(H); i++) {
        L1_state.mapped.push_back( vector<bool>() );
        for (unsigned int j = 0; j < igraph_vcount(G); j++) {
            L1_state.mapped.back().push_back(false);
        }
        L1_state.reject.push_back( new vec<Lit>() );
    }


    /******************************
     * Check all previously found solutions
     ******************************/
    for (unsigned int vid2 = 0; vid2 < igraph_vcount(H); vid2++) {
        for (unsigned int k=0; k < solutions.size(); k++) {
            if (L1_state.mapped[vid2][VECTOR(*solutions[k])[vid2]] == false) {
                L1_state.mapped[vid2][VECTOR(*solutions[k])[vid2]] = true;
                L1_state.reject[vid2]->push( isosat->translate(M21(vid2, VECTOR(*solutions[k])[vid2], true)) );

                if (L1_state.reject[vid2]->size() == igraph_vcount(G))
                    break;
            }
        }
    }


    /******************************
     * Find level
     ******************************/
    if (!quite) {
        cout << "L1(" << G->max_L1() << "): ";
        cout.flush();
    }
    for (unsigned int level = G->max_L1(); level > 1; level--) {
        if ( L1(level, true, &L1_state, vf2) )
            return level;
        if (!quite) {
            cout << '*';
            cout.flush();
        }
    }
    return 1;

}



/*************************************************************************//**
 * @brief	
 * @version						v0.01b
 ****************************************************************************/
int Security::vf2_solve (igraph_bool_t *iso, igraph_vector_t *map12, igraph_vector_t *map21, L1_struct *mapped) {

    L1_Thread thread;
    thread.open(true, false);
    
    /******************************
     * Child
     ******************************/                    
    if (thread.child()) {
        igraph_subisomorphic_vf2(G, H, &colour_G, &colour_H, 0, 0, iso, map12, map21, &check_map, 0, mapped);
        string output = report(map21);
        igraph_vector_destroy(map21);
        thread.write(output);
        thread.close(false, true);
        _exit(0);
    }

    /******************************
     * Parent
     ******************************/                    
    #define MAX_COUNT 10000
    bool finished(false);
    for (unsigned int i=0; i < MAX_COUNT; i++) {
        string response = thread.read();
        if (response.size() > 0) {
            parse(response, map21);
            if (VECTOR(*map21)[0] == 0 && VECTOR(*map21)[1] == 0)
                *iso = false;
            else
                *iso = true;
            finished = true;
            break;
        }
        usleep(1000);
    }
    
    if (!finished) {
        *iso = false;
        thread.close(true, false);
        thread.kill();
    }

    return 0;
}



/*************************************************************************//**
 * @brief	
 * @version						v0.01b
 ****************************************************************************/
bool Security::L1 (int max_count, bool quite, L1_struct *_L1_state, bool vf2) {

    if (vf2 && isosat != NULL) {
        delete isosat;
        isosat = NULL;
    }

    if (max_count > igraph_vcount(G))
        return false;

    /******************************
     * Setup
     ******************************/
    L1_struct *L1_state;
    if (_L1_state == NULL) {
        L1_state = new L1_struct();
        for (unsigned int i = 0; i < igraph_vcount(H); i++) {
            L1_state->mapped.push_back( vector<bool>() );
            for (unsigned int j = 0; j < igraph_vcount(G); j++) {
                L1_state->mapped.back().push_back(false);
            }
            L1_state->reject.push_back( new vec<Lit>() );
        }
    } else {
        L1_state = _L1_state;
    }

    if (!quite) {
        cout << " L1(" << max_count << "): ";
        cout.flush();
    }
 
    /******************************
     * Run tests
     ******************************/
    bool result(true);
    for (unsigned int l = 2; l <= max_count; l++) {

        /******************************
         * 
         ******************************/
        igraph_vector_t *map21 = new igraph_vector_t;
        igraph_vector_init(map21, igraph_vcount(H));

        for (unsigned int vid2 = 0; vid2 < igraph_vcount(H); vid2++) {
            if (L1_state->reject[vid2]->size() < l) {

                // update count and reject list
                igraph_bool_t iso(false);
                
                if (vf2) {
                    L1_state->vid2 = vid2;
                    vf2_solve(&iso, NULL, map21, L1_state);
                } else {
                    isosat->solve(&iso, NULL, map21, L1_state->reject[vid2]);
                }

                if (iso) {
                    #ifndef NDEBUG
                        igraph_bool_t test_iso(false);
                        igraph_test_isomorphic_map (G, H, &colour_G, &colour_H, 0, 0, &test_iso, NULL, map21,
                                                    &igraph_compare_transitives, 0, 0);
                        if (!test_iso) {
                            H->print();
                            igraph_vector_print(map21);
                        }
                        assert(test_iso);
                    #endif
                    assert ( L1_state->mapped[vid2][VECTOR(*map21)[vid2]] == false );

                    solutions.push_back(map21);
                    for (unsigned int i = 0; i < igraph_vector_size(map21); i++) {
                        if (L1_state->mapped[i][VECTOR(*map21)[i]] == false) {
                            L1_state->mapped[i][VECTOR(*map21)[i]] = true;
                            if (vf2)
                                L1_state->reject[i]->push( mkLit(0) );  
                            else
                                L1_state->reject[i]->push( isosat->translate(M21(i, VECTOR(*map21)[i], true)) );
                        }
                    }
                    map21 = new igraph_vector_t;
                    igraph_vector_init(map21, igraph_vcount(H));
                } else {
                    if (_L1_state == NULL)
                        delete L1_state;
                    return false;
                }

            }
        }
        igraph_vector_destroy(map21);

        if (!quite) {
            cout << '*';
            cout.flush();
        }
    }
    if (_L1_state == NULL)
        delete L1_state;

    return true;
}




/*************************************************************************//**
 * @brief	
 * @version						v0.01b
 run circuits/c17.blif tech_lib/minimal.genlib -w tmp -t -1
 ****************************************************************************/
void Security::S1_rand (int threads, int min_L1, bool quite) {
    
    /******************************
     * Setup
     ******************************/
    vector<int> good;
    for (unsigned int eid = 0; eid < igraph_ecount(G); eid++)
        if (!H->test_edge(G->get_edge(eid)))
            good.push_back(eid);
            
    #ifndef NRAND
      random_shuffle(good.begin(), good.end());
    #endif
        
    vector<L1_Thread*> busy_threads, free_threads;
    for (unsigned int i=0; i<threads; i++)
        free_threads.push_back( new L1_Thread() );
        

    /******************************
     * Add edges until L1 == min_L1
     ******************************/
    bool done(false);
    while (!done || busy_threads.size() > 0) {
   
        /******************************
         * Load Threads (create sub-processes)
         ******************************/
        if (!done && free_threads.size() > 0) {
            int test_index = good.back();
            good.pop_back();
            add_edge(test_index);

            busy_threads.push_back(free_threads.back());
            free_threads.pop_back();
            busy_threads.back()->open(true,false);

            /******************************
             * Child
             ******************************/
            if ( busy_threads.back()->child() ) {              // Child (PID == 0)

                Edge test_edge = G->get_edge(test_index);
                int test_L1 = L1();

                string output;
                output = "S1_rand ("  + G->get_name() + ").child(" + num2str(getpid()) + ")";
                output = report(output, G, H, test_L1, solutions.size(), test_edge);

                #ifdef DEBUG
                  cout << output << endl;
                #endif
                
                busy_threads.back()->write(output);
                busy_threads.back()->close(false, true);
                _exit(0);
            }
           
        }
         
        /******************************
         * Unload Threads (Parent)
         ******************************/
        do {
            for (unsigned int j=0; j<busy_threads.size(); j++) {
                string response = busy_threads[j]->read();
                // do something with response
                if (response.size() > 0) {

                    int L0, test_L1;
                    Edge test_edge;

                    stringstream r_stream(response);
                    string line;
                    while( getline(r_stream, line) )
                        parse(line, G, test_L1, L0, test_edge);
                    
                    string output;
                    output = "S1_rand ("  + G->get_name() + ")";
                    output = report(output, G, H, test_L1, L0, test_edge);
                    cout << endl << output;

                    free_threads.push_back(busy_threads[j]);
                    busy_threads.erase(busy_threads.begin()+j);

                    if (test_L1 < min_L1) done = true; 
                }
            }
        } while (free_threads.size() == 0);
    }

}



/*************************************************************************//**
 * @brief	
 * @version						v0.01b
 ****************************************************************************/
void Security::S1_greedy (int threads, int min_L1, int max_L1, bool quite) {

    /******************************
     * Setup
     ******************************/
    if (max_L1 == -1) max_L1 = G->max_L1();
    if (igraph_ecount(H) == 0) add_free_edges(max_L1);

    vector<L1_Edge*> edges;
    vector<L1_Edge*> edge_list;
    for (unsigned int eid = 0; eid < igraph_ecount(G); eid++)
        if (!H->test_edge(G->get_edge(eid))) {
            int from, to;
            igraph_edge(G, eid, &from, &to);
            edges.push_back( new L1_Edge(eid, Edge(from,to), max_L1) );
            edge_list.push_back( edges.back() );
        }

    #ifndef NRAND
      random_shuffle(edge_list.begin(), edge_list.end());
    #endif

    #ifdef VF2
      bool vf2_flippy(false);
    #endif
    vector<L1_Thread*> busy_threads, free_threads;
    for (unsigned int i=0; i<threads; i++) {
        free_threads.push_back( new L1_Thread() );
        #ifdef VF2
          if (vf2_flippy)
              free_threads.back()->vf2 = true;
          vf2_flippy = !vf2_flippy;
        #endif
    }
        
    /******************************
     * Add edges until L1 == min_L1
     ******************************/
    while (max_L1 >= min_L1 && edge_list.size() > 0) {

        cout << "  E(" << edge_list.size() << ") ";
        cout.flush();
        
        /******************************
         * Presort eids int max(L1)
         ******************************/
        L1_Edge *best_edge = edge_list[0];
        sort    (edge_list.begin(), edge_list.end(), l1_edge_lt);
        reverse (edge_list.begin(), edge_list.end());
        int sat_index(0), vf2_index(0);

        #ifdef DEBUG
          cout << endl << "edge_list.sort(" << edge_list.size() << ") : ";
          for (unsigned int i = 0; i < edge_list.size(); i++)
              cout << "(" << edge_list[i]->eid << ", " << edge_list[i]->L1_prev << "," << edge_list[i]->L1_sat << "," << edge_list[i]->L1_vf2 << ") ";
          cout << endl;
        #endif

        while (sat_index < edge_list.size()) {

            /******************************
             * Load Threads (create sub-processes)
             ******************************/
            if (free_threads.size() > 0) {
                busy_threads.push_back(free_threads.back());
                free_threads.pop_back();

                if (busy_threads.back()->vf2)
                    if (vf2_index >= edge_list.size())
                        busy_threads.back()->vf2 = false;
                    
                if (busy_threads.back()->vf2) {
                    busy_threads.back()->test_edge = edge_list[vf2_index++];
                } else {
                    busy_threads.back()->test_edge = edge_list[sat_index++];
                }

                busy_threads.back()->open(true,false);

                /******************************
                 * Child
                 ******************************/
                if ( busy_threads.back()->child() ) {              // Child (PID == 0)

                    L1_Edge *test_edge = busy_threads.back()->test_edge;

                    #ifdef MEASURE_TIME_S1
                      clock_t tic = clock();
                    #endif

                    add_edge(test_edge->eid);

                    #ifdef DEBUG
                      cout << endl;
                      cout << "Child(" << getpid() << ") : before clean "<< solutions.size() << endl;
                    #endif
                    clean_solutions();

                    #ifdef DEBUG
                      cout << "Child(" << getpid() << ") :  after clean "<< solutions.size() << endl;
                    #endif
                    int old_size = solutions.size();
                    #ifdef MEASURE_TIME_S1
                      clock_t toc = clock();
                      #ifdef DEBUG
                        cout << "Child(" << getpid() << ") :   time clean ";
                             << (double) (toc-tic)/CLOCKS_PER_SEC << endl;
                      #endif
                    #endif

                    if (busy_threads.back()->vf2) {
                        if (test_edge->L1_prev < min_L1)
                            test_edge->L1_vf2 = 1;
                        if (test_edge->L1_prev <= best_edge->L1())
                            test_edge->L1_vf2 = test_edge->L1_prev;
                        else
                            test_edge->L1_vf2 = L1(true, true);
                    } else {
                        if (test_edge->L1_prev < min_L1)
                            test_edge->L1_sat = 1;
                        if (test_edge->L1_prev <= best_edge->L1())
                            test_edge->L1_sat = test_edge->L1_prev;
                        else
                            test_edge->L1_sat = L1();
                    }

                    string output;
                    if (busy_threads.back()->vf2) {
                        output = "S1_greedy.vf2 ("  + G->get_name() + ").child(" + num2str(getpid()) + ")";
                        output = report(output, G, H, test_edge->L1_vf2, solutions.size(), test_edge->edge);
                    } else {
                        output = "S1_greedy.sat ("  + G->get_name() + ").child(" + num2str(getpid()) + ")";
                        output = report(output, G, H, test_edge->L1_sat, solutions.size(), test_edge->edge);
                    }

                    #ifdef DEBUG
                      cout << output;
                    #endif

                    #ifdef USE_SOLNS
                      vector<igraph_vector_t*>::iterator it_begin = solutions.begin();
                      for (unsigned int i = 0; i < old_size; i++) {
                          it_begin++;
                          if (it_begin == solutions.end()) break;
                      }
                      for (vector<igraph_vector_t*>::iterator it = it_begin; it != solutions.end(); ++it)
                          output += report(*it);
                    #endif

                    busy_threads.back()->write(output);
                    busy_threads.back()->close(false, true);
                    
                    #ifdef MEASURE_TIME_S1
                      toc = clock();
                      cout << endl << "Child(" << getpid() << ") : Total Time: ";
                      cout << (double) (toc-tic)/CLOCKS_PER_SEC << endl;
                    #endif

                    _exit(0);
                }
            }
         
            /******************************
             * Unload Threads (Parent)
             ******************************/
            do {

                for (unsigned int j=0; j<busy_threads.size(); j++) {
                    string response = busy_threads[j]->read();
                    // do something with response
                    if (response.size() > 0) {

                        #ifdef MEASURE_TIME_S1
                          clock_t tic = clock();
                        #endif                   

                        L1_Edge *test_edge = busy_threads[j]->test_edge;
                        int L0;

                        stringstream r_stream(response);
                        string line;
                        while( getline(r_stream, line) ) {
                            
                            Edge tmp;
                            if (busy_threads[j]->vf2) {
                                parse(line, G, test_edge->L1_vf2, L0, tmp);
                                test_edge->L1_vf2 = min(max_L1, test_edge->L1_vf2);
                            } else {
                                parse(line, G, test_edge->L1_sat, L0, tmp);
                                test_edge->L1_sat = min(max_L1, test_edge->L1_sat);
                            }
                            
                            #ifdef USE_SOLNS
                              // recive solutions
                              igraph_vector_t *map21 = new igraph_vector_t();
                              igraph_vector_init (map21, igraph_vcount(H));
                              if ( parse(line, map21) ) {
                                  solutions.push_back(map21);
                              } else {
                                  igraph_vector_destroy (map21);
                                  delete map21;
                              }
                            #endif

                        }

                        // Store best results
                        if ( test_edge->L1() > best_edge->L1()) {
                            best_edge = test_edge;
                        }
                        if (busy_threads[j]->vf2)
                            cout << 'v';
                        else
                            cout << 's';
                        cout.flush();

                        #ifdef DEBUG
                          string output;
                          if (busy_threads[j]->vf2) {
                              output = "S1_greedy.vf2 ("  + G->get_name() + ").parent(" + num2str(busy_threads[j]->pid) + ")";
                              output = report(output, G, H, test_edge->L1_vf2, solutions.size(), test_edge->edge);
                          } else {
                              output = "S1_greedy.sat ("  + G->get_name() + ").parent(" + num2str(busy_threads[j]->pid) + ")";
                              output = report(output, G, H, test_edge->L1_sat, solutions.size(), test_edge->edge);
                          }
                          cout << endl << output;
                        #endif

                        free_threads.push_back(busy_threads[j]);
                        busy_threads.erase(busy_threads.begin()+j);
                        j = -1; // j++

                        #ifdef MEASURE_TIME_S1
                          clock_t toc = clock();
                          cout << endl << "Parent: pipe Time: ";
                          cout << (double) (toc-tic)/CLOCKS_PER_SEC << endl;
                        #endif
                    }
                    if (best_edge->L1() == max_L1) break;
                }
                if (best_edge->L1() == max_L1) break;

            } while (free_threads.size() == 0);
            
            // were done, clean up threads
            if (best_edge->L1() == max_L1)
                break;
        }

        // empty left over threads
        while (busy_threads.size() > 0) {
            if (busy_threads[0]->read().size() == 0 ) {
                #ifdef DEBUG
                  cout << "Parent: Kill left over thread(" << busy_threads[0]->pid << ")" << endl;
                #endif
                busy_threads[0]->close(true, false);
                busy_threads[0]->kill();
            }
            free_threads.push_back(busy_threads[0]);
            busy_threads.erase(busy_threads.begin());
        }

        if (best_edge->L1() < min_L1)
            break;

        // add to graph, remove from list, reset edges
        add_edge(best_edge->eid);
        max_L1 = best_edge->L1();
        
        int best_edge_index(-1);
        for (unsigned int i=0; i<edge_list.size(); i++) {
            if ( edge_list[i] == best_edge) best_edge_index = i;
            if ( edge_list[i]->L1() > 0 )   edge_list[i]->L1_prev = edge_list[i]->L1();
            edge_list[i]->L1_sat = -1;
            edge_list[i]->L1_vf2 = -1;
        }
        edge_list.erase(edge_list.begin()+best_edge_index);


        #ifdef MEASURE_TIME_S1
          clock_t tic = clock();
        #endif
        #ifdef USE_SOLNS
          clean_solutions();
        #endif
        #ifdef MEASURE_TIME_S1
          clock_t toc = clock();
          cout << endl << "Master: clean_solution() Time: ";
          cout << (double) (toc-tic)/CLOCKS_PER_SEC << endl;
        #endif

        string output;
        output = "S1_greedy ("  + G->get_name() + ")";
        output = report(output, G, H, best_edge->L1_prev, solutions.size(), best_edge->edge);
        cout << endl << output;
    }
    
    for (unsigned int i=0; i<edges.size(); i++)
        delete edges[i];
}




