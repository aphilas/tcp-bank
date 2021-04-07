#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h> 
#include <netinet/in.h> 
#include <unistd.h>
#include <time.h>
#include <string.h>
#include "vector/vector.h"

typedef struct {
	long int timestamp;
	int amount;
} Transaction;

typedef struct {
	int acc_no;
  int balance;
  void* transactions;
} Account;

Vector* accounts;
Account* open_account(void);
Transaction* create_transaction(int acc_no, int amount);
Account* find_account(int acc_no);
Transaction* get_transaction(Account* account, int index);
int get_balance(int acc_no);
int deposit(int acc_no, int amount); // returns 0 for success, -1 for failure
int withdraw(int acc_no, int amount); // returns 0 for success, -1 for failure
int	close_account(int acc_no); // returns 0 for success, -1 for failure
void statement(int acc_no, char* buf, int buf_size);
const char* timestamp_to_str(long int n);
	
typedef enum { o, d, w, s, c, b } TransactionType;

static inline char* string_from_transaction_type(TransactionType t) {
	static const char *strings[] = { "open account", "deposit", "withdraw", "get statement", "close account", "balance" };
	return (char*) strings[t];
}

typedef struct {
	TransactionType transactionType;
	float amount;
} ClientMessage;

ClientMessage parse_message(const char * message);
void process_message(ClientMessage clientMessage, char *buf, int buf_size);

int main(){
	accounts = create(sizeof(Account));

	int client_socket, server_socket, n;
	struct sockaddr_in server_address;
	char client_response[256], server_response[256];

	server_socket = socket(AF_INET, SOCK_STREAM, 0);
	server_address.sin_family = AF_INET;
	server_address.sin_port = htons(9302);
	server_address.sin_addr.s_addr = INADDR_ANY;

	bind(server_socket, (struct sockaddr*) &server_address, sizeof(server_address));
	listen(server_socket, 8);

	for (;;) {
		client_socket = accept(server_socket, NULL, NULL);
    while ((n = recv(client_socket, &client_response, sizeof(client_response), 0)) > 0)  {
			printf("Client has sent: %s\n", client_response);

			ClientMessage clientMessage = parse_message(client_response);
			process_message(clientMessage, server_response, sizeof(server_response));
			send(client_socket, server_response, sizeof(server_response), 6);
    }

    if (n < 0) {
      perror("Read error");
      exit(1);
    }

    close(client_socket);
  }

	close(server_socket);

	return 0;
}

// o, d, w, s, c, b
void process_message(ClientMessage clientMessage, char *buf, int buf_size) {
	Account* account;
	int result;
	float balance;
	static char string[256];
	char statement_string[512];
 
	switch (clientMessage.transactionType) {
		case o: 
			account = open_account();

			if (account == NULL ) {
				strncpy(buf, "There was an error opening the account. Please try again.", buf_size-1);
				buf[buf_size-1] = '\0';
			} else {
				strncpy(buf, "Account opened successfully.", buf_size-1);
				buf[buf_size-1] = '\0';
			}
			break;

		case d:
			result = deposit(1, clientMessage.amount); // TODO: update i -> f

			if (result == 0) { 
				strncpy(buf, "Deposit successful", buf_size-1);
				buf[buf_size-1] = '\0';
			} else {
				strncpy(buf, "Deposit failed", buf_size-1);
				buf[buf_size-1] = '\0';
			}
			break;

		case w:
			result = withdraw(1, clientMessage.amount); // TODO: update i -> f

			if (result == 0) { 
				strncpy(buf, "Withdrawal successful", buf_size-1);
				buf[buf_size-1] = '\0';
			} else if (result == -2) {
				strncpy(buf, "You have insufficient funds", buf_size-1);
				buf[buf_size-1] = '\0';
			} else {
				strncpy(buf, "Withdrawal failed", buf_size-1);
				buf[buf_size-1] = '\0';
			}
			break;

		case s:
			statement(1, statement_string, sizeof(statement_string));
			strncpy(buf, statement_string, buf_size-1);
			buf[buf_size-1] = '\0';
			break;

		case c:
			result = close_account(1);
			if (result == 0) { 
				strncpy(buf, "Account closure successful", buf_size-1);
				buf[buf_size-1] = '\0';
			} else {
				strncpy(buf, "Account closure failed", buf_size-1);
				buf[buf_size-1] = '\0';
			}
			break;

		case b:
			balance = get_balance(1); // TODO
			sprintf(string, "Your balance is %.2f\n", balance);
			strncpy(buf, string, buf_size-1);
			buf[buf_size-1] = '\0';
			break;
	}
}

