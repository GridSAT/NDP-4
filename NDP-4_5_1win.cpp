// NDP-4_5_1win.cpp
//
// Non-Deterministic Processor (NDP) - Efficient Parallel SAT-Solver with Profiling
//
// Copyright (c) 2025 GridSAT Stiftung
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Affero General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Affero General Public License for more details.
// 
// You should have received a copy of the GNU Affero General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.
// 
// GridSAT Stiftung - Georgstr. 11 - 30159 Hannover - Germany - ipns://gridsat.eth - info@gridsat.io
//
//
//
// 
// +++ READ.me +++
// 
// This NDP is an efficient parallel SAT-Solver. It parses DIMACS, factorizes, and performs parallel
// searches using BFS (Breadth-First Search) and DFS (Depth-First Search) techniques.
// It supports custom configuration through command-line options, utilizes multi-threading via OpenMP,
// and outputs results to a dynamically generated file name based on the input parameters and, and outputs
// results to a dynamically generated file name based on the input parameters and the current UTC time.
// 
// 
// REQUIREMENTS:
// 
// 	Input:			Generate DIMACS files at Paul Purdom and Amr Sabry's CNF Generator at:
// 					https://cgi.luddy.indiana.edu/~sabry/cnf.html
// 					For bit-wise input generation, use e.g.: https://bigprimes.org/RSA-challenge
// 	or
// 					generate DIMACS locally with: https://github.com/GridSAT/CNF_FACT-MULT
// 					or on IPFS ipfs://QmYuzG46RnjhVXQj7sxficdRX2tUbzcTkSjZAKENMF5jba
// 
//   GMP Library:	GNU Multiple Precision Arithmetic Library to handle arbitrary-precision arithmetic.
//
//	 OpenMP:		For parallel computing.
// 
// 
// COMPILATION:
//
//	Ensure you have at least GCC 9+, preferably GCC 11 or 12): g++ --version
//
// 	To compile the program on Windows (tested on Windows 11), use the following options:
//
//	If you are using MSYS2 MinGW-w64:
//
//	g++ -fopenmp -std=c++17 -Ofast -march=native -mtune=native -fomit-frame-pointer -funroll-loops \
//	-fprefetch-loop-arrays -flto=auto -ffast-math -static \
//	-o NDP-4_5_1win.exe NDP-4_5_1win.cpp $(pkg-config --cflags --libs gmp) -lstdc++fs
//
//	If you don’t have pkg-config:
//
//	g++ -fopenmp -std=c++17 -Ofast -march=native -mtune=native -fomit-frame-pointer -funroll-loops \
//  -fprefetch-loop-arrays -flto=auto -ffast-math -static \
//  -o NDP-4_5_1win.exe NDP-4_5_1win.cpp -lgmpxx -lgmp -lstdc++fs
//
//	Ensure MSYS2 has the required packages installed:
//
//	pacman -S mingw-w64-x86_64-gcc mingw-w64-x86_64-gmp mingw-w64-x86_64-libomp
//
//
//	CAUTION:
//
//	If you truly want a fully static binary, use MinGW-w64 with -static -static-libgcc -static-libstdc++
//	If dynamic linking is fine, drop the -static flag for smaller binaries!
//
// 
// 	For Linux, use NDP-4_5_1.cpp and follow instructions.
// 
// 
// 	CLI USAGE:
// 
// 	Once compiled, the program can be run from the command line using the following format:
// 
// 	./NDP-4_5_1win <dimacs_file> [-d depth | -t max_tasks | -q max_queue_size | -r reserved cores] [-o output_directory]
// 
// 	Command-Line Options:
// 
//     <dimacs_file>: The path to the input DIMACS file.
//     -d depth: Set a custom depth for BFS iterations. (Optional)
//     -t max_tasks: Set the maximum number of tasks for BFS. (Optional)
//     -q max_queues: Limit the maximum number of tasks in the BFS queue. (Optional)
//     -r reserve_cores: Reserve a certain number of CPU cores for the system, reducing the number of cores used by the program. (Optional)
//     -o output_directory: Specify a custom output directory for the result files. (Optional)
// 
// 	Basic execution: NDP-4_5_1win.exe inputs/RSA/rsaFACT-24bit.dimacs
// 
// 	This will run the program using the default settings for BFS and DFS and output the results to
// 	the current working directory.
//		
//	Defaults:	max_tasks = num_vars
//				Reserved Cores = 0
//				output_directory = input_directory
//	 
// 	Setting a custom depth: NDP-4_5_1win.exe inputs/RSA/rsaFACT-24bit.dimacs -d 5000
// 
// 	Limiting the number of tasks: NDP-4_5_1win.exe inputs/RSA/rsaFACT-24bit.dimacs -t 1000
//
// 	Setting a custom Queue Size: NDP-4_5_1win.exe inputs/RSA/rsaFACT-24bit.dimacs -q 256
// 	
// 	Saving results to a specific directory: NDP-4_5_1win.exe inputs/RSA/rsaFACT-24bit.dimacs -o /path/to/output
// 
// 
// OUTPUT:
// 
//     The output file will be saved in the format: NDP-4_5_1win.exe-<input_file_name>_<truncated_problem_id>_<cli-options>.txt
//     
//     Example: NDP-4_5_1win.exe_rsaFACT13688e7-24bit_8dfcb_auto_r0.txt (no cli option for Depth/#Tasks/Queue Size, no reserved cores)
// 
// 
// 	NOTE:
// 			only accepts input generated by Paul Purdom and Amr Sabry's CNF Generator - for code comments,
// 			paste code into ChatGPT
//
//  
// +++ READ.me END +++
// 
// 
// Standard library includes#include <iostream>
#include <vector>
#include <algorithm>
#include <ctime>
#include <stack>
#include <tuple>
#include <sstream>
#include <string>
#include <fstream>
#include <regex>
#include <memory>
#include <cmath>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <future>
#include <unordered_set>
#include <filesystem>
#include <direct.h>
#include <numeric>
#include <windows.h>
#include <set>
#include <omp.h>
#include <gmpxx.h>
#include <chrono>
#include <unordered_map>
#include <iomanip>

