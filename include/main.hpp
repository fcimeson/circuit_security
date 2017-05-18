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
// http://www.stack.nl/~dimitri/doxygen/


/*****************************************************************************
 * INCLUDE
 ****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <ctime>
#include <queue>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <iomanip>
#include <boost/program_options.hpp>

#include "general.hpp"
#include "circuit.hpp"
#include "security.hpp"


#include "minisat/core/Solver.h"
using namespace Minisat;

#include "formula.hpp"
#include "subisosat.hpp"


using namespace std;
using namespace isosat;
namespace po = boost::program_options;


/*****************************************************************************
 * Defs
 ****************************************************************************/



