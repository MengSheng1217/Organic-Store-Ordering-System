
#include<iostream>
#include<vector>
#include <stdlib.h>
#include<conio.h>
#include<dos.h>
#include<string>
#include <chrono>
#include <thread>
#include <windows.h>
#include<iomanip>
#include <cctype>
#include<fstream>
#include <shellapi.h>
#include <algorithm> //for line 197, change alphaber to capital letter 
#include <ctime>// include this to get the current time for receipt in line 359
#include <sstream>

#ifndef ENABLE_VIRTUAL_TERMINAL_PROCESSING
#define ENABLE_VIRTUAL_TERMINAL_PROCESSING 0x0004
#endif
#pragma warning(disable : 4996)
using namespace std;


void clear_receipt();
void editQuantity(int, int);
void logininterface();
void user_login();
void user_register();
void mainmenu();
void order_list();
void  user_menu();
string user;
string promoCode;
std::string filename;
std::string code_file;
extern int orderCount;//global variable 
const int MAX_ITEMS = 100;


class Item {
private:
    string name;
    double price;

public:
    Item() : name(""), price(0.0) {}

    void setName(const string& n) { name = n; }
    void setPrice(double p) { price = p; }

    string getName() const { return name; }
    double getPrice() const { return price; }
};

Item items[MAX_ITEMS];
int itemCount = 0;

class Order {
private:
    string name;
    double price;
    int quantity;

public:
    Order() : name(""), price(0.0) {}

    void setName(const string& n) { name = n; }
    void setPrice(double p) { price = p; }
    void setQuantity(int q) { quantity = q; }

    string getName() const { return name; }
    double getPrice() const { return price; }
    int getQuantity() const { return quantity; }
};

Order orders[MAX_ITEMS];
int orderCount = 0;

void setConsoleColors(WORD attributes) {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hConsole, attributes);
}

void loadingwastemytime() {
    system("cls");
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(hConsole, &csbi);
    WORD originalColor = csbi.wAttributes; 
    
    char a = 177; 
    char b = 219; 
    
    cout << "\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\t\t\t\t\t\t\t\t\t\t\t\tPlease wait while loading\n\n";
    cout << "\t\t\t\t\t\t\t\t\t\t\t\t";
    
    for (int i = 0; i <= 15; i++)
        cout << a;

    cout << "\r"; 
    cout << "\t\t\t\t\t\t\t\t\t\t\t\t";
    
    
    int colors[] = {12, 14, 10, 11, 9, 13, 15};
    
    for (int i = 0; i <= 15; i++) {
        SetConsoleTextAttribute(hConsole, colors[i % 7]);
        cout <<b;
        
        
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        for (int j = 0; j <= 2e8; j++); 
    }
    
    
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    
    
    SetConsoleTextAttribute(hConsole, originalColor);
    system("cls");
}
 
 
void getTerminalSize(int& cols, int& rows)
{
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
    cols = csbi.srWindow.Right - csbi.srWindow.Left + 1;
    rows = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;

}


// Clear screen with diagonal wipe effect from top-left to bottom-right
void clearDiagonalWipe(int delayMs = 5, char wipeChar = ' ')
{
    int cols, rows;
    getTerminalSize(cols, rows);

    // Enable ANSI escape codes on Windows
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD dwMode = 0;
    GetConsoleMode(hOut, &dwMode);
    dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
    SetConsoleMode(hOut, dwMode);

    // Hide cursor
    cout << "\033[?25l";
    cout.flush();

    // Calculate total diagonals
    int totalDiagonals = rows + cols - 1;

    // Create a visual representation of the wipe progress
    vector<string> screen(rows, string(cols, ' '));

    // Wipe diagonally from top-left to bottom-right
    for (int d = 0; d < totalDiagonals; d++) {
        for (int r = 0; r < rows; r++) {
            int c = d - r;
            if (c >= 0 && c < cols) {
                // Move cursor and clear character
                cout << "\033[" << (r + 1) << ";" << (c + 1) << "H" << wipeChar;
                screen[r][c] = wipeChar;
            }
        }

        // Add a decorative border to visualize the wipe
        if (d % 3 == 0) {
            for (int r = 0; r < rows; r++) {
                int c = d - r;
                if (c >= 0 && c < cols && r > 0 && r < rows - 1 && c > 0 && c < cols - 1) {
                    // Add decorative border effect
                    if (screen[r - 1][c] == wipeChar && screen[r][c - 1] == wipeChar &&
                        screen[r][c] != wipeChar) {
                        cout << "\033[" << (r + 1) << ";" << (c + 1) << "H" << "\033[38;5;45m" << "░";
                    }
                }
            }
        }

        cout.flush();
        this_thread::sleep_for(chrono::milliseconds(delayMs));
    }

    // Reset cursor and show it again
    cout << "\033[H\033[0m\033[?25h";
    cout.flush();
}
void redirectToURL(const std::wstring& url) {
    ShellExecuteW(
        NULL,               // hWnd (no parent window)
        L"open",           // Operation (wide string)
        url.c_str(),        // URL (wide string)
        nullptr,            // Parameters
        nullptr,            // Default directory
        SW_SHOWNORMAL       // Show command
    );
}
void loadItems() {
    ifstream fin("item_list.txt");
    if (!fin) {
        cout << "File cannot be opened\n";
        system("pause");
        exit(1);
    }

    itemCount = 0;
    string name;
    double price;

    while (itemCount < MAX_ITEMS && getline(fin, name)) {
        items[itemCount].setName(name);
        while (itemCount < MAX_ITEMS && fin >> price)
        {
            items[itemCount].setPrice(price);
            itemCount++;
            break;
        }

        // Skip empty lines
        string dummy;
        getline(fin, dummy);

    }
    fin.close();
}