#ifdef ENABLE_PROFILING
#define PROFILE_SCOPE(name) ScopedTimer timer##__LINE__(name)
#else
#define PROFILE_SCOPE(name)
#endif

// ==========================
// PROFILING INFRASTRUCTURE
// ==========================
std::mutex profiler_mutex;
std::unordered_map<std::string, std::pair<double, int>> profiler_data;

class ScopedTimer {
public:
    ScopedTimer(const std::string &name)
        : name_(name), start_(std::chrono::high_resolution_clock::now()) { }
    ~ScopedTimer() {
        auto end = std::chrono::high_resolution_clock::now();
        double elapsed = std::chrono::duration<double>(end - start_).count();
        std::lock_guard<std::mutex> lock(profiler_mutex);
        auto &entry = profiler_data[name_];
        entry.first += elapsed;
        entry.second += 1;
    }
private:
    std::string name_;
    std::chrono::high_resolution_clock::time_point start_;
};


// ==========================
// END PROFILING INFRASTRUCTURE
// ==========================

std::ostringstream output_ss;
using big_int = mpz_class;
std::string version = "\n NDP-version: 4.5.1";

// Global variable for DFS control.
std::atomic<bool> dfs_running{true};

void dumpProfilingResults() {
    std::lock_guard<std::mutex> lock(profiler_mutex);
    std::cout << "\n\n=== Profiling Results ===\n";
    for (const auto &entry : profiler_data) {
        const std::string &func = entry.first;
        double total_time = entry.second.first;
        int calls = entry.second.second;
        std::cout << "Function [" << func << "]: Total time = " << total_time 
                  << " s, Calls = " << calls 
                  << ", Avg = " << (calls ? total_time/calls : 0) << " s\n";
    }
    std::cout << "=========================\n";
}

std::string getWorkingDirectory() {
    PROFILE_SCOPE("getWorkingDirectory");
    char temp[PATH_MAX];
    if (getcwd(temp, sizeof(temp)) != nullptr)
        return std::string(temp);
    else
        return std::string("");
}

int get_processor_count() {
    PROFILE_SCOPE("get_processor_count");
    SYSTEM_INFO sysinfo;
    ::GetSystemInfo(&sysinfo);
    return sysinfo.dwNumberOfProcessors;
}

std::string formatDuration(double seconds) {
    PROFILE_SCOPE("formatDuration");
    int months = static_cast<int>(seconds / (60 * 60 * 24 * 30));
    seconds -= months * 60 * 60 * 24 * 30;
    int days = static_cast<int>(seconds / (60 * 60 * 24));
    seconds -= days * 60 * 60 * 24;
    int hours = static_cast<int>(seconds / (60 * 60));
    seconds -= hours * 60 * 60;
    int minutes = static_cast<int>(seconds / 60);
    seconds -= minutes * 60;
    std::stringstream ss;
    if (months > 0) ss << months << " months ";
    if (days > 0) ss << days << " days ";
    if (hours > 0) ss << hours << " hours ";
    if (minutes > 0) ss << minutes << " minutes ";
    ss << seconds << " seconds\n";
    return ss.str();
}

std::string getCurrentUTCTime() {
    PROFILE_SCOPE("getCurrentUTCTime");
    auto now = std::chrono::system_clock::now();
    std::time_t now_c = std::chrono::system_clock::to_time_t(now);
    std::tm utc_tm = *std::gmtime(&now_c);
    std::stringstream ss;
    ss << std::put_time(&utc_tm, "%Y-%m-%d %H:%M:%S UTC");
    return ss.str();
}

