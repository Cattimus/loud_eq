#include "wav.hpp"

//copy constructor and assignment operator (needs deep copy)
Wav::Wav(const Wav& wav)
{
	*this = wav;
}
void Wav::operator=(const Wav& wav)
{
	//private
	file_size = wav.file_size;
	sample_size = wav.sample_size;

	//public
	format_code = wav.format_code;
	channels = wav.channels;
	samples_per_sec = wav.samples_per_sec;
	block_align = wav.block_align;
	bits_per_sample = wav.bits_per_sample;

	samples = wav.samples;
	sample_size = wav.sample_size;

	//deep copy data
	data = vector<char>(samples * sample_size);
	memcpy(data.data(), wav.data.data(), samples * sample_size);
}

static void exit_gracefully(FILE* to_close, const char* filename)
{
	fclose(to_close);
	printf("ERROR: Wav file %s is not valid.\n", filename);
	return;
}

Wav::Wav(string filename)
{
	FILE* input = fopen(filename.c_str(), "rb");
	char chunk_id[4];

	//chunk ID for the first chunk should be RIFF
	fread(chunk_id, 4, 1, input);
	if(strncmp(chunk_id, "RIFF", 4) != 0) 
	{
		exit_gracefully(input, filename.c_str());
	}

	//read file size for later writing (so we don't have to compute it again)
	fread(&file_size, 4, 1, input);

	//WAVE ID
	fread(chunk_id, 4, 1, input);
	if(strncmp(chunk_id, "WAVE", 4) != 0)
	{
		exit_gracefully(input, filename.c_str());
	}

	//fmt chunk
	fread(chunk_id, 4, 1, input);
	if(strncmp(chunk_id, "fmt ", 4) != 0)
	{
		exit_gracefully(input, filename.c_str());
	}

	uint32_t chunk_size = 0;
	fread(&chunk_size, 4, 1, input);
	if(chunk_size != 16)
	{
		exit_gracefully(input, filename.c_str());
	}

	fread(&format_code, 2, 1, input);
	if(format_code != 1)
	{
		exit_gracefully(input, filename.c_str());
	}

	fread(&channels, 2, 1, input);
	if(channels != 2)
	{
		exit_gracefully(input, filename.c_str());
	}

	fread(&samples_per_sec, 4, 1, input);
	fseek(input, 4, SEEK_CUR);
	fread(&block_align, 2, 1, input);
	fread(&bits_per_sample, 2, 1, input);

	//data chunk
	fread(chunk_id, 4, 1, input);
	if(strncmp(chunk_id, "data", 4) != 0)
	{
		exit_gracefully(input, filename.c_str());
	}

	//read data from file
	fread(&chunk_size, 4, 1, input);
	data = vector<char>(chunk_size + 1);
	fread(data.data(), 1, chunk_size, input);
	fclose(input);

	sample_size = (bits_per_sample / 8);
	samples = chunk_size / sample_size;
}

//write wave file to new filename
void Wav::write(string filename)
{
	FILE* output = fopen(filename.c_str(), "wb");
	fwrite("RIFF", 4, 1, output);
	fwrite(&file_size, 4, 1, output);
	fwrite("WAVE", 4, 1, output);
	fwrite("fmt ", 4, 1, output);

	//fmt chunk size
	uint32_t temp = 16;
	fwrite(&temp, 4, 1, output);

	fwrite(&format_code, 2, 1, output);
	fwrite(&channels, 2, 1, output);
	fwrite(&samples_per_sec, 4, 1, output);

	//bytes_per_sec
	temp = samples_per_sec * (bits_per_sample / 8) * channels;
	fwrite(&temp, 4, 1, output);

	fwrite(&block_align, 2, 1, output);
	fwrite(&bits_per_sample, 2, 1, output);
	
	//data chunk
	uint32_t chunk_size = sample_size * samples;
	fwrite("data", 4, 1, output);
	fwrite(&chunk_size, 4, 1, output);
	fwrite(data.data(), 1, chunk_size, output);

	//check if we need a padding byte
	if(chunk_size % 2 != 0)
	{
		temp = 0;
		fwrite(&temp, 1, 1, output);
	}
	fclose(output);
}
