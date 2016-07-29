#!/bin/sh

cd denali
csky-elf-gdb denali_dvbc.elf
cd -

sleep 1

csky-elf-gdb out.elf



