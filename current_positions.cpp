#include <iostream>
#include <string>
#include <curl/curl.h>
#include "json/json.h"
#include "functions.h"

// Function to handle the response from cURL
static size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* outBuffer) {
    size_t totalSize = size * nmemb;
    if (outBuffer) {
        outBuffer->append((char*)contents, totalSize);
    }
    return totalSize;
}

// Function to make a cURL request with authorization and get the response as a JSON object
Json::Value sendApiRequest(const std::string& url, const Json::Value& requestData, const std::string& authToken) {
    CURL* curl = curl_easy_init();
    std::string readBuffer;
    Json::Value responseJson;

    if (curl) {
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_POST, 1L);

        // Convert JSON requestData to string
        std::string jsonData = requestData.toStyledString();
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, jsonData.c_str());

        // Set headers, including Authorization
        struct curl_slist* headers = nullptr;
        headers = curl_slist_append(headers, "Content-Type: application/json");
        std::string bearerToken = "Authorization: Bearer " + authToken;
        headers = curl_slist_append(headers, bearerToken.c_str());
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

        // Set up callback to capture response
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);

        // Perform the request
        CURLcode res = curl_easy_perform(curl);
        if (res != CURLE_OK) {
            std::cerr << "cURL error: " << curl_easy_strerror(res) << std::endl;
        }

        // Clean up
        curl_slist_free_all(headers);
        curl_easy_cleanup(curl);
    }

    // Parse response JSON
    std::istringstream stream(readBuffer);
    stream >> responseJson;

    return responseJson;
}

// Function to display positions from the JSON response
void displayPositions(const Json::Value& positions) {
    for (const auto& position : positions) {
        std::cout << "Instrument Name: " << position["instrument_name"].asString() << std::endl;
        std::cout << "Size: " << position["size"].asDouble() << " " << position.get("size_currency", "USD").asString() << std::endl;
        std::cout << "Average Price: " << position["average_price"].asDouble() << std::endl;
        std::cout << "Mark Price: " << position["mark_price"].asDouble() << std::endl;
        std::cout << "Total Profit/Loss: " << position["total_profit_loss"].asDouble() << std::endl;
        std::cout << "-----------------------------------" << std::endl;
    }
}

// Main function to fetch and display user positions with authorization
void fetchUserPositions(const std::string& authToken, const std::string& currency = "BTC", const std::string& kind = "future", int subaccount_id = 0) {
    // Prepare the API request
    std::string url = "https://test.deribit.com/api/v2/private/get_positions"; // Replace with your API endpoint
    Json::Value requestData;
    requestData["method"] = "private/get_positions";
    requestData["params"]["currency"] = currency;
    requestData["params"]["kind"] = kind;
    if (subaccount_id != 0) {
        requestData["params"]["subaccount_id"] = subaccount_id;
    }
    requestData["jsonrpc"] = "2.0";
    requestData["id"] = 1;

    // Send API request with authorization and get the response
    Json::Value responseJson = sendApiRequest(url, requestData, authToken);

    // Check for errors in the response
    if (!responseJson.isObject()) {
        std::cerr << "Error: Invalid response from server" << std::endl;
        return;
    }

    if (responseJson.isMember("error")) {
        std::cerr << "Error: " << responseJson["error"]["message"].asString() << std::endl;
        return;
    }

    // Display positions if available
    if (responseJson["result"].isArray()) {
        displayPositions(responseJson["result"]);
    } else {
        std::cout << "No positions found." << std::endl;
    }
}
