#include <dirent.h>
#include <unistd.h>
#include <string>
#include <vector>
#include <iostream>
#include <iomanip>

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
  string os, kernel, junk;
  string line;
  std::ifstream stream(kProcDirectory + kVersionFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> os >> junk >> kernel;
  }
  return kernel;
}

// NO BONUS: Didn't update this to use std::filesystem
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

// DONE: Read and return the system memory utilization
float LinuxParser::MemoryUtilization() 
{ 
  int v = 0;
  string line;
  string key;
  int memtotal = 0;
  int memfree = 0;
  float Mem_Percentage = 0.0;
  std::ifstream filestream(kProcDirectory+kMeminfoFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      while (linestream >> key >> v) {
        if (key == "MemTotal:") {
          memtotal = v;
        }
        if (key == "MemFree:") {
          memfree = v;
          break;
        }
        v = 0;
      }
    }
  }

  int memused = memtotal - memfree;
  if ( memtotal > 0 )
  {
    Mem_Percentage = ((float)memused)/memtotal;
  }
  else
  {
    Mem_Percentage = 0.0;
  }
  return Mem_Percentage; 
}

// DONE: Read and return the system uptime
long LinuxParser::UpTime() 
{ 
  std::ifstream file;
  double doubleuptime = 0.0;
  long longuptime = 0;
  file.open("/proc/uptime");
  file >> doubleuptime;
  file.close();
  longuptime = (long) doubleuptime;
  return longuptime;
}

// I think I did this elsewhere: Read and return the number of jiffies for the system
// long LinuxParser::Jiffies() { return 0; }

// I think I did this elsewhere: Read and return the number of active jiffies for a PID
// long LinuxParser::ActiveJiffies(int pid[[maybe_unused]]) { return 0; }

// I think I did this elsewhere: Read and return the number of active jiffies for the system
// long LinuxParser::ActiveJiffies() { return 0; }

// I think I did this elsewhere: Read and return the number of idle jiffies for the system
// long LinuxParser::IdleJiffies() { return 0; }

// DONE: Read and return CPU utilization
float LinuxParser::CpuUtilization()
{ 
  float result = 0.0;
  int v[8] {0,0,0,0,0,0,0,0};
  int user = 0;
  int nice = 0;
  int system = 0;
  int idle = 0;
  int iowait = 0;
  int irq = 0;
  int softirq = 0;
  int steal = 0;
  static int prevuser = 0;
  static int prevnice = 0;
  static int prevsystem = 0;
  static int previdle = 0;
  static int previowait = 0;
  static int previrq = 0;
  static int prevsoftirq = 0;
  static int prevsteal = 0;
  string line;
  string key;

  std::ifstream filestream(kProcDirectory+kStatFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      while (linestream >> key >> v[0] >> v[1] >> v[2] >> v[3] >> v[4] >> v[5] >> v[6] >> v[7]) {
        if (key == "cpu") {
          user = v[0];
          nice = v[1];
          system = v[2];
          idle = v[3];
          iowait = v[4];
          irq = v[5];
          softirq = v[6];
          steal = v[7];
          break;
        }
      }
    }
  }

  int PrevIdle = previdle + previowait;
  int Idle = idle + iowait;
  int PrevNonIdle = prevuser + prevnice + prevsystem + previrq + prevsoftirq + prevsteal;
  int NonIdle = user + nice + system + irq + softirq + steal;
  int PrevTotal = PrevIdle + PrevNonIdle;
  int Total = Idle + NonIdle;
  // differentiate: actual value minus the previous one
  int totald = Total - PrevTotal;
  int idled = Idle - PrevIdle;
  float CPU_Percentage = ((float)(totald - idled))/totald;
  if ( totald>1.0 || totald<0.0 )
  {
    prevuser = user;
    prevnice = nice;
    prevsystem = system;
    previdle = idle;
    previowait = iowait;
    previrq = irq;
    prevsoftirq = softirq;
    prevsteal = steal;
  }
  result = CPU_Percentage;
  return result; 
}

// DONE: Read and return the total number of processes
int LinuxParser::TotalProcesses() 
{ 
  string line;
  string key;
  int value = 0;
  int tempvalue = 0;
  std::ifstream filestream(kProcDirectory+kStatFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      while (linestream >> key >> tempvalue) {
        if (key == "processes") {
          value = tempvalue;
          break;
        }
      }
    }
  }
  return value; 
}

// DONE: Read and return the number of running processes
int LinuxParser::RunningProcesses() 
{
  string line;
  string key;
  int value = 0;
  int tempvalue = 0;
  std::ifstream filestream(kProcDirectory+kStatFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      while (linestream >> key >> tempvalue) {
        if (key == "procs_running") {
          value = tempvalue;
          break;
        }
      }
    }
  }
  return value; 
}

