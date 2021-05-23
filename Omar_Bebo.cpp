#include <iostream>
#include <fstream>
#include <iomanip>
#include <cstdlib>
#include "ClientData.cpp" // ClientData class definition

using namespace std;

int enterChoice();
void createTextFile(fstream&);
void updateRecord(fstream&);
void newRecord(fstream&);
void deleteRecord(fstream&);
void BackUp();
void Restore();
void outputLine( ostream&, const ClientData & );
int getAccount( const char * const );

enum Choices { PRINT = 1, UPDATE, NEW, DELETE,BACKUP,RESTORE,END };


                    //----------------------------------------Main----------------------------------------//
int main(){

    // open file for reading and writing
    fstream inOutCredit( "credit.dat", ios::in | ios::out | ios::binary );
    // exit program if fstream cannot open file
    if (!inOutCredit){
    cerr << "File could not be opened." << endl;
    exit ( EXIT_FAILURE );
    }

    int choice;
    // enable user to specify action
     while ((choice=enterChoice()) != END ){
        switch ( choice ){
        case PRINT:     createTextFile( inOutCredit );
        break;
        case UPDATE:    updateRecord( inOutCredit );
        break;
        case NEW:       newRecord( inOutCredit );
        break;
        case DELETE:    deleteRecord( inOutCredit );
        break;
        case BACKUP:    BackUp();
        break;
        case RESTORE:   Restore();
        break;                
        default:        cerr << "Incorrect choice" << endl;
        break;
        }         inOutCredit.clear();
     }
}
                    //----------------------------------------enterChoice----------------------------------------//
int enterChoice(){

    // display available options
    cout << "\nEnter your choice" << endl
        << "1 - store a formatted text file of accounts" << "_ called \"print.txt\" for printing" << endl
        << "2 - update an account" << endl
        << "3 - add a new account" << endl
        << "4 - delete an account" << endl
        << "5 - Backup Data"       << endl
        << "6 - Restore Data"      << endl
        << "7 - end program\n? ";


    int menuChoice;

    cin >> menuChoice;
    return menuChoice;
}
                    //----------------------------------------createTextFile----------------------------------------//
void createTextFile(fstream &readFromFile){

    //create text file
    ofstream outPrintFile( "print.txt", ios::out );

    // exit program if ofstream cannot create file
    if (!outPrintFile){
    cerr << "File could not be created." << endl;
    exit( EXIT_FAILURE );
    }

    // output column heads
    outPrintFile << left << setw( 10 ) << "Account" <<
                            setw( 16 )<< "Last Name" <<
                            setw( 11 ) << "First Name" << right <<
                            setw( 10 ) << "Balance" << endl;

    // set file-position pointer to beginning of readFromFile
    readFromFile.seekg( 0 );

    // read first record from record file
    ClientData client;
    readFromFile.read( reinterpret_cast< char * >( &client ),
    sizeof( ClientData ) );


    // copy all records from record file into text file
    while (!readFromFile.eof()){
    // write single record to text file
        if (client.getAccountNumber() != 0 ) // skip empty records
            outputLine( outPrintFile, client );

        // read next record from record file
        readFromFile.read( reinterpret_cast< char * >( &client ),
        sizeof( ClientData ) );
    } // end while
}
                    //----------------------------------------updateRecord----------------------------------------//
void updateRecord( fstream &updateFile ){

    // obtain number of account to update
    int accountNumber = getAccount( "Enter account to update" );

    // move file-position pointer to correct record in file
    updateFile.seekg( ( accountNumber - 1 ) * sizeof( ClientData ) );

    // read first record from file
    ClientData client;
    updateFile.read( reinterpret_cast< char * >( &client ),
    sizeof( ClientData ) );

    // update record
    if ( client.getAccountNumber() != 0 ){
        outputLine( cout, client );

        // request user to specify transaction
        cout << "\nEnter charge (+) or payment (-): ";
        double transaction; // charge or payment
        cin >> transaction;

        // update record balance
        double oldBalance = client.getBalance();
        client.setBalance( oldBalance + transaction );
        outputLine( cout, client ); // display the record

        // move file-position pointer to correct record in file
        updateFile.seekp( ( accountNumber - 1 ) * sizeof( ClientData ) );

        // write updated record over old record in file
        updateFile.write( reinterpret_cast< const char * >( &client ),
        sizeof( ClientData ) );
    }
    else // display error if account does not exist
    cerr << "Account #" << accountNumber << " has no information." << endl;
}
                    //----------------------------------------newRecord----------------------------------------//
