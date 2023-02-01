#include "compressor.hpp"

double Compressor::db_to_amp(double db)
{
	return pow(10, db / 20) * sample_range;
}

double Compressor::amp_to_db(double amp)
{
	return 20 * log10(amp / sample_range);
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

//calculate the root mean square value for a sample window
double Compressor::get_RMS(int16_t* data, size_t len)
{
	double total = 0;

	//calculate total square (of both channels)
	for(int i = 0; i < len; i += 2)
	{
		double l = *(data+i);
		double r = *(data+i+1);

		total += (l * l + r * r);
	}

	//calculate root mean square
	return sqrt(total / (double)len);
}

void Compressor::compress(Wav& wav)
{
	//initialize things that need data from the wav file
	samples_per_sec = wav.samples_per_sec;
	samples_in_window = samples_per_sec * (double)(sample_window / 1000.0);
	attack_samples = samples_per_sec * (double)(attack_time / 1000.0);
	release_samples = samples_per_sec * (double)(release_time / 1000.0);

	double gain_adjust = 0;
	double output_gain = 0;
	double attack_step = 1 / (double)attack_samples;
	double release_step = 1 / (double)release_samples;

	//iterate through whole file
	for(int i = 0; i < wav.samples; i += samples_in_window)
	{
		size_t len = samples_in_window;
		int16_t* window = (int16_t*)wav.get_window(i, len);

		//we have exceeded the bounds of the array
		if(window == NULL)
		{
			//attempt to get the last (smaller) window
			len = wav.samples - i;
			window = (int16_t*)wav.get_window(i, len);

			//window is not recoverable
			if(window == NULL)
			{
				break;
			}
		}

		//calculate RMS values
		double RMS = get_RMS(window, len);

		//attack (lower volume)
		if(RMS > threshold_amp)
		{
			int overamp = (RMS - threshold) / ratio;
			int target_amp = threshold_amp + overamp;
			int diff = RMS - target_amp;

			//test values are correct by multiplying by 0.5
			for(int x = 0; x < len; x += 2)
			{
				if(gain_adjust < 1)
				{
					gain_adjust += attack_step;
				}

				//calculate output gain adjustment
				output_gain = (RMS - (diff * gain_adjust)) / RMS;

				*(window+x) *= output_gain;
				*(window+x+1) *= output_gain;
			}
		}

		//release(raise volume back to normal)
		else
		{
			for(int x = 0; x < len; x += 2)
			{
				double diff = 1 - output_gain;

				if(gain_adjust > 0)
				{
					gain_adjust -= release_step;
				}

				double release_output = 1 - (diff * gain_adjust);
				*(window+x) *= release_output;
				*(window+x+1) *= release_output;
			}
		}
	}
}
