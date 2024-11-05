#include <iostream>
#include <string>
#include "functions.h"
using namespace std;


int main() {
    std::string clientId;
    std::string clientSecret;
    std::string token;
    while (1) {
        std::cout << std::endl;
        std::cout << "Enter the option you want to choose: " << std::endl;
        std::cout << "1. Get Account detail" << std::endl;
        std::cout << "2. Place order" << std::endl;
        std::cout << "3. Cancel order" << std::endl;
        std::cout << "4. Modify order" << std::endl;
        std::cout << "5. Get orderbook" << std::endl;
        std::cout << "6. View current positions" << std::endl;
        std::cout << "7. Exit" << std::endl;

        int choice;
        std::cin >> choice;

        std::string instrument_name="";
        std::string currency;
        std::string kind;
        int subaccount_id;
        int depth=1;
        switch (choice) {
            case 1:
                std::cout << "Getting Account Summary..." << std::endl;
                // Add logic to get account summary
                if(token.empty()){
                    std::cout<<"Please login first"<<std::endl;
                    std::cout<<"Please Enter client_id"<<std::endl;
                    std::cin>>clientId;
                    std::cout<<"Please Enter client_secret"<<std::endl;
                    std::cin>>clientSecret;
                    token = loginUser(clientId, clientSecret);
                    if(!token.empty()){
                        std::cout << "Login successful. Token: "<<token<<std::endl;
                    }
                }else{
                    std::cout << "Received token: " << token << std::endl;
                }
                break;
            case 2:
                std::cout << "Placing order..." << std::endl;
                // Add logic to place order
                placeOrder(token);
                break;
            case 3:
                std::cout << "Cancelling order..." << std::endl;
                // Add logic to cancel order
                cancelOrder(token);

                break;
            case 4:
                std::cout << "Modifying order..." << std::endl;
                // Add logic to modify order
                modifyOrder(token); 
                break;
            case 5:
                std::cout << "Getting orderbook..." << std::endl;

                std::cout << "Enter instrument name: ";
                std::cin >> instrument_name;

                std::cout << "Enter depth (optional, default is 1): ";
                std::cin >> depth;

                std::cout << std::endl;
                fetchOrderBook(instrument_name, depth);
                break;
            case 6:
                std::cout << "Viewing current positions..." << std::endl;
                // Add logic to view current positions
                std::cout << "Enter currency (BTC, ETH, etc.): ";
                std::cin >> currency;

                std::cout << "Enter kind (future, option, spot, etc.): ";
                std::cin >> kind;

                std::cout << "Enter subaccount ID (0 for default): ";
                std::cin >> subaccount_id;

                std::cout << std::endl;
                fetchUserPositions(token, currency, kind, subaccount_id);
                break;
            case 7:
                std::cout << "Exiting..." << std::endl;
                return 0;
            default:
                std::cout << "Invalid choice. Please try again." << std::endl;
                break;
        }
    }

    return 0;
}