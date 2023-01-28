#include <iostream>
using namespace std;

#include "wav.hpp"

int main()
{
	Wav wav("../media/peak_test.wav");
	cout << wav.format_code << endl;
	cout << wav.channels << endl;
	cout << wav.samples_per_sec << endl;
	cout << wav.bits_per_sample << endl;
	cout << wav.samples << endl;
	return 0;
}