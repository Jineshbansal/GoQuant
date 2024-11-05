#include <iostream>
#include <string>
#include <curl/curl.h>
#include <json/json.h>
#include "functions.h"

static size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* output) {
    size_t totalSize = size * nmemb;
    output->append((char*)contents, totalSize);
    return totalSize;
}

void fetchOrderBook(const std::string& instrument_name, int depth = 1) {
    // Set up the API URL and request parameters
    std::string url = "https://test.deribit.com/api/v2/public/get_order_book?instrument_name=" + instrument_name + "&depth=" + std::to_string(depth);

    CURL* curl;
    CURLcode res;
    std::string responseString;

    curl_global_init(CURL_GLOBAL_ALL);
    curl = curl_easy_init();
    if(curl) {
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &responseString);
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, NULL);
        
        // Perform the request
        res = curl_easy_perform(curl);
        if(res != CURLE_OK) {
            std::cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << std::endl;
        }
        // Clean up
        curl_easy_cleanup(curl);
    }
    curl_global_cleanup();

    // Parse the JSON response
    Json::Value jsonData;
    Json::CharReaderBuilder reader;
    std::string errs;

    std::istringstream responseStream(responseString);
    if (Json::parseFromStream(reader, responseStream, &jsonData, &errs)) {
        std::cout << "Order Book for " << instrument_name << ":\n";
        std::cout << "ID: " << jsonData["id"].asInt() << "\n";
        std::cout << "JSON-RPC: " << jsonData["jsonrpc"].asString() << "\n";
        std::cout << "Result:\n";

        const Json::Value& result = jsonData["result"];
        std::cout << "Instrument Name: " << result["instrument_name"].asString() << "\n";
        std::cout << "Best Bid Price: " << result["best_bid_price"].asFloat() << "\n";
        std::cout << "Best Bid Amount: " << result["best_bid_amount"].asFloat() << "\n";
        std::cout << "Best Ask Price: " << result["best_ask_price"].asFloat() << "\n";
        std::cout << "Best Ask Amount: " << result["best_ask_amount"].asFloat() << "\n";

        // Display bids
        std::cout << "\nBids:\n";
        for (const auto& bid : result["bids"]) {
            std::cout << "Price: " << bid[0].asFloat() << ", Amount: " << bid[1].asFloat() << "\n";
        }

        // Display asks
        std::cout << "\nAsks:\n";
        for (const auto& ask : result["asks"]) {
            std::cout << "Price: " << ask[0].asFloat() << ", Amount: " << ask[1].asFloat() << "\n";
        }
    } else {
        std::cerr << "Failed to parse JSON: " << errs << std::endl;
    }
}
