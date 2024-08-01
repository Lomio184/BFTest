#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>
#include <chrono>
#include <thread>
#include <vector>
#include <atomic>

// 비밀번호가 맞는지 확인하는 함수
bool is_correct_password(const std::string& guess, const std::string& target_password) {
    return guess == target_password;
}

// 브루트 포스 공격 함수
void brute_force_attack(const std::string& target_password, long long start, long long end, std::atomic<bool>& found, std::string& found_password) {
    for (long long i = start; i < end && !found.load(); ++i) {
        std::ostringstream oss;
        oss << std::setw(8) << std::setfill('0') << i;  // 8자리 숫자로 맞추기 위해 앞에 0을 채움
        std::string guess_password = oss.str();

        if (is_correct_password(guess_password, target_password)) {
            found_password = guess_password;
            found.store(true);  // 다른 스레드에게 비밀번호를 찾았음을 알림
            return;
        }
    }
}

int main() {
    std::string target_password = "12345678";  // 예시로 사용될 비밀번호
    std::atomic<bool> found(false);
    std::string found_password;

    // 하드웨어 동시성(코어 수) 가져오기
    unsigned int num_threads = 1;

    std::vector<std::thread> threads;
    long long total_range = 100000000;
    long long range_per_thread = total_range / num_threads;

    // 시작 시간 측정
    auto start_time = std::chrono::high_resolution_clock::now();

    // 스레드 생성 및 작업 분배
    for (unsigned int i = 0; i < num_threads; ++i) {
        long long start = i * range_per_thread;
        long long end = (i == num_threads - 1) ? total_range : (i + 1) * range_per_thread;
        threads.emplace_back(brute_force_attack, target_password, start, end, std::ref(found), std::ref(found_password));
    }

    // 모든 스레드가 작업을 마치길 기다림
    for (auto& t : threads) {
        if (t.joinable()) {
            t.join();
        }
    }

    // 종료 시간 측정
    auto end_time = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = end_time - start_time;

    if (found) {
        std::cout << "Password found: " << found_password << std::endl;
    } else {
        std::cout << "Password was not found." << std::endl;
    }

    // 실행 시간 출력
    std::cout << "Elapsed time: " << elapsed.count() << " seconds" << std::endl;

    return 0;
}
