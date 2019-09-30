/**
//	Nathan Van De Vyvere
//	October 30, 2019
//	CMPS 3013
//	Program A03
//
//	Description:
//	This program reads in a json file of animals that include their name, date,
//	longitude, latitude, priority, validation, and version. These animals are
//	then put into an array of heaps based on their dates and the number of 
//	inputed heaps. The heap then sorts them by highest priority in their 
//	respective heaps. The top 5 highest priority animals from each heap is then
//	extracted and printed out.
//
*/



#include "json_helper.cpp"
#include <fstream>
#include <iostream>
#include <vector>

#define _USE_MATH_DEFINES

#include <math.h>

using namespace std;

const static double EarthRadiusKm = 6372.8;

inline double DegreeToRadian(double angle) {
	return M_PI * angle / 180.0;
}

class Coordinate {
public:
	Coordinate(double latitude, double longitude) : myLatitude(latitude), myLongitude(longitude) {}

	double Latitude() const {
		return myLatitude;
	}

	double Longitude() const {
		return myLongitude;
	}

private:
	double myLatitude;
	double myLongitude;
};

double HaversineDistance(const Coordinate &p1, const Coordinate &p2) {
	double latRad1 = DegreeToRadian(p1.Latitude());
	double latRad2 = DegreeToRadian(p2.Latitude());
	double lonRad1 = DegreeToRadian(p1.Longitude());
	double lonRad2 = DegreeToRadian(p2.Longitude());

	double diffLa = latRad2 - latRad1;
	double doffLo = lonRad2 - lonRad1;

	double computation = asin(sqrt(sin(diffLa / 2) * sin(diffLa / 2) + cos(latRad1) * cos(latRad2) * sin(doffLo / 2) * sin(doffLo / 2)));
	return 2 * EarthRadiusKm * computation;
}

struct Animal {
	string animal_name;
	long date;
	double latitude;
	double longitude;
	float priority;
	bool validated;
	string version;

	Animal() {
		animal_name = "";
		date = 0;
		latitude = 0.0;
		longitude = 0.0;
		priority = 0.0;
		validated = 0;
		version = "";
	}

	Animal(string name, long _date, double lat, double lon, float pri, bool val, string ver) {
		animal_name = name;
		date = _date;
		latitude = lat;
		longitude = lon;
		priority = pri;
		validated = val;
		version = ver;
	}

	Animal(json j) {
		animal_name = j["animal_name"];
		date = j["date"];
		latitude = j["latitude"];
		longitude = j["longitude"];
		priority = j["priority"];
		validated = j["validated"];
		version = j["version"];
	}
};

/**
* Class Heap:
*
* Public Methods:
*    Heap()
*    void Insert(Animal)
*    void Extract
*/

class Heap {
private:
	vector<Animal> bucket;
	int Next=1;

	/**
	* Function IncreaseKey:
	*      Bubbles element up from given index. Compares priorities.
	*
	* Params:
	*     [int] index  - index of item to be increased
	* Returns
	*      void
	*/
	void BubbleUp(int i) {
		int p = Parent(i);
		while (p > 0 && bucket[i].priority > bucket[p].priority) {
			Swap(i, p);
			i = p;
			p = Parent(i);
		}
	}

	/**
	* Function DecreaseKey:
	*      Bubbles element down from given index. Compares priorities
	*
	* Params:
	*      [int] index - index of item to be decreased
	* Returns
	*      void
	*/
	void BubbleDown(int i) {
		int c = PickChild(i);

		while (c > 0) {
			if (bucket[i].priority < bucket[c].priority) {
				Swap(i, c);
				i = c;
				c = PickChild(i);
			}
			else {
				c = -1;
			}
		}
	}

	/**
	* Function Swap:
	*      Swaps two elements in a vector of Animals
	*
	* Params:
	*      [int] index1 - index of item to swap with
	*      [int] index2 - same
	* Returns
	*      void
	*/
	void Swap(int p, int i) {
		Animal temp = bucket[p];
		bucket[p] = bucket[i];
		bucket[i] = temp;
	}

	/**
	* Function Parent:
	*      Returns parent index of a given index
	*
	* Params:
	*      [int]index - index to calculate parent from
	* Returns
	*      index [int]
	*/
	int Parent(int i) {
		return int(i / 2);
	}

	/**
	* Function LeftChild:
	*      Returns left index of a given index
	*
	* Params:
	*      [int] index - index to calculate child from
	* Returns
	*      [int] index - left child index
	*/
	int LeftChild(int i) {
		return i * 2;
	}

