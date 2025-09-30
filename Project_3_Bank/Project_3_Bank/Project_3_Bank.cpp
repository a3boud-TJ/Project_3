#include <iostream>
#include <fstream>
#include <iomanip>
#include <limits>
#include <string>
#include <vector>

using namespace std;

enum enMainMenueOptions { eListClients = 1, eAddNewClient = 2, eDeleteClient = 3, eUpdateClient = 4, eFindClient = 5, eTransactionMenu = 6, eManageUsers = 7, eLogout = 8 };
enum enTransactionMenueOptions { eDeposit = 1, eWithdraw = 2, eTotalBalances = 3, eMainMenue = 4 };
enum enManageUseresMenueOptions { eListUsers = 1, eAddNewUsers = 2, eDeleteUser = 3, eUpdateUser = 4, eFindUser = 5, enMainMenue = 6 };
enum enMainMenuePermissions { eAll = -1, pListClients = 1, pAddNewClient = 2, pDeleteClient = 4, pUpdateClient = 8, pFindClient = 16, pTransaction = 32, pManageUsers = 64 };

const string ClientsFileName = "ClientData.txt";
const string UsersFileName = "Users.txt";
const string EncryptionKey = "A5-3";

void TransactionMenueScreen();
void MainMenueScreen();
void ManageUseresScreen();
void Login();

struct sClient
{
    string AccountNumber;
    string PinCode;
    string Name;
    string Phone;
    double AccountBalance;
    bool MarkForDelete = false;
};

struct sUser
{
    string UserName;
    string Password;
    int Permissions;
    bool MarkForDelete = false;
};

sUser loggedInUser;

string Seperetor(string Delim, short Num, string Delim2 = "", short Num2 = 0)
{
    string Sep = "";
    for (int i = 0; i < Num; i++)
        Sep = Sep + Delim;

    for (short j = 0; j < Num2; j++)
        Sep = Sep + Delim2;

    return Sep;
}

void PrintScreen(string Screen)
{
    cout << Seperetor("-", 50) << Seperetor("\n", 1);
    cout << Seperetor("\t", 2) << Screen;
    cout << Seperetor("\n", 1, "-", 50) << Seperetor("\n", 1);
}

bool CheckAccessPermission(enMainMenuePermissions Permission)
{
    if (loggedInUser.Permissions == enMainMenuePermissions::eAll)
        return true;
    if ((Permission & loggedInUser.Permissions) == Permission)
        return true;
    else
        return false;
}

void AccessDeniedMessage()
{
    cout << Seperetor("\n", 1, "-", 36) << Seperetor("\n", 1);
    cout << "Access Denied,\n";
    cout << "You Dont have permission To Do this,\n";
    cout << "Please Conact Your Admin\n";
    cout << Seperetor("-", 36, "\n", 1);

    return;
}

void ShowMainMenue()
{
    system("cls");
    MainMenueScreen();

}

void GoBackToMainMenue()
{
    cout << "\n\nPress any key to go back to Main Menue...";
    system("pause>0");
    ShowMainMenue();
}

void ShowTransactionMenue()
{
    system("cls");
    TransactionMenueScreen();

}

void GoBackToTransactionMenue()
{
    cout << "\n\nPress any key to go back to Transaction Menue...";
    system("pause>0");
    ShowTransactionMenue();
}

void ShowManageUseresMenue()
{
    system("cls");
    ManageUseresScreen();

}

void GoBackToManageUseresMenueMenue()
{
    cout << "\n\nPress any key to go back to Manage Useres Menue...";
    system("pause>0");
    ShowManageUseresMenue();
}

void GoBackToLoginScreen()
{
    Login();
}

string EncryptPassword(string Password, const string EncryptionKey)
{
    for (int i = 0; i < Password.length(); i++)
    {
        int KeyChar = EncryptionKey[i % EncryptionKey.length()];
        Password[i] = char((int)Password[i] + KeyChar);
    }

    return Password;
}

string DecryptPassword(string Password, const string EncryptionKey)
{
    for (int i = 0; i < Password.length(); i++)
    {
        int KeyChar = EncryptionKey[i % EncryptionKey.length()];
        Password[i] = char((int)Password[i] - KeyChar);
    }

    return Password;
}

short ReadMainMenueOption()
{
    short Choice = 0;

    cout << "Choose what do you want to do? [1 to 8]? ";
    cin >> Choice;
    while (cin.fail() || Choice < 1 || Choice > 8)
    {
        cin.clear();
        cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        cin >> Choice;
    }

    return Choice;
}

short ReadTransactionMenueOption()
{
    short Choice = 0;

    cout << "Choose what do you want to do? [1 to 4]? ";
    cin >> Choice;
    while (cin.fail() || Choice < 1 || Choice > 4)
    {
        cin.clear();
        cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        cin >> Choice;
    }

    return Choice;
}

