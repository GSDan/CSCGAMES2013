#include "Transaction.h"
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <queue>
#include <ctime>
#include <vector>
#include <stdlib.h>  
#include <math.h>  
#include <iomanip>   
#include <thread> 
#include <chrono>
#include <mutex>  

std::mutex mtx;
std::queue<int> times;


int validResponse(int lhs, int rhs){

	//will only allow the user to continue if they enter a number between lhs and rhs (inclusive)
	int usrIn = 100;
	bool valid = 0;
	while(!valid){
		std::cin >> usrIn;
		for(int i = 0; i < rhs; i++){
			if(lhs <= usrIn && rhs >= usrIn){
				valid = 1;
				return usrIn;
			}
		}
		std::cout << "\nInvalid response\n";
		std::cin.ignore();
		std::cin.clear();	//clears cin after error
		std::cin.sync();	//clears cin after error
	}
}

void populateQueue(std::queue<int> &q, int num){
	//populate a given queue with a given number of random ints
	for(int i = 0; i< num; i++){
		int temp = rand() % 25;
		q.push(temp);
	}

}

void addTransaction(char thread, std::queue<int> &q, int &money, std::vector<Transaction> &transactions){
	if(q.empty()){
		//no point in continuing
		return;
	}

	//ascertain if the action should be to withdraw or deposit
	//as the action alternate in the queue, we can use modulus to check
	bool withdrawal = false;
	if(q.size() % 2  == 0){
		withdrawal = true;
		money++;
	}

	//retrieve the bank account num from the queue and remove it
	int account = q.front();
	q.pop();

	//create a transaction object and add it to the vector
	Transaction trans(withdrawal, account, money, thread);
	transactions.push_back(trans);
}

std::vector<Transaction> createRow(){

	std::queue<int> threadA;
	int Avalue = rand() % 9;
	std::queue<int> threadB;
	int Bvalue = rand() % 9;
	std::queue<int> threadC;
	int Cvalue = rand() % 9;
	std::queue<int> threadD;
	int Dvalue = rand() % 9;
	std::queue<int> threadE;
	int Evalue = rand() % 9;

	populateQueue(threadA, 4);
	populateQueue(threadB, 4);
	populateQueue(threadC, 4);
	populateQueue(threadD, 4);
	populateQueue(threadE, 4);

	std::vector<Transaction> transactions;

	//while at least one thread has an element remaining
	while(!threadA.empty() || !threadB.empty() || !threadC.empty() || !threadD.empty() || !threadE.empty()){
		
		//randomly choose a thread to access
		int thread = rand() % 5;
		switch (thread)
		{
		case 0:
			addTransaction('a',threadA,Avalue,transactions);
			break;
		case 1:
			addTransaction('b',threadB,Bvalue,transactions);
			break;
		case 2:
			addTransaction('c',threadC,Cvalue,transactions);
			break;
		case 3:
			addTransaction('d',threadD,Dvalue,transactions);
			break;
		case 4:
			addTransaction('e',threadE,Evalue,transactions);
			break;
		}
	}
	return transactions;
}

void createRowThreaded(char thread, std::vector<Transaction> &transactions, std::queue<int> &threadQueue, int money){

	while(!threadQueue.empty()){

		mtx.lock();
			std::chrono::microseconds dura(times.front());
			times.pop();
		mtx.unlock();

		std::this_thread::sleep_for( dura );
		
		//ascertain if the action should be to withdraw or deposit
		//as the action alternates in the queue, we can use modulus to check
		bool withdrawal = false;
		if(threadQueue.size() % 2  == 0){
			withdrawal = true;
			money++;
		}

		//retrieve the bank account num from the queue and remove it
		int account = threadQueue.front();
		threadQueue.pop();

		//create a transaction object and add it to the vector
		Transaction trans(withdrawal, account, money, thread);

		mtx.lock();
			transactions.push_back(trans);
		mtx.unlock();
	}
}

