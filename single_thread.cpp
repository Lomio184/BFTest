#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>
#include <chrono>  // 시간 측정을 위해 추가

// 비밀번호가 맞는지 확인하는 함수
bool is_correct_password(const std::string& guess, const std::string& target_password) {
    return guess == target_password;
}

// 브루트 포스 공격 함수
std::string brute_force_attack(const std::string& target_password) {
    for (long long i = 0; i < 100000000; ++i) {  // 00000000부터 99999999까지 시도
        std::ostringstream oss;
        oss << std::setw(8) << std::setfill('0') << i;  // 8자리 숫자로 맞추기 위해 앞에 0을 채움
        std::string guess_password = oss.str();

        if (is_correct_password(guess_password, target_password)) {
            return guess_password;
        }
    }
    return "";
}

int main() {
    std::string target_password = "12345678";  // 예시로 사용될 비밀번호

    // 시작 시간 측정
    auto start = std::chrono::high_resolution_clock::now();

    // 브루트 포스 공격 시도
    std::string found_password = brute_force_attack(target_password);

    // 종료 시간 측정
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = end - start;

    if (!found_password.empty()) {
        std::cout << "Password found: " << found_password << std::endl;
    } else {
        std::cout << "Password was not found." << std::endl;
    }

    // 실행 시간 출력
    std::cout << "Elapsed time: " << elapsed.count() << " seconds" << std::endl;

    return 0;
}
