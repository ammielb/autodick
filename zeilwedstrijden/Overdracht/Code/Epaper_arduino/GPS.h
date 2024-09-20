#include <TinyGPS++.h>
#include <SoftwareSerial.h>

struct timeStruct {
  byte Year = 2021;
  byte Month = 12;
  byte Day = 3;
  byte Hour = 11;
  byte Minute = 41;
  byte Second = 50;
  byte DayOfWeek = 5;
};


class GPS {
  public:
	  GPS(TinyGPSPlus &GPS, SoftwareSerial &ss);
    int updateGPS();
    void getTime(timeStruct &t);
    void setTime(timeStruct &t);
    int getGPSQuality();
    int getDayOfWeek(int yr, int mnth, int dy);
  private:
    struct timeStruct t;
    TinyGPSPlus gps;
    SoftwareSerial *ss;
};
