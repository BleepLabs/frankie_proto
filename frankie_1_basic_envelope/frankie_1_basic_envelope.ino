// Title: oscillator
// Description: Control a sine wave freq with a knob
// Hardware: Daisy Seed
// Author: Ben Sergentanis
// Breadboard
// https://raw.githubusercontent.com/electro-smith/DaisyExamples/master/seed/Osc/resources/Osc_bb.png
// Schematic:
// https://raw.githubusercontent.com/electro-smith/DaisyExamples/master/seed/Osc/resources/Osc_schem.png

#include "DaisyDuino.h"

DaisyHardware hw;

size_t num_channels;

static Oscillator osc1;
static Adsr env1;


float pitchknob;
float env_out;
float sig;

uint32_t current_time = 0;
uint32_t prev_time_0 = 0;
uint32_t prev_time_1 = 0;
uint32_t interval_0 = 4000;
uint32_t interval_1 = 50;
uint32_t gate;


void MyCallback(float **in, float **out, size_t size) {

  for (size_t i = 0; i < size; i++) {

    sig = osc1.Process();
    env_out = env1.Process(gate);
    osc1.SetAmp(env_out);

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
  osc1.SetWaveform(osc1.WAVE_TRI);

  env1.Init(sample_rate);

  env1.SetAttackTime(.25, 0);
  env1.SetDecayTime(.25);
  env1.SetReleaseTime(1);

  env1.SetSustainLevel(.25);

  DAISY.begin(MyCallback);
}


void loop() {

  //how timing works : https://github.com/BleepLabs/dadageek-Feb21/blob/386f8c5c10dc9e32bb39fe438169f094fc1f2ba8/Class-1/dg_class1_-_2_Blink_two_LEDS/dg_class1_-_2_Blink_two_LEDS.ino
  current_time = millis();

  if (current_time - prev_time_0 > interval_0) {
    prev_time_0 = current_time;

    gate = !gate;
  }

  if (current_time - prev_time_1 > interval_1) {
    prev_time_1 = current_time;

    Serial.println(env_out, 4);
  }
}