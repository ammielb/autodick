#include "Rtc.hpp"

rtc_date_t Rtc::getDate(){
  M5.RTC.getDate(&RTCdate);
  return(RTCdate);
};

rtc_time_t Rtc::getTime(){
  M5.RTC.getTime(&RTCtime);
  return(RTCtime);
};

void Rtc::setDate(int day, int month, int year){
  RTCdate.year = year;
  RTCdate.mon = month;
  RTCdate.day = day;
  M5.RTC.setDate(&RTCdate);
};

void Rtc::setTime(int hour, int minute, int second){
  RTCtime.hour = hour;
  RTCtime.min = minute;
  RTCtime.sec = second;
  M5.RTC.setTime(&RTCtime);
};

int Rtc::compareTime(int hour, int minute, int second){
  int givenTime = (DateTime(RTCdate.year, RTCdate.mon, RTCdate.day, hour, minute, second)).unixtime();
  M5.RTC.getTime(&RTCtime);
  int timeNow = (DateTime(RTCdate.year, RTCdate.mon, RTCdate.day, RTCtime.hour, RTCtime.min, RTCtime.sec)).unixtime();
  return(timeNow - givenTime);
};

int Rtc::compareTime(rtc_time_t given, rtc_time_t compare) {
  int givenTime = (DateTime(RTCdate.year, RTCdate.mon, RTCdate.day, compare.hour, compare.min, compare.sec)).unixtime();
  M5.RTC.getTime(&RTCtime);
  int timeNow = (DateTime(RTCdate.year, RTCdate.mon, RTCdate.day, given.hour, given.min, given.sec)).unixtime();
  return(timeNow - givenTime);
}

String Rtc::toStringDate(rtc_date_t date_) {
  char dateBuffer[32];
  sprintf(dateBuffer, "%02d-%02d-%04d", date_.day, date_.mon, date_.year);
  return dateBuffer;
}

String Rtc::toStringTime(rtc_time_t time_) {
  char timeBuffer[32];
  sprintf(timeBuffer, "%02d:%02d:%02d", time_.hour, time_.min, time_.sec);
  return timeBuffer;
}

String Rtc::toString(rtc_date_t date_, rtc_time_t time_) {
  return toStringDate(date_) + " " + toStringTime(time_);
}

rtc_time_t Rtc::Time(int hour, int minute, int second) {
  rtc_time_t ret;
  ret.hour = hour;
  ret.min = minute;
  ret.sec = second;
  return ret;
}

rtc_time_t Rtc::addTime(rtc_time_t base, rtc_time_t add) {
  base.sec += add.sec;
  base.min += add.min;
  base.hour += add.hour;
  while(base.sec >= 60) {
    base.sec -= 60;
    base.min++;
  }
  while(base.min >= 60) {
    base.min -= 60;
    base.hour++;
  }
  while(base.hour >= 24) {
    base.hour -= 24;
  }
  while(base.sec < 0) {
    base.sec += 60;
    base.min--;
  }
  while(base.min < 0) {
    base.min += 60;
    base.hour--;
  }
  while(base.hour < 0) {
    base.hour += 24;
  }
  return base;
}
