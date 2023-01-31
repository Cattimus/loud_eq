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
	if(this->sample_rate_sec > 0)
	{
		attack_time_samples = sample_rate_sec * double(attack_time / 1000);
	}
}
int Compressor::get_attack_time_ms()
{
	return attack_time;
}

void Compressor::set_release_time_ms(int release)
{
	release_time = release;
	if(this->sample_rate_sec > 0)
	{
		release_time_samples = sample_rate_sec * double(release_time / 1000);
	}
}
int Compressor::get_release_time_ms()
{
	return release_time;
}

void Compressor::set_sample_window_ms(int window)
{
	sample_window = window;
	if(this->sample_rate_sec > 0)
	{
		sample_window_samples = sample_rate_sec * double(sample_window / 1000);
	}
}
int Compressor::get_sample_window_ms()
{
	return sample_window;
}