short ReadManageUseresMenueOption()
{
    short Choice = 0;

    cout << "Choose what do you want to do? [1 to 6]? ";
    cin >> Choice;
    while (cin.fail() || Choice < 1 || Choice > 6)
    {
        cin.clear();
        cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        cin >> Choice;
    }

    return Choice;
}

string ReadBaseInfo(string Message)
{
    string BaseInfo;

    cout << endl << Message;
    getline(cin >> ws, BaseInfo);

    return BaseInfo;
}

vector <string> SplitString(string S1, string Delim)
{
    vector <string> vString;

    short Pos = 0;
    string sWord;

    while ((Pos = S1.find(Delim)) != std::string::npos)
    {
        sWord = S1.substr(0, Pos);
        if (sWord != "")
            vString.push_back(sWord);

        S1.erase(0, Pos + Delim.length());
    }

    if (S1 != "")
        vString.push_back(S1);

    return vString;
}

sClient ConvertLinetoRecord(string Line, string Seperator = "#//#")
{
    sClient Client;

    vector<string> vClientData;
    vClientData = SplitString(Line, Seperator);

    if (vClientData.size() != 5)
    {
        cout << "Ignoring invalid line: [" << Line << "]\n";
        Client.MarkForDelete = true;
        return Client;
    }

    Client.AccountNumber = vClientData[0];
    Client.PinCode = vClientData[1];
    Client.Name = vClientData[2];
    Client.Phone = vClientData[3];
    Client.AccountBalance = stod(vClientData[4]);

    return Client;
}

vector <sClient> LoadCleintsDataFromFile(string FileName)
{
    vector <sClient> vClient;

    fstream MyFile;
    MyFile.open(FileName, ios::in);

    if (MyFile.is_open())
    {
        string Line;
        sClient Client;

        while (getline(MyFile, Line))
        {
            if (Line.empty())
                continue;

            Client = ConvertLinetoRecord(Line);

            if (!Client.MarkForDelete)
                vClient.push_back(Client);
        }

        MyFile.close();
    }

    return vClient;
}

void PrintClientRecord(sClient Client)
{
    cout << "| " << setw(15) << left << Client.AccountNumber;
    cout << "| " << setw(10) << left << Client.PinCode;
    cout << "| " << setw(30) << left << Client.Name;
    cout << "| " << setw(12) << left << Client.Phone;
    cout << "| " << setw(12) << left << Client.AccountBalance;
}

void PrintAllClientsData(vector <sClient>& vClient)
{
    cout << Seperetor("\n", 1, "\t", 4) << "Client List (" << vClient.size() << ") Client(s).\n";
    cout << Seperetor("_", 100) << "\n";

    cout << "\n| " << left << setw(15) << "Account Number";
    cout << "| " << left << setw(10) << "Pin Code";
    cout << "| " << left << setw(30) << "Client Name";
    cout << "| " << left << setw(12) << "Phone";
    cout << "| " << left << setw(12) << "Balance" << endl;
    cout << Seperetor("_", 100, "\n", 2);

    for (sClient& Client : vClient)
    {
        PrintClientRecord(Client);
        cout << endl;
    }
    cout << Seperetor("_", 100, "\n", 1);
}

void ShowAllClientsScreen()
{
    if (!CheckAccessPermission(enMainMenuePermissions::pListClients))
    {
        AccessDeniedMessage();
        return;
    }
    vector <sClient> vClient = LoadCleintsDataFromFile(ClientsFileName);
    PrintAllClientsData(vClient);
}

bool FindClientByAccountNumber(string AccountNumber, vector <sClient>& vClients, sClient& Client)
{
    for (sClient& C : vClients)
    {
        if (C.AccountNumber == AccountNumber)
        {
            Client = C;
            return true;
        }
    }

    return false;
}

sClient ReadNewClient()
{
    sClient Client;
    vector<sClient> vClient = LoadCleintsDataFromFile(ClientsFileName);

    cout << "Enter Account Number? ";
    getline(cin >> ws, Client.AccountNumber);

    while (FindClientByAccountNumber(Client.AccountNumber, vClient, Client))
    {
        cout << "Client with [" << Client.AccountNumber << "] already exists, Enter another Account Number? ";
        getline(cin >> ws, Client.AccountNumber);
    }

    cout << "Enter PinCode? ";
    getline(cin, Client.PinCode);

    cout << "Enter Name? ";
    getline(cin, Client.Name);

    cout << "Enter Phone? ";
    getline(cin, Client.Phone);

    cout << "Enter AccountBalance? ";
    cin >> Client.AccountBalance;

    return Client;
}