void newRecord( fstream &insertInFile ){

    // obtain number of account to create
    int accountNumber = getAccount( "Enter new account number" );

    // move file-position pointer to correct record in file
    insertInFile.seekg( ( accountNumber - 1 ) * sizeof( ClientData ) );

    // read record from file
    ClientData client;
    insertInFile.read( reinterpret_cast< char * >( &client ),sizeof( ClientData ) );

    // create record, if record does not previously exist
    if ( client.getAccountNumber() == 0 ){
        string lastName;
        string firstName;
        double balance;

        // user enters last name, first name and balance
        cout << "Enter lastname, firstname, balance\n? ";
        cin >> setw( 15 ) >> lastName;
        cin >> setw( 10 ) >> firstName;
        cin >> balance;

        // use values to populate account values
        client.setLastName( lastName );
        client.setFirstName( firstName );
        client.setBalance( balance );
        client.setAccountNumber( accountNumber );

        // move file-position pointer to correct record in file
        insertInFile.seekp( ( accountNumber - 1 ) * sizeof( ClientData ) );

        // insert record in file
        insertInFile.write( reinterpret_cast< const char * >( &client ),
        sizeof( ClientData ) );
    }
    else // display error if account already exists
        cerr << "Account #" << accountNumber << " already contains information." << endl;
}
                    //----------------------------------------deleteRecord----------------------------------------//
void deleteRecord( fstream &deleteFromFile ){

    // obtain number of account to delete
    int accountNumber = getAccount( "Enter account to delete" );

    // move file-position pointer to correct record in file
    deleteFromFile.seekg( ( accountNumber - 1 ) * sizeof( ClientData ) );

    // read record from file
    ClientData client;
    deleteFromFile.read( reinterpret_cast< char * >( &client ),
    sizeof( ClientData ) );

    // delete record, if record exists in file
    if ( client.getAccountNumber() != 0 ){
        ClientData blankClient; // create blank record

        // move file-position pointer to correct record in file
        deleteFromFile.seekp( ( accountNumber - 1 ) *
        sizeof( ClientData ) );

        // replace existing record with blank record
        deleteFromFile.write(reinterpret_cast< const char * >( &blankClient ),
        sizeof( ClientData ) );

        cout << "Account #" << accountNumber << " deleted.\n";
    }
    else // display error if record does not exist
    cerr << "Account #" << accountNumber << " is empty.\n";
}
                    //----------------------------------------'Backup Data'----------------------------------------//
void BackUp(){

    ofstream first("Backup.dat",ios::out | ios::binary);
    if(!first){
        cerr << "file couldnd not be opened." << endl;
        exit(EXIT_FAILURE);
    }
    ifstream second("credit.dat",ios::in | ios::binary);
    if(!second){
        cerr << "file couldnd not be opened." << endl;
        exit(EXIT_FAILURE);
    }
    copy(istreambuf_iterator<char>(second),istreambuf_iterator<char>(),ostreambuf_iterator<char>(first));
}
                    //----------------------------------------'Restore Data'----------------------------------------//
void Restore(){
    ofstream first("credit.dat",ios::out | ios::binary);
    if(!first){
        cerr << "file couldnd not be opened." << endl;
        exit(EXIT_FAILURE);
    }
    ifstream second("Backup.dat",ios::in | ios::binary);
    if(!second){
        cerr << "file couldnd not be opened." << endl;
        exit(EXIT_FAILURE);
    }
    copy(istreambuf_iterator<char>(second),istreambuf_iterator<char>(),ostreambuf_iterator<char>(first));
}
                    //----------------------------------------outputLine----------------------------------------//
void outputLine( ostream &output, const ClientData &record ){
    output << left << setw( 10 ) << record.getAccountNumber()
                   << setw( 16 ) << record.getLastName()
                   << setw( 11 ) << record.getFirstName()
                   << setw( 10 ) << setprecision( 2 ) << right << fixed
                   << showpoint << record.getBalance() << endl;
}
                    //----------------------------------------getAccount----------------------------------------//
int getAccount( const char * const prompt ){
    int accountNumber;

    // obtain account-number value
    do{
    cout << prompt << " (1 - 100): ";
    cin >> accountNumber;
    } while ( accountNumber < 1 || accountNumber > 100 );

    return accountNumber;
}