	/**
	* Function RightChild:
	*      Returns right index of a given index
	*
	* Params:
	*      [int] index - index to calculate child from
	* Returns
	*      [int] index - right child index
	*/
	int RightChild(int i) {
		return i * 2 + 1;
	}

	/**
	* Function PickChild:
	*      Return index of child to swap with or -1 to not swap.
	*
	* Params:
	*      [int] index - index of parent element
	* Returns
	*      [int] index - index to swap with or -1 to not swap
	*/
	int PickChild(int i) {
		if (RightChild(i) >= Next) {    //No right child
			if (LeftChild(i) >= Next) { //No left child
				return -1;
			}
			else { //you have a left no right
				return LeftChild(i);
			}
		}
		else {
			//right child exists
			if (bucket[i * 2 + 1].priority > bucket[i * 2].priority) {
				return RightChild(i);
			}
			else {
				return LeftChild(i);
			}
		}
	}

	/**
	* Function PrintExtract:
	*		Prints the name and priority of the Animal extraceted
	*		from the front of the heap.
	*
	* Params:
	*      [Animal] A - max Animal thgt was extracted
	* Returns
	*      void
	*/

	void PrintExtract(Animal a) {
		cout << a.animal_name << " " << a.priority << endl;
	}

public:
	/**
	* Function Heap:
	*      Constructor that creates a vector of Animals
	*		initiated with an empty Animal.
	*
	* Params:
	*      void
	* Returns
	*      void
	*/

	Heap() {
		bucket.push_back(Animal{});
		Next = 1;
	}

	/**
	* Function Insert:
	*      Insert value into heap.
	*
	* Params:
	*      [Animal] x - object to be inserted
	* Returns
	*      void
	*/
	void Insert(Animal x){
		if (x.validated == true) {
			x.priority = (1.0 / x.priority);
		}
		bucket.push_back(x);
		BubbleUp(Next);
		Next++;
	}

	/**
	* Function Extract:
	*      Removes top element from heap (whether min or max).
	*
	* Params:
	*      void
	* Returns
	*      void - calls PrintExtract to print object
	*/
	void Extract() {

		if (bucket.empty()) {
			return;
		}

		Animal retval = bucket[1];
		bucket[1] = bucket[--Next];

		if (bucket.size() > 1) {
			BubbleDown(1);
		}
		PrintExtract(retval);
	}
};



class AnimalHelper {
private:
	Animal **Animals;
	JsonHelper *J;
	json obj;
	int size;

public:
	AnimalHelper(string filename) {
		J = new JsonHelper(filename);
		size = J->getSize();
		Animals = new Animal *[size];
		for (int i = 0; i < size; i++) {
			obj = J->getNext();
			Animals[i] = new Animal(obj);
		}
	}

	/**
	* Function sort:
	*		Sorts the Animals into an array of Heaps based
	*		on the number of Heaps
	*
	* Params:
	*		[Heap []] B - array of Heaps to hold the Animals
	*		[int] n - number of Heaps
	* Returns
	*      void
	*/
	void sort(Heap B[], int n) {
		int index;
		Animal move;
		for (int i = 0; i < size; i++) {
			index = abs(Animals[i]->date) % n;
			move.animal_name = Animals[i]->animal_name;
			move.date = Animals[i]->date;
			move.latitude = Animals[i]->latitude;
			move.longitude = Animals[i]->longitude;
			move.priority = Animals[i]->priority;
			move.validated = Animals[i]->validated;
			move.version = Animals[i]->version;
			B[index].Insert(move);
		}
	}

	void PrintAnimals() {

		for (int i = 0; i < size; i++) {

			cout << Animals[i]->animal_name << " "
				<< Animals[i]->latitude << " "
				<< Animals[i]->longitude << " "
				<< Animals[i]->date << " "
				<< Animals[i]->priority << " "
				<< Animals[i]->validated << " "
				<< Animals[i]->version << endl;

		}
	}
};


int main(int argc, char **argv) {

	int NumberofBuckets;
	cin >> NumberofBuckets;
	Heap buckets[11];
	Animal extracted;
	AnimalHelper AH("animals.json");

	AH.sort(buckets, NumberofBuckets);
	
	for (int i = 0; i < NumberofBuckets; i++) {
		cout << endl << "Heap " << i << endl
			<< "============================================" << endl;
		for (int j = 1; j <= 5; j++) {
			buckets[i].Extract();
		}
	}

	return 0;
}
