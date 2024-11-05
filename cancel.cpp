#include <iostream>
#include <string>
#include <curl/curl.h>
#include <json/json.h>
#include "functions.h"
// Function to handle the response from curl
static size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* output) {
    size_t totalSize = size * nmemb;
    output->append(static_cast<const char*>(contents), totalSize);
    return totalSize;
}

// Function to take order cancellation input
std::string takeOrderCancellationInput() {
    std::string orderId;
    std::cout << "Enter order ID to cancel: ";
    std::cin >> orderId; // Get the order ID from the user
    return orderId;
}

// Function to cancel an order using JSON-RPC via curl
void cancelOrder(const std::string& accessToken) {
    std::string orderId = takeOrderCancellationInput(); // Get the order ID

    // Prepare the JSON-RPC request
    Json::Value request;
    request["jsonrpc"] = "2.0";
    request["id"] =1; // Use the order ID as the request ID
    request["method"] = "private/cancel"; // Method to cancel order

    // Add parameters to the request
    request["params"]["order_id"] = orderId;

    // Convert the request to a string
    Json::StreamWriterBuilder writer;
    std::string requestData = Json::writeString(writer, request);

    // Initialize curl
    CURL* curl;
    CURLcode res;
    std::string readBuffer;

    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl = curl_easy_init();

    if (curl) {
        // Set the URL for canceling orders
        curl_easy_setopt(curl, CURLOPT_URL, "https://test.deribit.com/api/v2/private/cancel");

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

            std::istringstream s(readBuffer);
            if (Json::parseFromStream(reader, s, &responseJson, &errors)) {
                std::cout << "ID: " << responseJson["id"].asInt() << std::endl;
                std::cout << "JSON-RPC Version: " << responseJson["jsonrpc"].asString() << std::endl;
                const Json::Value& result = responseJson["result"];

                std::cout << "Order ID: " << result["order_id"].asString() << std::endl;
                std::cout << "Cancellation Status: " << result["cancellation_status"].asString() << std::endl; // Adjust based on the actual response structure
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


