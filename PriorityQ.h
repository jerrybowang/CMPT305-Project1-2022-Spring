#ifndef PRIORITYQ_H
#define PRIORITYQ_H


#include <iostream>
#include <vector>
#include "patient.h"

using namespace std;


class PriorityQ{
	private:
	vector<Patient> H;
	vector<Patient> M;
	vector<Patient> L;
	public:


	// constructor and destructor
	PriorityQ(){}
	~PriorityQ(){}


	// getter
	int get_size() const{
		return H.size() + M.size() + L.size();
	}

	bool is_empty() const{
		return get_size() == 0;
	}

	// other methoid

	void push(Patient patient){
		switch (patient.get_Priority()){
			case High:
				H.push_back(patient);
				break;
			case Medium:
				M.push_back(patient);
				break;
			case Low:
				L.push_back(patient);
				break;
			default:
				cout<<"Error on PriorityQ push\n";
		}
	}

	Patient& top(){
		if (is_empty()){
			cout <<"PriorityQ: empty when top(), unexpected\n";
		}
		if (! H.empty()){
			return H.front();
		}
		else if (! M.empty()){
			return M.front();
		}
		else {
			return L.front();
		}
	}

	void pop(){
		if (is_empty()){
			cout <<"PriorityQ: empty when pop(), unexpected\n";
			return;
		}
		else if (! H.empty()){
			H.erase(H.begin());
		}
		else if (! M.empty()){
			M.erase(M.begin());
		}
		else{
			L.erase(L.begin());
		}
	}

};

#endif
