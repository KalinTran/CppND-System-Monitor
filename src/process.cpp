#include <unistd.h>
#include <cctype>
#include <sstream>
#include <string>
#include <vector>

#include "linux_parser.h"
#include "process.h"

using std::string;
using std::to_string;
using std::vector;

Process::Process(int pid, long hertz)
{
    string line;

    std::ifstream stream(LinuxParser::kProcDirectory + to_string(pid) +  LinuxParser::kStatFilename);

    getline(stream, line);
    std::istringstream linestream(line);
    std::istream_iterator<string> beg(linestream), end;
    vector<string> statInfo(beg, end);
    pid_ = pid;
    command_ = LinuxParser::Command(pid_);
    hertz_ =  hertz;
    utime_ = stof(statInfo[13]);
    stime_ = stof(statInfo[14]);
    cutime_ = stof(statInfo[15]);
    cstime_ = stof(statInfo[16]);
    starttime_ = stof(statInfo[21]);
}

int Process::Pid() 
{
    return pid_;
}

float Process::CpuUtilization()
{
    // float upTime = (float)(LinuxParser::UpTime());
    // float upTimePid = (float)(LinuxParser::UpTime(pid_));
    // float actJif = (float)(LinuxParser::ActiveJiffies());
    // float sec = (upTime - upTimePid) / (float)(sysconf(_SC_CLK_TCK));
    
    // return (actJif / (float)(sysconf(_SC_CLK_TCK) / sec));
    long uptime = LinuxParser::UpTime();
    float total_time = utime_ + stime_ + cutime_ + cstime_;

    float seconds = uptime - (starttime_ / hertz_);
    float cpu_usage = (total_time / hertz_) / seconds;

    return cpu_usage;

}

string Process::Command()
{
    return command_;
}

string Process::Ram() 
{
    return LinuxParser::Ram(this->pid_);
}

string Process::User() { return LinuxParser::User(this->pid_); }

long int Process::UpTime() { return LinuxParser::UpTime(this->pid_); }

bool Process::operator<(Process const& a) const 
{ 
    long ram = std::stol(LinuxParser::Ram(pid_));
    long othram = std::stol(LinuxParser::Ram(a.pid_));
    return ram < othram;
}