void AddDataLineToFile(string ClientsFileName, string stDataLine)
{
    fstream MyFile;

    MyFile.open(ClientsFileName, ios::out | ios::app);

    if (MyFile.is_open())
    {
        MyFile << stDataLine << endl;
        MyFile.close();
    }
}

string ConvertRecordToLine(sClient Client, string Seperator = "#//#")
{
    string stClientRecord = "";

    stClientRecord += Client.AccountNumber + Seperator;
    stClientRecord += Client.PinCode + Seperator;
    stClientRecord += Client.Name + Seperator;
    stClientRecord += Client.Phone + Seperator;
    stClientRecord += to_string(Client.AccountBalance);

    return stClientRecord;
}

void AddNewClient()
{
    char AddMore = 'Y';

    do
    {
        system("cls");

        PrintScreen("Add New Clients Screen");
        cout << "Adding New Clients:\n\n";

        sClient Client;

        Client = ReadNewClient();
        AddDataLineToFile(ClientsFileName, ConvertRecordToLine(Client));

        cout << "\nClient Added Successfully, do you want to add more Clients Y/N? ";
        cin >> AddMore;
    } while (toupper(AddMore) == 'Y');
}

void ShowAddNewClientsScreen()
{
    if (!CheckAccessPermission(enMainMenuePermissions::pAddNewClient))
    {
        AccessDeniedMessage();
        return;
    }

    AddNewClient();
}

void PrintClientCard(sClient Client)
{
    cout << "\nThe following are the client details: " << Seperetor("\n", 1, "-", 36);

    cout << "\nAccount Number  : " << Client.AccountNumber << "\n";
    cout << "Pin Code        : " << Client.PinCode << "\n";
    cout << "Name            : " << Client.Name << "\n";
    cout << "Phone           : " << Client.Phone << "\n";
    cout << "Account Balance : " << Client.AccountBalance << Seperetor("\n", 1, "-", 36);

}

void FindClient()
{
    string AccountNumber = ReadBaseInfo("Please enter AccountNumber? ");
    vector<sClient> vClient = LoadCleintsDataFromFile(ClientsFileName);
    sClient Client;

    if (!FindClientByAccountNumber(AccountNumber, vClient, Client))
    {
        cout << "\nClient with Account Number(" << AccountNumber << ") is NOT found!";
        return;
    }
    else
    {
        PrintClientCard(Client);
    }
}

void ShowFindClientScreen()
{
    if (!CheckAccessPermission(enMainMenuePermissions::pFindClient))
    {
        AccessDeniedMessage();
        return;
    }

    PrintScreen("Find Client Screen");
    FindClient();
}

void MarkClientForDeletebyAccountNumber(string AccountNumber, vector <sClient>& vClient)
{
    for (sClient& C : vClient)
    {
        if (C.AccountNumber == AccountNumber)
        {
            C.MarkForDelete = true;
            return;
        }
    }
}

void SaveClientDataToFile(string ClientsFileName, vector <sClient> vClient)
{
    fstream MyFile;
    MyFile.open(ClientsFileName, ios::out);

    string DataLine;
    if (MyFile.is_open())
    {
        for (sClient& C : vClient)
        {
            if (C.MarkForDelete == false)
            {
                DataLine = ConvertRecordToLine(C);
                MyFile << DataLine << endl;
            }
        }
        MyFile.close();
    }
}

void DeleteClient()
{
    string AccountNumber = ReadBaseInfo("Please enter AccountNumber? ");
    vector<sClient> vClient = LoadCleintsDataFromFile(ClientsFileName);
    sClient Client;
    char Answer = 'Y';

    if (FindClientByAccountNumber(AccountNumber, vClient, Client))
    {
        PrintClientCard(Client);

        cout << "\nAre you sure you want to delete this client? Y/N ? ";
        cin >> Answer;
        if (Answer == 'Y' || Answer == 'y')
        {
            MarkClientForDeletebyAccountNumber(AccountNumber, vClient);
            SaveClientDataToFile(ClientsFileName, vClient);

            vClient = LoadCleintsDataFromFile(ClientsFileName);

            cout << "\n\nClient Delete Successfully.";
        }
    }
    else
    {
        cout << "\nClient with Account Number(" << AccountNumber << ") is NOT found!";
    }
}

void ShowDeleteClientScreen()
{
    if (!CheckAccessPermission(enMainMenuePermissions::pDeleteClient))
    {
        AccessDeniedMessage();
        return;
    }

    PrintScreen("Delete Client Screen");
    DeleteClient();
}

