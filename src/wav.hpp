#pragma once
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <string>
using namespace std;

//only 16-bit PCM wav files will be supported. Extension types are not supported.
//this program will assume little-endian architecture
class Wav
{
public:
	uint16_t format_code;
	uint16_t channels;
	uint32_t samples_per_sec;
	uint16_t block_align;
	uint16_t bits_per_sample;
	char* data;

	int samples; //total number of samples in file

	Wav(string filename);
	~Wav();
};