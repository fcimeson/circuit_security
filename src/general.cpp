
/*************************************************************************//**
 *****************************************************************************
 * @file        general.cpp
 * @brief       
 * @author      Frank Imeson
 * @date        
 *****************************************************************************
 ****************************************************************************/

// http://www.ros.org/wiki/CppStyleGuide

#include "general.hpp"
//#define DEBUG


/*************************************************************************//**
 * @brief	
 * @version						v0.01b \n
 ****************************************************************************/
//bool test_free_mem(int mb) {
////    sg_init();
//    return sg_get_mem_stats()->free > mb*1024;
//}



/*************************************************************************//**
 * @brief	
 * @version						v0.01b \n
 ****************************************************************************/
//bool test_free_mem(double percent) {
////cout << "Total Mem = " << sg_get_mem_stats()->total << ", Free = " << sg_get_mem_stats()->free << endl;
////cout << "Percentage = " << (double) sg_get_mem_stats()->free/sg_get_mem_stats()->total << endl;
////cout << "return = " << ( ( (double) sg_get_mem_stats()->free/sg_get_mem_stats()->total ) > percent) << endl;
//    return ( (double) sg_get_mem_stats()->free/sg_get_mem_stats()->total ) > percent;
//}


/*************************************************************************//**
 * @brief	
 * @version						v0.01b \n
 ****************************************************************************/
string num2str(int num) {
    stringstream out;
    out << num;
    return out.str();
}



/*************************************************************************//**
 * @brief	
 * @version						v0.01b \n
 ****************************************************************************/
double urand () {
	return (double)rand()/(double)RAND_MAX;
}


/*************************************************************************//**
 * @brief	
 * @version						v0.01b \n
 ****************************************************************************/
string get_tmp () {
    stringstream out;

    char *tmp_dir = getenv ("TMPDIR");
    if (tmp_dir != NULL)
        out << tmp_dir;
    else
        out << "/tmp";
    
//    out << "/" << pthread_self();
    return out.str();
}


string get_work() {
    stringstream out;

    char *work_dir = getenv ("WORKDIR");
    if (work_dir != NULL)
        out << work_dir;
    else
        out << "./work";
    
    return out.str();
}



/*************************************************************************//**
 * @brief	
 * @version						v0.01b \n
 ****************************************************************************/
string get_home () {
    stringstream out;
    char *home_dir = getenv ("HOME");
    out << home_dir;
    return out.str();
}

/*************************************************************************//**
 * @brief	
 * @version						v0.01b \n
 ****************************************************************************/
string get_home (string add) {
    return get_home() + add;
}


/*************************************************************************//**
 * @brief	
 * @version						v0.01b \n
 ****************************************************************************/
void rm_rf (string content) {
    // Error (PID == -1)    
    pid_t pid = fork();
    if (pid == -1) {
        perror("Error: No PID \n");
        exit(1);    

    // Child (PID == 0)
    } else if (pid == 0) {
        execlp("/bin/rm","rm", "-rf", content.c_str(), NULL);
        perror("Error: Exec \n");
        _exit(1);

    // Parent (PID > 0)
    } else {                  
        wait(NULL); // wait for thread
    }
}



/*************************************************************************//**
 * @brief	
 * @version						v0.01b \n
 ****************************************************************************/
void print_file (string filename) {
    ifstream file;
    file.open(filename.c_str());
    while (file.good()) {
        string line;
        getline(file, line);
        printf("%s\n", line.c_str());        
    }
}



/*************************************************************************//**
 * @brief	
 * @version						v0.01b
 ****************************************************************************/
void Thread::open (bool c2p, bool p2c) {
    if (c2p) {
        if (pipe(c2p_pipe) < 0) {
            perror("perror(pipe)");
            exit(1);
        }
    }
    if (p2c) {
        if (pipe(p2c_pipe) < 0) {
            perror("perror(pipe)");
            exit(1);
        }
    }
    #ifdef DEBUG
      cout << "Thread Opened ( )("
           << c2p_pipe[0] << ","
           << c2p_pipe[1] << ","
           << p2c_pipe[0] << ","
           << p2c_pipe[1]
           << ")" << endl;
    #endif
    pid = fork();
   
    // Error
    if (pid == -1) {
        cout << "Error Number " << errno << endl;
        perror("Error: No PID \n     ");
        exit(1);
    // Child
    } else if (pid == 0) {
        if (c2p) {
            // close read pipe
            if (::close(c2p_pipe[0]) < 0) {
                stringstream err_msg;
                err_msg << "error: c2p[0] o-close(" << pid << ")";
                perror(err_msg.str().c_str());
                exit(1);
            }
        }
        if (p2c) {
            // close write pipe
            if (::close(p2c_pipe[1]) < 0) {
                stringstream err_msg;
                err_msg << "error: p2c[0] o-close(" << pid << ")";
                perror(err_msg.str().c_str());
                exit(1);
            }
        }
    // Parent
    } else {
        if (c2p) {
            // close write pipe
            if (::close(c2p_pipe[1]) < 0) {
                stringstream err_msg;
                err_msg << "error: c2p[1] o-close(" << pid << ")";
                perror(err_msg.str().c_str());
                exit(1);
            }
        }
        if (p2c) {
            // close read pipe
            if (::close(p2c_pipe[0]) < 0) {
                stringstream err_msg;
                err_msg << "error: p2c[0] o-close(" << pid << ")";
                perror(err_msg.str().c_str());
                exit(1);
            }
        }
    }
}


