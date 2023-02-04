#include "compressor.hpp"

double Compressor::db_to_amp(double db)
{
	return (pow(10, (db - 3.162) / 20.0f) * sample_range);
}

double Compressor::amp_to_db(double amp)
{
	return 20.0f * log10(amp / sample_range) - 3.162;
}

Compressor::Compressor()
{
	threshold_amp = db_to_amp(threshold);
	normalize_amp = db_to_amp(normalize_db);
	noise_floor_amp = db_to_amp(noise_floor);
}

//getters and setters
void Compressor::set_threshold(double threshold)
{
	this->threshold = threshold;
	threshold_amp = db_to_amp(threshold);
}
double Compressor::get_threshold()
{
	return threshold;
}

void Compressor::set_normalize(double normalize)
{
	this->normalize_db = normalize;
	normalize_amp = db_to_amp(normalize_db);
}
double Compressor::get_normalize()
{
	return normalize_db;
}

void Compressor::set_noise_floor(double noise_floor)
{
	this->noise_floor = noise_floor;
	noise_floor_amp = db_to_amp(noise_floor);
}
double Compressor::get_noise_floor()
{
	return noise_floor;
}

void Compressor::set_ratio(double ratio)
{
	this->ratio = ratio;
}
double Compressor::get_ratio()
{
	return ratio;
}

void Compressor::set_attack_time_ms(int attack)
{
	attack_time = attack;
}
int Compressor::get_attack_time_ms()
{
	return attack_time;
}

void Compressor::set_release_time_ms(int release)
{
	release_time = release;
}
int Compressor::get_release_time_ms()
{
	return release_time;
}

void Compressor::set_sample_window_ms(int window)
{
	sample_window = window;
}
int Compressor::get_sample_window_ms()
{
	return sample_window;
}

void Compressor::compress(Wav& wav)
{
	//initialize things that need data from the wav file
	samples_per_sec = wav.samples_per_sec;
	samples_in_window = samples_per_sec * (double)(sample_window / 1000.0f);
	attack_samples = samples_per_sec * (double)(attack_time / 1000.0f);
	release_samples = samples_per_sec * (double)(release_time / 1000.0f);

	double gain_adjust = 0;
	double output_gain = 0;
	double attack_step = 1 / (double)attack_samples;
	double release_step = 1 / (double)release_samples;

	queue<int64_t> old_values;

	uint64_t total = 0;
	int16_t* data = (int16_t*)((void*)wav.data.data());

	//iterate through whole file
	for(int i = 0; i < wav.samples; i++)
	{
		uint64_t cur = (int64_t)data[i] * (int64_t)data[i];
		total += cur;
		old_values.push(cur);

		//remove old sample from window if we go above the amount
		if(old_values.size() == (samples_in_window + 1))
		{
			uint64_t prev = old_values.front();
			total -= prev;
			old_values.pop();
		}

		//calculate RMS values
		double RMS = sqrt(total / old_values.size());

		//attack (lower volume)
		if(RMS > threshold_amp)
		{
			//amount (in decibels) we wish to be over the threshold
			double overamp = (amp_to_db(RMS) - threshold) / ratio;

			//target amplitude (converted back from db)
			double target_amp = db_to_amp(threshold + overamp);

			//difference in amplitude between our average and our target
			double diff = RMS - target_amp;

			if(gain_adjust <= 1)
			{
				gain_adjust += attack_step;
			}

			//calculate output gain adjustment
			output_gain = (RMS - (diff * gain_adjust)) / RMS;
			data[i] *= output_gain;
		}

		//release(raise volume back to normal)
		else
		{
			double diff = 1 - output_gain;

			if(gain_adjust >= 0)
			{
				gain_adjust -= release_step;
			}

			output_gain = 1 - (diff * gain_adjust);
			data[i] *= output_gain;
		}
	}
}

void Compressor::remove_peaks(Wav& wav)
{
	//get the average amplitude of the entire file
	uint64_t total = 0;
}
