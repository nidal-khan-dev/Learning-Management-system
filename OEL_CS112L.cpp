#include <iostream>
using namespace std;

class Weather {
	public:
		string condition;
		int penalty;

		Weather(string c ="clear", int p=0):condition(c),penalty(p) {}


};

class Racer {
	public:
		string name;
		int position;
		double energy;

	public:
		Weather weather;

		Racer(string n,double e) {
			name =n;
			position = 0;
			energy = e;
		}

		virtual void move()= 0;
		virtual ~Racer() {}

		friend ostream& operator<<(ostream& out,Racer& r) {
			out<<r.name<<" Position "<<r.position<<" energy "<<r.energy<<endl;
			return out;
		}

		bool operator>(Racer& other) {
			return position > other.position;
		}

		Racer& operator++() {
			energy+=10;
			return*this;
		}

};

class Tortoise: public Racer {
	public:
		static int count;

		Tortoise(string n,double e):Racer(n,e) {
			count++;
		}
		~Tortoise() {
			count--;
		}
		void move() {
			position +=5;

			if(weather.condition!="Mud")
				energy-=weather.penalty;
			energy-=2;
		}

};
int Tortoise::count=0;

class Outofexception {};

class Hare: public Racer {
	public:
		double energystrt;

		Hare(string n,double e): Racer(n,e) {
			energystrt =e;
		}

		void move() {
			position+=10;

			energy-=weather.penalty;
			energy-=6;

			if(energy < 0.1*energystrt)
				throw Outofexception();
		}

};


int main() {
	int n;
	cout<<"=======RACING GAME========\n\n";
	cout<<"enter racers: ";
	cin>>n;

	Racer** racers =new Racer*[n];

	for(int i = 0; i<n; i++) {
		if(i % 2==0) {
			racers[i] = new Tortoise("TORTOISE",100);
		} else  {
			racers[i] = new Hare("HARE",100);
		}
	}
	
	string conditions[3]={"Clear","Mud","Storm"};
	int penalties[3]={0,5,10};
	bool raceover =false;
	int turn =0;
	
	while(!raceover){
		cout<<"\nTurn "<<turn+1<<endl;
		
		string c = conditions[turn%3];
		int p = penalties[turn%3];
		
		for(int i=0;i<n;i++){
			racers[i]->weather = Weather(c,p);
		try{
			racers[i]->move();
		}	
		catch(Outofexception){
			cout<<"hare resting....";
			++(*racers[i]);
		}
		++(*racers[0]);
		
		cout<<*racers[i]<<endl;
		
		if(racers[i]->position >=100)
			raceover =true;
		
		}
		turn++;
		
	}
	for(int i=0;i<n;i++){
		delete[] racers[i];
	}
	delete[] racers;

}