std::string createProblemID(const std::string& input_number, int num_bits, int num_threads, const std::string& utcTime) {
    PROFILE_SCOPE("createProblemID");
    std::stringstream ss;
    ss << input_number << "-" << num_bits << "-" << num_threads << "-" << utcTime;
    std::string data = ss.str();
    std::size_t hash_value = std::hash<std::string>{}(data);
    std::stringstream hash_ss;
    hash_ss << std::hex << hash_value;
    return hash_ss.str().substr(0, 16);
}

// === Fixed-Size Clause Representation ===
struct Clause3 {
    int l[3];
};
using ClauseSet = std::vector<Clause3>;

// Parse DIMACS string: convert 1-literal clauses to {0,0,x} and 3-literal clauses to {x,y,z}.
ClauseSet parseDimacsString(const std::string &data) {
    PROFILE_SCOPE("parseDimacsString");
    std::istringstream file(data);
    ClauseSet result;
    std::string line;
    while (std::getline(file, line)) {
        if (line.empty() || line[0] == 'c' || line[0] == 'p')
            continue;
        std::istringstream iss(line);
        std::vector<int> lits;
        int literal;
        while (iss >> literal && literal != 0)
            lits.push_back(literal);
        if (lits.size() == 1) {
            Clause3 cl = {0, 0, lits[0]};
            result.push_back(cl);
        } else if (lits.size() == 3) {
            Clause3 cl = {lits[0], lits[1], lits[2]};
            result.push_back(cl);
        }
        // Ignore clauses of unexpected size.
    }
    return result;
}

// ResolutionStep: given a ClauseSet and integer i, produce two ClauseSets.
std::pair<ClauseSet, ClauseSet> ResolutionStep(const ClauseSet &A, int i) {
    PROFILE_SCOPE("ResolutionStep");
    ClauseSet LA, RA;
    LA.reserve(A.size());
    RA.reserve(A.size());
    {
        PROFILE_SCOPE("Compute_LA");
        int two_i = 2 * i;
        for (const auto &cl : A) {
            Clause3 newCl;
            bool skip = false;
            for (int j = 0; j < 3; ++j) {
                int val = cl.l[j] + i;
                if (val == two_i) { skip = true; break; }
                newCl.l[j] = val;
            }
            if (!skip)
                LA.push_back(newCl);
        }
    }
    {
        PROFILE_SCOPE("Adjust_LA");
        for (auto &cl : LA)
            for (int j = 0; j < 3; ++j)
                if (cl.l[j] != 0)
                    cl.l[j] -= i;
    }
    {
        PROFILE_SCOPE("Compute_RA");
        int minusTwo_i = -2 * i;
        for (const auto &cl : A) {
            Clause3 newCl;
            bool skip = false;
            for (int j = 0; j < 3; ++j) {
                int val = cl.l[j] - i;
                if (val == minusTwo_i) { skip = true; break; }
                newCl.l[j] = val;
            }
            if (!skip)
                RA.push_back(newCl);
        }
    }
    {
        PROFILE_SCOPE("Adjust_RA");
        for (auto &cl : RA)
            for (int j = 0; j < 3; ++j)
                if (cl.l[j] != 0)
                    cl.l[j] += i;
    }
    return {LA, RA};
}

// choice: choose a literal from ClauseSet.
inline int choice(const ClauseSet &A) __attribute__((always_inline));
inline int choice(const ClauseSet &A) {
    PROFILE_SCOPE("choice");
    for (const auto &cl : A) {
        int zeroCount = 0, nonzero = 0;
        for (int j = 0; j < 3; ++j) {
            if (cl.l[j] == 0)
                ++zeroCount;
            else
                nonzero = cl.l[j];
        }
        if (zeroCount == 2)
            return std::abs(nonzero);
    }
    for (const auto &cl : A) {
        int zeroCount = 0, nonzero = 0;
        for (int j = 0; j < 3; ++j) {
            if (cl.l[j] == 0)
                ++zeroCount;
            else
                nonzero = cl.l[j];
        }
        if (zeroCount == 1)
            return std::abs(nonzero);
    }
    if (!A.empty())
        return std::abs(A[0].l[0]);
    return 0;
}

// containsZeroSubarray: check if any clause is {0,0,0}.
inline bool containsZeroSubarray(const ClauseSet &A) __attribute__((always_inline));
inline bool containsZeroSubarray(const ClauseSet &A) {
    PROFILE_SCOPE("containsZeroSubarray");
    for (const auto &cl : A) {
        if (cl.l[0] == 0 && cl.l[1] == 0 && cl.l[2] == 0)
            return true;
    }
    return false;
}

