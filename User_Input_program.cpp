#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>
#include <stdexcept>

#ifdef __APPLE__
#include <mach-o/dyld.h>
#include <limits.h>
#endif

#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif

// Function to get the directory of the executable
std::string getExecutablePath() {
    char buffer[PATH_MAX];
#ifdef _WIN32
    GetModuleFileNameA(NULL, buffer, PATH_MAX);
#elif __APPLE__
    uint32_t size = sizeof(buffer);
    if (_NSGetExecutablePath(buffer, &size) != 0) {
        throw std::runtime_error("Buffer too small; need size " + std::to_string(size));
    }
#else
    ssize_t count = readlink("/proc/self/exe", buffer, PATH_MAX);
    if (count == -1) {
        perror("readlink");
        throw std::runtime_error("Error reading executable path");
    }
    buffer[count] = '\0'; // Null-terminate the path
#endif
    std::string path(buffer);
    size_t lastSlash = path.find_last_of("/\\");
    if (lastSlash == std::string::npos) {
        throw std::runtime_error("Unexpected path format");
    }
    return path.substr(0, lastSlash);
}

// Function to append user input to the provided file
void appendToFile(const std::string& filename) {
    std::ofstream outFile;
    outFile.open(filename, std::ios::app); // Open in append mode
    if (!outFile) {
        std::cerr << "Error opening file for appending: " << filename << std::endl;
        return;
    }
    
    std::string userInput;
    std::cout << "Enter text to append to the file: ";
    std::getline(std::cin, userInput);
    
    outFile << userInput << std::endl;
    outFile.close();
    std::cout << "User input appended to file: " << filename << std::endl;
}

// Function to reverse the content of a file and save it to a new file
void reverseFileContent(const std::string& inputFile, const std::string& outputFile) {
    std::ifstream inFile(inputFile);
    if (!inFile) {
        std::cerr << "Error opening file for reading: " << inputFile << std::endl;
        return;
    }
    
    std::string content((std::istreambuf_iterator<char>(inFile)), std::istreambuf_iterator<char>());
    inFile.close();
    
    std::reverse(content.begin(), content.end());
    
    std::ofstream outFile(outputFile);
    if (!outFile) {
        std::cerr << "Error opening file for writing: " << outputFile << std::endl;
        return;
    }
    
    outFile << content;
    outFile.close();
    std::cout << "Reversed content written to file: " << outputFile << std::endl;
}

int main() {
    // Get the executable directory
    std::string execDir;
    try {
        execDir = getExecutablePath();
    } catch (const std::exception& e) {
        std::cerr << "Error determining executable path: " << e.what() << std::endl;
        return 1;
    }

    std::cout << "Executable directory: " << execDir << std::endl;

    // Set the absolute paths for the files
    const std::string filename = execDir + "/CSC450_CT5_mod5.txt";
    const std::string reversedFilename = execDir + "/CSC450-mod5-reverse.txt";
    
    // Append user input to the file
    appendToFile(filename);
    
    // Reverse the content of the file and save to a new file
    reverseFileContent(filename, reversedFilename);
    
    std::cout << "User input appended and file content reversed successfully." << std::endl;
    
    return 0;
}
