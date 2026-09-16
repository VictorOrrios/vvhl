#!/bin/bash

clear && cmake --build build -j$(nproc) && ./build/Examples/Triangle/Triangle