// Satisfy_iterative: DFS search on ClauseSet.
std::vector<std::vector<int>> Satisfy_iterative(ClauseSet A, bool firstAssignment = false) {
    PROFILE_SCOPE("Satisfy_iterative");
    std::vector<std::pair<ClauseSet, std::vector<int>>> stack = { {A, {}} };
    std::vector<std::vector<int>> results;
    std::set<std::vector<int>> unique_results;
    bool found_first_assignment = false;
    while (!stack.empty()) {
        PROFILE_SCOPE("Satisfy_iterative_loop");
        auto [current_A, choices] = std::move(stack.back());
        stack.pop_back();
        if (containsZeroSubarray(current_A))
            continue;
        int i = choice(current_A);
        {
            PROFILE_SCOPE("Satisfy_iterative_insertchoices");
            if (i == 0) {
                if (unique_results.insert(choices).second) {
                    results.push_back(choices);
                    if (firstAssignment) { found_first_assignment = true; break; }
                }
                continue;
            }
        }
        {
            PROFILE_SCOPE("ResolutionStep_call");
            auto [LA, RA] = ResolutionStep(current_A, i);
            {
                PROFILE_SCOPE("Process_LA");
                bool conflict = false;
                for (const auto &cl : LA)
                    if (cl.l[0] == 0 && cl.l[1] == 0 && cl.l[2] == 0) { conflict = true; break; }
                if (LA.empty() || conflict) {
                    if (LA.empty()) {
                        std::vector<int> new_choices = choices;
                        new_choices.push_back(i);
                        if (unique_results.insert(new_choices).second) {
                            results.push_back(new_choices);
                            if (firstAssignment) { found_first_assignment = true; break; }
                        }
                    }
                } else {
                    std::vector<int> new_choices = choices;
                    new_choices.push_back(i);
                    stack.emplace_back(LA, new_choices);
                }
            }
            {
                PROFILE_SCOPE("Process_RA");
                bool conflict = false;
                for (const auto &cl : RA)
                    if (cl.l[0] == 0 && cl.l[1] == 0 && cl.l[2] == 0) { conflict = true; break; }
                if (RA.empty() || conflict) {
                    if (RA.empty()) {
                        std::vector<int> new_choices = choices;
                        new_choices.push_back(-i);
                        if (unique_results.insert(new_choices).second) {
                            results.push_back(new_choices);
                            if (firstAssignment) { found_first_assignment = true; break; }
                        }
                    }
                } else {
                    std::vector<int> new_choices = choices;
                    new_choices.push_back(-i);
                    stack.emplace_back(RA, new_choices);
                }
            }
        }
    }
    return results;
}

// Satisfy_iterative_BFS: BFS search on ClauseSet.
std::pair<std::queue<std::pair<ClauseSet, std::vector<int>>>, int> 
Satisfy_iterative_BFS(ClauseSet A, int max_iterations, int max_tasks, bool override_max_tasks, int &iterations, int max_queues) {
    PROFILE_SCOPE("Satisfy_iterative_BFS");
    std::queue<std::pair<ClauseSet, std::vector<int>>> queue;
    queue.push({A, {}});
    iterations = 0;
    int task_count = 1;
    while (!queue.empty()) {
        if (max_queues != -1 && queue.size() >= static_cast<size_t>(max_queues))
            break;
        if (max_queues == -1 && !override_max_tasks && task_count >= max_tasks)
            break;
        auto [current_A, choices] = queue.front();
        queue.pop();
        int i = choice(current_A);
        if (i == 0)
            continue;
        auto [LA, RA] = ResolutionStep(current_A, i);
        {
            bool conflict = false;
            for (const auto &cl : LA)
                if (cl.l[0] == 0 && cl.l[1] == 0 && cl.l[2] == 0) { conflict = true; break; }
            if (!LA.empty() && !conflict) {
                std::vector<int> new_choices = choices;
                new_choices.push_back(i);
                queue.push({LA, new_choices});
                task_count++;
                std::cout << "\r  Queue size: " << queue.size() << " - Depth: " << (iterations + 1)
                          << " - Tasks: " << task_count << std::flush;
            }
        }
        {
            bool conflict = false;
            for (const auto &cl : RA)
                if (cl.l[0] == 0 && cl.l[1] == 0 && cl.l[2] == 0) { conflict = true; break; }
            if (!RA.empty() && !conflict) {
                std::vector<int> new_choices = choices;
                new_choices.push_back(-i);
                queue.push({RA, new_choices});
                task_count++;
                std::cout << "\r  Queue size: " << queue.size() << " - Depth: " << (iterations + 1)
                          << " - Tasks: " << task_count << std::flush;
            }
        }
        iterations++;
        if (max_queues == -1 && iterations >= max_iterations)
            break;
    }
    std::cout << std::endl;
    return {queue, task_count};
}


