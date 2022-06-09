#include <iostream>
#include <fstream>
#include <mutex>
#include <cmath>
#include <thread>
#include "frontend/feature_pipeline.h"
#include "utils/flags.h"
#include "frontend/wav.h"
#include "utils/log.h"

using namespace wenet;
using namespace std;

std::mutex Lock1;

DEFINE_int32(num_samples_to_run, 6, "");
DEFINE_int32(num_threads, 3, "");
DEFINE_string(wav_scp, "/mnt/d/gitlab/VAD/test/data/testData_byHand/wav.scp", "");
DEFINE_string(outfile, "/mnt/d/github/catch_fbank/test/tmp/feat.1", "");


// FeaturePipelineConfig flags
DEFINE_int32(num_bins, 40, "num mel bins for fbank feature");
DEFINE_int32(sample_rate, 8000, "sample rate for audio");

namespace wenet {
  std::shared_ptr<FeaturePipelineConfig> InitFeaturePipelineConfigFromFlags() {

    auto feature_config = std::make_shared<FeaturePipelineConfig>(
        FLAGS_num_bins, FLAGS_sample_rate);
    return feature_config;
  }
}


int lines = 0;

void split_line(const string& line, string& key, string& val) {
  if (line.empty()) return;
  int i = 0;
  while (i < line.size() && !isspace(line[i])) ++i;
  key = line.substr(0, i);
  val = line.substr(i + 1);
}

void process(int id, istream& wav_scp, std::shared_ptr<wenet::FeaturePipelineConfig> feature_config) {
    auto feature_pipeline = std::make_shared<wenet::FeaturePipeline>(*feature_config);

    string outfile = FLAGS_outfile + "." + to_string(id);
    ofstream out_f(outfile.c_str());

    string line, wav_id, wav_path;
    while (lines < FLAGS_num_samples_to_run) {
        std::unique_lock<std::mutex> lck1(Lock1);
        if (getline(wav_scp, line)) {
            lines++;
            lck1.unlock();
            split_line(line, wav_id, wav_path);
            WavReader wav_reader(wav_path);
            const int num_sample = wav_reader.num_sample();
            //
            std::vector<float> wav(wav_reader.data(), wav_reader.data() + wav_reader.num_sample());
            std::vector<int16_t>  wav1(160*5);
            for(int i=0; i< 160*5; i++){
              std::cout << wav[i] << " " << static_cast<int16_t>(wav[i])<<" ";
              // std::cout <<  static_cast<float>(static_cast<int16_t>(wav[i]))<<" ";
              if((i+1)%160 == 0){
                std::cout << std::endl;
              }
            }
            
           
            feature_pipeline->Reset();
            feature_pipeline->AcceptWaveform(std::vector<float>(wav_reader.data(), wav_reader.data() + wav_reader.num_sample()));
            feature_pipeline->set_input_finished();
            LOG(INFO) <<"num_frames " << feature_pipeline->num_frames();
            //
            int num_requried_frames = std::numeric_limits<int>::max();
            std::vector<std::vector<float>> chunk_feats;
            if (!feature_pipeline->Read(num_requried_frames, &chunk_feats))
              CHECK_EQ(chunk_feats.size(), feature_pipeline->num_frames());
            //
            if(chunk_feats.size() > 0)
              CHECK_EQ(chunk_feats[0].size(), FLAGS_num_bins);
            //
            out_f << wav_id;
            if(chunk_feats.size() > 0){
              for(int i=0; i<FLAGS_num_bins; i++)
                out_f << " " << chunk_feats[0][i];
              for(auto& x :chunk_feats){
                for(auto& y : x){
                  out_f <<" " << y;
                }
              }
              for(int i=0; i<FLAGS_num_bins; i++)
                out_f << " " << chunk_feats[chunk_feats.size()-1][i];
            }
            out_f << std::endl;

        }
    }

    out_f.close();
}



int main(int argc, char* argv[]) {
  gflags::ParseCommandLineFlags(&argc, &argv, false);
  auto feature_config = wenet::InitFeaturePipelineConfigFromFlags();
  ifstream wav_scp(FLAGS_wav_scp);
  vector<thread> threads;
  // int lines = FLAGS_num_samples_to_run / FLAGS_num_threads;
  // int rem = FLAGS_num_samples_to_run;
  for (int i = 0; i < FLAGS_num_threads; ++i) {
    // if (i == FLAGS_num_threads - 1) lines = rem;
    // threads.push_back(thread(process, i, std::ref(wav_scp), lines));
    threads.push_back(thread(process, i, std::ref(wav_scp), feature_config));
    // rem -= lines;
  }
  for (int i = 0; i < FLAGS_num_threads; ++i) threads[i].join();
  wav_scp.close();
  return 0;
}
