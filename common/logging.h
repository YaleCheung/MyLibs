#ifndef LOGGING_HHH
#define LOGGING_HHH
#include <chrono>
#include "noncopyable.h"
#include "types.h"
#include <string>
#include <cstdio>
#include <cassert>

// non thread safe

using string = std::string;
using TimePoint = std::chrono::system_clock::time_point;


enum RollMethod{
    kNONE,
    kSIZEROLL, 
    kINTERVALROLL, 
    kHYBRIDROLL
};


class FileWriter : public NonCopyable {
    static const int BUFFERSIZE{32*1024};   // 32 kbyte;
public: 
    explicit FileWriter(string file_name);
    ~FileWriter();

    void write2File(const char* log_content, const size_t len);

    void flush();

    size_t getWrited_len() const {
        return _writed_len;
    }
private:

    FILE* _file;
    char _buffer[BUFFERSIZE];

    size_t _writed_len;
};

FileWriter::FileWriter(string filename) :
    _file(fopen(filename.c_str(), "ae")), _writed_len(0) {
    assert(_file);
    setbuffer(_file, _buffer, sizeof _buffer);
}

FileWriter::~FileWriter() {
    fclose(_file);
}

void FileWriter::write2File(const char* log_content, const size_t len) {
    size_t n = fwrite_unlocked(log_content, 1, len, _file);
    size_t remain = len - n;
    while (remain > 0) {
        size_t x = fwrite_unlocked(log_content, 1, remain, _file);

        if (x == 0) {
            int err = ferror(_file);
            if (err) 
                fprintf(stderr, "Write failed %s %d\n",__FILE__, __LINE__);
                break;
        }
        n += x;
        remain = len - n; // remain -= x
    }

    _writed_len += len;
}

void FileWriter::flush() {
    fflush(_file);
}

class LogManager : public NonCopyable{
public: 
    LogManager(const string& path, const string& base_name, uint32_t roll_size, uint32_t flush_interval) :
      _path(path), _base_name(base_name), _roll_size(roll_size),
      _flush_interval(flush_interval), _count(0),
      _start_time(std::chrono::system_clock::now()) {
        
    } 

    //getters
    string getBaseName(std::string path) {

    }
    void roll() {
    }
    
private:
    string _path;
    string _base_name;
    const uint32_t _roll_size;
    const uint32_t _flush_interval;
    const uint32_t _count;   // logfilecount in 

    TimePoint _start_time;
    TimePoint _last_create_time;
    TimePoint _last_flush_time;



    const static uint32_t _day_seconds = 60 * 60 * 24;

};

class Logging : NonCopyable {
};
#endif