/*************************************************************************//**
 * @brief	
 * @version						v0.01b
 ****************************************************************************/
void Thread::close (bool read, bool write) {
    if (child()) {
        if (read) {
            if (::close(p2c_pipe[0]) < 0) {
                stringstream err_msg;
                err_msg << "error: p2c[0] close(" << pid << ")";
                perror(err_msg.str().c_str());
                exit(1);
            }
        }
        if (write) {
            if (::close(c2p_pipe[1]) < 0) {
                stringstream err_msg;
                err_msg << "error: c2p[1] close(" << pid << ")";
                perror(err_msg.str().c_str());
                exit(1);
            }
        }
        #ifdef DEBUG
          cout << "Thread Closed (C)("
               << c2p_pipe[0] << ","
               << c2p_pipe[1] << ","
               << p2c_pipe[0] << ","
               << p2c_pipe[1]
               << ")" << endl;
        #endif
    } else {
        if (read) {
            if ( fcntl(c2p_pipe[0], F_GETFL) >= 0 ) {
                if (::close(c2p_pipe[0]) < 0) {
                    stringstream err_msg;
                    err_msg << "error: c2p[0] close(" << pid << ")";
                    perror(err_msg.str().c_str());
                    exit(1);
                }
            }
        }
        if (write) {
            if ( fcntl(p2c_pipe[1], F_GETFL) >= 0 ) {
                if (::close(p2c_pipe[1]) < 0) {
                    stringstream err_msg;
                    err_msg << "error: p2c[1] close(" << pid << ")";
                    perror(err_msg.str().c_str());
                    exit(1);
                }
            }
        }
        #ifdef DEBUG
          cout << "Thread Closed (P)("
               << c2p_pipe[0] << ","
               << c2p_pipe[1] << ","
               << p2c_pipe[0] << ","
               << p2c_pipe[1]
               << ")" << endl;
        #endif
    }
}






/*************************************************************************//**
 * @brief	
 * @version						v0.01b
 ****************************************************************************/
bool Thread::child () {
    return pid == 0;
}


/*************************************************************************//**
 * @brief	
 * @version						v0.01b
 ****************************************************************************/
bool Thread::parent () {
    return pid > 0;
}


/*************************************************************************//**
 * @brief	
 * @version						v0.01b
 ****************************************************************************/
int Thread::in_pipe () {
    if (child())
        return p2c_pipe[0];
    else
        return c2p_pipe[0];
}



/*************************************************************************//**
 * @brief	
 * @version						v0.01b
 ****************************************************************************/
int Thread::out_pipe () {
    if (child())
        return c2p_pipe[1];
    else
        return p2c_pipe[1];
}



/*************************************************************************//**
 * @brief	
 * @version						v0.01b
 ****************************************************************************/
bool Thread::test_read () {
    // Watch set for input
    fd_set set;
    FD_ZERO(&set);
    FD_SET(in_pipe(), &set);
    struct timeval timeout;
    timeout.tv_sec = 0;
    timeout.tv_usec = 10;
    
    return (select(in_pipe()+1, &set, NULL, NULL, &timeout) > 0);
}


/*************************************************************************//**
 * @brief	
 * @version						v0.01b
 ****************************************************************************/
string Thread::read () {
    stringstream input;

    if (test_read()) {
        char buff;
        while ( ::read(in_pipe(), &buff,  sizeof(char)) > 0 ) {
            input << buff;
        }
        close(true, false);
        waitpid(pid, NULL, 0);
    }
    return input.str();
}


/*************************************************************************//**
 * @brief	
 * @version						v0.01b
 ****************************************************************************/
void Thread::write (string output) {
    ::write(out_pipe(), output.c_str(), sizeof(char)*output.size());
    fsync(out_pipe());
}



/*************************************************************************//**
 * @brief	
 * @version						v0.01b
 ****************************************************************************/
void Thread::kill () {
    ::kill(pid, SIGKILL);
    waitpid(pid, NULL, 0);
    #ifdef DEBUG
      cout << "Thread Killed (P)("
           << c2p_pipe[0] << ","
           << c2p_pipe[1] << ","
           << p2c_pipe[0] << ","
           << p2c_pipe[1]
           << ")" << endl;
    #endif

}