void loadOrders() {
    ifstream fin("order_list.txt");
    orderCount = 0;
    if (fin) {
        string name;
        double price;
        int quantity;

        while (orderCount < MAX_ITEMS && getline(fin, name)) {
            orders[orderCount].setName(name);

            while (orderCount < MAX_ITEMS && fin >> price)
            {
                orders[orderCount].setPrice(price);
                while (orderCount < MAX_ITEMS && fin >> quantity)
                {
                    orders[orderCount].setQuantity(quantity);
                    orderCount++;
                    break;
                }
                break;
            }
            string dummy;
            getline(fin, dummy);
        }
        fin.close();

    }
}

void get_searching(){
    loadItems();
	bool returnToCategory = false;
	char directTocart;
	char searchAgain;
                    vector<int> matchedIndices;
                    string keyword;
                    system("color 71");
                    do {
                    	setConsoleColors(BACKGROUND_BLUE | BACKGROUND_GREEN | BACKGROUND_RED | FOREGROUND_BLUE);
                        cout << "\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\t\t\t\t\t\t\tEnter search keyword (or '0' to return): ";
                        cin.ignore();
                        getline(cin, keyword);
                         system("cls");
                        if (keyword == "0") {
                            returnToCategory = true;
                            break;
                        }
                    
					    	
        matchedIndices.clear();
        for (int i = 0; i < itemCount; i++) { 
            string itemName = items[i].getName();
            
           
            string lowerKeyword = keyword;
            string lowerName = itemName;
            transform(lowerKeyword.begin(), lowerKeyword.end(), lowerKeyword.begin(), ::tolower);
            transform(lowerName.begin(), lowerName.end(), lowerName.begin(), ::tolower);
            
           
            if (itemName.find(keyword) != string::npos || 
                lowerName.find(lowerKeyword) != string::npos) {
                matchedIndices.push_back(i);
            }
        }
                        if (matchedIndices.empty()) {
                            cout << "\n\n\n\n\n\n\n\n\n\t\t\t\t\t\t\t\t\t\tNo items found.\n";
                            setConsoleColors(BACKGROUND_BLUE | BACKGROUND_GREEN | BACKGROUND_RED | FOREGROUND_RED);
                            cout << "\t\t\t\t\t\t\t\t\t\tSearch again? (Y/N): ";
                             setConsoleColors(BACKGROUND_BLUE | BACKGROUND_GREEN | BACKGROUND_RED | FOREGROUND_BLUE);
                            cin >> searchAgain;
                            searchAgain = toupper(searchAgain);
                            system("cls");
                            
                            if (searchAgain != 'Y') {
                                returnToCategory = true;
                                break;
                            }
                        } else {
                            char addAnotherInSearch = 'Y';
                            do {
                                cout << "\n\n\n\n\n\n\n\n\t\t\t\t\t\t\tSearch Results:\n";
                                cout << "\t\t\t\t\t\t\t--------------------------------\n";
                                for (int j = 0; j < matchedIndices.size(); j++) {
                                    cout <<"\t\t\t\t\t\t\t" <<(j + 1) << ". " << items[matchedIndices[j]].getName()
                                         << " - RM" << fixed << setprecision(2) 
                                         << items[matchedIndices[j]].getPrice() << endl;
                                }
                                cout << "\t\t\t\t\t\t\t--------------------------------\n";
                                
                                cout << "\t\t\t\t\t\t\tEnter item number to order (0 to return): ";
                                int choice;
                                cin >> choice;
                                
                                if (choice == 0) {
                                    returnToCategory = true;
                                    clearDiagonalWipe();
                                    break;
                                }
                                if (choice < 1 || choice > matchedIndices.size()) {
                                	setConsoleColors(BACKGROUND_BLUE | BACKGROUND_GREEN | BACKGROUND_RED | FOREGROUND_RED);
                                    cout << "\t\t\t\t\t\t\t\t\t\tInvalid choice.\n";
                                    system("pause");
                                    system("cls");
                                    continue;
                                }
                                
                                int actualIndex = matchedIndices[choice - 1];
                                cout << "\t\t\t\t\t\t\t\t\t     Enter item quantity: ";
                                int quantity;
                                cin >> quantity;
                                
                                ofstream fout("order_list.txt", ios::app);
                                if (!fout) {
                                	setConsoleColors(BACKGROUND_BLUE | BACKGROUND_GREEN | BACKGROUND_RED | FOREGROUND_RED);
                                    cout << "Cannot open order_list.txt\n";
                                    system("pause");
                                    break;
                                }
                                fout << items[actualIndex].getName() << "\n" 
                                     << items[actualIndex].getPrice() << "\n" 
                                     << quantity << endl;
                                fout.close();
                                
                                cout << "\t\t\t\t\t\t   Added \"" << items[actualIndex].getName() << "\" (RM" 
                                     << items[actualIndex].getPrice() << ") x " << quantity << " to order list.\n\n";
                                
                                loadOrders();
                                
                                cout << "\t\t\t\t\t\t       Add another item in current search? (Y/N): ";
                                cin >> addAnotherInSearch;
                                addAnotherInSearch = toupper(addAnotherInSearch);
                                if(addAnotherInSearch == 'Y')
                                {
                                	system("cls");
                                	continue;
								}
								
                                cout << "\t\t\t\t\t\t       Proceed to cart?(Y/N): ";
                                cin >> directTocart;
                                if (directTocart == 'Y' || directTocart == 'y')
                                {
                                clearDiagonalWipe();
                                break;
                                }
                                else
                                {
                                clearDiagonalWipe();
                                break;
                                }
                                 system("cls");
                                
                            } while (addAnotherInSearch == 'Y');
                            
                                                            
                            if (searchAgain != 'Y') {
                                returnToCategory = true;
                                break;
                            if (addAnotherInSearch == 'N') {
                            	setConsoleColors(BACKGROUND_BLUE | BACKGROUND_GREEN | BACKGROUND_RED | FOREGROUND_BLUE);
                                cout << "\n\n\n\n\n\n\n\n\n\n\n\n\n\t\t\t\t\t\t\t\t   Continue searching? (Y/N): ";
                                cin >> searchAgain;
                                searchAgain = toupper(searchAgain);
                            
                                }
                            }
                        }
                        if (directTocart == 'Y' || directTocart == 'y')
                            {
                            break;
                            }
                    } while (true);
    if (directTocart == 'Y' || directTocart == 'y')
    {
    order_list();
    }
            
                    
}

