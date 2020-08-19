#pragma once
/// Glob files in the directory - used to bump version of the file to the next one.
///https://stackoverflow.com/questions/8401777/simple-glob-in-c-on-unix-system
#include <glob.h> // glob(), globfree()
#include <string.h> // memset()

namespace vpl{

std::vector<std::string> glob(const std::string& pattern) {

    using namespace std;

    // glob struct resides on the stack
    glob_t glob_result;
    memset(&glob_result, 0, sizeof(glob_result));

    // do the glob operation
    int return_value = glob(pattern.c_str(), GLOB_TILDE, NULL, &glob_result);

    if(return_value != 0) {
        globfree(&glob_result);

        if(return_value == GLOB_NOMATCH) {
            return std::vector<std::string>();
        }

        stringstream ss;
        ss << "glob() failed with return_value " << return_value << endl;
        throw std::runtime_error(ss.str());
    }

    // collect all the filenames into a std::list<std::string>
    vector<string> filenames;
    for(size_t i = 0; i < glob_result.gl_pathc; ++i) {
        filenames.push_back(string(glob_result.gl_pathv[i]));
    }

    // cleanup
    globfree(&glob_result);

    // done
    return filenames;
}

/// Command line arguments parsing helper functions
std::vector<std::string>::iterator getCmdOption(std::vector<std::string>& args, const std::string & option) {
    return std::find(args.begin(), args.end(), option);
}

bool cmdOptionExists(const std::vector<std::string>& args, const std::string& option){
    return (std::find(args.begin(), args.end(), option) != args.end());
}
} // end namepace vpl
