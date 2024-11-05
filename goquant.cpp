#include <iostream>
#include <string>
#include <curl/curl.h>
#include <json/json.h>  // You need a JSON library like `jsoncpp` for parsing the JSON response
#include "functions.h"
// Function to handle the response from the server


static size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp) {
    ((std::string*)userp)->append((char*)contents, size * nmemb);
    return size * nmemb;
}

std::string loginUser(const std::string& clientId, const std::string& clientSecret) {
    CURL* curl;
    CURLcode res;
    std::string readBuffer;

    // Initialize the CURL session
    curl = curl_easy_init();
    if (curl) {
        // Set the URL for the request
        curl_easy_setopt(curl, CURLOPT_URL, "https://test.deribit.com/api/v2/public/auth");

        // Set headers
        struct curl_slist* headers = nullptr;
        headers = curl_slist_append(headers, "Content-Type: application/json");
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

        // Prepare the JSON body for authentication
        std::string authData = R"({
            "method": "public/auth",
            "params": {
                "grant_type": "client_credentials",
                "client_id": ")" + clientId + R"(",
                "client_secret": ")" + clientSecret + R"("
            },
            "jsonrpc": "2.0",
            "id": 3
        })";

        // Set the POST data
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, authData.c_str());

        // Set the write callback to capture the response
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);

        // Perform the request and check for errors
        res = curl_easy_perform(curl);
        if (res != CURLE_OK) {
            std::cerr << "Error authenticating: " << curl_easy_strerror(res) << std::endl;
        } else {
            // Parse the JSON response using jsoncpp
            Json::Value jsonData;
            Json::CharReaderBuilder readerBuilder;
            std::string errs;

            std::istringstream stream(readBuffer);
            if (Json::parseFromStream(readerBuilder, stream, &jsonData, &errs)) {
                // std::cout << "Response: " << readBuffer << std::endl;

                if (jsonData["result"].isMember("access_token")) {
                    std::string accessToken = jsonData["result"]["access_token"].asString();
                    // std::cout << "Auth Token: " << accessToken << std::endl;
                    return accessToken;
                } else {
                    std::cerr << "Error: " << jsonData["error"]["message"].asString() << std::endl;
                }
            } else {
                std::cerr << "Error parsing JSON: " << errs << std::endl;
            }
        }

        // Clean up
        curl_easy_cleanup(curl);
        // curl_slist_free_all(headers);

    }

    return "";
}

