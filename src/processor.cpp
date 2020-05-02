#include "processor.h"
#include "linux_parser.h"
#include <thread>
#include <unistd.h>

// DONE: Return the aggregate CPU utilization
float Processor::Utilization() 
{ 
  float uptime =  LinuxParser::UpTime();
  float idletime = ((float)LinuxParser::IdleJiffies())/sysconf(_SC_CLK_TCK);
  int number_of_machine_cores = std::thread::hardware_concurrency();
  float utilisation = 
    (number_of_machine_cores * uptime - idletime)
    /(number_of_machine_cores * uptime);
  return utilisation;
}