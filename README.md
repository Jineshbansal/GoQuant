# OrderBook C++ Project

This project is a C++ implementation of an OrderBook for cryptocurrency trading, specifically designed to interact with the [test.deribit.com](https://test.deribit.com) API. The project uses the JSON and cURL libraries to make API requests and manage trading operations. Key functionalities include modifying, canceling, buying, selling, and viewing current positions.

## Table of Contents

- [Overview](#overview)
- [Features](#features)
- [Dependencies](#dependencies)
- [Installation](#installation)
- [Usage](#usage)
- [API reference](#api-reference)


## Overview

The OrderBook project enables users to simulate and test trading functionality on the Deribit testnet. By making API calls, the program manages orders with actions such as placing, modifying, canceling, and viewing positions. This is ideal for anyone looking to practice and test automated trading strategies in a sandbox environment.

## Features

- **Buy and Sell Orders**: Place orders to buy or sell.
- **Modify Orders**: Adjust order parameters.
- **Cancel Orders**: Remove active orders from the book.
- **Show Positions**: Display current open positions and order status.

## Dependencies

The project uses the following external libraries:
- [cURL](https://curl.se/libcurl/) – for handling HTTP requests to the Deribit API.
- [JSON for Modern C++](https://github.com/nlohmann/json) – for parsing and generating JSON data.

```bash
# Installing Dependencies

# For Ubuntu/Debian
sudo apt update
sudo apt install libcurl4-openssl-dev

# For JSON for Modern C++
# Download and add `json.hpp` from the JSON for Modern C++ repository:
# https://github.com/nlohmann/json
```
## Installation
```bash
# Clone the repository
git clone https://github.com/your-username/OrderBook.git
cd OrderBook

# Compile the project
g++ -o orderbook main.cpp -lcurl
```

## Usage
```bash
# Run the application
./orderbook
```

## API Reference
```bash
The project interacts with the Deribit API to perform trading actions. Consult the API documentation for details on the available endpoints and required authentication.
```
