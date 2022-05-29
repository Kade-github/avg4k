#include <streambuf>
#include <ios>
#include <string>

class log_stream : public std::streambuf {
public:
    log_stream(std::streambuf* origBuf)
    {
        orig = origBuf;
    }

private:
    std::queue<char> queue;
    std::streambuf* orig;

protected:

    virtual int overflow(int c) override {
        
        if (c == EOF || c <= -1 || c >= 256)
            return c;
        
        char ch = (char)c;

        queue.push(c);

        if (this->queue.size() > 100)
            dump();

        return c;
    }

public:



    void dump() {
        std::string push;
        while (!this->queue.empty()) {
           char top = this->queue.front();
            this->queue.pop();
            push += top;
        }
        
        orig->sputn(push.c_str(), push.length());
    }

};