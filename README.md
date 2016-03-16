FoodThought
----

Problem: Each employee gets allocated a fixed amount of money daily to order their lunch. Usually employees order a dish less than the allocated amount of money, and the remaining goes unused (It doesn't carry over to the next day). If one wants to order higher than the allocated amount of money, either they have to use their own personal money, or solicite money from some other employee who hasn't used his full allocation on that given day. Foodthought is a platform which creates a global fund of the unallocated money for the "needy" to use. It incentivizes people to donate their money in case they haven't used their full amount. In return, they are guarranted a higher probablity of being allocated money from global fund when they need it in the future. 

For more understanding about the implementation and various strategies [keynote] (https://github.com/skgbanga/FoodThought/blob/master/Presentation.key) [pdf] (https://github.com/skgbanga/FoodThought/blob/master/Presentation.pdf).


Requirements/Components


- A build system: [Tup](http://gittup.org/tup/)
- A logging module: [glog](https://github.com/google/glog)
- An event notification library: [libevent](http://libevent.org)
- A testing framework: [gtest](https://github.com/google/googletest/)
- [Boost](http://www.boost.org)
- C++11/14 (For a non-exhaustive list of c++11/14 features used, check c++11/14 section below)


C++11/14


- [PRNG] (http://en.cppreference.com/w/cpp/numeric/random)
- [regex] (http://en.cppreference.com/w/cpp/header/regex)
- [make_unique](http://en.cppreference.com/w/cpp/memory/unique_ptr/make_unique)
- [attribute sequence] (http://en.cppreference.com/w/cpp/language/attributes)
- [noexcept] (http://en.cppreference.com/w/cpp/language/noexcept_spec)
