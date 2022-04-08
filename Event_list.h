#ifndef EVENT_LIST_H
#define EVENT_LIST_H

#include <queue>
#include <iostream>
#include <vector>
#include "patient.h"

using namespace std;

// implementation of the event list


enum Event {Arrival, Service_E, Depart_E, Arrive_P, Service_P, Departure, Room_wait, Room_clean, Room_finish, END, PRINT};
typedef enum Event Event;




// class of event node

class Event_node{
	private:
	Event E = END;
	double event_time = 0;

	public:
	// constructor and destructor
	Event_node(){}
	Event_node(Event e, double Event_time){
		E = e;
		event_time = Event_time;
	}
	~Event_node(){}

	// gettrs
	Event get_event()const{
		return E;
	}

	double get_event_time()const{
		return event_time;
	}

	// debug methoid
	void print(){
		cout<<"Event node info:\n";
		string event = "";
		switch(E){
			case Arrival:
				event = "Arrival";
				break;
			case Service_E:
				event = "Service_E";
				break;
			case Depart_E:
				event = "Depart_E";
				break;
			case Arrive_P:
				event = "Arrive_P";
				break;
			case Service_P:
				event = "Service_P";
				break;
			case Departure:
				event = "Departure";
				break;
			case Room_wait:
				event = "Room_wait";
				break;
			case Room_clean:
				event = "Room_clean";
				break;
			case Room_finish:
				event = "Room_finish";
				break;
			case END:
				event = "END";
				break;
			case PRINT:
				event = "PRINT";
				break;
			default:
				event = "NONE";
			}
		cout<< "event type: "<< event<<endl;
		cout << "event time: "<< event_time<< endl;
	}


	// operator overload
	friend bool operator < (const Event_node& lhs, const Event_node& rhs)
	{
	    return lhs.get_event_time() < rhs.get_event_time();
	}
	
	friend bool operator > (const Event_node& lhs, const Event_node& rhs)
	{
	    return lhs.get_event_time() > rhs.get_event_time();
	}

};



// class of Event_list wrapper
typedef priority_queue<Event_node, vector<Event_node>, greater<vector<Event_node>::value_type>> Event_list;






#endif
