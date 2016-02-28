#include "myutil.h"
#include <sys/signal.h>
inline bool& shell_verbose_internal() {
    static bool x = false;
    return x;
}

inline bool shell_verbose() {
    return shell_verbose_internal();
}

inline void shell_set_verbose(bool x) {
    shell_verbose_internal() = x;
}

inline shared_ptr<FILE> shell_open_file(const std::string& path, const std::string& mode, size_t buffer_size = 0) {
    if (shell_verbose()) {
        std::cout << "Opening file [ " << path << " ] with mode [ " << mode << " ]";
    }
    FILE* fp;
    //PCHECK(fp = fopen(path.c_str(), mode.c_str())) << "path=[ " << path << " ] mode=[ " << mode << " ]";
    fp = fopen(path.c_str(), mode.c_str());
    std::cout << "path=[ " << path << " ] mode=[ " << mode << " ]";
    char* buffer = NULL;
    if (buffer_size > 0) {
        buffer = new char[buffer_size];
       // CHECK(0 == setvbuf(fp, buffer, _IOFBF, buffer_size));
        setvbuf(fp, buffer, _IOFBF, buffer_size);
    }
    return shared_ptr<FILE>(fp, [path, buffer](FILE* fp) {
        if (shell_verbose()) {
        std::cout << "Closing file [ " << path << " ]";
        }
        //PCHECK(0 == fclose(fp)) << "path=[ " << path << " ]";
        fclose(fp);//) << "path=[ " << path << " ]";
        delete[] buffer;
    });
}

inline shared_ptr<FILE> shell_open_pipe(const std::string& cmd, const std::string& mode, size_t buffer_size = 0) {
    if (shell_verbose()) {
        std::cout << "Opening pipe [ " << cmd << " ] with mode [ " << mode << " ]";
    }
    FILE* fp;
    //PCHECK(fp = guarded_popen(format_string("set -o pipefail; %s", cmd.c_str()).c_str(), mode.c_str())) << "cmd=[ " << cmd << " ] mode=[ " << mode << " ]";
    (fp = guarded_popen(format_string(" %s", cmd.c_str()).c_str(), mode.c_str()));
    if (!fp) {
        std::cout << "Pipe:" << format_string(" %s", cmd.c_str()) << " error\n";
    }
    char* buffer = NULL;
    if (buffer_size > 0) {
        buffer = new char[buffer_size];
        //CHECK(0 == setvbuf(fp, buffer, _IOFBF, buffer_size));
        if (!(0 == setvbuf(fp, buffer, _IOFBF, buffer_size))) {
            std::cout << "setvbuf error\n";
            return NULL;
        }
    }
    return shared_ptr<FILE>(fp, [cmd, buffer](FILE* fp) {
        if (shell_verbose()) {
            //LOG(INFO) << "Closing pipe [ " << cmd << " ]";
            std::cout << "Closing pipe [ " << cmd << " ]";
        }
        int err = guarded_pclose(fp);
        //PCHECK(err == 0 || err == (128 + SIGPIPE) * 256 || err == -1 && errno == ECHILD) << "err=" << err << " cmd=[ " << cmd << " ]";
        if (!(err == 0 || err == (128 + SIGPIPE) * 256 || err == -1 && errno == ECHILD)) {
            std::cout << "err=" << err << " cmd=[ " << cmd << " ]";
        }
        if (errno == ECHILD) {
            //LOG(WARNING) << "errno is ECHILD";
            std::cout << "errno is ECHILD";
        }
        delete[] buffer;
    });
}

inline void shell_execute(const std::string& cmd) {
    shell_open_pipe(cmd, "w");
}

inline std::string shell_get_command_output(const std::string& cmd) {
    shared_ptr<FILE> pipe = shell_open_pipe(cmd, "r", 1000);
    LineFileReader reader;
    if (reader.getdelim(&*pipe, 0)) {
        return reader.get();
    }
    return "reading from pipe error";
}