std::vector<Transaction> convertRow(std::string row){
	
	//convert transactions from strings to objects for processing

	char threadID = 'z';
	int accountNum = 100;
	char accountChar1 = 'a';
	char accountChar2 = 'a';
	std::string money = "";
	bool boolSet = false;
	bool withdrawal;

	std::vector<Transaction> transactions;

	//loop through each of the string's chars
	for(size_t i = 0; i < row.size(); i++){
		char thisChar = row[i];

		//if this character is a comma or new line, complete the current transaction
		//and add it to the vector
		if(thisChar == ',' || thisChar == '\t' || thisChar == '\n' || i == row.size()-1) {

			if(i == row.size()-1){
				money += thisChar;
			}

			Transaction trans(withdrawal, accountNum, atoi(money.c_str()), threadID); 
			transactions.push_back(trans);

			//reset values for next object
			threadID = 'z';
			boolSet = false;
			accountNum = 100;
			accountChar1 = 'a';
			accountChar2 = 'a';
			money = "";

		}
		else{
			if(threadID == 'z'){
				//thread ID has not been assigned for this transaction
				threadID = thisChar;
			}
			else if(!boolSet){
				//the withdrawal bool hasn't been set yet
				if(thisChar == 'W'){
					withdrawal = true;
					boolSet = true;
				}else if(thisChar == 'D'){
					withdrawal = false;
					boolSet = true;
				}
			}
			else if(accountNum == 100){
				if(thisChar != '_'){
					//account number can be double digit
					if(accountChar1 == 'a')
						accountChar1 = thisChar;
					else if(accountChar2 == 'a')
						accountChar2 = thisChar;
				}
				else{
					//assign account number
					std::string temp ="";
					temp.push_back(accountChar1);

					if(accountChar2 != 'a')
						temp.push_back(accountChar2);

					accountNum = atoi(temp.c_str());
				}
			}
			else{
				//last digit is amount of money. Can be double digit (10)
				//so we create a string, as as many chars as needed then convert
				money += thisChar;
			}
		}
	}

	return transactions;
}

bool assessRow(std::vector<Transaction> transactions, bool shouldPrint){

	bool partnerFound = false;

	for(int lhs = 0; lhs < transactions.size(); lhs++){

		//see if partner has been found
		if(!transactions[lhs].isChecked()){

			if(!transactions[lhs].isWithdrawal()){
				if (shouldPrint)
					std::cout << "\nA wild deposit appeared!\n";
				return false;
			}

			for(int rhs = (lhs + 1); rhs < transactions.size(); rhs++){
				if(transactions[lhs].isPartner(transactions[rhs])){
					//threads are partners
					//we now need to work BACKWARDS making sure that no one else used the
					//same account as the deposit inbetween these partner actions
					for(int i = rhs -1 ; i > lhs; i--){
						if(transactions[i].getAccountNum() == transactions[rhs].getAccountNum()){
							if (shouldPrint)
								std::cout << "\nError found upon retreading from element " << rhs;
							return false;
						}							
					}
					//if we got this far, we have a legit partnership and can move on
					transactions[rhs].setChecked(true);
					partnerFound = true;
					break;
				}					
				else if((transactions[lhs].getThreadID() == transactions[rhs].getThreadID()) 
						|| (transactions[lhs].getAccountNum() == transactions[rhs].getAccountNum())){
					/*transactions were made by the same thread but aren't partners
					OR transactions made on the same account by different threads
					before the first thread finished */
					if (shouldPrint){
						
						std::cout << "\n" << transactions[lhs].toString() << " WAS INTERRUPTED BY " << transactions[rhs].toString();
					}
					return false;
				}				
			}
			if(!partnerFound){
				//no partner found?
				if (shouldPrint){
					std::cout <<"\n"<< transactions[lhs].toString() <<" has no partner";
				}
				return false;
			}
			partnerFound = false;			
		}		
	}
	//all items had suitable partners and weren't interrupted
	return true;
}

void insertCoin(int &credit, int &spent){
	int response;
	std::cout << "You've run out of credit! Insert some money.\n\n1. 10p\n2. 20p\n3. 50p\n4. "<<(char)156 << "1.00\n";
	response = validResponse(1,4);
	switch(response){
		case 1:
			credit += 10;
			spent += 10;
			break;
		case 2:
			credit += 20;
			spent += 20;
			break;
		case 3:
			credit += 50;
			spent += 50;
			break;
		case 4:
			credit += 100;
			spent += 100;
			break;
	}
	system("cls");	
}

bool validateFile(std::string filename){
	//read in file at given address
	std::ifstream infile;
	infile.open(filename);

	if(!infile.is_open()){
		std::cout << "\nThe file " << filename << " was not found, aborting.\n";
		return false;
	}
	
	std::string line;
	std::cout << "\n\nAssessing file " << filename << "\n";
	int incorrect = 0;
	while(std::getline(infile, line)){
		std::size_t found = line.find("rows");
		if (found!=std::string::npos){
			//if the line contains the word "rows" pull the int from it and store
			std::stringstream ss(line);
			std::string temp;
			char eq;
			int numRows;
			ss >> temp >> eq >> numRows;

			//read in junk "WINNER" line
			std::getline(infile, line);

			for(int i = 0; i < numRows; i++){
				std::getline(infile, line);
				std::cout << "\nASSESSING:\n" << line;
				//line is now a row. Convert into transaction objects and assess
				std::vector<Transaction> transactions = convertRow(line);
				if(!assessRow(transactions, true)){
					incorrect ++;
					std::cout << "  FAILED\n";
				}
				else{
					std::cout << "  PASSED\n";
				}
			}
		}
	}
	
	if(incorrect == 0){
		std::cout << "\nFile scrubbed with no errors found.\n";
		return true;
	}else{
		std::cout << "\n" << incorrect << " errors found while assessing " << filename << "\n";
		return false;
	}
	
}

