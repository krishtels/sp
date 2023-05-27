#include <iostream>
#include <thread>
#include <mutex>
#include <vector>
#include <list>
#include <condition_variable>
#include <atomic>
#include <queue>

using namespace std;

mutex _mutex;
const int num = 100;
int ProducerNum[] = { 1, 2, 4 };
int ConsumerNum[] = { 1, 2, 4 };
int sum;

class Queue
{
private:
    queue<uint8_t> _queue;
public:

    void push(uint8_t val)
    {
        _mutex.lock();
        _queue.push(val);
        _mutex.unlock();
    }

    bool pop(uint8_t& val)
    {
        _mutex.lock();
        if (_queue.empty())
        {
            _mutex.unlock();
            this_thread::sleep_for(chrono::milliseconds(1));
            _mutex.lock();
        }
        if (!_queue.empty())
        {
            val = _queue.front();
            _queue.pop();
            _mutex.unlock();
            return true;
        }
        _mutex.unlock();
        return false;
    }
};


template<typename T>
class Task {
public:
    static void producer(T& _queue)
    {
        for (int i = 0; i < num; i++)
        {
            _queue.push(1);
        }
    }

    static void consumer(T& _queue)
    {
        int counter = 0;
        uint8_t val = 0;
        while (true)
        {
            if (!_queue.pop(val))
            {
                break;
            }
            counter += val;
        }
        sum += counter;
    }

    static void _task(T& _queue)
    {
        cout << "\t\t" << "+---------------+-----------------+-------------------------------------------+" << endl;
        for (int Producer : ProducerNum)
        {
            for (int Consumer : ConsumerNum)
            {
                vector<thread> _producers;
                vector<thread> _consumers;
                auto start = chrono::high_resolution_clock::now();
                sum = 0;
                for (int i = 0; i < Producer; i++)
                {
                    _producers.emplace_back(&Task<T>::producer, ref(_queue));
                }
                for (int i = 0; i < Consumer; i++)
                {
                    _consumers.emplace_back(&Task<T>::consumer, ref(_queue));
                }
                for (thread& _producer : _producers)
                {
                    _producer.join();
                }
                for (thread& _consumer : _consumers)
                {
                    _consumer.join();
                }

                auto end = chrono::high_resolution_clock::now();
                chrono::duration<double> duration = end - start;
                cout << "\t\t" << "|With producers : consumerNum = " << Producer << " : " << Consumer << " threads lasted " << duration.count() << " with result " << sum << "|\n";
                cout << "\t\t" << "+---------------+-----------------+-------------------------------------------+" << endl;
            }
        }
    }
};

int main()
{
    Queue queue;
    Task<Queue>::_task(queue);
}