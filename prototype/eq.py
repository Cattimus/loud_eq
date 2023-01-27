import wave
import struct
import math

#TODO - implement proof of concept for upward and downward compressors
#TODO - clean up code and abstract things into functions and/or classes

#constant values
sample_range = 2**15

#convert db to amplitude(16 bit signed)
def db_to_amp(db):
	return 10**(db / 20) * sample_range

#convert amplitude to db(16 bit signed)
def amp_to_db(amp):
	return 20 * math.log10(amp / sample_range)

#global values
window_size_ms = 200
sliding_scale_ms = 100
target_volume_db = -6
target_volume_amp = db_to_amp(target_volume_db)

#open wav file
infile = wave.open("test.wav", "rb")
params = infile.getparams()
samplewidth = infile.getsampwidth()
framesize = infile.getframerate()
channels = infile.getnchannels()
total_frames = infile.getnframes()

#calculate how many windows and frames to read
window_size_frames = int((framesize / 1000) * window_size_ms)
scale_size_frames = int((framesize / 1000) * sliding_scale_ms)
scale_size_bytes = scale_size_frames * 4
window_size_bytes = window_size_frames * 4
scales_in_file = int(total_frames / scale_size_frames * 4)

#entirety of the samples
samples = infile.readframes(total_frames)
output_samples = bytearray(total_frames * 4)
infile.close()

#get amplitude of window at the index
def get_average(index):
	
	l_total = 0
	r_total = 0
	for i in range(index, index + window_size_bytes, 4):

		#if we exceed the boundaries of our sample buffer
		if (i >= len(samples)):
			break

		sample = struct.unpack_from("<hh", samples, offset=i)
		l_total += sample[0]**2
		r_total += sample[1]**2

	#calculate RMS of each channel
	l_rms = math.sqrt(l_total / window_size_frames)
	r_rms = math.sqrt(r_total / window_size_frames)

	return (l_rms, r_rms)

#get the peak volume from the sample
def get_peak(index):
	
	peak_l = 0
	peak_r = 0
	for i in range(index, index + window_size_bytes, 4):

		#if we exceed the boundaries of our sample buffer
		if (i >= len(samples)):
			break

		sample = struct.unpack_from("<hh", samples, offset=i)
		if(abs(sample[0]) > peak_l):
			peak_l = abs(sample[0])
		if(abs(sample[1]) > peak_r):
			peak_r = abs(sample[1])

	return (peak_l, peak_r)

#adjust data for window
def adjust_data(index, amp_adjust):
	
	for i in range(index, index + window_size_bytes, 4):

		#if we exceed the boundaries of our sample buffer
		if (i >= len(samples)):
			break
		
		#retrieve values
		sample = struct.unpack_from("<hh", samples, offset=i)

		#adjust values up or down to target
		lval = int(sample[0] * amp_adjust[0])
		rval = int(sample[1] * amp_adjust[1])

		#write values back to array
		try:
			struct.pack_into("<hh", output_samples, i, lval, rval)
		except:
			print("Error: sample value is out of range. this should never happen.")
			print("Adjusted samples: ", lval, rval)
			print("Scaling values: ", amp_adjust[0], amp_adjust[1])
			struct.pack_into("<hh", output_samples, i, sample[0], sample[1])

#adjust data for window
def copy_data(index):
	
	for i in range(index, index + window_size_bytes, 4):

		#if we exceed the boundaries of our sample buffer
		if (i >= len(samples)):
			break
		
		#retrieve values
		sample = struct.unpack_from("<hh", samples, offset=i)

		#write sample to output
		struct.pack_into("<hh", output_samples, i, int(sample[0]), int(sample[1]))

#go through each window of file (50ms)
for i in range(0, scales_in_file):
	peak = get_peak(i * scale_size_bytes)
	
	l_scale = 1
	r_scale = 1
	try:
		#calculate adjustment for each channel
		l_scale = abs(target_volume_amp / peak[0])
		r_scale = abs(target_volume_amp / peak[1])
	except:
		l_scale = 1
			
	#adjust into buffer for output
	adjust_data(i * scale_size_bytes, (l_scale, r_scale))

#output data to new wav file
outfile = wave.open("output.wav", "wb")
outfile.setparams(params)
outfile.writeframes(output_samples)
outfile.close()
