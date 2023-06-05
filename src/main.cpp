#include <iostream>
#include <fstream>

#include "dylanclibs/log.h"
#include "dylanclibs/cli_parser.h"
#include "dylanclibs/string.h"

LogSettings LOG_SETTINGS;

int main(int argc, char* argv[]){

    //  Will leave these values false always, as they will not be required for
    //  the make, cmake or build file building the main executable of a given
    //  project binary.
    LOG_SETTINGS.ls_use_working_dir = false;
    LOG_SETTINGS.ls_print_to_file = false;
    
    try {
        LogInit(argv);
    } catch (std::runtime_error &err) {
        std::cout << err.what() << std::endl;
        exit(0);
    } catch (std::exception &e) {
        std::cout << "Program is unable to start: " << e.what() << std::endl;
        exit(0);
    }

    ParserOption headerFilePath = ParserOption(
        {"p", "header file path"},
        "The location of the version number header file.",
        true,
        "path to header");

    ParserOption versionNumber = ParserOption(
        {"n", "version number"},
        "Version number to increment.",
        {"MAJOR", "MINOR", "PATCH", "BUILD"},
        std::string("BUILD"));

    Parser parser = Parser(argc, argv, "Automatically increments version numbers "
            "in a header file for a project.");

    parser.AddHelpOption();
    parser.AddVersionOption();
    int r = parser.AddOptions({headerFilePath, versionNumber});
    if (r != 0){
        elog << "Failed adding some options to the parser: " << r;
    }

    if (!parser.Process()){
        flog << "Failed to parse args.";
    } else {
        //  Define the strings for the version number options
        const String ver_build_str = "BUILD";
        const String ver_patch_str = "PATCH";
        const String ver_minor_str = "MINOR";
        const String ver_major_str = "MAJOR";

        //  Get the values from the parser options
        String filepath = parser.GetValue(headerFilePath);
        String verNum = parser.GetValue(versionNumber);

        //  Open and read the file, or create a new one
        std::ifstream ifs;
        ifs.open(filepath.c_str());

        //  Define some local variables to store the current version numbers
        int major = 0;
        int minor = 0;
        int patch = 0;
        int build = 0;

        if (!ifs){
            log << "File does not exist, creating file...";
            std::ofstream ofs;
            ofs.open(filepath.c_str());
            ofs << "#define MAJOR_N 0\n"
                << "#define MINOR_N 0\n"
                << "#define PATCH_N 0\n"
                << "#define BUILD_N 0\n"
                << std::endl;
            ofs.close();
        } else {
            log << "File exists, reading data...";

            std::string output;
            String totalContent;
            while (std::getline(ifs, output)){
                totalContent += output + "\n";
            }

            std::vector<String> splitContent = totalContent.split('\n');
            for (String str : splitContent){
                std::vector<String> lineContent = str.split(' ');
                String value = lineContent[lineContent.size() - 1];

                if (str.contains(ver_build_str.c_str())){
                    build = std::stoi(value);
                } else if (str.contains(ver_patch_str.c_str())){
                    patch = std::stoi(value);
                } else if (str.contains(ver_minor_str.c_str())){
                    minor = std::stoi(value);
                } else if (str.contains(ver_major_str.c_str())){
                    major = std::stoi(value);
                }
            }
        }

        ifs.close();

        if (strcmp(verNum, ver_build_str.c_str()) == 0){
            log << "Build Number Identified.";
        } else if (strcmp(verNum, ver_patch_str.c_str()) == 0){
            log << "Patch Number Identified.";
        } else if (strcmp(verNum, ver_minor_str.c_str()) == 0){
            log << "Minor Number Identified.";
        } else if (strcmp(verNum, ver_major_str.c_str()) == 0){
            log << "Major Number Identified.";
        } else {
            flog << "Error occurred identifying build number from the parser: \""
                << verNum << "\".";
        }
    }

    LogShutdown;
    return 0;
}
