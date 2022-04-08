#ifndef ROOM_H
#define ROOM_H

#include <iostream>

using namespace std;

class Room{
	private:
	double service_time = 0;
	double arrival_time = 0;
	double leave_Q_time = 0;


	public:
	// constructor and destructor
	Room(){}
	Room(double Service_time){
		service_time = Service_time;
	}
	~Room(){}


	// getter
	double get_service_time() const{
		return service_time;
	}

	double get_arrival_time() const{
		return arrival_time;
	}

	double get_leave_Q_time() const{
		return leave_Q_time;
	}

	// setter
	void set_Q(double time){
		arrival_time = time;
	}

	void set_Q_leave(double time){
		leave_Q_time = time;
	}

	// other method
	void report_response_time(double &cumulative_response_Room){
		double waiting_time = leave_Q_time-arrival_time;

		if (waiting_time <0){
			cout << "Room: waiting_time < 0\n";
		}
		cumulative_response_Room += waiting_time + service_time;
	}

};


#endif
