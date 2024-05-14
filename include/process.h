#ifndef PROCESS_H
#define PROCESS_H

#include <string>
/*
Basic class for Process representation
It contains relevant attributes as shown below
*/
class Process {
 public:

  int Pid();
  std::string User();
  std::string Command();
  float CpuUtilization();
  std::string Ram();
  long int UpTime();
  Process(int pid, long hertz);
  bool operator<(Process const& a) const;

  // TODO: Declare any necessary private members
 private:
  int pid_;
  long hertz_;
  float utime_ = 0.0;
  float stime_ = 0.0;
  float cutime_ = 0.0;
  float cstime_ = 0.0;
  float starttime_ = 0.0;
  std::string command_{};
};

#endif