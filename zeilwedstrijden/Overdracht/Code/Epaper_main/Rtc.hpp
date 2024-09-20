#include <M5EPD.h>
#include <RTClib.h>

class Rtc{
  private:
    rtc_date_t RTCdate;
    rtc_time_t RTCtime;
    
  public:
    rtc_date_t getDate();                                       // Returns object with current date.
    rtc_time_t getTime();                                       // Returns object with current time.
    void setDate(int day, int month, int year);                 // Set RTC Date.
    void setTime(int hour, int minute, int second);             // Set RTC Time.
    int compareTime(int hour, int minute, int second);          // Returns the amount of seconds between the current time and a given time. If > 0: The current time is ahead of the given time.
    int compareTime(rtc_time_t given, rtc_time_t compare);      // Returns the amount of seconds between the current time and a given time.
    String toStringDate(rtc_date_t date);                       // Returns the date in a string format.
    String toStringTime(rtc_time_t time);                       // Returns the time in a string format.
    String toString(rtc_date_t date, rtc_time_t time);          // Returns the date and time in a string format.
    rtc_time_t Time(int hour, int minute, int second);          // Returns object with the given time.
    rtc_time_t addTime(rtc_time_t base, rtc_time_t add);        // Returns object with time given by the integer.
};