void ExtractInputsFromDimacs(const std::string& dimacsString, std::vector<int>& v1, std::vector<int>& v2) {
    PROFILE_SCOPE("ExtractInputsFromDimacs");
    std::regex regex_first_input(R"(Variables for first input \[msb,...,lsb\]: \[(.*?)\])");
    std::regex regex_second_input(R"(Variables for second input \[msb,...,lsb\]: \[(.*?)\])");
    std::smatch match;
    if (std::regex_search(dimacsString, match, regex_first_input)) {
        std::string numbers = match[1].str();
        std::istringstream iss(numbers);
        std::string number;
        while (std::getline(iss, number, ','))
            v1.push_back(std::stoi(number));
    } else {
        std::cerr << "\nError: Could not find 'first input' section in the DIMACS string.\n";
    }
    if (std::regex_search(dimacsString, match, regex_second_input)) {
        std::string numbers = match[1].str();
        std::istringstream iss(numbers);
        std::string number;
        while (std::getline(iss, number, ','))
            v2.push_back(std::stoi(number));
    } else {
        std::cerr << "\nError: Could not find 'second input' section in the DIMACS string.\n";
    }
}

std::string mpz_to_string(const mpz_class& num) {
    PROFILE_SCOPE("mpz_to_string");
    return num.get_str();
}

big_int binaryStringToDecimal(const std::string& binaryString) {
    PROFILE_SCOPE("binaryStringToDecimal");
    big_int result = 0;
    for (char c : binaryString) {
        result *= 2;
        if (c == '1')
            result += 1;
    }
    return result;
}

big_int processVector(const std::vector<int>& v, std::vector<int> vec) {
    PROFILE_SCOPE("processVector");
    std::string binaryString;
    std::unordered_set<int> v_set(v.begin(), v.end());
    for (int k : vec)
        binaryString += (v_set.find(k) != v_set.end()) ? '1' : '0';
    return binaryStringToDecimal(binaryString);
}

std::pair<big_int, big_int> convert(const std::vector<std::vector<int>>& v, 
                                    const std::vector<int>& v1, 
                                    const std::vector<int>& v2) {
    PROFILE_SCOPE("convert");
    if (v.empty())
        throw std::runtime_error("\nError: Input vector 'v' is empty.\n");
    const std::vector<int>& firstElement = v[0];
    big_int d1 = processVector(firstElement, v1);
    big_int d2 = processVector(firstElement, v2);
    return {d1, d2};
}

int calculate_max_tasks(int num_vars, int num_clauses) {
    PROFILE_SCOPE("calculate_max_tasks");
    return num_clauses - num_vars;
}

std::string formatPercentage(double part, double total) {
    PROFILE_SCOPE("formatPercentage");
    double percentage = (total > 0.0) ? (part / total) * 100.0 : 0.0;
    std::stringstream ss;
    ss << std::fixed << std::setprecision(2) << percentage << "%";
    return ss.str();
}

std::string formatFilename(const std::string& script_name, const std::string& filename, const std::string& problemID, const std::string& flag, int reserve_cores) {
    PROFILE_SCOPE("formatFilename");
    std::string sanitizedFilename = filename;
    size_t pos = sanitizedFilename.find(".dimacs");
    if (pos != std::string::npos)
        sanitizedFilename = sanitizedFilename.substr(0, pos);
    std::regex numberRegex(R"((\d{5})(\d+))");
    sanitizedFilename = std::regex_replace(sanitizedFilename, numberRegex, "$1e$2");
    std::string shortProblemID = problemID.substr(0, 5);
    std::stringstream ss;
    ss << script_name << "_" << sanitizedFilename << "_" << shortProblemID << "_" << flag << "_r" << reserve_cores << ".txt";
    return ss.str();
}

void exportResultsToFile(const std::string& filename, const std::string& content) {
    PROFILE_SCOPE("exportResultsToFile");
    std::ofstream outFile(filename);
    if (outFile.is_open()) { outFile << content; outFile.close(); }
    else { std::cerr << "\nError: Could not write to file " << filename << std::endl; std::terminate(); }
}

