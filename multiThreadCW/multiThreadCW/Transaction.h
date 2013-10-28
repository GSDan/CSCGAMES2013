#ifndef TRANSACT_H
#define TRANSACT_H
#include <iostream>
#include <sstream>

class Transaction{
private:
	int money;
	int accountNum;
	bool withdrawal;
	char threadID;
	bool checked;
	bool secondCheck;
public:
	Transaction(bool withdrawal, int accountNum, int money, char threadID){
		this->withdrawal = withdrawal;
		this->accountNum = accountNum;
		this->withdrawal = withdrawal;
		this->threadID = threadID;
		this->money = money;
		this->checked = false;
		this->secondCheck = false;
	}
	int getMoney(){
		return this->money;
	}
	int getAccountNum(){
		return this->accountNum;
	}
	bool isWithdrawal(){
		return this->withdrawal;
	}
	char getThreadID(){
		return this->threadID;	
	}
	bool isChecked(){
		return checked;
	}
	void setChecked(bool checked){
		this->checked = checked;
	}
	bool isChecked2(){
		return secondCheck;
	}
	void setChecked2(bool checked){
		this->secondCheck = checked;
	}
	std::string toString(){

		std::string ioString = "";
		ioString += threadID;
		if(withdrawal)
			ioString += "W";
		else
			ioString += "D";
		std::ostringstream oss;
		oss << accountNum;
		ioString += oss.str() + "_";
		oss.str("");
		oss.clear();
		oss << money;
		ioString += oss.str();

		return ioString;

	}
	void printInFull(){
		if(withdrawal){
			std::cout << "Withdrawal from account " << accountNum << " of $" << money << " by thread " << threadID << "\n"; 
		}else{
			std::cout << "Deposit to account      " << accountNum << " of $" << money << " by thread " << threadID << "\n"; 
		}
	}
	friend bool operator==( const Transaction lhs, const Transaction rhs);
};

bool operator==( const Transaction lhs, const Transaction rhs){
	//check if this transaction is a partner of the given one
		if(rhs.threadID == lhs.threadID &&
			rhs.money == lhs.money &&
			rhs.withdrawal != lhs.withdrawal){
			//thread ID and money are the same, transaction types are opposite 
			return true;
		}
		else
			return false;
}
#endif