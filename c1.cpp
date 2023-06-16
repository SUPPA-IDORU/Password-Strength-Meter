#include <iostream>
#include <fstream>
#include <regex>
#include <string>
#include <sstream>
#include <vector>

using namespace std;

const int MAX_USERS = 100;

class User {
public:
    string login;
    string password;
};

bool checkPasswordStrength(const string& password) {
    static const regex lengthRegex("^.{8,}$");
    static const regex complexityRegex("^(?=.*[a-z])(?=.*[A-Z])(?=.*\\d)(?=.*[@$!%*?&^()])[A-Za-z\\d@$!%*?&^()]+$");

    return regex_search(password, lengthRegex) && regex_search(password, complexityRegex);
}

bool isPasswordInDatabase(const string& password, const string& filename) {
    ifstream file(filename);
    if (!file.is_open()) {
        cerr << "Error opening file: " << filename << endl;
        return false;
    }

    string line;
    while (getline(file, line)) {
        
        istringstream iss(line);
        vector<string> tokens(istream_iterator<string>{iss}, istream_iterator<string>());

        for (const string& token : tokens) {
            if (token == password) {
                file.close();
                return true;
            }
        }
    }

    file.close();
    return false;
}

string generateRandomStrongPassword() {
    static const string characters = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789@$!%*?&";
    static const int length = 12;

    srand(static_cast<unsigned int>(time(nullptr)));

    while (true) {
        string password;
        for (int i = 0; i < length; ++i) {
            int index = rand() % characters.size();
            password += characters[index];
        }

        if (checkPasswordStrength(password)) {
            return password;
        }
    }
}

string enterPassword() {
    string password;
    cout << "Enter your password: ";
    cin >> password;
    return password;
}

int findUserIndex(const User users[], int userCount, const string& login) {
    for (int i = 0; i < userCount; ++i) {
        if (users[i].login == login) {
            return i;
        }
    }
    return -1;
}

int main() {
    User users[MAX_USERS];
    int userCount = 0;

    while (true) {
        cout << "Menu:" << endl;
        cout << "1. Register" << endl;
        cout << "2. Login" << endl;
        cout << "3. Exit" << endl;

        int choice;
        cout << "Enter your choice: ";
        cin >> choice;

        switch (choice) {
        case 1: {
            string login;
            string password;

            cout << "Registration" << endl;
            cout << "Enter your login: ";
            cin >> login;

            bool isStrong = false;
            bool isPasswordFound = false;
            while (!isStrong || isPasswordFound) {
                string suggestedPassword = generateRandomStrongPassword();
                cout << "Here is a suggested strong password: " << suggestedPassword << endl;

                password = enterPassword();

                isPasswordFound = isPasswordInDatabase(password, "passwords.txt");
                isStrong = checkPasswordStrength(password);

                if (!isStrong) {
                    cout << "Your password is weak. Please choose a stronger password." << endl;
                }

                if (isPasswordFound) {
                    cout << "Your password was found in the document. Please choose a different password." << endl;
                }
            }

            cout << "Your password is strong." << endl;

            User newUser;
            newUser.login = login;
            newUser.password = password;

            if (userCount < MAX_USERS) {
                users[userCount] = newUser;
                userCount++;
                cout << "Registration successful!" << endl;
            }
            else {
                cout << "Maximum number of users reached. Cannot register a new user." << endl;
            }

            break;
        }

        case 2: {
            string login;
            string password;

            cout << "Login" << endl;
            cout << "Enter your login: ";
            cin >> login;
            password = enterPassword();

            int userIndex = findUserIndex(users, userCount, login);

            if (userIndex != -1 && users[userIndex].password == password) {
                cout << "Login successful!" << endl;

                bool isPasswordFound = isPasswordInDatabase(password, "passwords.txt");
                if (isPasswordFound) {
                    cout << "Your password was found in the document. We recommend changing your password." << endl;

                    cout << "Would you like to change your password? (y/n): ";
                    char choice;
                    cin >> choice;

                    if (choice == 'y' || choice == 'Y') {
                        string newPassword;
                        string suggestedPassword = generateRandomStrongPassword();
                        cout << "Here is a suggested strong password: " << suggestedPassword << endl;
                        newPassword = enterPassword();
                        if (users[userIndex].password == newPassword) {
                            cout << "Already exist" << endl;
                        }
                        else if (checkPasswordStrength(newPassword)) {
                            users[userIndex].password = newPassword;
                            cout << "Password changed successfully!" << endl;
                            break;
                        }
                        else {
                            cout << "Your new password is weak. Please choose a stronger password." << endl;
                            string suggestedPassword = generateRandomStrongPassword();
                            cout << "Here is a suggested strong password: " << suggestedPassword << endl;
                        }
                    }
                }
            }
            else {
                cout << "Invalid login or password." << endl;
            }

            break;
        }

        case 3: {
            cout << "Exiting program." << endl;
            return 0;
        }

        default:
            cout << "Invalid choice. Please try again." << endl;
        }

        cout << endl;
    }
}