// process_queue now operates on ClauseSet.
std::vector<std::vector<int>> process_queue(
    std::queue<std::pair<ClauseSet, std::vector<int>>> queue, 
    bool parallel, big_int input_number, int num_bits, int num_vars, int num_clauses, 
    std::vector<int>& v1, std::vector<int>& v2, 
    std::chrono::high_resolution_clock::time_point bfs_start, 
    std::chrono::high_resolution_clock::time_point dfs_start, 
    int num_threads, int task_count, const std::string& script_name, 
    const std::string& filename, const std::string& cli_flag, int reserve_cores, 
    const std::string& output_directory, bool override_max_tasks, int iterations, int total_cores) 
{
    PROFILE_SCOPE("process_queue");
    std::vector<std::vector<int>> final_choices;
    std::mutex queue_mutex;
    std::atomic<bool> found(false);
    std::atomic<int> thread_count(0);
    size_t initial_queue_size = queue.size();
    if (parallel) {
        std::chrono::duration<double> bfs_duration = dfs_start - bfs_start;
        std::cout << "\n    BFS time: " << bfs_duration.count() << " seconds  -  DFS parallel initiated..\n\n" << std::endl;
        auto start_time = std::chrono::high_resolution_clock::now();
        // Replace the problematic pragma with a valid empty block.
        #pragma omp parallel
        { }
        std::thread time_printer([&]() {
            PROFILE_SCOPE("time_printer");
            while (!found.load()) {
                std::this_thread::sleep_for(std::chrono::seconds(1));
                auto now = std::chrono::high_resolution_clock::now();
                auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(now - start_time);
                std::cout << "\033[F\033[K DFS Threads: " << thread_count.load() << std::endl;
                std::cout << "\033[K    DFS time: " << elapsed.count() << " seconds" << std::flush;
            }
        });
        #pragma omp parallel shared(queue, final_choices, queue_mutex, found, thread_count)
        {
            #pragma omp single
            thread_count.store(omp_get_num_threads());
            while (true) {
                std::pair<ClauseSet, std::vector<int>> current_task;
                {
                    std::lock_guard<std::mutex> lock(queue_mutex);
                    if (!queue.empty() && !found.load()) {
                        current_task = queue.front();
                        queue.pop();
                    } else break;
                }
                thread_count++;
                auto [v_i, c_i] = current_task;
                auto new_choices = Satisfy_iterative(v_i, true);
                for (const auto& nc : new_choices) {
                    std::vector<int> final_choices_i = c_i;
                    final_choices_i.insert(final_choices_i.end(), nc.begin(), nc.end());
                    #pragma omp critical
                    {
                        if (!found.load()) {
                            final_choices.push_back(final_choices_i);
                            auto dfs_end = std::chrono::high_resolution_clock::now();
                            found.store(true);
                            dfs_running = false;
                            time_printer.join();
                            std::ostringstream output_ss;
                            output_ss << "\n              Thread " << omp_get_thread_num() << " found a solution!\n" << std::endl;
                            std::chrono::duration<double> dfs_duration = dfs_end - dfs_start;
                            std::chrono::duration<double> ndp_duration = dfs_end - bfs_start;
                            auto [d1, d2] = convert(final_choices, v1, v2);
                            output_ss << "        Bits: " << num_bits;
                            output_ss << "\n        VARs: " << num_vars;
                            output_ss << "\n     Clauses: " << num_clauses;
                            output_ss << "\n\nInput Number: " << input_number << std::endl;
                            output_ss << "      FACT 1: " << d1 << std::endl;
                            output_ss << "      FACT 2: " << d2 << std::endl;
                            output_ss << (d1 * d2 == input_number ? "              verified." : "              FALSE") << std::endl;
                            output_ss << "\n    BFS time: " << bfs_duration.count() << " seconds (" 
                                      << formatPercentage(bfs_duration.count(), ndp_duration.count()) << ")" << std::endl;
                            output_ss << "              " << formatDuration(bfs_duration.count()) << std::endl;
                            output_ss << "    DFS time: " << dfs_duration.count() << " seconds (" 
                                      << formatPercentage(dfs_duration.count(), ndp_duration.count()) << ")" << std::endl;
                            output_ss << "              " << formatDuration(dfs_duration.count()) << std::endl;
                            output_ss << "    NDP time: " << ndp_duration.count() << " seconds" << std::endl;
                            output_ss << "              " << formatDuration(ndp_duration.count()) << std::endl;
                            output_ss << " Total Cores: " << total_cores << std::endl;
                            output_ss << "   NDP Cores: " << num_threads << std::endl;
                            output_ss << " DFS Threads: " << thread_count.load() << std::endl;
                            output_ss << "  Queue Size: " << initial_queue_size << std::endl;
                            output_ss << "       Depth: " << iterations << std::endl;
                            output_ss << "       Tasks: " << task_count << std::endl;
                            output_ss << version << std::endl;
                            output_ss << "      DIMACS: " << filename << std::endl;
                            std::string utcTime = getCurrentUTCTime();
                            output_ss << "   Zulu time: " << utcTime << std::endl;
                            std::string problemID = createProblemID(mpz_to_string(input_number), num_bits, num_threads, utcTime);
                            output_ss << "  Problem ID: " << problemID << std::endl;
                            std::cout << output_ss.str();
                            std::string input_filename_only = std::filesystem::path(filename).filename().string();
                            std::string output_filename = formatFilename(script_name, input_filename_only, problemID, cli_flag, reserve_cores);
                            std::string full_output_path = output_directory + "/" + output_filename;
                            exportResultsToFile(full_output_path, output_ss.str());
                            std::cout << "Result saved: " << full_output_path << std::endl;
                            std::cout << "\n" << std::endl;
                            dumpProfilingResults();
                            std::terminate();
                        }
                    }
                    if (found.load()) break;
                }
                thread_count--;
                if (found.load()) break;
            }
        }
        if (final_choices.empty()) {
            auto dfs_end = std::chrono::high_resolution_clock::now();
            dfs_running = false;
            std::chrono::duration<double> dfs_duration = dfs_end - dfs_start;
            std::chrono::duration<double> ndp_duration = dfs_end - bfs_start;
            std::cout << " DFS Threads: " << thread_count.load() << std::endl;
            std::cout << "    DFS time: " << dfs_duration.count() << " seconds" << std::endl;
            std::ostringstream output_ss;
            output_ss << "\n        Bits: " << num_bits;
            output_ss << "\n        VARs: " << num_vars;
            output_ss << "\n     Clauses: " << num_clauses;
            output_ss << "\n\nInput Number: " << input_number << std::endl;
            output_ss << "              Prime!\n" << std::endl;
            output_ss << "    BFS time: " << bfs_duration.count() << " seconds (" 
                      << formatPercentage(bfs_duration.count(), ndp_duration.count()) << ")" << std::endl;
            output_ss << "              " << formatDuration(bfs_duration.count()) << std::endl;
            output_ss << "    DFS time: " << dfs_duration.count() << " seconds (" 
                      << formatPercentage(dfs_duration.count(), ndp_duration.count()) << ")" << std::endl;
            output_ss << "              " << formatDuration(dfs_duration.count()) << std::endl;
            output_ss << "    NDP time: " << ndp_duration.count() << " seconds" << std::endl;
            output_ss << "              " << formatDuration(ndp_duration.count()) << std::endl;
            output_ss << " Total Cores: " << total_cores << std::endl;
            output_ss << "   NDP Cores: " << num_threads << std::endl;
            output_ss << " DFS Threads: " << thread_count.load() << std::endl;
            output_ss << "  Queue Size: " << initial_queue_size << std::endl;
            output_ss << "       Depth: " << iterations << std::endl;
            output_ss << "       Tasks: " << task_count << std::endl;
            output_ss << version << std::endl;
            output_ss << "      DIMACS: " << filename << std::endl;
            std::string utcTime = getCurrentUTCTime();
            output_ss << "   Zulu time: " << utcTime << std::endl;
            std::string problemID = createProblemID(mpz_to_string(input_number), num_bits, num_threads, utcTime);
            output_ss << "  Problem ID: " << problemID << std::endl;
            std::cout << output_ss.str();
            std::string input_filename_only = std::filesystem::path(filename).filename().string();
            std::string output_filename = formatFilename(script_name, input_filename_only, problemID, cli_flag, reserve_cores);
            std::string full_output_path = output_directory + "/" + output_filename;
            exportResultsToFile(full_output_path, output_ss.str());
            std::cout << "Result saved: " << full_output_path << std::endl;
            std::cout << "\n" << std::endl;
            dumpProfilingResults();
            std::terminate();
        }
    }
    return final_choices;
}

