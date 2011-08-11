#!/bin/sh
opt="-Os"
o_jump="-DUSE_FAST_JUMPS"
o_comb="-DUSE_COMBINE_INSTRUCTIONS"
sample="samples/yapi.bf"

echo "Building all..."
g++ src/*.cpp -Isrc/ -O0 -o bf_base
g++ src/*.cpp -Isrc/ $opt -o bf_none
g++ src/*.cpp -Isrc/ $opt $o_jump -o bf_jump
g++ src/*.cpp -Isrc/ $opt $o_comb -o bf_combine
g++ src/*.cpp -Isrc/ $opt $o_jump $o_comb -o bf_jump_combine

echo "Benchmarking..."
echo "No optimization, no jump, no combining"
time ./bf_base $sample

echo "Optimized, no jump, no combining"
time ./bf_none $sample

echo "Optimized, jump, no combine"
time ./bf_jump $sample

echo "Optimized, no jump, combine"
time ./bf_combine $sample

echo "Optimized, jump, combine"
time ./bf_jump_combine $sample
