#include <iostream>
#include <fstream>
#include <cstring>
#include <cmath>

void addShort(std::ofstream& file_out, short value) {
  char buffer[2];
  buffer[0] = value & 0xFF;
  buffer[1] = (value & 0xFF00) >> 8;
  file_out.write(buffer, 2);
}

void addInt(std::ofstream& file_out, int value) {
  char buffer[4];
  buffer[0] = value & 0xFF;
  buffer[1] = (value & 0xFF00) >> 8;
  buffer[2] = (value & 0xFF0000) >> 16;
  buffer[3] = (value & 0xFF000000) >> 24;
  file_out.write(buffer, 4);
}

void addSample(std::ofstream& file_out, float sample) {
  char buffer[2];
  int16_t sample_16 = (int16_t)(sample * 32767);
  buffer[0] = sample_16 & 0xFF;
  buffer[1] = (sample_16 >> 8) & 0xFF;
  file_out.write(buffer, 2);
}

void get_freq(int& freq, char note, int octave) {
  char note_table[12] = {'a', 'A', 'b', 'c', 'C', 'd', 'D', 'e', 'f', 'F', 'g', 'G'};
  int freq_table[12] = {440, 466, 494, 523, 554, 587, 622, 659, 698, 740, 784, 831};
  for (int i = 0; i < 12; ++i) {
    if (note_table[i] == note) {
      if (octave == 0) freq = freq_table[i] / 2;
      else if (octave == 1) freq = freq_table[i];
      else if (octave == 2) freq = freq_table[i] * 2;
      else if (octave == 3) freq = freq_table[i] * 4;
      else if (octave == 4) freq = freq_table[i] * 8;
      else if (octave == 5) freq = freq_table[i] * 16;
      else freq = 0;
      return;
    }
  }
}

int main(int argc, char* argv[]) {
  char bpm[8];

  if (argc != 2) {
    std::cout << "Usage: " << argv[0] << " [filename].txt" << std::endl;
    return -1;
  }

  std::ifstream file_in;
  std::string input_path = std::string("music/") + argv[1];
  file_in.open(input_path.c_str(), std::ios::binary);

  if (!file_in) {
    std::cout << "Failed opening .txt file: " << input_path << '\n';
    return -1;
  }

  // Derive output filename from input filename
  std::string input_base = argv[1];
  size_t dot_pos = input_base.find_last_of('.');
  if (dot_pos == std::string::npos) {
    std::cout << "Invalid input filename: no extension" << '\n';
    return -1;
  }
  input_base = input_base.substr(0, dot_pos);
  std::string output_filename = input_base + ".wav";

  file_in >> bpm;

  std::string output_path = std::string("wav/") + output_filename;
  std::ofstream file_out;
  file_out.open(output_path.c_str(), std::ios::binary | std::ios::trunc);

  if (!file_out) {
    std::cout << "Failed opening wav file: " << output_path << '\n';
    return -1;
  }

  //RIFF
  //ChunkID - offset: 0 bytes size: 4 bytes - big endian
  file_out.write("RIFF", 4);

  //ChunkSize - offset: 4 bytes size: 4 bytes - little endian
  int chunk_size;
  chunk_size = 4 + 8 + 16 + 8 + 0;
  addInt(file_out, chunk_size);
  //update later

  //Format - offset: 8 bytes size: 4 bytes - big endian
  file_out.write("WAVE", 4);


  //fmt sub-chunk
  //Subchunk1ID - offset: 12 bytes size: 4 bytes - big endian
  file_out.write("fmt ", 4);

  //Subchunk1Size - offset: 16 bytes size: 4 bytes - little endian
  int sub_chunk1_size = 16; //PCM
  addInt(file_out, sub_chunk1_size);

  //AudioFormat - offset: 20 bytes size: 2 bytes - little endian
  short audio_format = 1;
  addShort(file_out, audio_format);

  //NumChannels - offset: 22 bytes size: 2 bytes - little endian
  short num_channels = 1; //Mono
  addShort(file_out, num_channels);

  //SampleRate - offset: 24 bytes size: 4 bytes - little endian
  int sample_rate = 44100;
  addInt(file_out, sample_rate);

  //ByteRate - offset: 28 bytes size: 4 bytes - little endian
  short bits_per_sample = 16;
  int byte_rate = sample_rate * num_channels * bits_per_sample / 8;
  addInt(file_out, byte_rate);

  //BlockAlign - offset: 32 bytes size: 2 bytes - little endian
  short block_align = num_channels * bits_per_sample / 8;
  addShort(file_out, block_align);

  //BitsPerSample - offset: 34 bytes size: 2 bytes - little endian
  addShort(file_out, bits_per_sample);


  //Data sub-chunk
  //Subchunk2ID - offset: 36 bytes size: 4 bytes - big endian
  file_out.write("data", 4);

  //Subchunk2Size - offset: 40 bytes size: 4 bytes - little endian
  int num_samples = 0;
  int sub_chunk2_size = num_samples * num_channels * bits_per_sample / 8;
  addInt(file_out, sub_chunk2_size);
  //update later


  char note;
  int octave, numerator, denominator, freq, num_bytes;
  float sec_per_beat, beats, duration;

  num_bytes = 0;
  sec_per_beat = 60.0 / atof(bpm);

  while (file_in >> note) {
    if (note != 's') {
      file_in >> octave;
      get_freq(freq, note, octave);
      if (freq == 0) {
        std::cout << "Invalid note: " << note << ' ' << octave << '\n';
      }
    }

    file_in >> numerator;
    file_in >> denominator;

    beats = 4.0 * numerator / denominator;
    duration = beats * sec_per_beat;

    num_samples = (int)(duration * sample_rate);

    for (int i = 0; i < num_samples; ++i) {
      float sample = (note == 's') ? 0.0 : std::cos(freq * i * 6.284 / sample_rate);
      addSample(file_out, sample);
    }

    //tracking size of samples (each sample is 2 bytes) to update Subchunk2Size later
    num_bytes += num_samples * 2;
  }

  //Update Subchunk2Size
  sub_chunk2_size = num_bytes;
  file_out.seekp(40, std::ios::beg);
  addInt(file_out, sub_chunk2_size);

  //Update chunk size
  chunk_size = 4 + 8 + sub_chunk1_size + 8 + sub_chunk2_size;
  file_out.seekp(4, std::ios::beg);
  addInt(file_out, chunk_size);

  file_in.close();
  file_out.close();

  std::cout << "Generated file: " << output_path << '\n';

  return 0;
}