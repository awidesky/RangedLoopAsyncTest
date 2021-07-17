#ifndef TESTRANGE_HPP
#define TESTRANGE_HPP

#include <future>
#include <thread>
#include <vector>
#include <utility>
#include <chrono>

template <typename R, typename Func, typename Binop>
R testRange(int from, int end, Func f, Binop op, int pool) {
  std::cout << "thread number : " << std::thread::hardware_concurrency() << "\n";
  std::cout << "\ntesting from " << std::hex << from << " to " << std::hex << end << "\n";
  int cnt = (end - from) / pool;
  int to = cnt + from;
  
  std::vector<std::future<R>> v;

  auto start = std::chrono::steady_clock::now();
  for(int i = 0; i < pool; i++) {
    v.push_back(std::async(std::launch::async, [from, to, &f, &op](){
      R var = f(from);
      for(int j = from + 1; j < to; j++) {
        var = op(f(j), var);
      }
      return var;
    }));
    
    from = to;
    
    if (i == pool - 1) {
      to = end + 1;
    } else {
      to += cnt;
    }
  }
  std::cout << "all task(" << v.size() << ") queued!\n";
  R re = v[0].get();
  for(int j = 1; j < pool; j++) {
    std::cout << "\n#" << j - 1 << " task executed\n";
    re = op(v[j].get(), re);
  }

  auto end1 = std::chrono::steady_clock::now();
  auto diff = end1 - start;
  std::cout << "\n" << std::chrono::duration <double, std::milli> (diff).count() << " ms" << std::endl;
  return re;
}

#endif