#include <iostream>
#include <mutex>
#include <condition_variable>
#include <chrono>
#include <stdexcept>

/**
 * @brief A template class for a thread-safe queue with a fixed size.
 * @tparam T Type of elements stored in the queue.
 */
template<typename T>
class Queue {
public:
    /**
     * @brief Construct a new Queue object with a given size.
     * @param size Maximum number of elements in the queue.
     */
    Queue(int size) : size(size), front(0), rear(0), count(0) {
        elements = new T[size];
    }

    /**
     * @brief Destroy the Queue object and release allocated memory.
     */
    ~Queue() {
        delete[] elements;
    }

    /**
     * @brief Push a new element into the queue.
     * @param element The element to be added to the queue.
     */
    void Push(T element) {
        std::unique_lock<std::mutex> lock(mtx);

        if (count == size) {
            // Queue is full, drop the oldest element
            front = (front + 1) % size;
            --count;
        }

        elements[rear] = element;
        rear = (rear + 1) % size;
        ++count;
        cv.notify_one();
    }

    /**
     * @brief Pop an element from the queue. Block if the queue is empty.
     * @return The element removed from the queue.
     */
    T Pop() {
        std::unique_lock<std::mutex> lock(mtx);
        cv.wait(lock, [this]() { return count > 0; });

        T element = elements[front];
        front = (front + 1) % size;
        --count;

        return element;
    }

    /**
     * @brief Pop an element from the queue with a timeout. Throw an exception if timeout occurs.
     * @param milliseconds Maximum time to wait for an element in milliseconds.
     * @return The element removed from the queue.
     * @throws std::runtime_error If timeout occurs before an element is available.
     */
    T PopWithTimeout(int milliseconds) {
        std::unique_lock<std::mutex> lock(mtx);

        if (!cv.wait_for(lock, std::chrono::milliseconds(milliseconds), [this]() { return count > 0; })) {
            throw std::runtime_error("Timeout while waiting to pop element");
        }

        T element = elements[front];
        front = (front + 1) % size;
        --count;

        return element;
    }

    /**
     * @brief Get the current number of elements in the queue.
     * @return The number of elements in the queue.
     */
    int Count() {
        std::lock_guard<std::mutex> lock(mtx);
        return count;
    }

    /**
     * @brief Get the maximum number of elements that can be stored in the queue.
     * @return The maximum number of elements.
     */
    int Size() {
        return size;
    }

private:
    T* elements;                  ///< Pointer to dynamically allocated array of elements.
    int size;                     ///< Maximum number of elements in the queue.
    int front;                    ///< Index of the front element in the queue.
    int rear;                     ///< Index of the rear element in the queue.
    int count;                    ///< Current number of elements in the queue.
    std::mutex mtx;               ///< Mutex for synchronizing access to the queue.
    std::condition_variable cv;   ///< Condition variable for blocking operations.
};

// Example usage
int main() {
    Queue<int> q(5);

    // Push elements to the queue
    q.Push(1);
    q.Push(2);
    q.Push(3);

    // Pop elements from the queue
    std::cout << "Popped: " << q.Pop() << std::endl;
    std::cout << "Popped with timeout: " << q.PopWithTimeout(1000) << std::endl;

    std::cout << "Current count: " << q.Count() << std::endl;
    std::cout << "Max size: " << q.Size() << std::endl;

    return 0;
}
