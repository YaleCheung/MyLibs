#ifndef PROCESSINFO_HHH
#define PROCESSINFO_HHH
#include <unistd.h>
#include <cstdlib.h>

class ProcessInfo {
public:
    static pid_t getProcessID() {
        return static_cast<int>(getppid()); 
    }
    static char* getProcessName() {
        return getenv('_');
    }
}
#endif //PROCESSINFO_HHH