int main(){
	srand((unsigned int)time( NULL ) );
	int numRows;
	bool finishedGame = false;
	bool finishedProgram = false;
	int response;
	double odds = 0.17;
	int credit = 0;
	int spent = 0;

	while(!finishedProgram){
		system("cls");
		std::cout << "*********\nMAIN MENU\n*********\n\nWould you like to play the game, assess an existing file or quit?\n\n1. Play game!\n2. Assess a file\n3. Quit\n";
		response = validResponse(1,3);
		if(response == 1){
			//play the game
			finishedGame = false;
			credit = 0;
			spent = 0;

			while(!finishedGame){

			system("cls");
			if(credit < 10)
				insertCoin(credit, spent);

			std::cout << "How many rows would you like to generate?  CREDIT: "<< credit << "\n";
			std::cin >> numRows;
			credit -= 10;
			system("cls");

			//toFile string will eventually be printed to file
			std::string toFile = "";

			//number of successful rows
			double hit = 0;

			for(int i = 0; i < numRows; i++){
				
				
				for(int j = 0; j < 20; j++){
					times.push(rand() % 5000);	
				}

				std::queue<int> threadA;
				int Amoney = rand() % 9;
				std::queue<int> threadB;
				int Bmoney = rand() % 9;
				std::queue<int> threadC;
				int Cmoney = rand() % 9;
				std::queue<int> threadD;
				int Dmoney = rand() % 9;
				std::queue<int> threadE;
				int Emoney = rand() % 9;
				
				populateQueue(threadA, 4);
				populateQueue(threadB, 4);
				populateQueue(threadC, 4);
				populateQueue(threadD, 4);
				populateQueue(threadE, 4);

				
				//create threads and set them off
				std::vector<Transaction> row;
				
				std::thread tA(createRowThreaded,'a',std::ref(row), threadA, Amoney);				
				std::thread tB(createRowThreaded,'b',std::ref(row), threadB, Bmoney);				
				std::thread tC(createRowThreaded,'c',std::ref(row), threadC, Cmoney);				
				std::thread tD(createRowThreaded,'d',std::ref(row), threadD, Dmoney);	
				std::thread tE(createRowThreaded,'e',std::ref(row), threadE, Emoney);

				//wait for all to finish
				tA.join();tB.join();tC.join();tD.join();tE.join();
				
				//std::vector<Transaction> row = createRow();
				std::string rowString;
				for(int j = 0; j < row.size(); j++){
					rowString += row[j].toString() += ",";
				}
				std::cout << "\n\n" << rowString;
				if( assessRow(row, true)){
					hit++;
					for(int j =0; j < row.size(); j++){
						toFile += row[j].toString();
						if(j != row.size()-1)
							toFile += ",";  
						else
							toFile += "\n";
					}			
				}
			
			}

			if(numRows == hit){
				int winnings = 100 / (pow(odds, numRows)*100)*9;
				std::ofstream outfile;
				outfile.open("CWoutput.txt", std::ios_base::app);
				outfile << "rows = " << numRows << "\nWINNER\n" << toFile << "\n\n";
				std::cout << "\n\nYou are a winner!\n";
				std::wcout << "You have been credited with " << winnings << "p\n";
				credit += winnings;
			}

			std::cout << "\n\n" << hit << " successful rows, with a success rate of " << (int)(hit/numRows*100) << "%";
			std::cout << "\nThe odds of winning were " << pow(odds, numRows)*100<<"%\n";
			std::cout << "\n\nPlay again?\n1. Yes\n2. No\n";
			response = validResponse(1,2);
			if(response != 1){
				
				double earnings = credit - spent;
			
				system("cls");	
				if(earnings < 0)
					std::cout << "\nYou lost "<<(char)156 << std::setprecision(2) << abs(earnings)/100 << ", sucker!";
				else
					std::cout << "\nYou won "<<(char)156 <<std::setprecision(2) << earnings/100 << ", wow!";
				std::cout << "\n\n1. Continue\n";
				response = validResponse(1,1);
				finishedGame = true;

			}

			}
		}
		
		else if(response == 2){
			system("cls");
			std::cout << "Enter the name of the file to assess:";
			std::string filename;
			std::cin >> filename;
			validateFile(filename);
			std::cout << "\n\n1. Continue\n";
			response = validResponse(1,1);
		}else{
			finishedProgram = true;
		}


	}
		
}
