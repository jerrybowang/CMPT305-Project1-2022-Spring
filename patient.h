#ifndef PATIENT_H
#define PATIENT_H

#include <iostream>
#include <string>
#include <vector>
#include <queue>

using namespace std;

enum Priority {High, Medium, Low};
typedef enum Priority Priority;

class Patient{
	private:
	Priority P;
	double arrival_time = 0; // same as when node enter Q
	double service_time_E = 0;
	double service_time_P = 0;
	double arrival_time_E = 0; //when node enter the Q
	double arrival_time_P = 0; //when node enter the Q
	double departure_time = 0;
	double departure_time_E = 0; //when node leave the Q
	double departure_time_P = 0; //when node leave the Q


	public:
	// constructor and destructor
	Patient(){}
	Patient(double Arrival_time, Priority p){
		arrival_time = Arrival_time;
		P = p;
	}
	~Patient(){}

	// getters
	double get_arrival_time() const{
		return arrival_time;
	}

	double get_service_time_E() const{
		return service_time_E;
	}

	double get_service_time_P() const{
		return service_time_P;
	}

	double get_arrival_time_E() const{
		return arrival_time_E;
	}

	double get_arrival_time_P() const{
		return arrival_time_P;
	}

	double get_departure_time() const{
		return departure_time;
	}

	double get_departure_time_E() const{
		return departure_time_E;
	}

	double get_departure_time_P() const{
		return departure_time_P;
	}

	Priority get_Priority() const{
		return P;
	}

	// debug method
	void print(){
		string Priority;
		switch (P){
			case High:
				Priority = "High";
				break;
			case Medium:
				Priority = "Medium";
				break;
			case Low:
				Priority = "Low";
				break;
			default:
				Priority = "Not Set";
		}
		cout<<"Node info:\n";
		cout<<"Priority:"<< Priority<<endl;
		
		cout<<"arrival_time:"<< arrival_time<<endl;
		cout<<"arrival_time_E:"<< arrival_time_E<<endl;
		cout<<"departure_time_E:"<< departure_time_E<<endl;
		cout<<"service_time_E:"<< service_time_E<<endl;
		cout<<"arrival_time_P:"<< arrival_time_P<<endl;
		cout<<"departure_time_P:"<< departure_time_P<<endl;
		cout<<"service_time_P:"<< service_time_P<<endl;
		cout<<"departure_time:"<< departure_time<<endl;
		cout<< endl;
		
	}

	// setters

	void set_service_time_E(double time){
		service_time_E = time;
	}

	void set_service_time_P(double time){
		service_time_P = time;
	}

	void set_departure_time(double time){
		departure_time = time;
	}

	void set_E(double time){
		if (arrival_time_E >0){
			cout<< "set E error: already set\n";
		}
		arrival_time_E = time;
	}

	void set_E_leave(double time){
		if (departure_time_E >0){
			cout<< "set E leave error: already set\n";
		}
		departure_time_E = time;
	}

	void set_P(double time){
		if (arrival_time_P >0){
			cout<< "set P error: already set\n";
		}
		arrival_time_P = time;
	}

	void set_P_leave(double time){
		if (departure_time_P >0){
			cout<< "set P leave error: already set\n";
		}
		departure_time_P = time;
	}


	// other methoids
	void report_wait_time_E(double &cumulative_waiting_E){
		double waiting_time = departure_time_E-arrival_time_E;

		if (waiting_time <0){
			cout << "Patient: waiting_time E < 0\n";
			print();
		}

		
		cumulative_waiting_E += waiting_time;
	}

	void report_wait_time_P(double cumulative_waiting_P[]){
		double waiting_time = departure_time_P-arrival_time_P;

		if (waiting_time <0){
			cout << "Patient: waiting_time P < 0\n";
			print();
		}


		switch (P){
			case High:
				cumulative_waiting_P[0] += waiting_time;
				break;
			case Medium:
				cumulative_waiting_P[1] += waiting_time;
				break;
			case Low:
				cumulative_waiting_P[2] += waiting_time;
				break;
			default:
				cout<<"Error on report_wait_time_P\n";
		}
	}

	void report_response_time(double cumulative_response[]){
		double response_time = departure_time - arrival_time;

		if (response_time <0){
			cout << "Patient: response_time < 0\n";
			print();
		}
		
		switch (P){
			case High:
				cumulative_response[0] += response_time;
				break;
			case Medium:
				cumulative_response[1] += response_time;
				break;
			case Low:
				cumulative_response[2] += response_time;
				break;
			default:
				cout<<"Error on report_response_time\n";
		}
	}


	// operator overload
	friend bool operator < (const Patient& lhs, const Patient& rhs)
	{
	    return lhs.get_arrival_time() < rhs.get_arrival_time();
	}
	
	friend bool operator > (const Patient& lhs, const Patient& rhs)
	{
	    return lhs.get_arrival_time() > rhs.get_arrival_time();
	}


};


// ordered list
typedef priority_queue<Patient, vector<Patient>, greater<vector<Patient>::value_type>> Ordered_patient;



#endif