// DONE: Read and return the command associated with a process
string LinuxParser::Command(int pid) 
{
  string line;
  std::ostringstream filename;
  filename << kProcDirectory << pid << kCmdlineFilename;
  std::ifstream filestream(filename.str());
  if (filestream.is_open()) {
    std::getline(filestream, line);
  }
  return line;
}

// DONE: Read and return the memory used by a process
string LinuxParser::Ram(int pid) 
{
  string line;
  string key;
  int v;
  int memmb;
  std::ostringstream filename;
  filename << kProcDirectory << pid << kStatusFilename;
  std::ifstream filestream(filename.str());
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      while (linestream >> key >> v) {
        if (key == "VmSize:") {
          memmb = v/1024;
          break;
        }
      }
    }
  }
  std::ostringstream result;
  result << memmb;
  return result.str(); 
}

// TODO: Read and return the user ID associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Uid(int pid) 
{ 
  string line;
  string key;
  string v;
  string uid;
  std::ostringstream filename;
  filename << kProcDirectory << pid << kStatusFilename;
  std::ifstream filestream(filename.str());
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      while (linestream >> key >> v) {
        if (key == "Uid:") {
          //std::cout << key << "  " << v << "\n";
          uid = v;
          break;
        }
      }
    }
  }
  return uid; 
}

// TODO: Read and return the user associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::User(int pid) 
{ 
  string line;
  string uid = Uid(pid); 
  string username;
  std::ifstream filestream(kPasswordPath);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      size_t pos1 = line.find(":");
      if ( pos1==string::npos )
      {
        continue;
      }
      size_t pos2 = line.find(":",pos1+1);
      if ( pos2==string::npos )
      {
        continue;
      }
      size_t pos3 = line.find(":",pos2+1);
      if ( pos3==string::npos )
      {
        continue;
      }
      string thisusername = line.substr(0,pos1);
      string thisuid = line.substr(pos2+1,pos3-pos2-1);
      //std::cout << "a" << uid << "a  b" << thisuid << "b\n";

      if (thisuid==uid)
      {
        username = thisusername;
        break;
      }
    }
  }
  return username; 
}

// TODO: Read and return the uptime of a process
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::ProcessUpTime(int pid) 
{  
  string v[22];
  int starttime = 0;
  string line;
  std::ostringstream filename;
  filename << kProcDirectory << pid << kStatFilename;
  std::ifstream filestream(filename.str());
  if (filestream.is_open()) {
    if (std::getline(filestream, line)) {
      //std::cout << line << "\n";
      std::istringstream linestream(line);
      linestream >> v[1] >> v[2] >> v[3] >> v[4] >> v[5] >> v[6] >> v[7] >> v[8] >> v[9] >> v[10] 
                 >> v[11] >> v[12] >> v[13] >> v[14] >> v[15] >> v[16] >> v[17] >> v[18] >> v[19] >> v[20] 
                 >> v[21]
                 >> starttime;
    }
  }
  return starttime/HERTZ;
}

float LinuxParser::ProcessCpuUtilization(int pid)
{
  string line;
  int vv;
  int uptime = 0;
  
  std::ifstream filestream1(kProcDirectory + kUptimeFilename);
  if (filestream1.is_open()) {
    if (std::getline(filestream1, line)) {
      std::istringstream linestream(line);
      if (linestream >> vv)
      {
        uptime = vv;
      }
    }
  }

  string v[22];
  int utime = 0;
  int stime = 0;
  int cutime = 0;
  int cstime = 0;
  int starttime = 0;
  std::ostringstream filename;
  filename << kProcDirectory << pid << kStatFilename;
  std::ifstream filestream2(filename.str());
  if (filestream2.is_open()) {
    if (std::getline(filestream2, line)) {
      std::istringstream linestream(line);
      linestream >> v[1] >> v[2] >> v[3] >> v[4] >> v[5] >> v[6] >> v[7] >> v[8] >> v[9] >> v[10] >> v[11] >> v[12] >> v[13] 
                 >> utime >> stime >> cutime >> cstime 
                 >> v[18] >> v[19] >> v[20] >> v[21]
                 >> starttime;
    }
  }

  // First we determine the total time spent for the process:
  int total_time = utime + stime;

  // We also have to decide whether we want to include the time from children processes. If we do, then we add those values to total_time:
  total_time = total_time + cutime + cstime;

  // Next we get the total elapsed time in seconds since the process started:
  int seconds = uptime - (starttime / (float)HERTZ);

  // Finally we calculate the CPU usage percentage:
  float cpu_usage = ((total_time / (float)HERTZ) / seconds);

  return cpu_usage;

}