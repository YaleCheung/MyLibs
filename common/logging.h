#ifndef LOGGING_HHH
#define LOGGING_HHH
#include <chrono>
#include "noncopyable.h"
#include "types.h"
#include <string>
#include <cstdio>
#include <cassert>
#include <cstring>
#include "ProcessInfo.h"

// non thread safe

using string = std::string;
using chrono = std::chrono;
using system_clock = chrono::system_clock;
using TimePoint = chrono::system_clock::time_point;


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
    _file(::fopen(filename.c_str(), "ae")), _writed_len(0) {
    assert(_file);
    ::setbuffer(_file, _buffer, sizeof _buffer);
}

FileWriter::~FileWriter() {
    ::fclose(_file);
}

void FileWriter::write2File(const char* log_content, const size_t len) {
    size_t n = ::fwrite_unlocked(log_content, 1, len, _file);
    size_t remain = len - n;
    while (remain > 0) {
        size_t x = ::fwrite_unlocked(log_content, 1, remain, _file);

        if (x == 0) {
            int err = ::ferror(_file);
            if (err) 
                ::fprintf(stderr, "Write failed %s %d\n",__FILE__, __LINE__);
                break;
        }
        n += x;
        remain = len - n; // remain -= x
    }

    _writed_len += len;
}

void FileWriter::flush() {
    ::fflush(_file);
}

class LogManager : public NonCopyable{
    static const int FILENAMEMAXLEN{64};
    static const int THREADIDLEN{64};
public: 

    LogManager(const string& base_name, uint32_t roll_size, uint32_t flush_interval) :
      _base_name(base_name), _roll_size(roll_size),
      _flush_interval(flush_interval), _count(0),
      _start_time(std::chrono::system_clock::now()) {
        
    } 

    string getFileName(const string& base_name, TimePoint time){
        _base_name = base_name;
        auto filename = string("");
        filename.reserve(base_name.size() + FILENAMEMAXLEN);
        filename = _base_name;

        char tm_buf[32] = {'\0'};
        auto now = chrono::system_clock::now();
        time_t t = system_clock::to_time_t(now);
        std::tm tm = *std::localtime(&t);
        ::strftime(tm_buf, sizeof(tm_buf), ".%Y%m%d-%H%M%S.", &tm);
        filename += tm_buf;

        char pid_buf[THREADIDLEN] = {'\0'};
        snprintf(pid_buf, sizeof(pid_buf), ".%d.", ProcessInfo::getProcessID());

    }

    void roll() {
        ::assert()
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

    const static uint32_t _day_seconds{60 * 60 * 24};
    std::unique_ptr<FileWriter> _file_writer;
};

class Logging : NonCopyable {
};
#endif
