import numpy as np
import matplotlib.pyplot as plt
from scipy.io import wavfile
from scipy.signal import spectrogram

def plot_waterfall(wav_file, window_size=1024, overlap=512):
    # Read the WAV file
    sample_rate, data = wavfile.read(wav_file)

    # Compute the spectrogram
    f, t, Sxx = spectrogram(data, fs=sample_rate, nperseg=window_size, noverlap=overlap)

    # Plot the waterfall plot
    plt.pcolormesh(t, f, 10 * np.log10(Sxx), shading='auto')
    plt.ylabel('Frequency [Hz]')
    plt.xlabel('Time [sec]')
    plt.title('Waterfall Plot')
    plt.colorbar(label='Power/Frequency (dB/Hz)')
    plt.show()

# Example usage
plot_waterfall("P:\Temp\sample.wav")
