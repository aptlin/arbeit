#include <cassert>
#include <cmath>
#include <fstream>
#include <iostream>
#include <iomanip>
#include <limits>
#include <string>
#include <vector>

// * Environment Variables
std::size_t SLICE_SIZE = 1000000000;
std::size_t SLICE_LENGTH = static_cast<std::size_t>(std::log10(SLICE_SIZE));
std::string FILE_DIR = "files";
std::string INPUT_FILENAME = FILE_DIR + "/input.txt";
std::string OUTPUT_FILENAME = FILE_DIR + "/output.txt";

// * Classes
template<typename T>
class HumongousNum {
 private:
  std::vector<uint32_t> slices;
  void checkNum(T num) {
    assert(num >= 0 && num < SLICE_SIZE);
  }
 public:
   explicit HumongousNum(T num) {
     checkNum(num);
     slices.push_back(num);
  }

  HumongousNum& operator*=(T rhs) {
    checkNum(rhs);
    uint32_t carry = 0;
    for (std::size_t idx = 0; idx < slices.size(); ++idx) {
      uint64_t product = static_cast<uint64_t>(slices[idx]) * static_cast<uint64_t>(rhs) + carry;
      slices[idx] = static_cast<uint32_t>(product % SLICE_SIZE);
      carry = static_cast<uint32_t>(product / SLICE_SIZE);
    }
    if (carry != 0) {
      slices.push_back(carry);
    }
    return *this;
  }

  template <typename U>
  friend std::ostream &operator<<(std::ostream &stream,
                                   const HumongousNum<U> &num);
};

template <typename T>
inline std::ostream &operator<<(std::ostream &stream,
                                const HumongousNum<T> &num) {
  char oldFillChar = stream.fill('0');
  for (auto iter = num.slices.rbegin(); iter != num.slices.rend(); ++iter) {
    stream <<  *iter << std::setw(SLICE_LENGTH);
  }
  stream.fill(oldFillChar);
  stream.width(0);
  return stream;
}

// * Helpers
template<typename T>
HumongousNum<T> factorial(T num) {
  HumongousNum<T> result(T(1));
  for (T factor = 2; factor <= num; ++factor) {
    result *= factor;
  }
  return result;
}

template <typename T>
T readNumInput(const std::string& filename) {
  std::ifstream input;
  input.open(filename);

  if (!input) {
    std::cerr << "Failed to open the file, aborting.\n";
    exit(1);
  }
  T num;
  input >> num;
  input.close();
  return num;
}

template <typename T>
void writeVar(const T& entity, const std::string& outputFilename = OUTPUT_FILENAME) {
  std::ofstream outputFile(outputFilename);
  outputFile << entity << "\n";
  outputFile.close();
}


// * Driver Code

int main()
{
  std::size_t num = readNumInput<std::size_t>(INPUT_FILENAME);
  writeVar<HumongousNum<std::size_t>>(factorial(num));
  return 0;
}
