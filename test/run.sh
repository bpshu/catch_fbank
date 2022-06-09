wav_scp="/mnt/d/gitlab/VAD/test/data/testData_byHand/wav.scp"
wav_scp="/mnt/d/github/catch_fbank/test/tmp/wav.scp"
featfile="/mnt/d/github/catch_fbank/test/tmp/feat.2"

# wav_scp="tmp/wav.scp"
featfile="/mnt/d/github/catch_fbank/test/tmp/feat.1"
rm -rf $featfile*

featscp="/mnt/d/github/catch_fbank/test/tmp/feat.scp"
../build/catch_fbank_multithread_main_2nd \
    --wav_scp ${wav_scp} \
    --featfile ${featfile} \
    --featscp ${featscp} \
    --num_samples_to_run 4 \
    --num_threads 2

cat ${featscp}.* | sort >${featscp}
rm -f ${featscp}.*

for x in $featfile.*; do
    # echo $x
    ../../kaldi-trunk/src/featbin/copy-feats --binary=true ark,t:$x ark:$x.ark
done
#
