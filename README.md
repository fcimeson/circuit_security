# circuit_security

This is a repository of the code used for the paper, “Securing Computer Hardware Using 3D Integrated Circuit (IC) Technology and Split Manufacturing for Obfuscation”. The code provides a solver that takes as input a bliff curcuit and finds sub-curciuts that meet the k-security requirements defined in the paper.

Install instructions:
 - make
 
Runtime instructions:
  --circuit arg                         input circuit
  --tech_lib arg                        tech library
  -c [ --continue_file ] arg            input report to continue from
  -m [ --mono_lib ]                     treat each gate as a nand
  -b [ --budget ] arg (=100000000)      minisat conflict budget (-1 = 
                                        unlimited)
  -e [ --remove_edges ] arg (=0.600000024)
                                        edge percent to remove
  --print_graph                         print H graph
  --print_solns                         print isos
  --print_blif                          print G blif circuit
  --print_verilog                       print verilog circuits
  -t [ --test ] arg                     Run test suite #: 
                                          -99: Beta Testing 
                                          -1: Mem Testing 
                                           0: L0 [count, remove_percent] 
                                           1: L1 [count, remove_percent] 
                                           2: S1 - Random [min S1] 
                                           3: S1 - Greedy [min S1] 
                                        
  -n [ --num_threads ] arg (=1)         Number of threads
  -w [ --wdir ] arg (=./wdir)           Output directory
  -h [ --help ]                         produce help message