void item_list() {
    char addMore;
    char directTocart;
    system("color 71");
    do {
        loadItems();
        string temp;
        int quantity;
        ifstream item_menu("item_menu.txt");

        while (getline(item_menu, temp))
            cout << temp << endl;

        item_menu.close();
        
        cout << setw(100) << "1) Enter item number to order (0 to return)" << endl;
        cout << setw(89) << "2) Enter -1 to Search for Items\n" << endl; 
        cout << setw(79) << "Enter your choice: ";
        int choice;
        cin >> choice;
        
        if(choice == -1 ){
        	system("cls");
        	get_searching(); 
        	cout << setw(100) << "\n\n\n\n\n\n\n\n\n\n                           Add another item? (Y/N): ";
            cin >> addMore;
            if (addMore != 'Y' && addMore != 'y') {
                cout << setw(100) << "Proceed to cart?(Y/N): ";
                cin >> directTocart;
                if (directTocart == 'Y' || directTocart == 'y')
                {
                    clearDiagonalWipe();
                    order_list();
                }
                else
                {
                    clearDiagonalWipe();
                    user_menu();
                }
            }
            else
            {
                system("cls");
                continue;
            }

                }
		else if (choice > 0 && choice <= itemCount) {
            cout << setw(79) << "Enter item quantity : ";
            cin >> quantity;
            orders[choice - 1].setQuantity(quantity);
            ofstream fout("order_list.txt", ios::app);
            if (!fout) {
                cout << setw(110) << "Cannot open order_list.txt\n";
                system("pause");
                return;
            }
            fout << items[choice - 1].getName() << "\n" << items[choice - 1].getPrice() << "\n" << orders[choice - 1].getQuantity() << endl;
            fout.close();
            cout << setw(62) << "Added \"" << items[choice - 1].getName()
                << "\" (RM" << items[choice - 1].getPrice() << ") x " << orders[choice - 1].getQuantity() << " to order list.\n\n";

            loadOrders();
            setConsoleColors(BACKGROUND_BLUE | BACKGROUND_GREEN | BACKGROUND_RED | FOREGROUND_BLUE);
            cout << setw(79) <<"Add another item? (Y/N): ";
            cin >> addMore;
            if (addMore != 'Y' && addMore != 'y') {
                cout << setw(79) << "Proceed to cart?(Y/N): ";
                cin >> directTocart;
                if (directTocart == 'Y' || directTocart == 'y')
                {
                    clearDiagonalWipe();
                    order_list();
                }
                else
                {
                    clearDiagonalWipe();
                    user_menu();
                }
            }
            else
            {
                system("cls");
                continue;
            }


        }
        else if (choice == 0) {
            clearDiagonalWipe();
            user_menu();
            break;
        }
        else {
        	setConsoleColors(BACKGROUND_BLUE | BACKGROUND_GREEN | BACKGROUND_RED | FOREGROUND_RED);
            cout << "Invalid choice, try again." << endl;
            setConsoleColors(BACKGROUND_BLUE | BACKGROUND_GREEN | BACKGROUND_RED | FOREGROUND_BLUE);	
            system("pause");
            system("cls");
        }
    } while (true);
    system("pause");
    system("cls");
}


