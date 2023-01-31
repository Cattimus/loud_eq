#pragma once
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <iostream>
#include <string>
using namespace std;

//only 16-bit PCM wav files will be supported. Extension types are not supported.
//this program will assume little-endian architecture
class Wav
{
private:
	uint32_t file_size;

public:
	uint16_t format_code;
	uint16_t channels;
	uint32_t samples_per_sec;
	uint16_t block_align;
	uint16_t bits_per_sample;
	char* data;

	int samples; //total number of samples in file
	int sample_size;

	Wav(string filename);
	Wav(const Wav& wav); //copy constructor
	void operator=(const Wav& wav);
	~Wav();

	void write(string filename);

	//get a window of [size] at [offset]
	//both measurements are in samples not in bytes
	void* get_window(size_t offset, size_t size);
};
