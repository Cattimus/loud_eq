#include "wav.hpp"

Wav::~Wav()
{
	if(data != NULL)
	{
		free(data);
		data = NULL;
	}
}

Wav::Wav(string filename)
{
	FILE* input = fopen(filename.c_str(), "rb");
	char chunk_id[4];
	data = NULL; //make sure to null data pointer so we don't run into issues

	//chunk ID for the first chunk should be RIFF
	fread(chunk_id, 4, 1, input);
	if(strncmp(chunk_id, "RIFF", 4) != 0) 
	{
		printf("ERROR: Wav file %s is not valid.\n", filename.c_str());
		return;
	}

	//skip chunk size for the first chunk, since it does not matter
	fseek(input, 4, SEEK_CUR);

	//WAVE ID
	fread(chunk_id, 4, 1, input);
	if(strncmp(chunk_id, "WAVE", 4) != 0)
	{
		printf("ERROR: Wav file %s is not valid.\n", filename.c_str());
		return;
	}

	//fmt chunk
	fread(chunk_id, 4, 1, input);
	if(strncmp(chunk_id, "fmt ", 4) != 0)
	{
		printf("ERROR: Wav file %s is not valid.\n", filename.c_str());
		return;
	}

	uint32_t chunk_size = 0;
	fread(&chunk_size, 4, 1, input);
	if(chunk_size != 16)
	{
		printf("ERROR: Wav file %s is not supported.\n", filename.c_str());
		return;
	}

	fread(&format_code, 2, 1, input);
	if(format_code != 1)
	{
		printf("ERROR: Wav file %s is not supported.\n", filename.c_str());
		return;
	}

	fread(&channels, 2, 1, input);
	if(channels != 2)
	{
		printf("ERROR: Wav file %s is not supported.\n", filename.c_str());
		return;
	}

	fread(&samples_per_sec, 4, 1, input);
	fseek(input, 4, SEEK_CUR);
	fread(&block_align, 2, 1, input);
	fread(&bits_per_sample, 2, 1, input);

	//data chunk
	fread(chunk_id, 4, 1, input);
	if(strncmp(chunk_id, "data", 4) != 0)
	{
		printf("ERROR: Wav file %s is not supported.\n", filename.c_str());
		return;
	}

	//read data from file
	fread(&chunk_size, 4, 1, input);
	data = (char*)malloc(chunk_size);
	fread(data, 1, chunk_size, input);
	fclose(input);

	samples = chunk_size / (2 * (bits_per_sample / 8));
}
