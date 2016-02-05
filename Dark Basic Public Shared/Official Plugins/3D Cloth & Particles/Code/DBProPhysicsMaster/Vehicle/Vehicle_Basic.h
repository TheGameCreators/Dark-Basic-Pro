#ifndef DBPROPHYSICSMASTER_VEHICLE_BASIC_H
#define DBPROPHYSICSMASTER_VEHICLE_BASIC_H


class Vehicle_Basic : public Vehicle
{
public:
	Vehicle_Basic(int id) : Vehicle(id){}

	virtual int childClassID(){return classID;}
	//static const int classID='VBSC';
	static int classID;
};

VEHICLE_PLUGIN_INTERFACE(Vehicle_Basic);

#endif