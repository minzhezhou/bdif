#pragma once
#include <string> 
#include <cstring> 
#include <vector>
//#include "boost/lexical_cast.hpp"
//#include "boost/noncopyable.hpp"
#include <mutex>
#include <thread>
#include <cstdio>
#include <cmath>
#include <memory>
#include <cstdlib>
#include <exception>
#include <vector>
#include <list>
#include <deque>
#include <memory>
#include <fstream>
#include <iostream>
#include <map>
#include <unordered_map>
#include <ctime>
#include <functional>

using std::shared_ptr;
using std::weak_ptr;
using std::unique_ptr;

class ScopeExit {
public:
    explicit ScopeExit(std::function<void ()> f) : _f(std::move(f)) {
    }
    ~ScopeExit() {
        _f();
    }
private:
    std::function<void ()> _f;
};

// Get time in seconds.
inline double current_realtime() {
    struct timespec tp;
    clock_gettime(CLOCK_REALTIME, &tp);
    return tp.tv_sec + tp.tv_nsec * 1e-9;
}

// Call func(args...). If interrupted by signal, recall the function.
template<class FUNC, class... ARGS>
auto ignore_signal_call(FUNC&& func, ARGS&&... args) -> typename std::result_of<FUNC(ARGS...)>::type {
    for (;;) {
        auto err = func(args...);
        if (err < 0 && errno == EINTR) {
            //LOG(INFO) << "Signal is caught. Ignored.";
            std::cout << "Signal is caught. Ignored.";
            continue;
        }
        return err;
    }
}

inline std::mutex& global_fork_mutex() {
    static std::mutex mutex;
    return mutex;
}

// popen and pclose are not thread-safe
inline FILE* guarded_popen(const char* command, const char* type) {
    std::lock_guard<std::mutex> lock(global_fork_mutex());
    return popen(command, type);
}

inline int guarded_pclose(FILE* stream) {
    std::lock_guard<std::mutex> lock(global_fork_mutex());
    return pclose(stream);
}


template<class... ARGS>
void format_string_append(std::string& str, const char* fmt, ARGS&&... args) { // use VA_ARGS may be better ?
    int len = snprintf(NULL, 0, fmt, args...);
    if (!(len >= 0)) {
        std::cout << "error len = 0\n";
        return;
        //exit(0);
    }
    size_t oldlen = str.length();
    str.resize(oldlen + len + 1);
    //CHECK(snprintf(&str[oldlen], (size_t)len + 1, fmt, args...) == len);
    (snprintf(&str[oldlen], (size_t)len + 1, fmt, args...) == len);
    str.resize(oldlen + len);
}

template<class... ARGS>
void format_string_append(std::string& str, const std::string& fmt, ARGS&&... args) {
    format_string_append(str, fmt.c_str(), args...);
}

template<class... ARGS>
std::string format_string(const char* fmt, ARGS&&... args) {
    std::string str;
    format_string_append(str, fmt, args...);
    return std::move(str);
}

template<class... ARGS>
std::string format_string(const std::string& fmt, ARGS&&... args) {
    return format_string(fmt.c_str(), args...);
}

template<class T = std::string>
std::vector<T> split_string(const std::string& str, char delim) {
	size_t num = 1;
	const char* p;
	for (p = str.c_str(); *p != 0; p++) {
		if (*p == delim) {
			num++;
		}
	}
	std::vector<T> list(num);
	const char* last = str.c_str();
	num = 0;
	for (p = str.c_str(); *p != 0; p++) {
		if (*p == delim) {
			list[num++] = T(last, p - last);
			//list[num++] = boost::lexical_cast<T>(last, p - last);
			last = p + 1;
		}
	}
	list[num++] = T(last, p - last);
	//list[num++] = boost::lexical_cast<T>(last, p - last);
	return list;
}