void displayOrderList(bool discountApplied = false, double discount = 0.0, double discountedTotal = 0.0) {
    double total = 0.0,befdistotal=0.0;
    cout << setw(75)<<" " << "=====================Your Shopping Cart =======================" << endl;
    if (orderCount == 0) {
    	setConsoleColors(BACKGROUND_BLUE | BACKGROUND_GREEN | BACKGROUND_RED | FOREGROUND_RED);
        cout << setw(120) << "Your Shopping cart is empty." << endl;
        return;
    }
    else {
        for (int i = 0; i < orderCount; ++i) {
            cout <<setw(75)<<" " <<left << setw(3) << (i + 1) << " : " << setw(30)
                << orders[i].getName() << "RM" << right << setw(6)
                << fixed << setprecision(2) << orders[i].getPrice() << " x "
                << setw(3) << orders[i].getQuantity() << endl;
            total += orders[i].getPrice() * orders[i].getQuantity();
        }
        befdistotal = total;
        cout << setw(75)<<" " << "---------------------------------------------------------------" << endl;
        cout << setw(134) << "Total Price: RM" << fixed << setprecision(2) << total << endl;


        if (discountApplied) //to display the discount detail 
        {
            system("cls");
            cout << setw(75) << " " << "=====================Your Shopping Cart =======================" << endl;
            for (int i = 0; i < orderCount; ++i) {
                cout << setw(75) << " " << left << setw(3) << (i + 1) << " : " << setw(30)
                    << orders[i].getName() << "RM" << right << setw(6)
                    << fixed << setprecision(2) << orders[i].getPrice() << " x "
                    << setw(3) << orders[i].getQuantity() << endl;
                total += orders[i].getPrice() * orders[i].getQuantity();
            }
            cout << setw(75) << " " << "----------------------------------------------------------------" << endl;
            cout << setw(134) << "RM " << fixed << setprecision(2) << befdistotal << endl;
            cout<<setw(110)<<" " << "(Discount:20%)      " << "-RM " << fixed << setprecision(2) << discount << endl;
            cout << setw(75) << " " << "----------------------------------------------------------------" << endl;
            cout<<setw(134)<< "Total Price: RM " << fixed << setprecision(2) << discountedTotal << endl;


        }
    }
}

