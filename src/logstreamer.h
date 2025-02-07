// Created by Carl Johan Gribel.
// Licensed under the MIT License. See LICENSE file for details.

#ifndef streamlogger_h
#define streamlogger_h

#include <vector>
#include <ostream>
#include <fstream>

namespace logstreamer
{
    
    //! Distributes log output to multiple stream with priority
    //
    // Note: ostream <=> basic_ostream<char>
    //
    class logstreamer_t
    {
        std::vector< std::pair<std::ostream*, int> > strms; // Streams & priorities
        std::vector<std::ofstream*> file_strms;             // Open file streams
        int priolvl = 0;                                    // Current priority
        
        // Signature for std stream manipulators such as endl, setw, setfill ...
        using iomanip_func = std::ostream& (*)(std::ostream&);
        
    public:
        
        struct priority_t
        {
            int level;
            priority_t(int level) : level(level) {}
        };
        
        logstreamer_t() = default;
        
        logstreamer_t(std::ostream& strm, int priolvl)
        {
            add_ostream(strm, priolvl);
        }
        
        logstreamer_t(const std::string& file, int priolvl)
        {
            add_ofstream(file, priolvl);
        }
        
        //    logstreamer_t(std::ostream& strm, int strm_priolvl, const std::string& file, int file_priolvl)
        //    {
        //        add_ostream(strm, strm_priolvl);
        //        add_ofstream(file, file_priolvl);
        //    }
        
        void add_ostream(std::ostream& strm, int priolvl)
        {
            strms.push_back( {{&strm}, {priolvl}} );
        }
        
        void add_ofstream(const std::string& file, int priolvl)
        {
            std::ofstream *filestrm = new std::ofstream;
            filestrm->open(file, std::ios::out);
            if (filestrm->is_open())
            {
                strms.push_back( {{filestrm},{priolvl}});
                file_strms.push_back(filestrm);
            }
        }
        
        template<class T>
        logstreamer_t& operator << (const T& item)
        {
            for (auto& strm : strms)
                if (strm.second <= priolvl)
                    *(strm.first) << item;
            
            return *this;
            
        }
        
        logstreamer_t& operator << (iomanip_func iofunc)
        {            
            for (auto& strm : strms)
                if (strm.second <= priolvl)
                    *(strm.first) << iofunc;
            
            return *this;
        }
        
        logstreamer_t& operator << (const priority_t& priority)
        {
            priolvl = priority.level;
            return *this;
        }
        
        ~logstreamer_t()
        {
            for (auto* fstrm : file_strms) { fstrm->close(); delete fstrm; }
        }
    };
    
    enum { PRTVERBOSE = 0, PRTSTRICT };
    using priority = logstreamer_t::priority_t;
    
}

#if 0
#include <iomanip>

static void loggertest()
{
    using namespace logstreamer;
    
    logstreamer_t streams;
    //streams.add_ostream(std::cout, HIGH_VERBOSE);
    streams.add_ofstream("/Users/ag1498/Desktop/loggertest_verbose.txt", VERBOSE);
    streams.add_ofstream("/Users/ag1498/Desktop/loggertest_strict.txt", STRICT);
    
    int a = 5; std::string b = "<string-text>";
    streams << priority(VERBOSE);
    streams << "hello 1 " << a << ", " << b << std::endl;
    streams << "hello 2 " << a << ", " << b << std::endl;
    streams << "body" << std::setw(3) << std::setfill('0') << 1 << std::endl;
    streams << "body" << std::setw(3) << std::setfill('0') << 12 << std::endl;
    streams << "body" << std::setw(3) << std::setfill('0') << 123 << std::endl;
    streams << priority(VERBOSE) << "prio at HIGH..." << std::endl;
    streams << priority(STRICT) << "prio at STRICT..." << std::endl;
    
    // Output
    /*
     hello 1 5, string-text
     hello 2 5, string-text
     body001
     body012
     body123
     priority = 0
     prio at 0...
     priority = 1
     prio at 1...
     priority = 2
     prio at 2...
     
     */
}
#endif

#if 0
struct logstrm_t
{
    bool strm_stdout, strm_file;
    enum  { MODE_STDOUT, MODE_FILE, MODE_STDOUT_FILE };
    std::ofstream outstrm;
    
    logstrm_t(unsigned mode, const std::string& file = "")
    {
        strm_stdout = (mode != MODE_FILE);
        if (mode != MODE_STDOUT) {
            outstrm.open(file, std::ios::out);
            strm_file = outstrm.is_open();
        }
    }
    
    template<class T>
    logstrm_t& operator << (const T& data)
    {
        if (strm_stdout) std::cout << data;
        if (strm_file) strm_file << data;
    }
    
    ~logstrm_t() { if (outstrm.is_open()) outstrm.close(); }
    
} logstrm = {logstrm_t::MODE_FILE, ""};
#endif

#endif /* streamlogger_h */