sClient ChangeClientRecord(string AccountNumber)
{
    sClient Client;

    Client.AccountNumber = AccountNumber;

    cout << "\nEnter PinCode? ";
    getline(cin >> ws, Client.PinCode);

    cout << "Enter Name? ";
    getline(cin, Client.Name);

    cout << "Enter Phone? ";
    getline(cin, Client.Phone);

    cout << "Enter AccountBalance? ";
    cin >> Client.AccountBalance;

    return Client;
}

void UpdateClient()
{
    string AccountNumber = ReadBaseInfo("Please enter AccountNumber? ");
    vector<sClient> vClient = LoadCleintsDataFromFile(ClientsFileName);
    sClient Client;
    char Answer = 'Y';

    if (FindClientByAccountNumber(AccountNumber, vClient, Client))
    {
        PrintClientCard(Client);

        cout << "\nAre you sure you want to update this client? Y/N ? ";
        cin >> Answer;
        if (Answer == 'Y' || Answer == 'y')
        {
            for (sClient& C : vClient)
            {
                if (C.AccountNumber == AccountNumber)
                    C = ChangeClientRecord(AccountNumber);
            }
            SaveClientDataToFile(ClientsFileName, vClient);

            cout << "\n\nClient Update Successfully.";
        }
    }
    else
    {
        cout << "\nClient with Account Number(" << AccountNumber << ") is NOT found!";
    }
}

void ShowUpdateClientScreen()
{
    if (!CheckAccessPermission(enMainMenuePermissions::pUpdateClient))
    {
        AccessDeniedMessage();
        return;
    }

    PrintScreen("Update Client Screen");
    UpdateClient();
}

double ReadAmount(string Message)
{
    double N;

    cout << endl << Message;
    cin >> N;

    while (cin.fail())
    {
        cin.clear();
        cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        cin >> N;
    }

    return N;
}

void EditAmount(string AccountNumber, vector<sClient>& vClients, double DepositAmount)
{
    for (sClient& C : vClients)
    {
        if (C.AccountNumber == AccountNumber)
        {
            C.AccountBalance += DepositAmount;
            return;
        }
    }
}

void Deposit(vector <sClient> vClients, sClient& Client)
{
    string AccountNumber = ReadBaseInfo("Please enter AccountNumber? ");

    while (!FindClientByAccountNumber(AccountNumber, vClients, Client))
    {
        cout << "\nClient with [" << AccountNumber << "] does NOT exist.\n";
        AccountNumber = ReadBaseInfo("Please enter AccountNumber? ");
    }

    PrintClientCard(Client);

    double DepositAmount = ReadAmount("Please enter deposit amount? ");
    char Answer;

    cout << "\nAre you sure want perform this transaction? Y/N? ";
    cin >> Answer;

    if (toupper(Answer) == 'Y')
    {
        EditAmount(AccountNumber, vClients, DepositAmount);
        SaveClientDataToFile(ClientsFileName, vClients);

        cout << "\n\nDeposit Amount Successfully.";
    }
    else
        return;
}

void ShowDepositScreen()
{
    sClient Client;
    vector <sClient> vClients = LoadCleintsDataFromFile(ClientsFileName);

    PrintScreen("Deposit Screen");
    Deposit(vClients, Client);
}

bool CheckAmount(double WithdrawAmount, sClient Client)
{
    if (WithdrawAmount > Client.AccountBalance)
        return true;
    else
        return false;
}

void Withdraw(vector <sClient> vClients, sClient& Client)
{
    string AccountNumber = ReadBaseInfo("Please enter AccountNumber? ");

    while (!FindClientByAccountNumber(AccountNumber, vClients, Client))
    {
        cout << "\nClient with [" << AccountNumber << "] does NOT exist.\n";
        AccountNumber = ReadBaseInfo("Please enter AccountNumber? ");
    }
    PrintClientCard(Client);

    double WithdrawAmount = ReadAmount("Please enter withdraw amount? ");
    char Answer;

    while (CheckAmount(WithdrawAmount, Client))
    {
        cout << "Amount Exceed the balance, you can withdraw up to : " << Client.AccountBalance;
        WithdrawAmount = ReadAmount("\nPlease enter another amount? ");
    }

    cout << "\n\nAre you sure want perform this transaction? Y/N? ";
    cin >> Answer;

    if (toupper(Answer) == 'Y')
    {
        EditAmount(AccountNumber, vClients, (-1) * WithdrawAmount);
        SaveClientDataToFile(ClientsFileName, vClients);

        cout << "\nWithdraw Amount Successfully.";
    }
    else
        return;
}

void ShowWithdrawScreen()
{
    sClient Client;
    vector <sClient> vClients = LoadCleintsDataFromFile(ClientsFileName);

    PrintScreen("Withdraw Screen");
    Withdraw(vClients, Client);
}

