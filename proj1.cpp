#include <iostream>
#include <cmath>
#include <vector>
#include <string>
#include <cstdlib>
#include <queue>
#include "Room.h"
#include "patient.h"
#include "PriorityQ.h"
#include "Event_list.h"


using namespace std;



// global STAT vars
int people_in_system = 0;

// three departure for each group, ordered H,M,L
int departure_count[3] = {0};

// Number of patients who leave the system because it’s at full capacity when they arrive
int leaving_count = 0;

int cleanup_count = 0; // counter for cleaned room

// three response for each group, ordered H,M,L
double cumulative_response[3] = {0};

double cumulative_waiting_E = 0;

// response time for room to be clean
double cumulative_response_Room = 0;

// three waiting for each group, ordered H,M,L
double cumulative_waiting_P[3] = {0};





// global vars


bool END_signal = false;

int occupancy = 0; // the B
int room_available = 0; // the R
int m1_avalible = 0; // the "m1" nurses
int m2_avalible = 0; // the "m2" janitors

double mu_clean =0; // the cleanup time





// helper functions
double ran_exp(double lambda){
	double u = (double)rand() / ((double)RAND_MAX + 1.0);
	return (-log(1 - u)) / lambda;
}

void ini_log (vector<Patient>& trace, double lambda, double mu_E,double mu_P, Priority p){
	double max_time = 24*60.0; // 24 hour, unit = minute
	double cul_time = ran_exp(lambda);

	// insert new
	while (cul_time < max_time){
		trace.push_back(Patient(cul_time,p));
		cul_time += ran_exp(lambda);
	}

	// set up service times
	for (int i = 0; i < trace.size(); i++){
		trace[i].set_service_time_E(ran_exp(mu_E));
	}

	for (int i = 0; i < trace.size(); i++){
		trace[i].set_service_time_P(ran_exp(mu_P));
	}

}


// list of process event function
void process_Arrival(double event_time, Ordered_patient& ordered_log, vector<Patient>& evaluation_Q, Event_list& event_list);

// ----- Warren

void process_Service_E(double event_time ,vector<Patient>& evaluation_Q, vector<Patient>& intermediate_Q, Event_list& event_list);

void process_Depart_E(double event_time , vector<Patient>& evaluation_Q, Event_list& event_list);

void process_Arrive_P(double event_time, Event_list& event_list,vector<Patient>& intermediate_Q, PriorityQ& priorityQ);

void process_Service_P(double event_time , PriorityQ& priorityQ, Event_list& event_list);

void process_Departure(double event_time, Event_list& event_list);

// ------ Levi

void process_Room_wait(double event_time, Event_list& event_list, vector<Room>& room_Q);

void process_Room_clean(vector<Room>& room_Q, Event_list& event_list, double event_time);

void process_Room_finish(Event_list& event_list, double event_time, vector<Room>& room_Q );

void process_END();

void process_PRINT(double event_time);



// main event processor
void process_Event(Event_list& event_list, Ordered_patient& ordered_log, vector<Patient>& evaluation_Q, PriorityQ& priorityQ, vector<Room>& room_Q,vector<Patient>& intermediate_Q){
	if (event_list.empty()){
		cout << "process_Event: error on event_list, empty\n";
		return;
	}

	Event_node node = event_list.top();
	double event_time = node.get_event_time();
	Event event = node.get_event();
  // node.print();
	// delete this event node from the Q
	event_list.pop();

	switch(event){
		case Arrival:
			process_Arrival(event_time, ordered_log, evaluation_Q, event_list);
			break;
		case Service_E:
			process_Service_E(event_time, evaluation_Q, intermediate_Q, event_list);
			break;
		case Depart_E:
			process_Depart_E(event_time, evaluation_Q, event_list);
			break;
		case Arrive_P:
			process_Arrive_P(event_time, event_list, intermediate_Q, priorityQ);
			break;
		case Service_P:
			process_Service_P(event_time , priorityQ, event_list);
			break;
		case Departure:
			process_Departure(event_time, event_list);
			break;
		case Room_wait:
			process_Room_wait(event_time, event_list, room_Q);
			break;
		case Room_clean:
			process_Room_clean( room_Q, event_list, event_time);
			break;
		case Room_finish:
			process_Room_finish(event_list,  event_time, room_Q);
			break;
		case END:
			process_END();
			break;
		case PRINT:
			process_PRINT( event_time);
			break;
		default:
			cout<< "process_Event: error getting event type\n";
	}

}


