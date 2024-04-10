# ESHELL

![cool meme](https://imgur.com/UwPdmpw.jpg)

## Overview

This project simulates a Linux shell, incorporating nearly all standard functionalities and additional extensions. I have done this project for CENG334: Operating Systems course.

## Features

- Executes Linux commands
- Supports piping.
- Supports parallel execution.
- Supports sequential execution.
- Supports subshells
- No memory leaks, no zombie processes.

## Code Count

I'm not counting parser.h and parser.c here because it was written by TA.

| Language  | Files | Lines   | Code    | Comments | Blanks |
| --------- | ----- | ------- | ------- | -------- | ------ |
| C Header  | 2     | 53      | 44      | 0        | 9      |
| C++       | 3     | 386     | 349     | 18       | 19     |
| Makefile  | 1     | 3       | 3       | 0        | 0      |
| **Total** | **6** | **442** | **396** | **18**   | **28** |

## Benchmark

| Command | Mean [ms] | Min [ms] | Max [ms] | Relative |
|:---|---:|---:|---:|---:|
| `echo 'find . -name "*.txt" \| sort ' \| ./eshell ` | 6.8 ± 1.7 | 3.7 | 10.4 | 1.00 |
| `echo 'find . -name "*.txt" \| sort ' \| bash ` | 10.7 ± 0.9 | 7.9 | 11.9 | 1.58 ± 0.42 |


| Command | Mean [s] | Min [s] | Max [s] | Relative |
|:---|---:|---:|---:|---:|
| `echo 'ls -R / \| sort \| wc -l ' \| ./eshell` | 3.000 ± 0.141 | 2.871 | 3.241 | 1.02 ± 0.05 |
| `echo 'ls -R / \| sort \| wcl -l' \| bash` | 2.940 ± 0.021 | 2.911 | 2.973 | 1.00 |


| Command | Mean [s] | Min [s] | Max [s] | Relative |
|:---|---:|---:|---:|---:|
| `echo '/bin/rg -r -e "error" /home/bahadir/' \| ./eshell` | 7.923 ± 1.670 | 5.747 | 10.942 | 1.00 |
| `echo '/bin/rg -r -e "error" /home/bahadir/' \| bash` | 8.180 ± 1.580 | 6.560 | 11.706 | 1.03 ± 0.30 |

| Command | Mean [ms] | Min [ms] | Max [ms] | Relative |
|:---|---:|---:|---:|---:|
| `echo 'uptime; df -h; free -m \| tr /a-z/ /A-Z/' \| ./eshell` | 12.7 ± 1.8 | 10.0 | 16.2 | 1.00 |
| `echo 'uptime; df -h; free -m \| tr /a-z/ /A-Z/' \| bash` | 14.3 ± 3.2 | 4.3 | 17.2 | 1.12 ± 0.29 |

