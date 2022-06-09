#!/usr/bin
wav_scp="/mnt/d/github/catch_fbank/test/tmp/wav.scp"
featfile="/mnt/d/github/catch_fbank/test/tmp/feat.1"
rm -rf $featfile*
featscp="/mnt/d/github/catch_fbank/test/tmp/feat.scp"

num_samples_to_run=4
num_threads=2

../build/catch_fbank_multithread_main_2nd \
    --wav_scp ${wav_scp} \
    --featfile ${featfile} \
    --featscp ${featscp} \
    --num_samples_to_run $num_samples_to_run \
    --num_threads $num_threads

cat ${featscp}.* | sort >${featscp}
rm -f ${featscp}.*

# for x in $featfile.*; do
#     # echo $x
#     ../../kaldi-trunk/src/featbin/copy-feats --binary=true ark,t:$x ark:$x.ark
# done
# #