double applyDiscount(double total) {
	string lines; 
	string promoCode;
    bool validCode = false;
    double discountedTotal = total;

    do {
        cout << "Please enter your promotion code (enter 0 if none): ";
        cin >> promoCode;
        transform(promoCode.begin(), promoCode.end(), promoCode.begin(), ::toupper);

        if (promoCode == "0") {
            return total; // No discount applied
        }

        ifstream infile(code_file.c_str());
        if (!infile) {
        	setConsoleColors(BACKGROUND_BLUE | BACKGROUND_GREEN | BACKGROUND_RED | FOREGROUND_RED);
            cout << "Error: Could not open promo codes file." << endl;
            setConsoleColors(BACKGROUND_BLUE | BACKGROUND_GREEN | BACKGROUND_RED | FOREGROUND_BLUE);
            return total;
        }

        vector<string> lines;
        string line;
        bool found = false;
        bool alreadyUsed = false;
        
        // Read all lines into vector
        while (getline(infile, line)) {
            lines.push_back(line);
        }
        infile.close();

        // Search for promo code
        for (int i = 0; i < lines.size(); i++) {
            // Create a stream from the line
            istringstream iss(lines[i]);
            string code;
            int status;

            // Parse code and status
            if (iss >> code >> status) {
                if (code == promoCode) {
                    found = true;
                    if (status == 1) {
                        // Valid unused code - mark for update
                        ostringstream oss;
                        oss << code << " " << 0; // Mark as used
                        lines[i] = oss.str();
                        
                         // Write updated lines back to file
                        ofstream outfile(code_file.c_str());
                        if (!outfile) {
                            setConsoleColors(BACKGROUND_BLUE | BACKGROUND_GREEN | BACKGROUND_RED | FOREGROUND_RED);
                            cout << "Error: Could not update promo codes file." << endl;
                            return total;
                        }
                        for (const auto& l : lines) {
                            outfile << l << endl;
                        }
                        outfile.close();
                        
                        cout << "Promo code applied! 20% discount granted." << endl;
                        discountedTotal = total * 0.8; // Apply 20% discount
                        validCode = true; // Set validation flag
                    }
                    else {
                    alreadyUsed = true;
                    }
  
                    break; // Stop searching after finding the code
                }
            }
        }

        if (validCode) {
            break; // Exit loop if valid code applied
        }

        if (!found) {
        	setConsoleColors(BACKGROUND_BLUE | BACKGROUND_GREEN | BACKGROUND_RED | FOREGROUND_RED);
            cout << "Invalid promo code. Try again? (Y/N): ";
            setConsoleColors(BACKGROUND_BLUE | BACKGROUND_GREEN | BACKGROUND_RED | FOREGROUND_BLUE);
        } else if (alreadyUsed) {
        	setConsoleColors(BACKGROUND_BLUE | BACKGROUND_GREEN | BACKGROUND_RED | FOREGROUND_RED);
            cout << "This promo code has already been used. Try again? (Y/N): ";
            setConsoleColors(BACKGROUND_BLUE | BACKGROUND_GREEN | BACKGROUND_RED | FOREGROUND_BLUE);
        }
        
        char retry;
        cin >> retry;
        if (toupper(retry) != 'Y') {
            break; // User chooses not to retry
        }
        
    } while (true);

    return discountedTotal;

     

    // Write updated lines back to file
    ofstream outfile(code_file.c_str());
if (!outfile) {
	setConsoleColors(BACKGROUND_BLUE | BACKGROUND_GREEN | BACKGROUND_RED | FOREGROUND_RED);
    cout << "Error: Could not update promo codes file." << endl;
    return total;
}
for (const auto& l : lines) {
    outfile << l << endl;
}
outfile.close();
      setConsoleColors(BACKGROUND_BLUE | BACKGROUND_GREEN | BACKGROUND_RED | FOREGROUND_GREEN);

    cout << "Promo code applied! 20% discount granted." << endl;
    return total * 0.8; // Apply 20% discount
}
void order_list() {
    system("color 71");
    loadOrders();

    double total = 0.0;// total only
    double discount = 0.0; //discount only
    double discountedTotal = 0.0; //total after discount 
    bool discountApplied = false;// display dicount detail when it is true


    for (int i = 0; i < orderCount; i++) {
        total += orders[i].getPrice() * orders[i].getQuantity();
    }

    char action;
    do {
        displayOrderList(discountApplied, discount, discountedTotal);

        cout << "Proceed to payment?(Y/N)[enter D to remove item, E to edit item]: ";
        cin >> action;
        action = toupper(action);

        if (action == 'Y')
        {
            if (orderCount == 0) {
                cout << "\n Your order list is empty.\n";
                system("pause");
                system("cls");
                continue;
            }
            else
            {
                double newDiscountedTotal = applyDiscount(total);//get the discount form applydiscount function and store into discountedTotal
                if (newDiscountedTotal < total) {
                    discount = total - newDiscountedTotal; //calculate the total amount  
                    discountedTotal = newDiscountedTotal;
                    total = discountedTotal; //update the total 
                    discountApplied = true;
                }
                system("pause");
                system("cls");

                displayOrderList(discountApplied, discount, discountedTotal);
                cout << "Check out? (Y/N): ";
                char confirm;
                cin >> confirm;


                if (toupper(confirm) != 'Y') {
                    cout << "Purchase canceled." << endl;
                    system("pause");
                    system("cls");
                    continue;
                }
                else
                {

                }
                
                ofstream fout("receipt.txt");                                                           //write in receipt
                if (!fout) {
                    cout << "Cannot create receipt.txt\n";
                    system("pause");
                    return;
                }

                time_t now = time(0); //get the time now 
                char* dt = ctime(&now);
                string timeStr = dt;
                timeStr.erase(timeStr.find('\n'), 1);
                fout << setw(59) << " " << "+------------------------------------------------------------+" << endl;
                fout << setw(59) << " " << "|                                                            |" << endl;
                fout << setw(59) << " " << "|                          Receipt                           |" << endl;
                fout << setw(59) << " " << "|                                                            |" << endl;
                fout << setw(59) << " " << "+------------------------------------------------------------+" << endl;
                fout << setw(59) << " " << "|Time: " << timeStr << "                              |" << endl;
                for (int i = 0; i < orderCount; ++i) {
                    fout << setw(60)<<"|" << left << setw(3) << (i + 1) << " : " << setw(30)
                        << orders[i].getName() << "  RM" << right << setw(6) << fixed
                        << setprecision(2) << orders[i].getPrice() << " x " << setw(3)
                        << orders[i].getQuantity()<<"\t|" << endl;
                }

                if (discountApplied) {//display the discount detail when it is true 
                    fout<<setw(59)<<" " << "+------------------------------------------------------------+\n";
                    fout <<setw(59)<<" "<< "|                            (Discount: 20%)" << " RM " << fixed << setprecision(2) << discount << "         |" << endl;
                }
                fout << setw(59) << " " << "|                                                            |" << endl;
                fout << setw(59) << " " << "|                               Total: RM" << fixed << setprecision(2) << (discountApplied ? discountedTotal : total) << "            \t|" << endl;//if discountapplied = true , system will chose discounttotal , ifnot will chose total 
                fout << setw(59) << " " << "|                                                            |" << endl;
                fout << setw(59) << " " << "+------------------------------------------------------------+" << endl;
                fout.close();
               

                std::ofstream history(filename.c_str(), ios::app); {                                     //write in history
                    if (!history) {
                        cout << "Cannot create history.txt\n";
                        system("pause");
                        return;
                    }
                    else {
                        time_t now = time(0); // Get current time
                        char* dt = ctime(&now);
                        string timeStr = dt;
                        timeStr.erase(timeStr.find('\n'), 1); // Remove newline

                        // Write time header to history
                        history << "===== Purchase on: " << timeStr << " =====\n";

                        // Write order items
                        for (int i = 0; i < orderCount; ++i) {
                            history << orders[i].getName()
                                << " - RM" << fixed << setprecision(2) << orders[i].getPrice()
                                << " x " << orders[i].getQuantity() << "\n";
                        }

                        // Write total
                        double totalAmount = discountApplied ? discountedTotal : total;
                        history << "------------------------------------\n";
                        history << "Total: RM" << fixed << setprecision(2) << totalAmount << "\n";
                        history << "===================================\n\n";
                    }
                    history.close();
                }
                ofstream clearFile("order_list.txt", ios::trunc);
                clearFile.close();
                orderCount = 0;
                total = 0.0;

                cout << "Thank you for buying in our store." << endl;
                system("pause");
                system("cls");
                user_menu();
                break;
            }
        }

        else if (action == 'D')
        {
            if (orderCount == 0) {
                cout << "Order list is empty.\n";
                system("pause");
                system("cls");
                continue;
            }
            system("cls");
            displayOrderList();
            cout << "Enter item number to delete (0 to cancel): ";
            int choice;
            cin >> choice;

            if (choice > 0 && choice <= orderCount) {
                cout << "Removed \"" << orders[choice - 1].getName()
                    << "\" (RM" << orders[choice - 1].getPrice() << ") from order list." << endl;


                total -= orders[choice - 1].getPrice();


                for (int i = choice - 1; i < orderCount - 1; i++) {
                    orders[i] = orders[i + 1];
                }
                orderCount--;


                ofstream fout("order_list.txt");
                for (int i = 0; i < orderCount; i++) {
                    fout << orders[i].getName() << "\n" << orders[i].getPrice() << orders[i].getQuantity() << endl;
                }
                fout.close();
                system("pause");
                system("cls");
            }
            else if (choice != 0) {
                cout << "Invalid choice." << endl;
                system("pause");
                system("cls");
            }
        }
        else if (action == 'N') 
        {
            cout << "Continue shopping?(Y/N):";
            char c;
            cin >> c;
            if(c=='y'||c=='Y')
            {
                clearDiagonalWipe();
                item_list();
            }
            else
            {
                clearDiagonalWipe();
                user_menu();
            }
        }
        else if (action == 'E')
        {
        	if (orderCount == 0) {
            cout << "Your order list is empty.\n";
            system("pause");
            system("cls");
            continue;
            }
            
            int choice;
            int newquantity;
            while (true) {
                cout << "Enter the item you like to edit (0 to exit): ";
                cin >> choice;
                
                if (choice == 0)
                {
                    break;
                }

                else if (choice > orderCount || choice < 0)
                {
                    cout << "Invalid item!Please try again\n";
                    system("pause");
                    system("cls");
                    displayOrderList();
                    continue;
                }

                else {
                    cout << "\nEnter quantity u want it to be? :";
                    cin >> newquantity;
                    editQuantity(choice, newquantity);
                    system("cls");
                    break;
                }

            }
        }

        else
        {
            cout << "Invalid choice, try again." << endl;
            system("pause");
            system("cls");
        }

    } while (true);
}