void PrintClientBalanceRecord(sClient Client)
{
    cout << "| " << setw(15) << left << Client.AccountNumber;
    cout << "| " << setw(30) << left << Client.Name;
    cout << "| " << setw(12) << left << Client.AccountBalance;
}

double GetTotalBalances(vector <sClient>& vClient)
{
    double TotBalances = 0;
    for (sClient& C : vClient)
        TotBalances += C.AccountBalance;

    return TotBalances;
}

void TotalBalances(vector <sClient>& vClient)
{
    for (sClient& Client : vClient)
    {
        PrintClientBalanceRecord(Client);
        cout << endl;
    }
    cout << Seperetor("_", 100) << "\n";
    cout << Seperetor("\n", 1, "\t", 4) << "Total Balances = " << GetTotalBalances(vClient);
}

void ShowTotalBalancesScreen()
{
    vector <sClient> vClient = LoadCleintsDataFromFile(ClientsFileName);

    cout << Seperetor("\n", 1, "\t", 4) << "Balance List (" << vClient.size() << ") Client(s).\n";
    cout << Seperetor("_", 100) << "\n";

    cout << "\n| " << left << setw(15) << "Account Number";
    cout << "| " << left << setw(30) << "Client Name";
    cout << "| " << left << setw(12) << "Balance" << endl;
    cout << Seperetor("_", 100, "\n", 2);

    TotalBalances(vClient);
}

void PerfromTransactionMenueOption(enTransactionMenueOptions TransactionMenueOption)
{
    switch (TransactionMenueOption)
    {
    case enTransactionMenueOptions::eDeposit:
        system("cls");
        ShowDepositScreen();
        GoBackToTransactionMenue();
        break;

    case enTransactionMenueOptions::eWithdraw:
        system("cls");
        ShowWithdrawScreen();
        GoBackToTransactionMenue();
        break;

    case enTransactionMenueOptions::eTotalBalances:
        system("cls");
        ShowTotalBalancesScreen();
        GoBackToTransactionMenue();
        break;

    case enTransactionMenueOptions::eMainMenue:
        system("cls");
        ShowMainMenue();
        break;
    }
}

void TransactionMenueScreen()
{
    if (!CheckAccessPermission(enMainMenuePermissions::pTransaction))
    {
        AccessDeniedMessage();
        return;
    }

    cout << Seperetor("=", 45, "\n", 2);
    cout << Seperetor("\t", 2) << "Transactons Menue Screen\n\n";
    cout << Seperetor("=", 45, "\n", 1);

    cout << Seperetor("\t", 1) << "[1] Deposit.\n";
    cout << Seperetor("\t", 1) << "[2] Withdraw.\n";
    cout << Seperetor("\t", 1) << "[3] Total Balance.\n";
    cout << Seperetor("\t", 1) << "[4] Main Menue.\n";
    cout << Seperetor("=", 45, "\n", 1);

    PerfromTransactionMenueOption((enTransactionMenueOptions)ReadTransactionMenueOption());
}

sUser ConvertLinetoRecordUser(string Line, string Seperator = "#//#")
{
    sUser User;

    vector<string> vUserData;
    vUserData = SplitString(Line, Seperator);

    if (vUserData.size() != 3)
    {
        cout << "Ignoring invalid line: [" << Line << "]\n";
        User.MarkForDelete = true;
        return User;
    }

    User.UserName = vUserData[0];
    User.Password = DecryptPassword(vUserData[1], EncryptionKey);
    User.Permissions = stoi(vUserData[2]);

    return User;
}

vector <sUser> LoadUserDateFromFile(string FileName)
{
    vector <sUser> vUsers;

    fstream MyFile;
    MyFile.open(FileName, ios::in);

    if (MyFile.is_open())
    {
        string Line;
        sUser User;

        while (getline(MyFile, Line))
        {
            if (Line.empty())
                continue;

            User = ConvertLinetoRecordUser(Line);

            if (!User.MarkForDelete)
                vUsers.push_back(User);
        }

        MyFile.close();
    }

    return vUsers;
}

sUser ReadUserInfo()
{
    sUser User;

    cout << "Enter UserName? ";
    getline(cin >> ws, User.UserName);

    cout << "Enter Password? ";
    getline(cin >> ws, User.Password);

    User.Password = EncryptPassword(User.Password, EncryptionKey);

    return User;
}

bool FindUserByUserNameAndPassword(sUser Users, sUser& loggedInUser)
{
    vector <sUser> vUsers = LoadUserDateFromFile(UsersFileName);

    for (sUser& U : vUsers)
    {
        if (U.UserName == Users.UserName && U.Password == DecryptPassword(Users.Password, EncryptionKey))
        {
            loggedInUser = U;
            return true;
        }
    }

    return false;
}

