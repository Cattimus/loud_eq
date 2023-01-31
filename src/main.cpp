#include <iostream>
using namespace std;

#include "wav.hpp"
#include "compressor.hpp"

int main()
{
	//test normal constructor of wav
	Wav wav("../media/peak_test.wav");
	cout << wav.format_code << endl;
	cout << wav.channels << endl;
	cout << wav.samples_per_sec << endl;
	cout << wav.bits_per_sample << endl;
	cout << wav.samples << endl;
	wav.write("../output/test.wav");

	//test assignment operator of wav
	wav = Wav("../media/longtest.wav");
	cout << wav.format_code << endl;
	cout << wav.channels << endl;
	cout << wav.samples_per_sec << endl;
	cout << wav.bits_per_sample << endl;
	cout << wav.samples << endl;
	wav.write("../output/test2.wav");

	Compressor comp;
	comp.compress(wav);

	return 0;
}
