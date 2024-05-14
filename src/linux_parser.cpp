#include <dirent.h>
#include <unistd.h>
#include <iomanip>
#include <string>
#include <vector>

#include "linux_parser.h"

using std::stof;
using std::string;
using std::to_string;
using std::vector;

// DONE: An example of how to read data from the filesystem
string LinuxParser::OperatingSystem() {
  string line;
  string key;
  string value;
  std::ifstream filestream(kOSPath);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ' ', '_');
      std::replace(line.begin(), line.end(), '=', ' ');
      std::replace(line.begin(), line.end(), '"', ' ');
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "PRETTY_NAME") {
          std::replace(value.begin(), value.end(), '_', ' ');
          return value;
        }
      }
    }
  }
  return value;
}

// DONE: An example of how to read data from the filesystem
string LinuxParser::Kernel() {
  string os, kernel, version;
  string line;
  std::ifstream stream(kProcDirectory + kVersionFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> os >> version >> kernel;
  }
  return kernel;
}

// BONUS: Update this to use std::filesystem
vector<int> LinuxParser::Pids() {
  vector<int> pids;
  DIR* directory = opendir(kProcDirectory.c_str());
  struct dirent* file;
  while ((file = readdir(directory)) != nullptr) {
    // Is this a directory?
    if (file->d_type == DT_DIR) {
      // Is every character of the name a digit?
      string filename(file->d_name);
      if (std::all_of(filename.begin(), filename.end(), isdigit)) {
        int pid = stoi(filename);
        pids.push_back(pid);
      }
    }
  }
  closedir(directory);
  return pids;
}

float LinuxParser::MemoryUtilization() {
  std::string line, key;
  float total = 0.0f;
  float free = 0.0f;
  float val= 0.0f;
  std::ifstream fstream(kProcDirectory+kMeminfoFilename);

  if (fstream.is_open()) 
  {
    while (std::getline(fstream, line)) 
    {
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line);
      while (linestream >> key >> val) 
      {
        if (key == "MemTotal") 
        {
          total = val;
        }
        if (key == "MemFree") 
        {
          free = val;
          break;
        }
      }
    }
  }

  return (float)(total - free) / (float)total;
}

long LinuxParser::UpTime() 
{
  long out;
  std::string line;
  std::ifstream fStream(kProcDirectory+kUptimeFilename);
  if (fStream.is_open())
  {
    std::getline(fStream, line);
    std::istringstream linestream(line);
    linestream >> out;
  }

  return out;
}

long LinuxParser::Jiffies()
{
  long jiff{0};
  std::vector<std::string> cpuUti;
  cpuUti = LinuxParser::CpuUtilization();
  for (int i = kUser_; i <= kSteal_; ++i)
  {
    jiff =  jiff + std::stol(cpuUti[i]);
  }
  return jiff;
}

long LinuxParser::ActiveJiffies(int pid)
{
  std::string line;
  std::vector<std::string> buff;
  std::ifstream filestream(kProcDirectory+std::to_string(pid)+kStatFilename);
  long total = 0;
  if (filestream.is_open())
  {
    std::getline(filestream, line);
    std::stringstream sstream(line);
    while (std::getline(sstream, line, ' '))
    {
      buff.emplace_back(line);
    }
    total = std::stol(buff[13])+std::stol(buff[14])+std::stol(buff[15])+std::stol(buff[16]);
  }
  return total;
}

long LinuxParser::ActiveJiffies()
{
  return LinuxParser::Jiffies()-LinuxParser::IdleJiffies();
}

long LinuxParser::IdleJiffies() {
  long idle = 0;
  std::vector<std::string> cpuUti;
  cpuUti = LinuxParser::CpuUtilization();
  for (int i = kIdle_; i <= kIOwait_; i++)
  {
    idle += std::stol(cpuUti[i]);
  }
  return idle;
}

vector<string> LinuxParser::CpuUtilization()
{
  string key;
  string line;
  vector<string> cpuUti;
  string usr, nice, sys, idle, iowait, irq, sirq, steal, guest, guest_nice;
  std::ifstream filestream(kProcDirectory+kStatFilename);

  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      while (linestream >> key >> usr >> nice >> sys >> idle >> iowait 
                        >> irq >> sirq >> steal >> guest >>  guest_nice)
      {
        if (key == "cpu")
        {
          cpuUti = {usr, nice, sys, idle, iowait, irq, sirq, steal, guest, guest_nice};
          break;
        }
      }
    }
  }
  return cpuUti;
}

int LinuxParser::TotalProcesses()
{
  int val = 0;
  std::ifstream fstream(kProcDirectory + kStatFilename);
  std::string line, key;
  if (fstream.is_open())
  {
    while (std::getline(fstream, line))
    {
      std::istringstream linestream(line);
      while (linestream >> key >> val)
      {
        if (key == "processes")
        {
          return val;
        }
      }
    }
  }
  return val;
}

int LinuxParser::RunningProcesses()
{
  int val = 0;
  std::ifstream fstream(kProcDirectory + kStatFilename);
  std::string key, line;
  if (fstream.is_open())
  {
    while (std::getline(fstream, line))
    {
      std::istringstream lstream(line);
      lstream >> key >> val;
      if (key == "procs_running")
      {
        return val;
      }
    }
  }
  return val;
}

string LinuxParser::Command(int pid)
{
  std::string line;
  std::ifstream fstream(kProcDirectory+to_string(pid)+kCmdlineFilename);
  if (fstream.is_open())
  {
    std::getline(fstream, line);
    if (line == "")
    {
      return "None";
    }
    return line;
  }
  return "";
}

string LinuxParser::Ram(int pid)
{
  std::string line, val, key;
  std::ifstream filestream(kProcDirectory+std::to_string(pid) +kStatusFilename);
  if (filestream.is_open())
  {
    while (std::getline(filestream, line))
    {
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream lstream(line);
      while (lstream >> key >> val)
      {
        if (key == "VmSize")
        {
          std::stringstream ram;
          ram << std::fixed << std::setprecision(3) << stof(val) / 1000;
          return ram.str();
        }
      }
    }
  }
  return "0";
}

string LinuxParser::Uid(int pid) 
{
  std::string key,line,val;
  std::ifstream fstream(kProcDirectory+std::to_string(pid)+kStatusFilename);
  if (fstream.is_open())
  {
    while (std::getline(fstream, line))
    {
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line);
      while (linestream >> key >> val) {
        if (key == "Uid") {
          return val;
        }
      }
    }
  }
  return "";
}

string LinuxParser::User(int pid)
{
  std::ifstream fstream(kPasswordPath);
  std::string line, val,key, uid;
  if (fstream.is_open())
  {
    while (std::getline(fstream, line))
    {
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line);
      while (linestream >> key >> val >> uid)
      {
        if (uid == LinuxParser::Uid(pid))
        {
          return key;
          break;
        }
      }
    }
  }
  return "";
}

long LinuxParser::UpTime(int pid)
{
  std::string line, value;
  unsigned long time = 0;
  std::vector<std::string> buf;
  std::ifstream fstream(kProcDirectory + std::to_string(pid) + kStatFilename);
  if (fstream.is_open())
  {
    std::getline(fstream, line);
    std::stringstream sstream(line);
    while (std::getline(sstream, line, ' '))
    {
      buf.emplace_back(line);
    }
    if (buf.size() > 0)
    {
      time = std::stol(buf[21]) / sysconf(_SC_CLK_TCK);
      return time;
    }
  }
  return 0;
}