bool LoadUserInfo(sUser User)
{
    if (FindUserByUserNameAndPassword(User, loggedInUser))
        return true;
    else
        return false;
}

bool FindUserByUserName(vector <sUser>& vUsers, string UserName, sUser& User)
{
    for (sUser& U : vUsers)
    {
        if (U.UserName == UserName)
        {
            User = U;
            return true;
        }
    }

    return false;
}

string PrintPasswordAsStars()
{
    string Password = "";
    for (short i = 0; i < 8; i++)
    {
        Password += "*";
    }

    return Password;
}

string CheckifAdmin(string Password, sUser loggedInUser, string UserName)
{
    if (loggedInUser.UserName == UserName)
        return Password;
    else if (loggedInUser.UserName == "Admin")
        return EncryptPassword(Password, EncryptionKey);
    else
        return PrintPasswordAsStars();
}

void PrintUserRecord(sUser User)
{
    cout << "| " << setw(15) << left << User.UserName;
    cout << "| " << setw(10) << left << CheckifAdmin(User.Password, loggedInUser, User.UserName);
    cout << "| " << setw(30) << left << User.Permissions;
}

void PrintAllUsersData(vector <sUser>& vUsers)
{
    cout << Seperetor("\n", 1, "\t", 4) << "Users List (" << vUsers.size() << ") User(s).\n";
    cout << Seperetor("_", 100, "\n", 2);

    cout << "| " << left << setw(15) << "User Name";
    cout << "| " << left << setw(10) << "Password";
    cout << "| " << left << setw(30) << "Permissions" << endl;
    cout << Seperetor("_", 100, "\n", 2);

    for (sUser& User : vUsers)
    {
        PrintUserRecord(User);
        cout << endl;
    }
    cout << Seperetor("_", 100, "\n", 1);
}

void ShowListUsers()
{
    vector <sUser> vUsers = LoadUserDateFromFile(UsersFileName);
    PrintAllUsersData(vUsers);
}

int ReadUserPermission()
{
    int Permissions = 0;
    char Answer = 'N';

    cout << "\nDo you want to give full access? Y/N? ";
    cin >> Answer;

    if (toupper(Answer) == 'Y')
        return -1;

    cout << "\nDo you want to give access to : \n";

    cout << "\nShow Client List? y/n? ";
    cin >> Answer;
    if (toupper(Answer) == 'Y')
        Permissions += enMainMenuePermissions::pListClients;

    cout << "\nAdd New Client? y/n? ";
    cin >> Answer;
    if (toupper(Answer) == 'Y')
        Permissions += enMainMenuePermissions::pAddNewClient;

    cout << "\nDelete Client? y/n? ";
    cin >> Answer;
    if (toupper(Answer) == 'Y')
        Permissions += enMainMenuePermissions::pDeleteClient;

    cout << "\nUpdate Client? y/n? ";
    cin >> Answer;
    if (toupper(Answer) == 'Y')
        Permissions += enMainMenuePermissions::pUpdateClient;

    cout << "\nFind Client? y/n? ";
    cin >> Answer;
    if (toupper(Answer) == 'Y')
        Permissions += enMainMenuePermissions::pFindClient;

    cout << "\nTransactions? y/n? ";
    cin >> Answer;
    if (toupper(Answer) == 'Y')
        Permissions += enMainMenuePermissions::pTransaction;

    cout << "\nManage Users? y/n? ";
    cin >> Answer;
    if (toupper(Answer) == 'Y')
        Permissions += enMainMenuePermissions::pManageUsers;

    return Permissions;
}

sUser ReadNewUser(vector <sUser> vUser, sUser User)
{
    cout << "Enter UserName? ";
    getline(cin >> ws, User.UserName);

    while (FindUserByUserName(vUser, User.UserName, User))
    {
        cout << "Users with [" << User.UserName << "] already exists, Enter another UserName? ";
        getline(cin >> ws, User.UserName);
    }

    cout << "Enter Password? ";
    getline(cin, User.Password);

    User.Permissions = ReadUserPermission();

    return User;
}

string ConvertUserRecordToLine(sUser User, string Seperator = "#//#")
{
    string stUserRecord = "";

    stUserRecord += User.UserName + Seperator;
    stUserRecord += EncryptPassword(User.Password, EncryptionKey) + Seperator;
    stUserRecord += to_string(User.Permissions);

    return stUserRecord;
}

