# DO NOT USE THIS!
# This was meant as a one time hack for systems which do not have tup.
# This doesn't even build tests!

import os

g_commands = [
   "palantir: g++ -pedantic -std=c++14 -Wall -pthread -I.. -I../../boost_1_60_0/ -isystem ../../googletest/googletest/include -c Palantir.cpp -o Palantir.o",
   "palantir: ar crs libpalantir.a Palantir.o",
   "utils:    g++ -pedantic -std=c++14 -Wall -pthread -I.. -I../../boost_1_60_0/ -isystem ../../googletest/googletest/include -c StringUtils.cpp -o StringUtils.o",
   "utils:    ar crs libutils.a StringUtils.o",
   "server:   g++ -pedantic -std=c++14 -Wall -pthread -I.. -I../../boost_1_60_0/ -isystem ../../googletest/googletest/include -I../../libevent/include -I../../glog/include -c Server.cpp -o Server.o",
   "main:     g++ -pedantic -std=c++14 -Wall -pthread -I.. -I../../boost_1_60_0/ -isystem ../../googletest/googletest/include -I../../libevent/include -I../../glog/include -c FoodThought.cpp -o FoodThought.o",
   "server:   g++ -pedantic -std=c++14 -Wall -pthread -I.. -I../../boost_1_60_0/ -isystem ../../googletest/googletest/include -I../../libevent/include -I../../glog/include -c ClientHandler.cpp -o ClientHandler.o",
   "server:   ar crs libserver.a ClientHandler.o Server.o ../palantir/libpalantir.a ../utils/libutils.a",
   "main:     g++ FoodThought.o ../server/libserver.a ../palantir/libpalantir.a ../utils/libutils.a ../../glog/lib/libglog.a ../../libevent/lib/*.a -lpthread -lunwind -o FoodThought",
];

def main():
   cwd = os.getcwd();
   for line in g_commands:
      tokens = line.split(":");
      tokens = [x.strip() for x in tokens];
      directory = tokens[0];
      command   = tokens[1];
      os.chdir(cwd + "/" + directory);
      print "Executing ", command;
      os.popen(command);
      os.chdir(cwd);

if __name__ == "__main__":
   main();
