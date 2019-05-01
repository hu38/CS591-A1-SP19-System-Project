#!/bin/sh
cd main
mkdir lsm_data
make main
rm -rf lsm_data/*
./main Insert 4 5 t
