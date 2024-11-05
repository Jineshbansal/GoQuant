#include <string>
#ifndef FUNCTIONS_H
#define FUNCTIONS_H
void fetchOrderBook(const std::string& , int );
void fetchUserPositions(const std::string&,const std::string&, const std::string& , int subaccount_id );
std::string loginUser(const std::string& , const std::string& );   // Function declaration
void placeOrder(const std::string&);
void modifyOrder(const std::string&);
void cancelOrder(const std::string&);
double getValidatedDouble(const std::string&);

#endif