//-----------------------------------------------------



int main(int argc, char* argv[]) {
	// input arguments
	if (argc >= 14){
		// assign input
		double lambda_high_priority = atof(argv[1]);
		double lambda_medium_priority = atof(argv[2]);
		double lambda_low_priority = atof(argv[3]);
		double mu_evaluation = atof(argv[4]);
		double mu_high_priority = atof(argv[5]);
		double mu_medium_priority = atof(argv[6]);
		double mu_low_priority = atof(argv[7]);
		double mu_cleanup = atof(argv[8]);
		int B = atoi(argv[9]);
		int R = atoi(argv[10]);
		int m1 = atoi(argv[11]);
		int m2 = atoi(argv[12]);
		int Seed = atoi(argv[13]);




		// Add error checks for input variables here, exit if incorrect input

		if (lambda_high_priority < 0){
			printf("incorrect input: lambda_high_priority < 0\n");
			return 0;
		}
		if (lambda_medium_priority < 0){
			printf("incorrect input: lambda_medium_priority < 0\n");
			return 0;
		}
		if (lambda_low_priority < 0){
			printf("incorrect input: lambda_low_priority < 0\n");
			return 0;
		}
		if (mu_evaluation <= 0){
			printf("incorrect input: mu_evaluation <= 0\n");
			return 0;
		}
		if (mu_high_priority <= 0 && lambda_high_priority != 0){
			printf("incorrect input: mu_high_priority <= 0\n");
			return 0;
		}
		if (mu_medium_priority <= 0 && lambda_medium_priority != 0){
			printf("incorrect input: mu_medium_priority <= 0\n");
			return 0;
		}
		if (mu_low_priority <= 0 && lambda_low_priority != 0){
			printf("incorrect input: mu_low_priority <= 0\n");
			return 0;
		}
		if (mu_cleanup <= 0){
			printf("incorrect input: mu_clean_up <= 0\n");
			return 0;
		}
		if (B <= 0){
			printf("incorrect input: B <= 0\n");
			return 0;
		}
		if (R <= 0){
			printf("incorrect input: R <= 0\n");
			return 0;
		}
		if (m1 <= 0){
			printf("incorrect input: m1 <= 0\n");
			return 0;
		}
		if (m2 <= 0){
			printf("incorrect input: m2 <= 0\n");
			return 0;
		}



		// initialize global param
		occupancy = B; // do not change in event
		room_available = R; // change based on event
		m1_avalible = m1; // change based on event
		m2_avalible = m2; // change based on event
		mu_clean = mu_cleanup;

		// initialize random seed
		srand(Seed);


		// initialize temp trace log
		vector<Patient> trace_log;

		// merge all three into one ordered log
		Ordered_patient ordered_log;

		// initialize high priority one if lamda >0
		if (lambda_high_priority != 0){
			ini_log(trace_log, lambda_high_priority, mu_evaluation, mu_high_priority, High);
		}


		// add to trace log
		while (!trace_log.empty()){
			ordered_log.push(trace_log.front());
			trace_log.erase(trace_log.begin());
		}


		
		// initialize medium priority one if lamda >0
		if (lambda_medium_priority != 0){
			ini_log(trace_log, lambda_medium_priority, mu_evaluation,mu_medium_priority, Medium);
		}


		// add to trace log
		while (!trace_log.empty()){
			ordered_log.push(trace_log.front());
			trace_log.erase(trace_log.begin());
		}



		
		// initialize low priority one if lamda >0
		if (lambda_low_priority != 0){
			ini_log(trace_log, lambda_low_priority, mu_evaluation,mu_low_priority, Low);
		}

		// add to trace log
		while (!trace_log.empty()){
			ordered_log.push(trace_log.front());
			trace_log.erase(trace_log.begin());
		}
		// free heap usage
		trace_log.shrink_to_fit();

		


		// initialize queues
		vector<Patient> evaluation_Q;

		PriorityQ priorityQ;

		vector<Room> room_Q;

		vector<Patient> intermediate_Q;






		// initialize event_lists
		Event_list event_list;
		// add end event
		event_list.push(Event_node(END, 24*60.0));

		// add print event per hour, only 23 needed
		for (int i = 1; i < 24; i++){
			event_list.push(Event_node(PRINT, i*60.0));
		}

		// add first arrive to start, if there is any
		if (!ordered_log.empty()){
			event_list.push(Event_node(Arrival, ordered_log.top().get_arrival_time()));
		}




		// run simulation
		while (END_signal == false){
			process_Event(event_list, ordered_log, evaluation_Q, priorityQ, room_Q, intermediate_Q);
		}


		// print last report
		cout << "End of simulation\n";
		process_PRINT(24*60.0);

	}
	else{
		cout << "Insufficient number of arguments provided!\n";
	}
	return 0;
}



