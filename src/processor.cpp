#include "linux_parser.h"
#include <unistd.h>
#include "processor.h"

static float prevActJif = (float)LinuxParser::ActiveJiffies();
static float prevJif = (float)LinuxParser::Jiffies();

float Processor::Utilization() 
{
  jiffies = (float)LinuxParser::Jiffies();
  activeJiffies = (float)LinuxParser::ActiveJiffies();

  float JifDiff = jiffies - prevJif;
  float actJifDiff = activeJiffies - prevActJif;

  prevJif = jiffies;
  prevActJif = activeJiffies;

  if (JifDiff == 0.0f)
  {
    return 0.0f;
  }

  return (actJifDiff / JifDiff);
}