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

// 주어진 숫자를 문자로 변환하는 함수
std::string number_to_password(long long number, const std::string& characters) {
    std::string password;
    int base = characters.size();
    while (number > 0) {
        password += characters[number % base];
        number /= base;
    }
    while (password.size() < 8) {
        password += characters[0];
    }
    std::reverse(password.begin(), password.end());
    return password;
}

// 브루트 포스 공격 함수
void brute_force_attack(const std::string& target_password, long long start, long long end, const std::string& characters, std::atomic<bool>& found, std::string& found_password) {
    for (long long i = start; i < end && !found.load(); ++i) {
        std::string guess_password = number_to_password(i, characters);

        if (is_correct_password(guess_password, target_password)) {
            found_password = guess_password;
            found.store(true);  // 다른 스레드에게 비밀번호를 찾았음을 알림
            return;
        }
    }
}

int main() {
    std::string target_password = "Aa1Bb2Cc";  // 예시로 사용될 비밀번호
    std::atomic<bool> found(false);
    std::string found_password;

    std::string characters = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";

    // 가능한 조합의 수 계산
    long long total_combinations = 1;
    for (int i = 0; i < 8; ++i) {
        total_combinations *= characters.size();
    }

    // 하드웨어 동시성(코어 수) 가져오기
    unsigned int num_threads = std::thread::hardware_concurrency();
    if (num_threads == 0) num_threads = 2;  // 하드웨어 동시성을 확인할 수 없으면 기본값 설정

    std::vector<std::thread> threads;
    long long range_per_thread = total_combinations / num_threads;

    // 시작 시간 측정
    auto start_time = std::chrono::high_resolution_clock::now();

    // 스레드 생성 및 작업 분배
    for (unsigned int i = 0; i < num_threads; ++i) {
        long long start = i * range_per_thread;
        long long end = (i == num_threads - 1) ? total_combinations : (i + 1) * range_per_thread;
        threads.emplace_back(brute_force_attack, target_password, start, end, std::ref(characters), std::ref(found), std::ref(found_password));
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
