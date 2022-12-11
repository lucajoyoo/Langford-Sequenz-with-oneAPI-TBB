#include <iostream>
#include <bitset>
#include <iomanip>
#include <vector>
#include <chrono>
#include <tbb/parallel_for.h>
#include <mutex>

//#define SEQ

int fun_schablone(int n) {      // bei n=1 -> 101 | bei n=2 -> 1001 | bei n=3 -> 10001 | bei n=4 -> 100001
    int schablone = 1;
    schablone = (schablone << n + 1);
    schablone = (schablone | 1);
    return schablone;
}

int main() {

    int input_N = 0;
    std::bitset<32> schablone, bitset, schablone_tmp;
    std::vector<std::bitset<32>> waiting_queue, funkt;
    std::mutex m; 

    std::cout << "Please enter a N: ";
    std::cin >> input_N;
    std::cout << "Startbitset: " << bitset << "\n\n";
    waiting_queue.push_back(bitset);

#ifdef SEQ
    std::chrono::steady_clock::time_point begin_t = std::chrono::steady_clock::now();
    for (int i = input_N; i > 0; i--)
    {
        schablone = fun_schablone(i);

        // In the queue are the bitstes to be checked
            for (int i = 0; i < waiting_queue.size(); i++) {
            /* Take a bitset from the queue and move the template with all possibilities over the current bitset. */
                for (int j = 0; 1 != schablone_tmp[(input_N*2)-1]; j++) { 
                    schablone_tmp = (schablone << j);
                    std::bitset<32> modified_bitset;
                    unsigned long zahlBit_1 = waiting_queue.at(i).to_ullong();
                    unsigned long zahlSchablone = schablone_tmp.to_ullong();
                    modified_bitset = waiting_queue.at(i) | schablone_tmp;
                    unsigned long zahlBit_2 = modified_bitset.to_ullong();
                    if(zahlBit_2 == zahlBit_1 + zahlSchablone)
                        funkt.push_back(modified_bitset);
                }
                schablone_tmp.reset();
            }
        waiting_queue.clear();
        std::copy(funkt.begin(), funkt.end(), std::back_inserter(waiting_queue));
        funkt.clear();
    }
    std::chrono::steady_clock::time_point end_t = std::chrono::steady_clock::now();
#endif

#ifndef SEQ

    std::chrono::steady_clock::time_point begin_t = std::chrono::steady_clock::now();
    for (int i = input_N; i > 0; i--)
    {
        schablone = fun_schablone(i);

        tbb::parallel_for(tbb::blocked_range<size_t>(0, waiting_queue.size()), [&](tbb::blocked_range<size_t>& r)
            { 
                for (int i = r.begin(); i < r.end(); ++i)
                {
                    for (int j = 0; 1 != schablone_tmp[(input_N * 2) - 1]; j++) {
                        schablone_tmp = (schablone << j);
                        unsigned long zahlBit_1 = waiting_queue.at(i).to_ullong();
                        unsigned long zahlSchablone = schablone_tmp.to_ullong();
                        std::bitset<32> modified_bitset = waiting_queue.at(i) | schablone_tmp;
                        unsigned long zahlBit_2 = modified_bitset.to_ullong();
                        if (zahlBit_2 == zahlBit_1 + zahlSchablone)
                        {
                            m.lock();
                            funkt.push_back(modified_bitset);
                            m.unlock();
                        }
                    }
                   
                    schablone_tmp.reset();
                }
            });

        waiting_queue.clear();
        std::copy(funkt.begin(), funkt.end(), std::back_inserter(waiting_queue));
        funkt.clear();
    }
    std::chrono::steady_clock::time_point end_t = std::chrono::steady_clock::now();
#endif

    std::cout << "Possibilities: " << waiting_queue.size() / 2 << std::endl << std::endl;
    std::cout << "Time: " << std::chrono::duration_cast<std::chrono::microseconds>(end_t - begin_t).count() << "[us]\n";
    std::cout << "Time: " << std::chrono::duration_cast<std::chrono::milliseconds>(end_t - begin_t).count() << "[ms]\n";
    std::cout << "Time: " << std::chrono::duration_cast<std::chrono::seconds>(end_t - begin_t).count() << "[s]\nWOW! This was really quick! :D\n";
    return 0;
}