// implementation of event function
void process_Arrival(double event_time ,Ordered_patient& ordered_log, vector<Patient>& evaluation_Q, Event_list& event_list){
	// case: no more people comming
	if (ordered_log.empty()){
		return;
	}

	// grab Patient from ordered_log
	Patient patient = ordered_log.top();
	ordered_log.pop();

	// check capacity
	if (people_in_system > occupancy){
		// send to else where
		leaving_count++;
		// schdule next arrival (if still have people on log)
		if (!ordered_log.empty()){
			event_list.push(Event_node(Arrival, ordered_log.top().get_arrival_time()));
		}
		return;
	}

	// case we have spot
	people_in_system++;
	// get patient into E_Q, record the enter time
	patient.set_E(event_time);
	evaluation_Q.push_back(patient);
	// if we have enough m1, schdule service right away
	if (m1_avalible > 0){
		event_list.push(Event_node(Service_E, event_time));
	}

	// schdule next arrival (if still have people on log)
	if (!ordered_log.empty()){
		event_list.push(Event_node(Arrival, ordered_log.top().get_arrival_time()));
	}

}



// -----------

void process_Service_E(double event_time ,vector<Patient>& evaluation_Q, vector<Patient>& intermediate_Q, Event_list& event_list){
	//if no people in E_Q
	if (evaluation_Q.empty()){
		return;
	}

	//if there are people waiting in E_Q and m1 > 0
	if(m1_avalible>0){

		// one of m1 nurses beginning serving
		m1_avalible--;

		// get the patient that need to be serviced
		Patient patient = evaluation_Q.front();
		evaluation_Q.erase(evaluation_Q.begin());

		//set the departure time from E_Q
		patient.set_E_leave(event_time);
		//patient.report_wait_time_E(cumulative_waiting_E);

		// update the event time
		double schedule_time = event_time + patient.get_service_time_E();

		// get patient into intermediate_Q, record enter time of intermediate_Q
		intermediate_Q.push_back(patient);

		// schdule next event Depart_E
		event_list.push(Event_node(Depart_E, schedule_time));
	}
	else{
		// if m1 <=0, do nothing
		return;
	}

}


void process_Depart_E(double event_time, vector<Patient>& evaluation_Q, Event_list& event_list){
	// if E_Q is not empty, then schedule next event
	if (!evaluation_Q.empty() && m1_avalible>0){
		event_list.push(Event_node(Service_E, event_time));
	}

	//once a patient leave E then, m1++
	m1_avalible++;

	// schdule arrive_P
	event_list.push(Event_node(Arrive_P, event_time));



}

void process_Arrive_P(double event_time, Event_list& event_list,vector<Patient>& intermediate_Q, PriorityQ& priorityQ){

	// grabe it from the intermediate_Q
	Patient patient = intermediate_Q.front();
	intermediate_Q.erase(intermediate_Q.begin());

	//set the patient arrival time of P_Q
	patient.set_P(event_time);


	// push it into P_Q
	priorityQ.push(patient);


	//schedule next event Service in P.
	if(room_available>0){
		event_list.push(Event_node(Service_P, event_time));
	}
}

