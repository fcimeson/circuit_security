
/*************************************************************************//**
 *****************************************************************************
 * @file        main.cpp
 * @brief       
 * @author      Frank Imeson
 * @date        2012-01-02
 *****************************************************************************
 ****************************************************************************/


#include "main.hpp"

#define DEBUG



/********************************************************************************
 * Notes:

  conflict_budget = 10000, min before we see changes from -i c17 -l 02 -mt4
  conflict_budget = 1000000, max before "it takes to long" for -i c432 -l 03 -t4


 ********************************************************************************/


/********************************************************************************
 * Global
 ********************************************************************************/




/********************************************************************************
 * Functions
 ********************************************************************************/




/*****************************************************************************
 * Main
 ****************************************************************************/
int main(int argc, char **argv) {

    srand ( time(NULL) );
       
    /******************************
     * Command Line Parser
     ******************************/
         
    bool mono_lib(false), print_gate(false), print_solns(false), print_sat(false), print_blif(false), print_verilog(false);
    int num_threads, budget;
    float remove_percent(0.6);
    vector<string> test_args;
    
    po::options_description optional_args("Usage: [options]");
    optional_args.add_options()
        ("circuit",         po::value<string>(),                                    "input circuit")
        ("tech_lib",        po::value<string>(),                                    "tech library")
        ("continue_file,c", po::value<string>(),                                    "input report to continue from")
        ("mono_lib,m",      po::value(&mono_lib)->zero_tokens(),                    "treat each gate as a nand")
        ("budget,b",        po::value<int>(&budget)->default_value(100000000),      "minisat conflict budget (-1 = unlimited)")
        ("remove_edges,e",  po::value<float>(&remove_percent)->default_value(0.6),  "edge percent to remove")
        ("print_graph",     po::value(&print_gate)->zero_tokens(),                  "print H graph")
        ("print_solns",     po::value(&print_solns)->zero_tokens(),                 "print isos")
        ("print_blif",      po::value(&print_blif)->zero_tokens(),                  "print G blif circuit")
        ("print_verilog",   po::value(&print_verilog)->zero_tokens(),               "print verilog circuits")
        ("test,t",          po::value< vector<string> >(&test_args),                 "Run test suite #: \n"
                                                                                    "  -99: Beta Testing \n"
                                                                                    "  -1: Mem Testing \n"
                                                                                    "   0: L0 [count, remove_percent] \n"
                                                                                    "   1: L1 [count, remove_percent] \n"
                                                                                    "   2: S1 - Random [min S1] \n"
                                                                                    "   3: S1 - Greedy [min S1] \n")
        ("num_threads,n",   po::value<int>(&num_threads)->default_value(1),         "Number of threads")
        ("wdir,w",          po::value<string>()->default_value("./wdir"),           "Output directory")
        ("help,h",                                                                  "produce help message")
    ;

    po::positional_options_description positional_args;
    positional_args.add("circuit", 1);
    positional_args.add("tech_lib", 1);
    positional_args.add("test", -1);
    po::variables_map vm;

    try {
        po::store(po::command_line_parser(argc, argv).options(optional_args).positional(positional_args).run(), vm);
        po::notify(vm);
    } catch(exception& e) {
        cerr << "error: " << e.what() << "\n";
        return 1;
    } catch(...) {
        cerr << "Exception of unknown type!\n";
    }

    if (vm.count("help")) {
        cout << optional_args << "\n";
        return 0;
    }

    string circuit_filename, circuit_name, tech_filename, tech_name, working_dir, report_filename;
   
    // input circuit
    if (vm.count("circuit") == 1) {
        circuit_filename = vm["circuit"].as<string>();
        circuit_name = circuit_filename.substr(circuit_filename.rfind('/')+1);
        circuit_name = circuit_name.substr(0, circuit_name.find('.'));
    } else {
        cout << optional_args << "\n";
        return 0;
    }

    // input tech lib
    if (vm.count("tech_lib")) {
        tech_filename = vm["tech_lib"].as<string>();
        tech_name = tech_filename.substr(tech_filename.rfind('/')+1);
        tech_name = tech_name.substr(0, tech_name.find('.'));
    } else {
        cout << optional_args << "\n";
        return 0;
    }

    cout << "Circuit : " << circuit_filename << "\n";
    cout << "Tech Lib: " << tech_filename << "\n";

   
    /******************************
     * Setup working dir
     ******************************/
    working_dir = vm["wdir"].as<string>();
    mkdir(working_dir.c_str(), S_IRWXU);
    cout << "WDIR    : " << working_dir << "\n";


    /******************************
     * Convert circuit using tech_lib
     ******************************/
    string outfile = working_dir + circuit_filename.substr(circuit_filename.rfind('/'));
    cout << "Outfile : " << outfile << "\n";
    sis_convert(circuit_filename, tech_filename, outfile);
    circuit_filename = outfile;
    // copy tech_lib
    if ( tech_filename != string(working_dir + tech_filename.substr(tech_filename.rfind('/')) ) ) {
        ifstream src( tech_filename.c_str() );
        ofstream dst( string(working_dir + tech_filename.substr(tech_filename.rfind('/')) ).c_str() );
        dst << src.rdbuf();
        dst.close();
    }

   
    /******************************
     * Load circuit
     ******************************/
    Circuit circuit;
    load_circuit(&circuit, circuit_filename, mono_lib);
    
    Security *security;
    Circuit G, H;
    G.copy(&circuit);
    G.remove_io();

    circuit.save( working_dir + "/circuit.gml" );
    G.save( working_dir + "/G_circuit.gml" );


    /****************************************************************
     * print G
     ****************************************************************/
    if ( test_args.size() > 0 && -1 == atoi(test_args[0].c_str())) {
         G.print();
    }


    /****************************************************************
     * L0
     ****************************************************************/
    if ( test_args.size() > 0 && 0 == atoi(test_args[0].c_str())) {

        int max_count(2);
        if (test_args.size() == 2)
            max_count = atoi(test_args[1].c_str());

        H.copy(&G);
        H.rand_del_edges(remove_percent);
        H.save( working_dir + "/H_circuit.gml" );

        security = new Security(&G, &H);
        security->setConfBudget(budget);

        cout << "Rand L0: |V(G)| = "  << (int) igraph_vcount(&G);
        cout << ", |E(G)| = "         << (int) igraph_ecount(&G);
        cout << ", |V(H)| = "         << (int) igraph_vcount(&H);
        cout << ", |E(H)| = "         << (int) igraph_ecount(&H) << "\n";
        
        cout << " " << boolalpha << security->L0(max_count, false) << endl;
    }



    /****************************************************************
     * L1
     ****************************************************************/
    if ( test_args.size() > 0 && 1 == atoi(test_args[0].c_str())) {

        int max_L1(2);
        if (test_args.size() == 2)
            max_L1 = atoi(test_args[1].c_str());

        H.copy(&G);
        H.rand_del_edges(remove_percent);
        
        if (vm.count("continue_file")) {
            H.rand_del_edges((float) 1.0);
            string filename = vm["continue_file"].as<string>();
            ifstream file;
            try {
                file.open(filename.c_str());
                
                while (file.good()) {
                    string line;
                    int L0, L1;
                    Edge edge;

                    getline(file, line);
                    if (parse(line, &G, L1, L0, edge)) {
                        if (L1 >= max_L1) {
                            H.add_edge(edge);
                            cout << "L1 = " << max_L1 << ", +<" << edge.first << "," << edge.second << ">" << endl;
                        }
                    }
                }
            } catch(...) {}
        }

        H.save( working_dir + "/H_circuit.gml" );

        security = new Security(&G, &H);
        security->setConfBudget(budget);

        cout << "Rand L1: |V(G)| = "  << (int) igraph_vcount(&G);
        cout << ", |E(G)| = "         << (int) igraph_ecount(&G);
        cout << ", |V(H)| = "         << (int) igraph_vcount(&H);
        cout << ", |E(H)| = "         << (int) igraph_ecount(&H) << "\n";
        
        cout << " " << boolalpha << security->L1(max_L1, false) << endl;
    }



    /****************************************************************
     * #L1
     ****************************************************************/
    if ( test_args.size() == 1 && 2 == atoi(test_args[0].c_str())) {

        int max_L1(2);
        if (test_args.size() == 2)
            max_L1 = atoi(test_args[1].c_str());

        H.copy(&G);
        H.rand_del_edges(remove_percent);

        if (vm.count("continue_file")) {
            H.rand_del_edges((float) 1.0);
            string filename = vm["continue_file"].as<string>();
            ifstream file;
            try {
                file.open(filename.c_str());
                
                while (file.good()) {
                    string line;
                    int L0, L1;
                    Edge edge;

                    getline(file, line);
                    if (parse(line, &G, L1, L0, edge)) {
                        H.add_edge(edge);
                        max_L1 = L1;
                        cout << "L1 = " << max_L1 << ", +<" << edge.first << "," << edge.second << ">" << endl;
                    }
                }
            } catch(...) {}
        }

        H.save( working_dir + "/H_circuit.gml" );

        security = new Security(&G, &H);
        security->setConfBudget(budget);

        cout << "Rand L1: |V(G)| = "  << (int) igraph_vcount(&G);
        cout << ", |E(G)| = "         << (int) igraph_ecount(&G);
        cout << ", |V(H)| = "         << (int) igraph_vcount(&H);
        cout << ", |E(H)| = "         << (int) igraph_ecount(&H) << "\n";
        cout << " " << security->L1(false);
    }



    /****************************************************************
     * S1_rand
     ****************************************************************/
    if ( test_args.size() == 1 && 3 == atoi(test_args[0].c_str())) {

        int max_L1 = G.max_L1();
        H.copy(&G);
        H.rand_del_edges((float) 1.0);

        security = new Security(&G, &H);
        security->setConfBudget(budget);

        string output;
        output = "S1_rand ("  + G.get_name() + ")";
        output = report(output, &G, &H, max_L1);
        cout << output;

        security->S1_rand(num_threads);
    }



    /****************************************************************
     * S1_greedy
     ****************************************************************/
    if ( test_args.size() >= 1 && 4 == atoi(test_args[0].c_str())) {

        int min_L1(2), max_L1 = G.max_L1();
        H.copy(&G);
        H.rand_del_edges((float) 1.0);
        bool done(false);
        
        if ( test_args.size() == 3 ) {
            min_L1 = atoi(test_args[1].c_str());
            max_L1 = atoi(test_args[2].c_str());
        } else if ( test_args.size() == 2 )
            min_L1 = atoi(test_args[1].c_str());

        if (vm.count("continue_file")) {
            string filename = vm["continue_file"].as<string>();
            ifstream file;
            try {
                file.open(filename.c_str());
                
                while (file.good()) {
                    string line;
                    int L0, L1;
                    Edge edge;

                    getline(file, line);
                    if (parse(line, &G, L1, L0, edge)) {
                        if (L1 < min_L1) {
                            done = true;
                            break;
                        }
                        H.add_edge(edge);
                        max_L1 = L1;
                        cout << "L1 = " << max_L1 << ", +<" << edge.first << "," << edge.second << ">" << endl;
                    }
                }
            } catch(...) {}
        }

        if ( test_args.size() == 3 ) {
            min_L1 = atoi(test_args[1].c_str());
            max_L1 = atoi(test_args[2].c_str());
        } else if ( test_args.size() == 2 )
            min_L1 = atoi(test_args[1].c_str());

        security = new Security(&G, &H);
        security->setConfBudget(budget);

        string output;
        output = "S1_greedy ("  + G.get_name() + ")";
        output = report(output, &G, &H, max_L1);
        cout << output;

        fstream report;
        if (!done)
            security->S1_greedy(num_threads, min_L1, max_L1);

    }


    /****************************************************************
     * L1(label)
     ****************************************************************/
    if ( test_args.size() >= 1 && 5 == atoi(test_args[0].c_str())) {

        string label = "";
        if (test_args.size() == 2)
            label = test_args[1];

        int max_L1(2);
        H.copy(&G);
        H.rand_del_edges(remove_percent);
        
        if (vm.count("continue_file")) {
            H.rand_del_edges((float) 1.0);
            string filename = vm["continue_file"].as<string>();
            ifstream file;
            try {
                file.open(filename.c_str());
                
                while (file.good()) {
                    string line;
                    int L0, L1;
                    Edge edge;

                    getline(file, line);
                    if (parse(line, &G, L1, L0, edge)) {
                        H.add_edge(edge);
                        max_L1 = L1;
                        cout << "L1 = " << max_L1 << ", +<" << edge.first << "," << edge.second << ">" << endl;
                    }
                }
            } catch(...) {}
        }

        H.save( working_dir + "/H_circuit.gml" );

        security = new Security(&G, &H);
        security->setConfBudget(budget);

        security->L1(label);       
    }


    if ( test_args.size() >= 1 && atoi(test_args[0].c_str()) >= 0) {
        H.save( working_dir + "/H_circuit.gml" );
        delete security;
    }

    if (print_gate)
        G.print();

    if (print_blif)
        print_file(circuit_filename);
        
    if (print_solns)
        security->print_solutions();

    if (print_verilog)
        security->print_solutions();


    printf("\n\ndone 0 \n");
    return 0;
}








