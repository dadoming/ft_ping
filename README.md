# ft_ping

This project is part of the Network branch of the 42 curriculum.

## Table of Contents

- [Features](#features)
- [Installation](#installation)
- [Usage](#usage)
  - [Options](#options)

## Description

`ft_ping` is a simplified implementation of the classic `ping` utility, designed to send ICMP (Internet Control Message Protocol) echo requests to network hosts and measure the response time of the echo replies.<br>

## Features

- **ICMP Packet Handling:** Sends ICMP echo requests and listens for echo replies.
- **Customizable with Flags:**
  - `-c [count]`: Stop after sending and receiving a specified number of echo requests.
  - `-v`: Verbose mode for detailed output.
  - `-q`: Quiet mode, displaying only summary statistics.
  - `-n`: Numeric output, bypassing DNS lookups.
- **Statistics:** Provides round-trip time statistics, including min, max, and average times.

## Installation

Clone this repository and navigate to the project directory:

```bash
git clone https://github.com/dadoming/ft_ping.git
cd ft_ping
```

Compile the program using the provided Makefile:

```bash
make
```

## Usage

Run the program with the target host as an argument:

```bash
./ft_ping [hostname]
```

### Options

- `-c [count]`: Stop after sending and receiving a specified number of echo requests.
- `-v`: Verbose mode for detailed output.
- `-q`: Quiet mode, displaying only summary statistics.
- `-n`: Numeric output, bypassing DNS lookups.
