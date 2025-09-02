
//not compiling - filters not included?


#include "DaisyDuino.h"


DaisyHardware hw;

float chromatic[121] = {
  16.3516, 17.32391673, 18.35405043, 19.44543906, 20.60172504, 21.82676736, 23.12465449, 24.499718, 25.95654704, 27.50000365, 29.13523896, 30.86771042, 32.7032, 34.64783346, 36.70810085, 38.89087812, 41.20345007, 43.65353471, 46.24930897, 48.99943599, 51.91309407, 55.00000728, 58.27047791, 61.73542083, 65.40639999, 69.29566692, 73.4162017, 77.78175623, 82.40690014, 87.30706942, 92.49861792, 97.99887197, 103.8261881, 110.0000146, 116.5409558, 123.4708417, 130.8128, 138.5913338, 146.8324034, 155.5635124, 164.8138003, 174.6141388, 184.9972358, 195.9977439, 207.6523763, 220.0000291, 233.0819116, 246.9416833, 261.6255999, 277.1826676, 293.6648067, 311.1270248, 329.6276005, 349.2282776, 369.9944716, 391.9954878, 415.3047525, 440.0000581, 466.1638231, 493.8833665, 523.2511997, 554.3653352, 587.3296134, 622.2540496, 659.2552009, 698.4565551, 739.9889431, 783.9909755, 830.6095048, 880.0001162, 932.3276461, 987.7667329, 1046.502399, 1108.73067, 1174.659227, 1244.508099, 1318.510402, 1396.91311, 1479.977886, 1567.981951, 1661.219009, 1760.000232, 1864.655292, 1975.533466, 2093.004798, 2217.46134, 2349.318453, 2489.016198, 2637.020803, 2793.82622, 2959.955772, 3135.963901, 3322.438019, 3520.000464, 3729.310584, 3951.066931, 4186.009596, 4434.92268, 4698.636906, 4978.032395, 5274.041605, 5587.652439, 5919.911543, 6271.927802, 6644.876037, 7040.000927, 7458.621167, 7902.133861, 8372.019192, 8869.845359, 9397.273811, 9956.06479, 10548.08321, 11175.30488, 11839.82309, 12543.8556, 13289.75207, 14080.00185, 14917.24233, 15804.26772, 16744.03838
};

size_t num_channels;

static Oscillator osc1;
static Oscillator osc2;
static Oscillator lfo1;
static Adsr env1;
static OnePole filter1;

float pitchknob;
float env_out;
float sig;

uint32_t current_time = 0;
uint32_t prev_time_0 = 0;
uint32_t prev_time_1 = 0;
uint32_t interval_0 = 1000;
uint32_t interval_1 = 50;
uint32_t gate;
float note_to_osc1, note_to_osc2;

void MyCallback(float **in, float **out, size_t size) {

  for (size_t i = 0; i < size; i++) {

    float lfo1_out = note_to_osc1 + (lfo1.Process() * 500.0);
    filter1.SetFreq(lfo1_out);


    env_out = env1.Process(gate);
    float osc1_out = osc1.Process();
    float osc2_out = osc2.Process();
    sig = ((osc1_out + osc2_out) * .5) * env_out;

    sig = filter1.Process(sig);

    //osc1.SetAmp(env_out);

    for (size_t chn = 0; chn < num_channels; chn++) {
      out[chn][i] = sig;
    }
  }
}

void setup() {
  float sample_rate;
  // Initialize for Daisy pod at 48kHz
  hw = DAISY.init(DAISY_SEED, AUDIO_SR_48K);
  num_channels = hw.num_channels;
  sample_rate = DAISY.get_samplerate();

  osc1.Init(sample_rate);
  osc1.SetFreq(440);
  osc1.SetAmp(0.8);
  osc1.SetWaveform(osc1.WAVE_POLYBLEP_SQUARE);

  osc2.Init(sample_rate);
  osc2.SetFreq(440);
  osc2.SetAmp(0.8);
  osc2.SetWaveform(osc2.WAVE_POLYBLEP_SQUARE);

  lfo1.Init(sample_rate);
  lfo1.SetFreq(.25);
  lfo1.SetAmp(0.8);
  lfo1.SetWaveform(osc1.WAVE_SIN);


  env1.Init(sample_rate);

  env1.SetAttackTime(.01, 0);
  env1.SetDecayTime(.2);
  env1.SetReleaseTime(.25);
  env1.SetSustainLevel(.5);

  filter1.Init(sample_rate);

  filter1.SetFilterMode(filter1.FILTER_MODE_LOW_PASS);
  //filter1.SetRes(.5);

  DAISY.begin(MyCallback);
}


void loop() {

  //how timing works : https://github.com/BleepLabs/dadageek-Feb21/blob/386f8c5c10dc9e32bb39fe438169f094fc1f2ba8/Class-1/dg_class1_-_2_Blink_two_LEDS/dg_class1_-_2_Blink_two_LEDS.ino
  current_time = millis();

  if (current_time - prev_time_0 > interval_0) {
    prev_time_0 = current_time;

    gate = !gate;

    if (gate == 1) {
      int random1 = random(20, 40);
      note_to_osc1 = chromatic[random1];
      note_to_osc2 = note_to_osc1 * .501;
      osc1.SetFreq(note_to_osc1);
      osc2.SetFreq(note_to_osc2);
    }
  }

  if (current_time - prev_time_1 > interval_1) {
    prev_time_1 = current_time;

    Serial.println(env_out, 4);
  }
}
