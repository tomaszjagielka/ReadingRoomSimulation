#include <iostream>
#include <thread>
#include <vector>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <random>
#include <chrono>

const int NUMBER_OF_PROCESSES = 5;
const int MESSAGE_QUEUE_CAPACITY = 2;

enum class Role { READER, WRITER };

class Process {
public:
    Process(int id) : id(id), role(Role::READER) {}

    void operate() {
        while (true) {
            relax();
            useReadingRoom();
        }
    }

    void relax() {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));

        std::lock_guard<std::mutex> lock(roleMutex);

        if (randGen() % 2 == 0) {
            role = (role == Role::READER) ? Role::WRITER : Role::READER;
            roles[id] = role;
            
            std::lock_guard<std::mutex> coutLock(coutMutex);
            std::cout << "[SWITCH\t" << id << "] Process " << id << " switched to " << ((role == Role::READER) ? "READER" : "WRITER") << " role." << std::endl;        }
    }

    void useReadingRoom() {
        if (role == Role::READER) {
            read();
        } else {
            write();
        }
    }

    void read() {
        std::unique_lock<std::mutex> lock(mutex);
        conditionVariable.wait(lock, []{ return !messageQueue.empty(); });

        auto& [message, readersCount] = messageQueue.front();
        {
            std::lock_guard<std::mutex> coutLock(coutMutex);
            std::cout << "[READ\t" << id << "] Reader " << id << " is reading message: '" << message << "'" << std::endl;
        }

        if (--readersCount <= 0) {
            {
                std::lock_guard<std::mutex> coutLock(coutMutex);
                std::cout << "[DELETE\t-] Message: '" << message << "' has been deleted." << std::endl;
            }
            messageQueue.pop();
        }
        
        lock.unlock();
        conditionVariable.notify_all();
    }

    void write() {
        std::unique_lock<std::mutex> lock(mutex);

        bool wasWaiting = false;
        if (messageQueue.size() >= MESSAGE_QUEUE_CAPACITY) {
            {
                std::lock_guard<std::mutex> coutLock(coutMutex);
                std::cout << "[WAIT\t" << id << "] Message queue is full. Writer " << id << " is waiting." << std::endl;
            }
            wasWaiting = true;
        }
        
        conditionVariable.wait(lock, []{ return messageQueue.size() < MESSAGE_QUEUE_CAPACITY; });

        if (wasWaiting) {
            std::lock_guard<std::mutex> coutLock(coutMutex);
            std::cout << "[RESUME\t" << id << "] Writer " << id << " has resumed operation." << std::endl;
        }

        int readersCount = countReaders();

        std::string message = "Hello from writer " + std::to_string(id) + "!";
        {
            std::lock_guard<std::mutex> coutLock(coutMutex);
            std::cout << "[WRITE\t" << id << "] Writer " << id << " is writing message: '" << message << "'" << std::endl;
        }
        messageQueue.emplace(message, readersCount);

        lock.unlock();
        conditionVariable.notify_all();
    }

private:
    int id;
    Role role;
    static std::mutex mutex, roleMutex, coutMutex;
    static std::condition_variable conditionVariable;
    static std::queue<std::pair<std::string, int>> messageQueue;
    static std::mt19937 randGen;

    static int countReaders() {
        std::lock_guard<std::mutex> lock(roleMutex);
        int count = 0;

        for (int i = 0; i < NUMBER_OF_PROCESSES; ++i) {
            if (roles[i] == Role::READER) {
                ++count;
            }
        }

        return count;
    }

    static Role roles[NUMBER_OF_PROCESSES];
};

std::mutex Process::mutex, Process::roleMutex, Process::coutMutex;
std::condition_variable Process::conditionVariable;
std::queue<std::pair<std::string, int>> Process::messageQueue;
Role Process::roles[NUMBER_OF_PROCESSES] = {Role::READER, Role::READER, Role::READER, Role::READER, Role::READER};
std::mt19937 Process::randGen(std::random_device{}());

int main() {
    std::vector<std::thread> threads;

    for (int i = 0; i < NUMBER_OF_PROCESSES; ++i) {
        threads.emplace_back(&Process::operate, Process(i));
    }

    for (auto &t : threads) {
        t.join();
    }

    return 0;
}