void process_Service_P(double event_time , PriorityQ& priorityQ, Event_list& event_list){

    //if no people in P_Q
  if (priorityQ.get_size()<=0){
		return;
	}

  //if there are people waiting in P_Q and enough rooms
  if(room_available>0){

    // one of rooms has been used
    room_available--;

    // get the patient that need to be serviced
    Patient patient = priorityQ.top();
    priorityQ.pop();

    //record the cumulative waiting time of P
    patient.set_P_leave(event_time);
    patient.report_wait_time_P(cumulative_waiting_P);

    // update the event time
    double schedule_time = event_time + patient.get_service_time_P();

    // once patients has been serviced in P, then they can leave
    patient.set_departure_time(schedule_time);
    patient.report_response_time(cumulative_response);

    // just count the departure patient cumulative waiting E
    patient.report_wait_time_E(cumulative_waiting_E);

    // count the departure number of H,M,L
    switch (patient.get_Priority()){
			case High:
				departure_count[0]++;
				break;
			case Medium:
				departure_count[1]++;
				break;
			case Low:
				departure_count[2]++;
				break;
			default:
				cout<<"Error on departure_count\n";
		}

    // schdule next event Departure from system
    event_list.push(Event_node(Departure, schedule_time));
	}
  else{
    //if room_available<=0, then patients stay at P_Q, do nothing
    return;
  }


}

void process_Departure(double event_time, Event_list& event_list){

	//one people leave the system
	people_in_system--;

	//schedule room wait event
	event_list.push(Event_node(Room_wait, event_time));
}


// -----------

void process_Room_wait(double event_time, Event_list& event_list, vector<Room>& room_Q){
	// define a temp variable to store room clean time
	Room temp;
	// room cleanup time is based on a service time distribution
	double service_time = ran_exp(mu_clean);

	// store clean time and arrival time back to temp
	temp = Room(service_time);
	temp.set_Q(event_time);


	// push temp variable back to room queue
	room_Q.push_back(temp);

	// schdule next clean event
	if (m2_avalible >0){
		event_list.push(Event_node(Room_clean, event_time));
	}
}

void process_Room_clean(vector<Room>& room_Q, Event_list& event_list, double event_time){


	// if there is a janitor avaiable clean the room, else do noting (no janitor avaiable)
	if(m2_avalible >0){
		// get the room that need to be cleaned from the room queue
		Room temp = room_Q.front();

		// record when room leave Q
		temp.set_Q_leave(event_time);

		// update the room response time
		temp.report_response_time(cumulative_response_Room);

		// update the event time
		double schedule_time =event_time + temp.get_service_time();

		// pop the first room in the queue
		room_Q.erase(room_Q.begin());

		// decrement the avaiable  janitor
		m2_avalible--;

		// schdule next Room finish event
		event_list.push(Event_node(Room_finish, schedule_time));
	}
}

void process_Room_finish(Event_list& event_list, double event_time,vector<Room>& room_Q ){

	// update the room cleaned count
	cleanup_count++;
	// update the room avaiable count
	room_available++;
	// update the avaiable janitor
	m2_avalible++;

	// if we still have room need to be cleaned, schedule a room clean event
	if( !room_Q.empty()){
		event_list.push(Event_node(Room_clean, event_time));
	}

	// schedule next start service event
	event_list.push(Event_node(Service_P, event_time));
}

void process_END(){
	// change the signal states to true (default false)
	END_signal = true;
}


