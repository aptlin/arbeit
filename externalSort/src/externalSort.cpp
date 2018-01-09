#include <algorithm>
#include <cstdio>
#include <fstream>
#include <iostream>
#include <set>
#include <string>
#include <sys/stat.h>

// * Variables
std::size_t LINE_SIZE = 1024;
std::size_t TMP_NUM = 0;  // the number of tmp files created
std::string FILES_DIR = "files";
std::string INPUT_FILE = FILES_DIR + "/input.txt";
std::string MEMORY_FILE = FILES_DIR + "/memory.txt";
std::string OUTPUT_FILE = FILES_DIR + "/output.txt";
std::string TMP_DIR = "tmp";
std::string TMP_PREFIX = TMP_DIR + "/tmp_";

// * Helpers

std::size_t filesize(const std::string& filename) {
  struct stat buffer;
  if (stat(filename.c_str(), &buffer) != 0) {
    return 0;
  }
  return buffer.st_size;
}

std::size_t readMemoryLimit(const std::string& filename) {
  std::ifstream input;
  input.open(filename);

  if (!input) {
    std::cerr << "Failed to open the file, aborting.\n";
    exit(1);
  }
  std::size_t limit;
  input >> limit;
  input.close();
  return limit;
}

void sliceIntoRuns(const std::string &input, const std::size_t &memoryLimit,
                   const std::string &prefix = TMP_PREFIX) {
  std::ifstream in;
  in.open(input);

  std::size_t inputSize = filesize(input);
  long long currentSize = inputSize;
  while (currentSize > 0)
  {
    std::multiset<std::string> buffer;
    std::size_t bufferSize = 0;
    std::string line;
    while (bufferSize < memoryLimit * 1024) {
      std::getline(in, line);
      if ( in.eof() )
      {
        break;
      }
      buffer.insert(line);
      bufferSize += LINE_SIZE;
    }
    std::ofstream out(prefix + std::to_string(TMP_NUM));
    for (auto line : buffer) {
      out << line << "\n";
    }

    out.close();
    currentSize -= filesize(prefix + std::to_string(TMP_NUM));
    ++TMP_NUM;
  }
  in.close();
}

void mergeFiles(const std::string &firstFilename,
                const std::string &secondFilename,
                const std::string &outputFilename) {
  std::ifstream firstFile(firstFilename);
  std::ifstream secondFile(secondFilename);
  std::ofstream outFile(outputFilename);

  std::string lineFromFirst;
  std::string lineFromSecond;

  if (firstFile) {
    std::getline(firstFile, lineFromFirst);
  }

  if (secondFile) {
    std::getline(secondFile, lineFromSecond);
  }

  bool areBothOpen = true;

  while (firstFile || secondFile) {
    if (firstFile && secondFile) {
      if (lineFromSecond <= lineFromFirst) {
        outFile << lineFromSecond << "\n";
        std::getline(secondFile, lineFromSecond);
      } else {
        outFile << lineFromFirst << "\n";
        std::getline(firstFile, lineFromFirst);
      }
    } else {
      std::string line;
      if (areBothOpen) {
        if (!firstFile) {
          outFile << lineFromSecond << "\n";
        } else if (!secondFile){
          outFile << lineFromFirst << "\n";
        }
      } else {
        if (firstFile) {
          std::getline(firstFile, line);
        } else if (secondFile) {
          std::getline(secondFile, line);
        }
        if (firstFile || secondFile)
        {
          outFile << line << "\n";
        }
      }
      areBothOpen = false;
    }
  }
  firstFile.close();
  secondFile.close();
  outFile.close();
}

void mergeRun(const std::size_t& firstFileIdx, const std::size_t& secondFileIdx, const std::string& prefix) {
  mergeFiles(prefix + std::to_string(firstFileIdx),
             prefix + std::to_string(secondFileIdx),
             prefix + "run_" + std::to_string(firstFileIdx));
  std::rename((prefix + "run_" + std::to_string(firstFileIdx)).c_str(),
              (prefix + std::to_string(firstFileIdx)).c_str());
}

void mergeRuns(const std::string& output, const std::string& prefix=TMP_PREFIX) {
  while (TMP_NUM > 1) {
    if (TMP_NUM & 1) {
      mergeRun(TMP_NUM - 2, TMP_NUM - 1, prefix);
      --TMP_NUM;
    }
    for (std::size_t fileIdx = 0; fileIdx <= (TMP_NUM - 1) / 2; ++fileIdx) {
      mergeRun(fileIdx, fileIdx + TMP_NUM / 2, prefix);
    }
    TMP_NUM /= 2;
  }
    std::rename(( prefix + "0" ).c_str(), output.c_str());
}

void externalSort(std::string input, std::string output, std::size_t memoryLimit){
  sliceIntoRuns(input, memoryLimit);
  mergeRuns(output);
}

// * Driver Code
int main() {
  std::size_t memoryLimit = readMemoryLimit(MEMORY_FILE);
  if (memoryLimit < 1) {
    std::cerr << "Low memory, aborting.\n";
    exit(1);
  }
  const int dir_err = mkdir(TMP_DIR.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
  // if (-1 == dir_err) {
  //     std::cerr << "Cannot create a directory for temporary files, aborting.\n";
  //     exit(1);
  //   }
  externalSort(INPUT_FILE, OUTPUT_FILE, memoryLimit);

  return 0;
}
