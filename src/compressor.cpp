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
