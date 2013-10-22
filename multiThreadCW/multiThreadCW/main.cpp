#include "Transaction.h"
#include <string>
#include <iostream>
#include <queue>
#include <ctime>
#include <vector>
#include <stdlib.h>  

//populate a given queue with a given number of random ints
void populateQueue(std::queue<int> &q, int num){
		
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
		std::string result;
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


int main(){
	srand((unsigned int)time( NULL ) ); //reseed rand
	int numRows;

	std::cout << "How many rows would you like to generate?\n";
	std::cin >> numRows;

	double hit = 0;

	for(int i = 0; i < numRows; i++){
		std::vector<Transaction> row = createRow();
		for(int j =0; j < row.size(); j++){
			std::cout << row[j].toString();
			if(j != row.size()-1)
				std::cout << ",";  
		}
		if( assessRow(row, true)){
			hit++;
			std::cout << " <-- WINNER";
		}
		std::cout << "\n\n";
	}

	std::cout << hit << " successful rows, with a success rate of " << hit/numRows*100 << "%";

	int x;
	std::cin >> x;
}
