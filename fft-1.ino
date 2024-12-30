//FFT Reading compiled code

#include "arduinoFFT.h"

// Configuration
const uint16_t samples = 128;       // Number of samples (must be a power of 2)
const double samplingFrequency = 5000; // Sampling frequency in Hz

// Pin Definitions
const int ADC_PIN = 34;             // ADC pin connected to the filtered LNA output

// FFT Variables
double vReal[samples];              // Real part of FFT input
double vImag[samples];              // Imaginary part of FFT input

// Create FFT object
ArduinoFFT<double> FFT = ArduinoFFT<double>(vReal, vImag, samples, samplingFrequency);

void setup() {
  Serial.begin(115200);
  pinMode(ADC_PIN, INPUT);
  Serial.println("Starting EMI Detection...");
}

void loop() {
  // Sample the ADC for FFT
  for (uint16_t i = 0; i < samples; i++) {
    unsigned long startMicros = micros();
    vReal[i] = analogRead(ADC_PIN); // Read ADC pin
    vImag[i] = 0.0;                 // Set imaginary part to 0
    while (micros() - startMicros < (1000000 / samplingFrequency)) {
      // Wait to maintain the correct sampling rate
    }
  }

  // Apply FFT
  FFT.windowing(FFTWindow::Hamming, FFTDirection::Forward); // Apply Hamming window
  FFT.compute(FFTDirection::Forward);                      // Perform FFT
  FFT.complexToMagnitude();                                // Calculate magnitudes

  // Find the dominant frequency
  double peakFrequency = FFT.majorPeak();
  if (peakFrequency > 0) {
    Serial.print("EMI Detected at Frequency: ");
    Serial.print(peakFrequency, 2);
    Serial.println(" Hz");
  } else {
    Serial.println("No significant EMI detected.");
  }

  delay(2000); // Optional delay between readings
}