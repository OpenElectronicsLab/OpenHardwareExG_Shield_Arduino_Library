#!/bin/bash
grep -i "warning\|error" ide.out | grep -v '^/home/eric/.arduino15/'
