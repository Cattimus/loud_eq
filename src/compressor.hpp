#pragma once

#include <iostream>
#include <cmath>
using namespace std;

#include "wav.hpp"

class Compressor
{
private:
	//values the user will never have to interact with
	int sample_range = 32768;
	double threshold_amp = 0;
	double normalize_amp = 0;
	double noise_floor_amp = 0;

	//values that are different for different audio streams
	int attack_samples = 0;
	int release_samples = 0;
	int samples_in_window = 0;
	int samples_per_sec = 0;

	//values user might want to modify
	double threshold    = -18;
	double normalize_db = -6;
	double noise_floor  = -60;
	double ratio      = 4;
	int attack_time   = 20;
	int release_time  = 500;
	int sample_window = 50;

	//helper functions
	double db_to_amp(double db);
	double amp_to_db(double amp);

	double get_RMS(int16_t* data, size_t len);


public:
	Compressor();

	//compress a wav file in-place
	void compress(Wav& audio);

	//getters and setters
	void set_threshold(double threshold);
	double get_threshold();

	void set_normalize(double normalize);
	double get_normalize();

	void set_noise_floor(double noise_floor);
	double get_noise_floor();

	void set_ratio(double ratio);
	double get_ratio();

	void set_attack_time_ms(int attack);
	int get_attack_time_ms();

	void set_release_time_ms(int release);
	int get_release_time_ms();

	void set_sample_window_ms(int window);
	int get_sample_window_ms();
};
