import wave
import struct
import math

#compress the dynamic range of a wav file (as a proof of concept)
class Compressor:

	#we will be checking that every wav file is signed 16 bit PCM
	__sample_range = 2**15

	#any value above this will be decreased by the downward compressor(db)
	threshold = -25
	__threshold_amp = 0

	#how aggressive do we want the compressor to be
	ratio = 4

	#attack and release params
	attack_time_ms = 100
	__attack_time_samples = 0

	release_time_ms = 1000
	__release_time_samples = 0

	#convert db to amplitude(16 bit signed)
	def __db_to_amp(self, db):
		return 10**(db / 20) * self.__sample_range

	#convert amplitude to db(16 bit signed)
	def __amp_to_db(self, amp):
		return 20 * math.log10(amp / self.__sample_range)

	#get the average amplitude of a section of samples
	@staticmethod
	def __get_amp(index: int, buffer: bytes, length: int):
		l_total = 0
		r_total = 0
		for i in range(index, index + (length * 4), 4):

			#if we exceed the boundaries of our sample buffer
			if (i >= len(buffer)):
				break

			sample = struct.unpack_from("<hh", buffer, offset=i)
			l_total += sample[0]**2
			r_total += sample[1]**2

		#calculate RMS of each channel
		l_rms = math.sqrt(l_total / length)
		r_rms = math.sqrt(r_total / length)

		return (l_rms, r_rms)

	#find the peak value of a section of samples
	@staticmethod
	def __get_peak(index: int, buffer: bytes, length: int):
	
		peak_l = 0
		peak_r = 0
		for i in range(index, index + (length * 4), 4):

			#if we exceed the boundaries of our sample buffer
			if (i >= len(buffer)):
				break

			sample = struct.unpack_from("<hh", buffer, offset=i)
			if(abs(sample[0]) > peak_l):
				peak_l = abs(sample[0])
			if(abs(sample[1]) > peak_r):
				peak_r = abs(sample[1])

		return (peak_l, peak_r)

	#compress an audio file
	def compress(self, in_file: str, out_file: str):

		#open the wave file and establish some data
		inf = wave.open(in_file, "rb")
		wavparams = inf.getparams()

		#check to make sure our data is what we expect
		if(wavparams.sampwidth != 2):
			print("Compressor Error: wav file does not contain 16 bit signed PCM data.")
			return

		#read all samples into a buffer
		samples = bytearray(inf.readframes(wavparams.nframes))
		sample_rate = wavparams.framerate
		inf.close()

		#initialize amp values
		self.__threshold_amp = self.__db_to_amp(self.threshold)

		#initialize sample values
		self.__attack_time_samples = int((sample_rate / 1000) * self.attack_time_ms)
		self.__release_time_samples = int((sample_rate / 1000) * self.release_time_ms)

		#50ms window size
		window_size = int(sample_rate * (50 / 1000))

		#attack is the period of time to lower the volume
		#release is the period of time to bring the volume back up to normal
		#attack and release will be combined into one calculation to determine the gain to be applied
		#this gain will fluctuate over windows dependent upon if the window is above or below the threshold
		
		#iterate through the samples by byte
		for i in range(0, len(samples), 4 * window_size):

			#get current sample amplitude
			data = self.__get_amp(i, samples, window_size)

			#get the highest value of the two values
			sample = max(data[0], data[1])
			
			#our sample meets the criteria for volume lowering
			if(sample >= self.__threshold_amp):
				
				continue
		
		outf = wave.open(out_file, "wb")
		outf.setparams(wavparams)
		outf.writeframes(samples)
		outf.close()
