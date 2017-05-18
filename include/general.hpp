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

// http://www.parashift.com/c++-faq-lite/
// http://www.emacswiki.org/emacs/KeyboardMacrosTricks

#ifndef GENERAL_H		// guard
#define GENERAL_H

/*****************************************************************************
 * INCLUDE
 ****************************************************************************/


#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <queue>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <sys/wait.h>
#include "sys/stat.h"
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>

using namespace std;


/*****************************************************************************
 * Defs
 ****************************************************************************/





/*****************************************************************************
 * Prototypes
 ****************************************************************************/

double urand ();
string get_tmp ();
string get_work ();
string get_home ();
string get_home (string add);
void rm_rf (string content);
void print_file (string filename);
string num2str(int num);

bool test_free_mem(int mb);
bool test_free_mem(double percent);


/*****************************************************************************
 * Classes
 ****************************************************************************/



class Thread {
private:
    int    c2p_pipe[2], p2c_pipe[2];
    
    int    in_pipe   ();
    int    out_pipe  ();
    bool   test_read ();
public:
    pid_t  pid;
    void   open  (bool c2p=true, bool p2c=true);
    void   close (bool read=true, bool write=true);
   
    string read   ();
    void   write  (string);
    void   kill   ();
    bool   child  ();
    bool   parent ();
};


















#endif