void AddNewUser()
{
    char AddMore = 'Y';

    do
    {
        system("cls");

        PrintScreen("Add New Users Screen");
        cout << "Adding New User:\n\n";

        vector <sUser> vUser = LoadUserDateFromFile(UsersFileName);
        sUser User;

        User = ReadNewUser(vUser, User);
        AddDataLineToFile(UsersFileName, ConvertUserRecordToLine(User));

        cout << "\nUser Added Successfully, do you want to add more Users Y/N? ";
        cin >> AddMore;
    } while (toupper(AddMore) == 'Y');
}

void ShowAddNewUserScreen()
{
    AddNewUser();
}

void PrintUserCard(sUser User)
{
    cout << Seperetor("-", 20, "\n", 1);
    cout << "UserName    : " << User.UserName << "\n";
    cout << "Password    : " << CheckifAdmin(User.Password, loggedInUser, User.UserName) << "\n";
    cout << "Permissions : " << User.Permissions << "\n";
    cout << Seperetor("-", 20, "\n", 1);
}

void MarkUserForDeletebyUserName(string UserName, vector <sUser>& vUser)
{
    for (sUser& U : vUser)
    {
        if (U.UserName == UserName)
        {
            U.MarkForDelete = true;
            return;
        }
    }
}

void SaveUserDataToFile(string UserFileName, vector <sUser>& vUser)
{
    fstream MyFile;
    MyFile.open(UserFileName, ios::out);

    string DataLine;
    if (MyFile.is_open())
    {
        for (sUser& U : vUser)
        {
            if (U.MarkForDelete == false)
            {
                DataLine = ConvertUserRecordToLine(U);
                MyFile << DataLine << endl;
            }
        }
        MyFile.close();
    }
}

void DeleteUser()
{
    string UserName = ReadBaseInfo("Please enter UserName? ");
    vector<sUser> vUser = LoadUserDateFromFile(UsersFileName);
    sUser User;
    char Answer = 'Y';

    if (UserName == "Admin")
    {
        cout << "\nYou Cannot Delete This User\n";
        return;
    }

    if (FindUserByUserName(vUser, UserName, User))
    {
        PrintUserCard(User);

        cout << "\nAre you sure you want to delete this User? Y/N ? ";
        cin >> Answer;
        if (Answer == 'Y' || Answer == 'y')
        {
            MarkUserForDeletebyUserName(UserName, vUser);
            SaveUserDataToFile(UsersFileName, vUser);

            vUser = LoadUserDateFromFile(UsersFileName);

            cout << "\n\nUser Delete Successfully.";
        }
    }
    else
    {
        cout << "\nUser with UserName (" << UserName << ") is NOT found!";
    }
}

void ShowDeleteUserScreen()
{
    PrintScreen("Delete User Screen");
    DeleteUser();
}

sUser ChangeUserRecord(string UserName)
{
    sUser User;

    User.UserName = UserName;

    cout << "\nEnter Password? ";
    getline(cin >> ws, User.Password);

    User.Permissions = ((UserName == "Admin") ? -1 : ReadUserPermission());

    return User;
}

void UpdateUser()
{
    string UserName = ReadBaseInfo("Please enter UserName? ");
    vector<sUser> vUser = LoadUserDateFromFile(UsersFileName);
    sUser User;
    char Answer = 'Y';

    if (FindUserByUserName(vUser, UserName, User))
    {
        PrintUserCard(User);

        cout << "\nAre you sure you want to update this User? Y/N ? ";
        cin >> Answer;
        if (Answer == 'Y' || Answer == 'y')
        {
            for (sUser& U : vUser)
            {
                if (U.UserName == UserName)
                    U = ChangeUserRecord(UserName);
            }
            SaveUserDataToFile(UsersFileName, vUser);

            cout << "\n\nUser Update Successfully.";
        }
    }
    else
    {
        cout << "\nClient with User Name (" << UserName << ") is NOT found!";
    }
}

void ShowUpdateUserScreen()
{
    PrintScreen("Update User Screen");
    UpdateUser();
}

void FindUser()
{
    string UserName = ReadBaseInfo("Please enter UserName? ");
    vector <sUser> vUser = LoadUserDateFromFile(UsersFileName);
    sUser User;

    if (!FindUserByUserName(vUser, UserName, User))
    {
        cout << "\nUser with User Name (" << UserName << ") is NOT found!";
        return;
    }
    else
        PrintUserCard(User);
}

void ShowFindUserScreen()
{
    PrintScreen("Find User Screen");
    FindUser();
}

