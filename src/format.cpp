#include <string>
#include <cstdio>
#include "format.h"

using std::string;

// DONE: Complete this helper function
// INPUT: Long int measuring seconds
// OUTPUT: HH:MM:SS
string Format::ElapsedTime(long seconds) 
{
    char tempresult[9]; // Allows for a six digit hour
    string result;
    long timeleft = seconds;
    timeleft =  timeleft > (360000-1) ? 360000-1 : timeleft;
    int hours = timeleft/60/60;
    timeleft = timeleft-hours*60*60;
    int minutes = timeleft/60;
    timeleft = timeleft-minutes*60;
    int justseconds = timeleft;
    sprintf(tempresult,"%.2d:%.2d:%.2d",hours,minutes,justseconds);
    result = tempresult;
    return result;
}