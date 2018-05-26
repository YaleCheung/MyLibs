#ifndef SLICE_HHH
#define SLICE_HHH

#include <string.h>
#include <string>
#include <cassert>
// fixed size string, because string is too complex and needs more size;
class Slice {
public:
    Slice() :
      data_(""), size_(0) { }
    Slice(const char* d, size_t n) : 
      data_(d), size_(n) { };
    // compatible with string
    Slice(const std::string str) : data_(str.data()), size_(str.length()) {};
    // only parameter is char*
    Slice(const char* s) : data_(s), size_(strlen(s)) {};

    // like std::string, return a pointer of data_
    const char* Data() const {
        return data_;
    }

    // size
    int size() const {
        return size_;
    }
    // empty
    bool empty () const {
        return size_ == 0;
    }
    
    // cut operator
    char operator[](size_t n) const {
        assert(n <= size_);
        return data_[n];
    }

    // clear
    void clear() {
        data_ = "";
        size_ = 0;
    }
    // remove prefix
    void remove_prefix(size_t n) {
        assert(n <= size());
        data_ += n;
        size_ -= n;
    }

    std::string to_string() const {
        return std::string(data_, size_);
    }

    // compare
    int compare(const Slice& b) const;

    // Return true iff "x" is a prefix of "*this"
    bool starts_with(const Slice& s) const {
        return ((size_ >= s.size_) && 
                (memcmp(data_, s.data_, s.size_) == 0));
    }
private:
    const char* data_;
    size_t size_;
};

inline bool operator== (const Slice& l, const Slice& r) {
    return ((l.size() == r.size()) && 
            (memcmp(l.data(), r.data(), l.size()) == 0));
}

inline bool operator!= (const Slice& l, const Slice& r) {
    return ((l.size() == r.size()) &&
            (memcmp(l.data(), r.data(), l.size()) != 0));

}

inline int Slice::compare(const Slice& s) const {
    const size_t min_len = (size_ < s.size_) ? size_ : s.size_;
    int ret = memcmp(data_, s.data_, min_len);
    if (0 == ret) {
        if (size_ < s.size_) 
            ret = -1;
        else if (size_ > s.size_)
            ret = 1;
    }
    return ret;
}
#endif // Slice_HHH
