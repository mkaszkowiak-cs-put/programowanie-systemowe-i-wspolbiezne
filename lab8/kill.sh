#!/bin/bash
ps aux | grep "/home/maciej/systemowe/lab" | grep -v "grep" | awk '{print $2}' | xargs kill