void editQuantity(int choice, int newquantity)
{
    orders[choice - 1].setQuantity(newquantity);

    ofstream editQ("order_list.txt");
    for (int i = 0; i < orderCount; i++) {
        editQ << orders[i].getName() << "\n" << orders[i].getPrice() << "\n" << orders[i].getQuantity() << endl;
    }
    editQ.close();
}


void receipt() {
	system("color 71");
    ifstream fin("receipt.txt");
    if (!fin) {
        cout << "No receipt found.\n";
        system("pause");
        return;
    }
    string line;
    while (getline(fin, line)) {
        cout << line << endl;
    }
    fin.close();
    system("pause");
    clearDiagonalWipe();
}

void history()
{
	system("color 71");
    std::ifstream userhistory(filename.c_str());
    if (!userhistory || userhistory.peek() == ifstream::traits_type::eof()) { //check if file is empty
        cout << "No history found.\n";
        system("pause");
        return;
    }
    string line;
    int i = 1;
    bool isHeader = false;


    cout << "---Your purchase history---" << endl;
    while (getline(userhistory, line)) {
        // Skip empty lines
        if (line.empty()) continue;

        // Format headers differently
        if (line.find("=====") != string::npos) {
            if (isHeader) {
                cout << "--------------------------------" << endl;
            }
            cout << "\n" << line << endl;
            isHeader = true;
        }
        else {
            if (isHeader) {
                cout << "Items:" << endl;
                isHeader = false;
            }
            cout << "  " << line << endl;
        }
    }
       
    
    cout << "---------------------------" << endl;
    userhistory.close();
    system("pause");
    clearDiagonalWipe();
}





