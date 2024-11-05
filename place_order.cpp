#include <iostream>
#include <string>
#include <curl/curl.h>
#include <json/json.h>
#include <limits> // For input validation
#include "functions.h"

// Struct to hold order details
struct Order {
    std::string instrumentName;
    std::string side;  // "buy" or "sell"
    std::string type;  // "limit" or "market"
    double amount;
    double price;      // Only for limit orders
};

// Function to handle the response from curl
static size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* output) {
    size_t totalSize = size * nmemb;
    output->append(static_cast<const char*>(contents), totalSize);
    return totalSize;
}

// Function to validate input (ensure it's a double)
double getValidatedDouble(const std::string& prompt) {
    double value;
    while (true) {
        std::cout << prompt;
        std::cin >> value;
        if (std::cin.fail()) {
            std::cin.clear(); // Clear the error flag
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Discard invalid input
            std::cout << "Invalid input. Please enter a valid number." << std::endl;
        } else {
            break;
        }
    }
    return value;
}

// Function to take order input in a more elegant way
Order takeOrderInput() {
    Order order;
    
    // Taking instrument name input
    std::cout << "Enter instrument name (e.g., BTC-PERPETUAL): ";
    std::cin >> order.instrumentName;

    // Taking side (buy or sell) with input validation
    while (true) {
        std::cout << "Enter side (buy/sell): ";
        std::cin >> order.side;
        if (order.side == "buy" || order.side == "sell") {
            break;
        } else {
            std::cout << "Invalid input. Please enter either 'buy' or 'sell'." << std::endl;
        }
    }

    // Taking order type (limit or market) with input validation
    while (true) {
        std::cout << "Enter order type (limit/market): ";
        std::cin >> order.type;
        if (order.type == "limit" || order.type == "market") {
            break;
        } else {
            std::cout << "Invalid input. Please enter either 'limit' or 'market'." << std::endl;
        }
    }

    // Taking and validating amount
    order.amount = getValidatedDouble("Enter amount: ");
    
    // If the order is of type "limit", also take the price and post-only flag
    if (order.type == "limit") {
        order.price = getValidatedDouble("Enter price: ");
        
    }

    return order;
}

// Function to place a buy or sell order using JSON-RPC via curl
void placeOrder(const std::string& accessToken) {
    const Order order = takeOrderInput();

    // Prepare the JSON-RPC request
    Json::Value request;
    request["jsonrpc"] = "2.0";
    request["id"] = 1;
    
    // Setting the method based on side (buy or sell)
    if (order.side == "buy") {
        request["method"] = "private/buy";
    } else if (order.side == "sell") {
        request["method"] = "private/sell";
    }

    // Add parameters to the request
    request["params"]["instrument_name"] = order.instrumentName;
    request["params"]["contracts"] = order.amount;
    request["params"]["type"] = order.type;
    // Add price and postOnly only if it's a limit order
    if (order.type == "limit") {
        request["params"]["price"] = order.price;
    }

    // Assign an empty array to "otococonfig"
    request["params"]["otococonfig"] = Json::arrayValue;

    // Convert the request to a string
    Json::StreamWriterBuilder writer;
    std::string requestData = Json::writeString(writer, request);

    // Determine URL based on side (buy or sell)
    std::string apiUrl = (order.side == "buy") ? "https://test.deribit.com/api/v2/private/buy" 
                                               : "https://test.deribit.com/api/v2/private/sell";

    // Initialize curl
    CURL* curl;
    CURLcode res;
    std::string readBuffer;

    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl = curl_easy_init();

    if (curl) {
        // Set the correct URL for buy or sell
        curl_easy_setopt(curl, CURLOPT_URL, apiUrl.c_str());

        // Set headers including authorization token
        struct curl_slist* headers = NULL;
        headers = curl_slist_append(headers, ("Authorization: Bearer " + accessToken).c_str());
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

        // Set the POST data and callback functions
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, requestData.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);

        // Perform the request
        res = curl_easy_perform(curl);
        if (res != CURLE_OK) {
            std::cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << std::endl;
        } else {
            // Parse the response
            Json::CharReaderBuilder reader;
            Json::Value responseJson;
            std::string errors;
            // std::cout<<readBuffer<<std::endl;
            std::istringstream s(readBuffer);
            if (Json::parseFromStream(reader, s, &responseJson, &errors)) {
                // Printing the general response details
                std::cout << "Response ID: " << responseJson["id"].asInt() << std::endl;
                std::cout << "JSON-RPC Version: " << responseJson["jsonrpc"].asString() << std::endl;
                
                const Json::Value& result = responseJson["result"];

                // Print trades info if it exists
                if (result.isMember("trades")) {
                    const Json::Value& trades = result["trades"];
                    std::cout << "\nTrades Info:" << std::endl;
                    for (const auto& trade : trades) {
                        std::cout << "  Trade ID: " << trade["trade_id"].asString() << std::endl;
                        std::cout << "  Liquidity: " << trade["liquidity"].asString() << std::endl;
                        std::cout << "  Order Type: " << trade["order_type"].asString() << std::endl;
                        std::cout << "  Instrument Name: " << trade["instrument_name"].asString() << std::endl;
                        std::cout << "  Contracts: " << trade["contracts"].asDouble() << std::endl;
                        std::cout << "  Direction: " << trade["direction"].asString() << std::endl;
                        std::cout << "  Price: " << trade["price"].asDouble() << std::endl;
                        std::cout << "  Amount: " << trade["amount"].asDouble() << std::endl;
                        std::cout << "  Mark Price: " << trade["mark_price"].asDouble() << std::endl;
                        std::cout << "  Fee: " << trade["fee"].asDouble() << " " << trade["fee_currency"].asString() << std::endl;
                        std::cout << "  Profit/Loss: " << trade["profit_loss"].asDouble() << std::endl;
                        std::cout << "  State: " << trade["state"].asString() << std::endl;
                        std::cout << "  Timestamp: " << trade["timestamp"].asInt64() << std::endl;
                        std::cout << "  ----------------------------" << std::endl;
                    }
                }

                // Print order info if it exists
                if (result.isMember("order")) {
                    const Json::Value& order = result["order"];
                    std::cout << "\nOrder Info:" << std::endl;
                    std::cout << "  Order ID: " << order["order_id"].asString() << std::endl;
                    std::cout << "  Instrument Name: " << order["instrument_name"].asString() << std::endl;
                    std::cout << "  Order State: " << order["order_state"].asString() << std::endl;
                    std::cout << "  Contracts: " << order["contracts"].asDouble() << std::endl;
                    std::cout << "  Average Price: " << order["average_price"].asDouble() << std::endl;
                    std::cout << "  Filled Amount: " << order["filled_amount"].asDouble() << std::endl;
                    std::cout << "  Time in Force: " << order["time_in_force"].asString() << std::endl;
                    std::cout << "  Direction: " << order["direction"].asString() << std::endl;
                    std::cout << "  Creation Timestamp: " << order["creation_timestamp"].asInt64() << std::endl;
                    std::cout << "  Last Update Timestamp: " << order["last_update_timestamp"].asInt64() << std::endl;
                    std::cout << "  Price: " << order["price"].asDouble() << std::endl;
                    std::cout << "  ----------------------------" << std::endl;
                }
            } else {
                std::cerr << "Failed to parse JSON response: " << errors << std::endl;
            }
        }
        // Clean up
        curl_easy_cleanup(curl);
        curl_slist_free_all(headers); // Free the headers
    }

    curl_global_cleanup();
}
