import wave
import struct
import math

#compress the dynamic range of a wav file (as a proof of concept)
class Compressor:

	#we will be checking that every wav file is signed 16 bit PCM
	__sample_range = 2**15

	#any value below this will be increased by the upward compressor(db)
	up_threshold = -45
	__up_threshold_amp = 0

	#any value above this will be decreased by the downward compressor(db)
	down_threshold = -25
	__down_threshold_amp = 0

	#how aggressive do we want the compressor to be
	ratio = 3

	#any value below this will be ignored by the compressor(db)
	noise_floor = -60
	__noise_floor_amp = 0

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
	def __get_amp(index, buffer, length):
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
	def __get_peak(index, buffer, length):
	
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
		self.__up_threshold_amp = self.__db_to_amp(self.up_threshold)
		self.__down_threshold_amp = self.__db_to_amp(self.down_threshold)
		self.__noise_floor_amp = self.__db_to_amp(self.noise_floor)

		#initialize sample values
		self.__attack_time_samples = int((sample_rate / 1000) * self.attack_time_ms)
		self.__release_time_samples = int((sample_rate / 1000) * self.release_time_ms)

		compress_down = False
		down_timer = 1

		compress_up = False
		up_timer = 1
		
		#iterate through the samples by byte
		for i in range(0, len(samples), 4):

			#get current sample(mix of left and right)
			data = struct.unpack_from("<hh", samples, offset=i)

			#get the loudest of the two channels
			sample = 0
			if(abs(data[0]) > abs(data[1])):
				sample = abs(data[0])
			else:
				sample = abs(data[1])
			
			#if the volume of both channels exceeds the threshold, start compressing for the window of time
			if(sample > self.__down_threshold_amp):
				compress_down = True
			else:
				compress_down = False
				down_timer = 1

			if(sample < self.__up_threshold_amp and sample > self.__noise_floor_amp):
				compress_up = True
			else:
				compress_up = False
				up_timer = 1

			#downward (lower) compression
			if(compress_down):

				#caluclate how much to adjust data
				above = sample - self.__down_threshold_amp
				attack_percent = 1 + (self.ratio * (down_timer / self.__attack_time_samples))
				adjustment = (self.__down_threshold_amp + (above / attack_percent)) / sample
				l = int(data[0] * adjustment)
				r = int(data[1] * adjustment)

				#write back to buffer
				struct.pack_into("<hh", samples, i, l, r)
				
				#increment value to calculate reduction
				if(down_timer < self.__attack_time_samples):
					down_timer += 1

				continue

			#upward (higher) compression
			elif(compress_up):
				#calculate how much to increase the volume by
				#reduce both channels by the same amount
				#apply to output buffer
				continue
		
		outf = wave.open(out_file, "wb")
		outf.setparams(wavparams)
		outf.writeframes(samples)
		outf.close()
