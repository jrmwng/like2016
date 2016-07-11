//#include "rtm.h"
#include <thread>
//#include <unistd.h>
#include <iostream>
#include <intrin.h>
#include <Windows.h>

using namespace std;

#define n_threads 1
#define OPSIZE 1000000000
typedef struct alignas(64) Account {
	long balance;
	long number;
} account_t;

typedef struct Bank {
	account_t* accounts;
	long size;
} bank_t;

bool done = 0;
long *tx, *_abort, *capacity, *debug, *failed, *conflict, *zero;

void* f1(bank_t* bank, int id) {
	for (int i = 0; i<OPSIZE; i++) {
		int src = 0; // rand() % bank->size;
		int dst = 1; // rand() % bank->size;
		while (src == dst) {
			dst = rand() % bank->size;
		}

		while (true) {
			_mm_prefetch(reinterpret_cast<char*>(&bank), _MM_HINT_T0);
			_mm_prefetch(reinterpret_cast<char*>(&bank->accounts), _MM_HINT_T0);
			_mm_prefetch(reinterpret_cast<char*>(&bank->accounts[src].balance), _MM_HINT_T0);
			_mm_prefetch(reinterpret_cast<char*>(&bank->accounts[dst].balance), _MM_HINT_T0);
			auto & src_account = bank->accounts[src];
			auto & dst_account = bank->accounts[dst];
			unsigned stat = _xbegin();
			if (stat == _XBEGIN_STARTED) {
				src_account.balance++;
				dst_account.balance--;
				_xend();
				tx[id]++;
				break;
			}
			else {
				for (int j = 1; j < 16; j++)
					bank->accounts[j * 1024 * 4096 / sizeof(account_t)].balance++;
				_abort[id]++;

				if (stat == 0) {
					zero[id]++;
				}
				if (stat & _XABORT_CONFLICT) {
					conflict[id]++;
				}
				if (stat & _XABORT_CAPACITY) {
					capacity[id]++;
				}
				if (stat & _XABORT_DEBUG) {
					debug[id]++;
				}
				if ((stat & _XABORT_RETRY) == 0) {
					failed[id]++;
					break;
				}
				if (stat & _XABORT_NESTED) {
					printf("[ PANIC ] _XABORT_NESTED\n");
					exit(-1);
				}
				if (stat & _XABORT_EXPLICIT) {
					printf("[ panic ] _XBEGIN_EXPLICIT\n");
					exit(-1);
				}
			}
		}
	}
	return NULL;
}
void* f2(bank_t* bank) {
	printf("_heartbeat function\n");
	long last_txs = 0, last_aborts = 0, last_capacities = 0, last_debugs = 0, last_faileds = 0, last_conflicts = 0, last_zeros = 0;
	long txs = 0, aborts = 0, capacities = 0, debugs = 0, faileds = 0, conflicts = 0, zeros = 0;
	while (1) {
		last_txs = txs;
		last_aborts = aborts;
		last_capacities = capacities;
		last_debugs = debugs;
		last_conflicts = conflicts;
		last_faileds = faileds;
		last_zeros = zeros;

		txs = aborts = capacities = debugs = faileds = conflicts = zeros = 0;
		for (int i = 0; i<n_threads; i++) {
			txs += tx[i];
			aborts += _abort[i];
			faileds += failed[i];
			capacities += capacity[i];
			debugs += debug[i];
			conflicts += conflict[i];
			zeros += zero[i];
		}

		printf("txs\t%ld\taborts\t\t%ld\tfaileds\t%ld\tcapacities\t%ld\tdebugs\t%ld\tconflit\t%ld\tzero\t%ld\n",
			txs - last_txs, aborts - last_aborts, faileds - last_faileds,
			capacities - last_capacities, debugs - last_debugs, conflicts - last_conflicts,
			zeros - last_zeros);

		Sleep(100);
	}
}

int main(int argc, char** argv) {
	constexpr int accounts = 16 * 1024 * 4096 / sizeof(account_t);

	bank_t* bank = new bank_t;
	bank->accounts = new account_t[accounts];
	bank->size = accounts;

	if (VirtualLock(bank->accounts, 4096) == FALSE)
	{
		__debugbreak();
	}

	for (int i = 0; i<accounts; i++) {
		bank->accounts[i].number = i;
		bank->accounts[i].balance = 0;
	}

	thread* pid[n_threads];
	tx = new long[n_threads];
	_abort = new long[n_threads];
	capacity = new long[n_threads];
	debug = new long[n_threads];
	failed = new long[n_threads];
	conflict = new long[n_threads];
	zero = new long[n_threads];

	thread* _heartbeat = new thread(f2, bank);
	for (int i = 0; i<n_threads; i++) {
		tx[i] = _abort[i] = capacity[i] = debug[i] = failed[i] = conflict[i] = zero[i] = 0;
		pid[i] = new thread(f1, bank, i);
	}

	//	sleep(5);
	for (int i = 0; i<n_threads; i++) {
		pid[i]->join();
	}
	return 0;
}