template<class T = std::string>
std::vector<T> split_string(char* &str, char delim, int cnt) {
	size_t num = 1;
	char* p;
    char* begin = str;
    if (*str == 0) {
        return std::vector<T>();
    }
	for (p = str; *p != 0 && num < cnt; p++) {
		if (*p == delim) {
			num++;
		}
	}
    while (*p != 0 && *p != delim) {
        p++;
    }
    if (*p == delim) {
        *p = 0;
        str = p + 1;
    } else {
        str = p;
    }

	std::vector<T> list(num);
	char* last = begin;
	num = 0;
	for (p = begin; *p != 0; p++) {
		if (*p == delim) {
			list[num++] = T(last, p - last);
			//list[num++] = boost::lexical_cast<T>(last, p - last);
			last = p + 1;
		}
	}
	list[num++] = T(last, p - last);
	//list[num++] = boost::lexical_cast<T>(last, p - last);
	return list;
}

// split string by spaces. Leading and tailing spaces are ignored. Consecutive spaces are treated as one delim.
template<class T = std::string>
std::vector<T> split_string(const std::string& str) {
	size_t num = 0;
	const char* p;
	for (p = str.c_str(); *p != 0; ) {
		if (!isspace(*p)) {
			num++;
			p++;
			while (*p != 0 && !isspace(*p)) {
				p++;
			}
		}
		else {
			p++;
		}
	}
	std::vector<T> list(num);
	num = 0;
	for (p = str.c_str(); *p != 0; ) {
		if (!isspace(*p)) {
			const char* last = p;
			p++;
			while (*p != 0 && !isspace(*p)) {
				p++;
			}
			//list[num++] = boost::lexical_cast<T>(last, p - last);
			list[num++] = T(last, p - last);
		}
		else {
			p++;
		}
	}
	return std::move(list);
}
/*
template<class T>
std::string join_strings(const std::vector<T>& strs, char delim) {
	std::string str;
	for (size_t i = 0; i < strs.size(); i++) {
		if (i > 0) {
			str += delim;
		}
		//str += boost::lexical_cast<std::string>(strs[i]);
	    str += strs[i];
    }
	return std::move(str);
}
*/

class LineFileReader{
public:
    ~LineFileReader() {
        ::free(_buffer);
    }
    char* getline(FILE* f) {
        return this->getdelim(f, '\n');
    }
    char* getdelim(FILE* f, char delim) {
        ssize_t ret = ::getdelim(&_buffer, &_buf_size, delim, f);
        if (ret >= 0) {
            if (ret >= 1 && _buffer[ret - 1] == delim) {
                _buffer[--ret] = 0;
            }
            _length = (size_t)ret;
            return _buffer;
        } else {
            _length = 0;
            (feof(f));
            return NULL;
        }
    }
    char* get() {
        return _buffer;
    }
    size_t length() {
        return _length;
    }
private:
    char* _buffer = NULL;
    size_t _buf_size = 0;
    size_t _length = 0;
};

template <typename T>
std::ostream& operator<<(std::ostream& ostr, const std::vector<T>& list)
{
	for (auto &i : list) {
		ostr << i << ",";
	}
	return ostr;
}

template<typename T1, typename T2>
std::ostream& operator<<(std::ostream& ostr, const std::pair<T1, T2>& p) {
	ostr << "[" << p.first << "," << p.second << "]";
	return ostr;
}

template<typename T1, typename T2>
std::ostream& operator<<(std::ostream& ostr, const std::map<T1, T2>& m) {
	for (auto &p : m) {
	    ostr << "[" << p.first << "->" << p.second << "], ";
    }
    return ostr;
}

template <typename T>
std::ostream& operator<<(std::ostream& ostr, const std::list<T>& llist)
{
	for (auto &i : llist) {
		ostr << " " << i;
	}
	return ostr;
}

template <typename T>
std::ostream& operator<<(std::ostream& ostr, const std::deque<T>& dq)
{
	for (auto &i : dq) {
		ostr << " " << i;
	}
	return ostr;
}