void user_menu() {
	
    char choice;
    while (true) {
        SetConsoleOutputCP(65001);
        mainmenu();
        cout<<setw(90) << "What would you like to do today : ";
        cin >> choice;
        clearDiagonalWipe();
        switch (choice) {
        case '1':
            item_list();
            break;
        case '2':
        	 loadItems();
             get_searching();
            break;
        case '3':
            order_list();
            break;
        case '4':
            receipt();
            break;
        case '5': 
        {
            history();
            break;
        }
        case'6':
        { logininterface(); break; }
        case'R':
        case'r':
        { redirectToURL(L"https://youtu.be/dQw4w9WgXcQ"); break; }
        default:
            cout << "Invalid choice, try again." << endl;
            system("pause");
        }
    }
}
// Function to get terminal dimensions
bool containsSpace(const string& str) {
    return str.find(' ') != string::npos;
}
void read_promocode()
{
    std::ifstream src("promo_code.txt");
    std::ofstream dst(code_file.c_str());

    std::string line;
    while (getline(src, line)) {
        dst << line << '\n';
    }

    src.close();
    dst.close();
}
void mainmenu()
{
    SetConsoleOutputCP(65001);//set console as UTF-8
    ifstream menu("main_menu.txt");//read file
    string table;
    if (!menu)
    {
        cerr << "File cannot be opened\n";
        system("pause");
        exit(1);
    }
    else
    {
        while (getline(menu, table))
        {
        	system("color 72");
            cout << table << endl;
        }

    }
    menu.close();
}

void icon()
{
    SetConsoleOutputCP(65001);//set console as UTF-8
    ifstream logo("Logo.txt");//read file
    string pic;
    if (!logo)
    {
        cerr << "File cannot be opened\n";
        system("pause");
        exit(1);
    }
    else 
    {
        while (getline(logo, pic))
        {
            cout << pic << endl;
        }
       
    }
    logo.close();
}

