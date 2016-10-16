
/*************************************************************************//**
 *****************************************************************************
 * @file        main.hpp
 * @brief       
 * @author      Frank Imeson
 * @date        2012-01-02
 *****************************************************************************
 ****************************************************************************/

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