std::string readFileToString(const std::string& filename) {
    PROFILE_SCOPE("readFileToString");
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "\nError: Could not open file " << filename << std::endl;
        return "";
    }
    std::ostringstream oss;
    oss << file.rdbuf();
    return oss.str();
}

int main(int argc, char* argv[]) {
    PROFILE_SCOPE("main");
    int max_queues = -1;
    int total_cores = get_processor_count();
    int reserve_cores = 0;
    int depth = 0;
    int max_tasks = 0;
    int num_threads = total_cores;  // OpenMP will handle this
    int num_bits = 0;
    int num_vars = 0;
    int num_clauses = 0;
    int iterations = 0;
    std::string script_name = std::filesystem::path(argv[0]).stem().string();
    if (argc < 2) {
        std::cerr << "\nUsage: " << argv[0] << " <filename> [-r reserve_cores] [-d depth | -t max_tasks] [-q max_queues] [-o output_directory]" << std::endl;
        return 1;
    }
    std::string filename = argv[1];
    std::string fileContent = readFileToString(filename);
    if (fileContent.empty())
        throw std::runtime_error("\nError reading file or file is empty.\n");
    std::smatch match;
    std::regex regex_product(R"(Circuit for product = ([0-9]+) \[)");
    std::regex regex_problem(R"(p cnf ([0-9]+) ([0-9]+))");
    big_int input_number = 0;
    if (std::regex_search(fileContent, match, regex_product))
        input_number = mpz_class(match[1].str());
    else {
        std::cerr << "\nError: Could not extract input number from DIMACS header.\n" << std::endl;
        return 1;
    }
    if (std::regex_search(fileContent, match, regex_problem)) {
        num_vars = std::stoi(match[1].str());
        num_clauses = std::stoi(match[2].str());
        std::regex regex_bits(R"(Variables for second input \[msb,...,lsb\]: \[.*?,\s*(\d+)\])");
        if (std::regex_search(fileContent, match, regex_bits))
            num_bits = std::stoi(match[1].str());
    } else {
        std::cerr << "\nError: Could not extract number of variables and clauses from DIMACS header.\n" << std::endl;
        return 1;
    }
    bool override_max_tasks = false;
    std::string output_directory = getWorkingDirectory();
    std::string cli_flag = "auto";
    if (argc >= 4) {
        for (int i = 1; i < argc; ++i) {
            std::string option = argv[i];
            if (option == "-q") {
                if (++i < argc) {
                    try { max_queues = std::stoi(argv[i]); cli_flag = "q" + std::to_string(max_queues); }
                    catch (...) { std::cerr << "\nError: The queue limit argument must be an integer.\n"; return 1; }
                } else { std::cerr << "\nError: Missing argument for -q option.\n"; return 1; }
            } else if (option == "-r") {
                if (++i < argc) {
                    try { reserve_cores = std::stoi(argv[i]); }
                    catch (...) { std::cerr << "\nError: The reserve cores argument must be an integer.\n"; return 1; }
                } else { std::cerr << "\nError: Missing argument for -r option.\n"; return 1; }
            } else if (option == "-d") {
                if (++i < argc) {
                    try { depth = std::stoi(argv[i]); cli_flag = "d" + std::string(argv[i]); override_max_tasks = true; }
                    catch (...) { std::cerr << "\nError: The depth argument must be an integer.\n"; return 1; }
                }
            } else if (option == "-t") {
                if (++i < argc) {
                    try { max_tasks = std::stoi(argv[i]); depth = max_tasks; cli_flag = "t" + std::string(argv[i]); }
                    catch (...) { std::cerr << "\nError: The max_tasks argument must be an integer.\n"; return 1; }
                }
            } else if (option == "-o") {
                if (++i < argc) { output_directory = argv[i]; }
            }
        }
    }
    int usable_cores = total_cores - reserve_cores;
    if (usable_cores < 0) { std::cerr << "\nError: Usable cores must be 0 or greater. Adjust reserve cores.\n"; return 1; }
    if (max_tasks == 0 && !override_max_tasks) { max_tasks = calculate_max_tasks(num_vars); depth = max_tasks; }
    std::cout << version << std::endl;
    std::cout << "\n Total Cores: " << total_cores << std::endl;
    std::cout << "      System: " << reserve_cores << std::endl;
    std::cout << "   NDP Cores: " << usable_cores << std::endl;
    std::cout << "\nInput Number: " << input_number << std::endl;
    std::cout << "        Bits: " << num_bits << std::endl;
    std::cout << "     Clauses: " << num_clauses << std::endl;
    std::cout << "        VARs: " << num_vars << std::endl;
    if (max_tasks > 0 && !override_max_tasks) std::cout << "  BFS #Tasks: " << max_tasks << std::endl;
    if (depth > 0 && override_max_tasks) std::cout << "       Depth: " << depth << std::endl;
    else if (max_queues > 0) std::cout << "  Queue size: " << max_queues << std::endl;
    std::cout << std::endl;
    
    // Parse clauses into ClauseSet.
    ClauseSet clauses = parseDimacsString(fileContent);
    if (clauses.empty()) throw std::runtime_error("\nError parsing DIMACS string.\n");
    
    omp_set_num_threads(usable_cores);
    // Start an empty parallel block to initialize threads.
    #pragma omp parallel
    { }
    dfs_running = true;
    
    std::vector<int> v1, v2;
    ExtractInputsFromDimacs(fileContent, v1, v2);
    
    auto bfs_start = std::chrono::high_resolution_clock::now();
    auto [results, task_count] = Satisfy_iterative_BFS(clauses, depth, max_tasks, override_max_tasks, iterations, max_queues);
    
    auto dfs_start = std::chrono::high_resolution_clock::now();
    std::vector<std::vector<int>> final_choices_parallel = process_queue(
        results, true, input_number, num_bits, num_vars, num_clauses,
        v1, v2, bfs_start, dfs_start, usable_cores, task_count,
        script_name, filename, cli_flag, reserve_cores, output_directory,
        override_max_tasks, iterations, total_cores);
    
    return 0;
}