void user_login()
{
	system("color 71");
    while (true) { //loop for user to retry
        string input_id;
        string input_password;

        char mnl;

        string user_id;
        string user_password;

        int num_user = 0;

        system("cls");
        ifstream users("users.txt");

        //start logging in
        cout << "\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n";
        cout <<setw(130)<< "+--------------------------------------------+" << endl;
        cout << setw(130) << "|                    Login                   |" << endl;
        cout << setw(130) << "+--------------------------------------------+" << endl;
        cout << setw(100) << "Enter user id :";
        getline(cin, input_id);

        //search for user id and user password
        while (users >> user_id >> user_password)
        {
            if (input_id == user_id) //if success to find user id from file,it will continue ask user to enter password
            {
                cout << setw(101) << "Enter password :";
                getline(cin, input_password);

                if (input_password == user_password) //if the password for the specific user id is right
                { 
                setConsoleColors(BACKGROUND_BLUE | BACKGROUND_GREEN | BACKGROUND_RED | FOREGROUND_GREEN);
                    cout << setw(102) << "Login Successful!" << endl;
                    user = input_id;
                    clear_receipt();
                    filename = user_id + "_history.txt";
                    code_file = user_id + "_code.txt";
                    system("pause");
                    clearDiagonalWipe();
                    user_menu();
                }

                else // wrong password
                {
                	setConsoleColors(BACKGROUND_BLUE | BACKGROUND_GREEN | BACKGROUND_RED | FOREGROUND_RED);
                    cout << setw(102) << "wrong password!\n\n";
                    setConsoleColors(BACKGROUND_BLUE | BACKGROUND_GREEN | BACKGROUND_RED | FOREGROUND_BLUE);
                    cout << setw(103) << "try again?(y/n) : ";
                    cin >> mnl;
                    cin.ignore();
                    system("cls");

                    if (mnl == 'n'|| mnl=='N')
                    {
                        users.close();
                        system("cls");
                        logininterface();
                    }

                    break; //if enter 2 ,go out from this while loop
                }
            }
        }
        if (input_id != user_id)	  //unsuccess to find user id from file,back to menu
        {
            users.close();
            setConsoleColors(BACKGROUND_BLUE | BACKGROUND_GREEN | BACKGROUND_RED | FOREGROUND_RED);
            cout << setw(105) << "Invalid user id!! \n\n";
            setConsoleColors(BACKGROUND_BLUE | BACKGROUND_GREEN | BACKGROUND_RED | FOREGROUND_BLUE);
            cout << setw(103) << "Try again?(y/n) : ";
            cin >> mnl;
            cin.ignore();

            if (mnl == 'n'|| mnl=='N')
            {
            	users.close();
            	system("cls");
                logininterface();
            }

            else
            {
                users.close();
                continue;
            }

        }
    }
}
void user_register()
{
    while (true) {
    	system("color 71");
        system("cls");
        ifstream check_user("users.txt");

        bool id_exist = false;
        bool space = false;

        string user_id;
        string user_password;

        string temp_id;
        string temp_password;

        cout << "\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n";
        cout << setw(110) << "+--------------------------------------------+" << endl;
        cout << setw(110) << "|                 Register                   |" << endl;
        cout << setw(110) << "+--------------------------------------------+" << endl;
        cout << setw(80) << "Enter user id : ";
        getline(cin, user_id);

        while (check_user >> temp_id >> temp_password)//search for user id and user password
        {
            if (user_id == temp_id)
            {
                id_exist = true;
                break;
            }
            else if (containsSpace(user_id)) //if user input space will be error 
            {
            	setConsoleColors(BACKGROUND_BLUE | BACKGROUND_GREEN | BACKGROUND_RED | FOREGROUND_RED);
                cout << "Error, user ID not allow to contain space, please try again!";
                system("pause");
                system("cls");
                space = true;
                break;
            }

            else if (user_id.empty())
            {
            	setConsoleColors(BACKGROUND_BLUE | BACKGROUND_GREEN | BACKGROUND_RED | FOREGROUND_RED);
                cout << "Error, input cannot be empty, please try again!";
                system("pause");
                system("cls");
                space = true;
                break;
            }
        }
        
        check_user.close();

        if (id_exist)
        {
        	setConsoleColors(BACKGROUND_BLUE | BACKGROUND_GREEN | BACKGROUND_RED | FOREGROUND_RED);
            cout << setw(110) << "ID is registered! Please try another user ID" << endl;
            system("pause");
            system("cls");
            continue;
        }
        else if (space)
        {
            continue;
        }

        cout << setw(81) << "Enter password : ";
        getline(cin, user_password);
        if (containsSpace(user_password))
        {
        	setConsoleColors(BACKGROUND_BLUE | BACKGROUND_GREEN | BACKGROUND_RED | FOREGROUND_RED);
            cout << "Error, user ID not allow to contain space, please try again!";
            system("pause");
            system("cls");
            space = true;
            continue;
        }

        else if (user_password.empty())
        {
        	setConsoleColors(BACKGROUND_BLUE | BACKGROUND_GREEN | BACKGROUND_RED | FOREGROUND_RED);
            cout << "Error, input cannot be empty, please try again!";
            system("pause");
            system("cls");
            space = true;
            continue;
        }
        system("cls");
        //write into file
        ofstream users("users.txt", ios::app);
        users << user_id << " " << user_password << endl;
        users.close();

        cout << "\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n";
        setConsoleColors(BACKGROUND_BLUE | BACKGROUND_GREEN | BACKGROUND_RED | FOREGROUND_GREEN);
        cout << setw(80) << "Successfully registered!!" << endl;
        user = user_id;
        filename = user_id + "_history.txt";  //creating a history file for this user
        std::ofstream userhistory(filename.c_str());
        userhistory.close();

        code_file = user_id + "_code.txt"; //create a promo code history file
        std::ofstream usercode(code_file.c_str());
        read_promocode();
        usercode.close();
        system("pause");
        clearDiagonalWipe();
        user_login();
    }
}
void logininterface()
{
	system("color 72");
    int choice;
    icon();
    cout <<"\n" << setw(48) << " " << "What would you like to do? : ";
    cin >> choice;
    cin.ignore();
    switch(choice)
    {
        case 1:
        {
            clearDiagonalWipe();
            user_login();
            clearDiagonalWipe();
            break;
        }
        case 2:
        {
            clearDiagonalWipe();
            user_register();
            clearDiagonalWipe();
            break;
        }
        case 3:
            exit(0);
            break;
        default:
        {
            cout << "Invalid input!!" << endl;
            break;
        }
    }
}

void clear_receipt()
{
    ofstream clearfile("receipt.txt");
    clearfile <<setw(95)<<" " <<"Your haven't order anything yet!";
    clearfile.close();
}
int main(){
	loadingwastemytime();
    logininterface();
	return 0;
}


