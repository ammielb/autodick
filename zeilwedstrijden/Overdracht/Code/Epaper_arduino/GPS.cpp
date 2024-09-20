#include "GPS.h"

GPS::GPS(TinyGPSPlus &gps, SoftwareSerial &ss) {
	this->gps = gps;
	this->ss = &ss;
	(*this->ss).begin(9600);
}

int GPS::updateGPS(){
	while ((*ss).available() > 0){
		gps.encode((*ss).read());
	}

	return 0;
}

void GPS::getTime(timeStruct &t) {
	t.Year = gps.date.year();
	t.Month = gps.date.month();
	t.Day = gps.date.day();
	t.Hour = gps.time.hour() + 2; // dont look O.o
	t.Minute = gps.time.minute();
	t.Second = gps.time.second();
	t.DayOfWeek = getDayOfWeek(t.Year, t.Month, t.Day);
}

int GPS::getDayOfWeek(int yr, int mnth, int dy) {
	int m = mnth;
	int d = dy;
	int yy;
	int yyyy = yr;
	int c;
	int mTable;
	int SummedDate;
	int DoW;
	int leap;
	int cTable;

	// Leap Year Calculation
	if((fmod(yyyy,4) == 0 && fmod(yyyy,100) != 0) || (fmod(yyyy,400) == 0))
	{ leap = 1; }
	else 
	{ leap = 0; }

	// Limit results to year 1900-2299 (to save memory)
	while(yyyy > 2299)
	{ yyyy = yyyy - 400; }
	while(yyyy < 1900)
	{ yyyy = yyyy + 400; }

	// Calculating century
	c = yyyy/100;

	// Calculating two digit year
	yy = fmod(yyyy, 100);

	// Century value based on Table
	if(c == 19) { cTable = 1; }
	if(c == 20) { cTable = 0; }
	if(c == 21) { cTable = 5; }
	if(c == 22) { cTable = 3; }

	// Jan and Feb calculations affected by leap years
	if(m == 1)
	{ if(leap == 1) { mTable = 6; }
	else          { mTable = 0; }}
	if(m == 2)
	{ if(leap == 1) { mTable = 2; }
	else          { mTable = 3; }}
	// Other months not affected and have set values
	if(m == 10) { mTable = 0; }
	if(m == 8) { mTable = 2; }
	if(m == 3 || m == 11) { mTable = 3; }
	if(m == 4 || m == 7) { mTable = 6; }
	if(m == 5) { mTable = 1; }
	if(m == 6) { mTable = 4; }
	if(m == 9 || m == 12) { mTable = 5; }

	// Enter the data into the formula
	SummedDate = d + mTable + yy + (yy/4) + cTable;

	// Find remainder
	DoW = fmod(SummedDate,7);  

	return DoW;
}