void PerfromManageUseresMenueOption(enManageUseresMenueOptions ManageUseresMenueOption)
{
    switch (ManageUseresMenueOption)
    {
    case enManageUseresMenueOptions::eListUsers:
        system("cls");
        ShowListUsers();
        GoBackToManageUseresMenueMenue();
        break;

    case enManageUseresMenueOptions::eAddNewUsers:
        system("cls");
        ShowAddNewUserScreen();
        GoBackToManageUseresMenueMenue();
        break;

    case enManageUseresMenueOptions::eDeleteUser:
        system("cls");
        ShowDeleteUserScreen();
        GoBackToManageUseresMenueMenue();
        break;

    case enManageUseresMenueOptions::eUpdateUser:
        system("cls");
        ShowUpdateUserScreen();
        GoBackToManageUseresMenueMenue();
        break;

    case enManageUseresMenueOptions::eFindUser:
        system("cls");
        ShowFindUserScreen();
        GoBackToManageUseresMenueMenue();
        break;

    case enManageUseresMenueOptions::enMainMenue:
        system("cls");
        ShowMainMenue();
        break;
    }
}

void ManageUseresScreen()
{
    if (!CheckAccessPermission(enMainMenuePermissions::pManageUsers))
    {
        AccessDeniedMessage();
        return;
    }

    cout << Seperetor("=", 45, "\n", 2);
    cout << Seperetor("\t", 2) << "Manage Users Menue Screen\n\n";
    cout << Seperetor("=", 45, "\n", 1);

    cout << Seperetor("\t", 1) << "[1] List Users.\n";
    cout << Seperetor("\t", 1) << "[2] Add New User.\n";
    cout << Seperetor("\t", 1) << "[3] Delete User.\n";
    cout << Seperetor("\t", 1) << "[4] Update User.\n";
    cout << Seperetor("\t", 1) << "[5] Find User.\n";
    cout << Seperetor("\t", 1) << "[6] Main Menue.\n";
    cout << Seperetor("=", 45, "\n", 1);

    PerfromManageUseresMenueOption((enManageUseresMenueOptions)ReadManageUseresMenueOption());
}

void PerfromMainMenueOption(enMainMenueOptions MainMenueOption)
{
    switch (MainMenueOption)
    {
    case enMainMenueOptions::eListClients:

        system("cls");
        ShowAllClientsScreen();
        GoBackToMainMenue();
        break;

    case enMainMenueOptions::eAddNewClient:
        system("cls");
        ShowAddNewClientsScreen();
        GoBackToMainMenue();
        break;

    case enMainMenueOptions::eDeleteClient:
        system("cls");
        ShowDeleteClientScreen();
        GoBackToMainMenue();
        break;

    case enMainMenueOptions::eUpdateClient:
        system("cls");
        ShowUpdateClientScreen();
        GoBackToMainMenue();
        break;

    case enMainMenueOptions::eFindClient:
        system("cls");
        ShowFindClientScreen();
        GoBackToMainMenue();
        break;

    case enMainMenueOptions::eTransactionMenu:
        system("cls");
        TransactionMenueScreen();
        GoBackToMainMenue();
        break;

    case enMainMenueOptions::eManageUsers:
        system("cls");
        ManageUseresScreen();
        GoBackToMainMenue();
        break;

    case enMainMenueOptions::eLogout:
        system("cls");
        GoBackToLoginScreen();
        break;
    }
}

void MainMenueScreen()
{
    system("cls");

    cout << Seperetor("=", 45, "\n", 2);
    cout << Seperetor("\t", 2) << "Main Menue Screen\n\n";
    cout << Seperetor("=", 45, "\n", 1);

    cout << Seperetor("\t", 1) << "[1] Show Client List.\n";
    cout << Seperetor("\t", 1) << "[2] Add New Client.\n";
    cout << Seperetor("\t", 1) << "[3] Delete Client.\n";
    cout << Seperetor("\t", 1) << "[4] Update Client Info.\n";
    cout << Seperetor("\t", 1) << "[5] Find Client.\n";
    cout << Seperetor("\t", 1) << "[6] Transactions.\n";
    cout << Seperetor("\t", 1) << "[7] Manage Users.\n";
    cout << Seperetor("\t", 1) << "[8] Logout.\n";
    cout << Seperetor("=", 45, "\n", 1);

    PerfromMainMenueOption((enMainMenueOptions)ReadMainMenueOption());

}

void Login()
{
    bool LoginFaild = false;

    do
    {
        system("cls");
        PrintScreen("Login Screen");

        if (LoginFaild)
            cout << "Invlaid UserName/Password!\n";

        sUser User = ReadUserInfo();

        LoginFaild = !LoadUserInfo(User);

    } while (LoginFaild);

    cout << loggedInUser.UserName;
    cout << endl << loggedInUser.Password;
    cout << endl << loggedInUser.Permissions;
    cout << endl << loggedInUser.MarkForDelete;
    system("pause>0");
    MainMenueScreen();
}

int main()
{
    Login();


    system("Pause>0");
    return 0;
}