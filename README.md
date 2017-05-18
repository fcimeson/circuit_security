# K-Security for 3D Circuits
This is the code repository for the paper, “Securing Computer Hardware Using 3D Integrated Circuit (IC) Technology and Split Manufacturing for Obfuscation”. The code provides a solver that takes as input a bliff curcuit and finds sub-curciuts that meet the k-security requirements defined in the paper.


# Citing this work
      @inproceedings{imeson2013securing,
        title={Securing Computer Hardware Using 3D Integrated Circuit (IC) Technology and Split Manufacturing for Obfuscation.},
        author={Imeson, Frank and Emtenan, Ariq and Garg, Siddharth and Tripunitara, Mahesh V},
        booktitle={USENIX Security},
        volume={13},
        year={2013}
      }


# Installation
This software depends on the sis circuit tool. It must be installed into the directory ~/bin. To compile the software type make.


# Usage
```
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
                                              4: Tests from paper

     -n [ --num_threads ] arg (=1)         Number of threads
     -w [ --wdir ] arg (=./wdir)           Output directory
     -h [ --help ]                         produce help message
```
##### Example:
```
$ circuit_security circuits/c27.blif tech_lib/lib2.genlib -t4 -m
  Circuit : circuits/c27.blif
  Tech Lib: tech_lib/lib2.genlib
  WDIR    : ./wdir
  Outfile : ./wdir/c27.blif

  S1_greedy (c27): |V(G)| = 7, |E(G)| = 7, |V(H)| = 7, |E(H)| = 0, L1 = 7
    E(7) sssssss
  S1_greedy (c27): |V(G)| = 7, |E(G)| = 7, |V(H)| = 7, |E(H)| = 1, #L0 = 14, L1 = 4, +<3,4>
    E(6) ss
  S1_greedy (c27): |V(G)| = 7, |E(G)| = 7, |V(H)| = 7, |E(H)| = 2, #L0 = 7, L1 = 4, +<0,2>
    E(5) s
  S1_greedy (c27): |V(G)| = 7, |E(G)| = 7, |V(H)| = 7, |E(H)| = 3, #L0 = 9, L1 = 4, +<1,5>
    E(4) ssss
  S1_greedy (c27): |V(G)| = 7, |E(G)| = 7, |V(H)| = 7, |E(H)| = 4, #L0 = 7, L1 = 3, +<0,6>
    E(3) sss
  S1_greedy (c27): |V(G)| = 7, |E(G)| = 7, |V(H)| = 7, |E(H)| = 5, #L0 = 4, L1 = 2, +<1,2>
    E(2) ss
```    
##### Interpretation:  
 - L1 = level of security  
 - H is the modified graph/circuit  
 - G is the un-modified graph/circuit  
 - +<X,Y> is the edge that is added to H  
 - E(#) sss... is feedback on how many edges can be added and the s's represent how many have been tested  


# License
Copyright 2017 Frank Imeson, Siddharth Garg, and Mahesh V. Tripunitara
Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at http://www.apache.org/licenses/LICENSE-2.0
Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.


# Contact information
Frank Imeson  
Department of Electrical and Computer Engineering  
University of Waterloo  
Waterloo, ON Canada  
web: https://ece.uwaterloo.ca/~fcimeson/  
email: fcimeson@uwaterloo.ca  
