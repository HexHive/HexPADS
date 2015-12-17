#!/bin/bash

if [ $# -eq 0 ]; then
    echo "$0 <test>, test is either CAIN, C6, CSC, or rowhammer"
    exit
fi

case "$1" in
"CAIN")
  echo "Running CAIN, code by Antonio Barresi"
  ./CAIN-anteater
  ;;
"C6")
  echo "Running C6, code by Clementine Maurice"
  ./C6-sender -c 25 -m 01011110010100101101000010010100011010001000010011010101101110101100001001011100001111000010101110000111100101011010101100000100010111100101001011010000100101000110100010000100110101011011101011000010010111000011110000101011100001111001010110101011000001000101111001010010110100001001010001101000100001001101010110111010110000100101110000111100001010111000011110010101101010110000010001011110010100101101000010010100011010001000010011010101101110101100001001011100001111000010101110000111100101011010101100000100010111100101001011010000100101000110100010000100110101011011101011000010010111000011110000101011100001111001010110101011000001000101111001010010110100001001010001101000100001001101010110111010110000100101110000111100001010111000011110010101101010110000010001011110010100101101000010010100011010001000010011010101101110101100001001011100001111000010101110000111100101011010101100000100010111100101001011010000100101000110100010000100110101011011101011000010010111000011110000101011100001111001010110101011000001000101111001010010110100001001010001101000100001001101010110111010110000100101110000111100001010111000011110010101101010110000010001011110010100101101000010010100011010001000010011010101101110101100001001011100001111000010101110000111100101011010101100000100010111100101001011010000100101000110100010000100110101011011101011000010010111000011110000101011100001111001010110101011000001000101111001010010110100001001010001101000100001001101010110111010110000100101110000111100001010111000011110010101101010110000010001011110010100101101000010010100011010001000010011010101101110101100001001011100001111000010101110000111100101011010101100000100010111100101001011010000100101000110100010000100110101011011101011000010010111000011110000101011100001111001010110101011000001000101111001010010110100001001010001101000100001001101010110111010110000100101110000111100001010111000011110010101101010110000010001011110010100101101000010010100011010001000010011010101101110101100001001011100001111000010101110000111100101011010101100000100010111100101001011010000100101000110100010000100110101011011101011000010010111000011110000101011100001111001010110101011000001000101111001010010110100001001010001101000100001001101010110111010110000100101110000111100001010111000011110010101101010110000010001011110010100101101000010010100011010001000010011010101101110101100001001011100001111000010101110000111100101011010101100000100010111100101001011010000100101000110100010000100110101011011101011000010010111000011110000101011100001111001010110101011000001000101111001010010110100001001010001101000100001001101010110111010110000100101110000111100001010111000011110010101101010110000010001011110010100101101000010010100011010001000010011010101101110101100001001011100001111000010101110000111100101011010101100000100010111100101001011010000100101000110100010000100110101011011101011000010010111000011110000101011100001111001010110101011000001000101111001010010110100001001010001101000100001001101010110111010110000100101110000111100001010111000011110010101101010110000010001011110010100101101000010010100011010001000010011010101101110101100001001011100001111000010101110000111100101011010101100000100010111100101001011010000100101000110100010000100110101011011101011000010010111000011110000101011100001111001010110101011000001000101111001010010110100001001010001101000100001001101010110111010110000100101110000111100001010111000011110010101101010110000010001011110010100101101000010010100011010001000010011010101101110101100001001011100001111000010101110000111100101011010101100000100010111100101001011010000100101000110100010000100110101011011101011000010010111000011110000101011100001111001010110101011000001000101111001010010110100001001010001101000100001001101010110111010110000100101110000111100001010111000011110010101101010110000010001011110010100101101000010010100011010001000010011010101101110101100001001011100001111000010101110000111100101011010101100000100010111100101001011010000100101000110100010000100110101011011101011000010010111000011110000101011100001111001010110101011000001000101111001010010110100001001010001101000100001001101010110111010110000100101110000111100001010111000011110010101101010110000010001011110010100101101000010010100011010001000010011010101101110101100001001011100001111000010101110000111100101011010101100000100010111100101001011010000100101000110100010000100110101011011101011000010010111000011110000101011100001111001010110101011000001000101111001010010110100001001010001101000100001001101010110111010110000100101110000111100001010111000011110010101101010110000010001011110010100101101000010010100011010001000010011010101101110101100001001011100001111000010101110000111100101011010101100000100010111100101001011010000100101000110100010000100110101011011101011000010010111000011110000101011100001111001010110101011000001000101111001010010110100001001010001101000100001001101010110111010110000100101110000111100001010111000011110010101101010110000010001011110010100101101000010010100011010001000010011010101101110101100001001011100001111000010101110000111100101011010101100000100010111100101001011010000100101000110100010000100110101011011101011000010010111000011110000101011100001111001010110101011000001000101111001010010110100001001010001101000100001001101010110111010110000100101110000111100001010111000011110010101101010110000010001011110010100101101000010010100011010001000010011010101101110101100001001011100001111000010101110000111100101011010101100000100010111100101001011010000100101000110100010000100110101011011101011000010010111000011110000101011100001111001010110101011000001000101111001010010110100001001010001101000100001001101010110111010110000100101110000111100001010111000011110010101101010110000010001011110010100101101000010010100011010001000010011010101101110101100001001011100001111000010101110000111100101011010101100000100010111100101001011010000100101000110100010000100110101011011101011000010010111000011110000101011100001111001010110101011000001000101111001010010110100001001010001101000100001001101010110111010110000100101110000111100001010111000011110010101101010110000010001011110010100101101000010010100011010001000010011010101101110101100001001011100001111000010101110000111100101011010101100000100010111100101001011010000100101000110100010000100110101011011101011000010010111000011110000101011100001111001010110101011000001000101111001010010110100001001010001101000100001001101010110111010110000100101110000111100001010111000011110010101101010110000010001011110010100101101000010010100011010001000010011010101101110101100001001011100001111000010101110000111100101011010101100000100
  ;;
"CSC")
  echo "Running CSC, code by Daniel Grauss"
  ./CTemplate-spy ./CTemplate-spy 1
  ;;
"rowhammer")
  echo "Running rowhammer, code by Google"
  ./rowhammer_test
  ;;
*)
  echo "$0 <test>, test is either CAIN, C6, CSC, or rowhammer"
  ;;
esac
