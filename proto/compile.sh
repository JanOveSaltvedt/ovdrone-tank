#!/bin/bash

protoc --cpp_out=../ ovdrone.proto
mv ../ovdrone.pb.cc ../ovdrone.pb.cpp