void process_PRINT(double event_time){
	// get the total number of departure for H, M, L
	int total_departure = departure_count[0] +departure_count[1] + departure_count[2];

	// get the cumulative waiting time gor P queue for H, M, L
	double  total_cumulative_waiting_P = cumulative_waiting_P[0] +cumulative_waiting_P[1] + cumulative_waiting_P[2];

	// get the cumulative reponse time for H, M, L
	double  cumulative_response_total = cumulative_response[0] + cumulative_response[1] + cumulative_response[2];


	// formuals to calculate Average_response time
	double Average_response = cumulative_response_total / total_departure;
	if (total_departure ==0) {
		Average_response = 0;
	}

	// formuals to calculate Average_response time for High-Priority group
	double  Average_response_H = cumulative_response[0]/ departure_count[0];
	if(departure_count[0]==0){
		Average_response_H = 0;
	}
	// formuals to calculate Average_response time for Meduim-Priority group
	double  Average_response_M = cumulative_response[1]/ departure_count[1];
	if(departure_count[1]==0){
		Average_response_M = 0;
	}

	// formuals to calculate Average_response time for Low-Priority group
	double  Average_response_L = cumulative_response[2]/ departure_count[2];
	if(departure_count[2]==0){
		Average_response_L = 0;
	}

	// formuals to calculate Average_number in system
	double Average_number = (total_departure / event_time) * Average_response;

	// formuals to calculate Average_waiting time in E queue
	double Average_waiting_E = cumulative_waiting_E / total_departure;
	if (total_departure ==0) {
		Average_waiting_E = 0;
	}

	//formuals to calculate Average_waiting time in P queue
	double Average_waiting_P = total_cumulative_waiting_P / total_departure;
	if (total_departure ==0) {
		Average_waiting_P = 0;
	}

	// formuals to calculate Average_waiting time High-Priority group
	double Average_waiting_P_H = cumulative_waiting_P[0]/departure_count[0];
	if(departure_count[0]==0){
		Average_waiting_P_H = 0;
	}

	// formuals to calculate Average_waiting time Medium-Priority group
	double Average_waiting_P_M = cumulative_waiting_P[1]/departure_count[1];
	if(departure_count[1]==0){
		Average_waiting_P_M = 0;
	}

	// formuals to calculate Average_waiting time Low-Priority group
	double Average_waiting_P_L = cumulative_waiting_P[2]/departure_count[2];
	if(departure_count[2]==0){
		Average_waiting_P_L = 0;
	}

	// formuals to calculate Average_clean_up time
	double Average_clean_up = cumulative_response_Room/ cleanup_count;
	if (cleanup_count == 0){
		Average_clean_up = 0;
	}


	// all the required print statements
	printf ("Total number of departures: %d \n", total_departure );

	printf ("Average number of patients in system: %f \n", Average_number);

	printf ("Average response time for all patients: %f \n",Average_response);

	if (departure_count[0] != 0){
		printf ("Average response time for High-Priority patients: %f \n", Average_response_H );
	}
	else{
		printf ("Average response time for High-Priority patients: N/A \n");
	}

	if (departure_count[1] != 0){
		printf ("Average response time for Medium-Priority patients: %f \n", Average_response_M);
	}
	else{
		printf ("Average response time for Medium-Priority patients: N/A \n");
	}

	if (departure_count[2] != 0){
		printf ("Average response time for Low-Priority patients: %f \n", Average_response_L );
	}
	else{
		printf ("Average response time for Low-Priority patients: N/A \n");
	}

	printf ("Average waiting time in the E queue: %f \n", Average_waiting_E );

	printf ("Average waiting time in the P queue: %f \n", Average_waiting_P );

	if (departure_count[0] != 0){
		printf ("Average waiting time for High-Priority patients: %f \n", Average_waiting_P_H );
	}
	else{
		printf ("Average waiting time for High-Priority patients: N/A \n");
	}

	if (departure_count[1] != 0){
		printf ("Average waiting time for medium-Priority patients: %f \n", Average_waiting_P_M );
	}
	else{
		printf ("Average waiting time for medium-Priority patients: N/A \n");
	}

	if (departure_count[2] != 0){
		printf ("Average waiting time for low-Priority patients: %f \n", Average_waiting_P_L );
	}
	else{
		printf ("Average waiting time for low-Priority patients: N/A \n");
	}

	printf ("Average cleanup time for each room: %f \n", Average_clean_up);

	printf ("Number of patients who leave the system because it's at full capacity when they arrive: %d \n\n", leaving_count);
}