ClientMessage parse_message(const char *message) {
	ClientMessage clientMessage;

	char *string = NULL;
	string = strdup(message);
	
	char *token = strtok(string, ",");

	if (token != NULL) {
		clientMessage.transactionType = (TransactionType) atoi(token);

		token = strtok(NULL, ",");

		if (token != NULL) {
			clientMessage.amount = atof(token);
		} else {
			clientMessage.amount = 0.0;
		}
	}

	free(string);
	return clientMessage;
}

// create account with 1-based index as account no.
Account* open_account(void) {
	Account* account = malloc(sizeof(Account));
	account->acc_no = accounts->used + 1;
	account->balance = 0;
	account->transactions = create(sizeof(Transaction));
	push(accounts, account);
	return account;
}

// find account by account no, check for NULL!
Account* find_account(int acc_no) { // O(n)
	for (int i = 0; i < accounts->used; i++) {
		Account* account = (Account *) get(accounts, i);
		if (account->acc_no == acc_no) {
			return account;
		}
	}

	return NULL;
}

// get transaction by index, check for NULL
Transaction* get_transaction(Account* account, int index) { // O(n)
	Transaction *transaction;

	if (account == NULL) {
		return NULL;
	}

	transaction = (Transaction *)	get(account->transactions, index);

	if (transaction == NULL) {
		return NULL;
	}

	return transaction;
}

// add transaction to account, +ve amount implies deposit/transfer, -ve implies withdrawal
Transaction* create_transaction(int acc_no, int amount) {
	Account* account;
	Transaction* transaction;
	int long timestamp;
	time_t now;

	time(&now);
	timestamp = (long int) now;
	account = find_account(acc_no);

	if (account == NULL) {
		return NULL;
	}

	transaction = malloc(sizeof(Transaction));
	transaction->amount = amount;
	transaction->timestamp = timestamp; 
	push(account->transactions, transaction);

	account->balance += amount;

	return transaction;
}

// get balance of account with specified account number
int get_balance(int acc_no) {
	Account *account;
	
	account = find_account(acc_no);
	if (account == NULL) {
		printf("Account not found\n");
		return 0;
	}

	return account->balance;
}

// deposit money to account with specified account number
int deposit(int acc_no, int amount) {
	Account *account;
	Transaction *transaction;
	
	account = find_account(acc_no);
	if (account == NULL) {
		printf("Account not found\n");
		return -1;
	}

	if (amount < 0) {
		printf("Deposit should be more than zero\n");
		return -1;
	}

	transaction = create_transaction(acc_no, amount);

	if (transaction == NULL) {
		printf("Transaction failed\n");
		return -1;
	}

	return 0;
}

// withdraw money from account with specified account number
int withdraw(int acc_no, int amount) {
	Account *account;
	Transaction *transaction;
	
	account = find_account(acc_no);
	if (account == NULL) {
		printf("Account not found\n");
		return -1;
	}

	if (account->balance < amount) {
		printf("Balance is not enough\n");
		return -2;
	}

	if (amount > 0) {
		amount = -amount;
	}

	transaction = create_transaction(acc_no, amount);

	if (transaction == NULL) {
		printf("Transaction failed\n");
		return -1;
	}

	return 0;
}

// not tested
int close_account(int acc_no) {
	Account *account;
	
	account = find_account(acc_no);
	if (account == NULL) {
		printf("Account not found\n");
		return -1;
	}

	delete(account->transactions);
	// delete(account);

	return 0;
}

// generate statement from account number
void statement(int acc_no, char* buf, int buf_size) {
	Account *account;
	Transaction *transaction;
	int no_of_transactions;
	char string[512]; // FIX

	account = find_account(acc_no);

	if (account == NULL) {
		printf("Account not found\n");
		return;
	}

	no_of_transactions = ((Vector *)(account->transactions))->used;

	for (int i = 0; i < no_of_transactions; i++) {
		long int timestamp;
		int amount;
		char temp[128] = "";

		transaction = get_transaction(account, i);

		if (transaction == NULL) {
			continue;
		}	

		sprintf(temp, "\nDate: %s\nType: %s\nAmount: %d\n", 
			timestamp_to_str((time_t) transaction->timestamp),
			transaction->amount >= 0 ? string_from_transaction_type(d) : string_from_transaction_type(w), 
			abs(transaction->amount)
		);
		strcat(string, temp);
	}

	strncpy(buf, string, buf_size-1);
	buf[buf_size-1] = '\0';
}

// convert UNIX timestamp to date and time string
const char* timestamp_to_str(long int n) {
	time_t now;
	struct tm timestamp;
	static char buf[80];
	// time(&now);
	now = n;
	timestamp = *localtime(&now);
	timestamp = *localtime(&now);
	strftime(buf, sizeof(buf), "%a %Y-%m-%d %H:%M:%S", &timestamp); // timestamp.tm_year
	return buf;
}