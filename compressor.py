import wave
import math
import numpy as np

#compress the dynamic range of a wav file (as a proof of concept)
class Compressor:

	#we will be checking that every wav file is signed 16 bit PCM
	__sample_range = 2**15

	#any value above this will be decreased by the downward compressor(db)
	threshold = -25
	__threshold_amp = 0

	#what value to normalize the audio to after it has been compressed
	normalize_db = -6
	__normalize_amp = 0

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
		dt = np.dtype(np.int16)
		dt = dt.newbyteorder("<")
		samples = np.array(np.frombuffer(inf.readframes(wavparams.nframes), dtype=dt))
		samples.setflags(write=True)
		sample_rate = wavparams.framerate
		inf.close()

		#initialize amp values
		self.__threshold_amp = self.__db_to_amp(self.threshold)

		#initialize sample values
		self.__attack_time_samples = int((sample_rate / 1000) * self.attack_time_ms)
		self.__release_time_samples = int((sample_rate / 1000) * self.release_time_ms)

		#50ms window size
		window_size = int(sample_rate * (50 / 1000))
		adjustment_max = max(self.attack_time_ms, self.release_time_ms)
		adjustment_amount = 0

		#attack is the period of time to lower the volume
		#release is the period of time to bring the volume back up to normal
		#attack and release will be combined into one calculation to determine the gain to be applied
		#this gain will fluctuate over windows dependent upon if the window is above or below the threshold


		
		#iterate through the samples by byte
		for i in range(0, len(samples), window_size):
			window = samples[i:i+window_size]

			#calculate amplitude of a sample (RMS)
			data = np.sqrt(np.mean(window.astype(np.int32)**2))

			#need to adjust this based on attack and release timings
			#limited to threshold + ratio
			above = data - self.__threshold_amp
			adjust = (self.__threshold_amp + (above / self.ratio)) / data
			
			#our sample meets the criteria for volume lowering
			if(data > self.__threshold_amp):
				np.multiply(window, adjust, out=window, casting="unsafe")
			else:
				continue
		
		outf = wave.open(out_file, "wb")
		outf.setparams(wavparams)
		outf.writeframes(samples.tobytes())
		outf.close()
