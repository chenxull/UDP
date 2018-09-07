#include <iostream>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <sys/malloc.h>
#include <vector>
#include <boost/thread/thread.hpp>

using namespace std;
using boost::asio::ip::udp;
boost::asio::io_service service;
boost::thread_group threads;

class Server {
public:
    char* buf = (char*) calloc(BUF_SIZE,sizeof(char));  // 改为calloc
    vector <string> msgpools;
public:
    Server(boost::asio::io_service& service) :
        sock(service, udp::endpoint(udp::v4(), 9998)) {
        start();
    }

private:
    void start() {
        memset(buf, 0, BUF_SIZE);
        sock.async_receive_from(boost::asio::buffer(buf,BUF_SIZE), remoteEndpoint,
                boost::bind(&Server::handleReceive, this,
                        boost::asio::placeholders::error,
                        boost::asio::placeholders::bytes_transferred));
    }
    void handleReceive(const boost::system::error_code& error,
            std::size_t bytes_transferred) {
        if (!error || error == boost::asio::error::message_size) {
            cout << remoteEndpoint << endl;
            //在服务器端接受到数据之后，在这里将数据写到指定的内存空间去
            msgpools.push_back(buf);
            sock.async_send_to(boost::asio::buffer(buf, bytes_transferred),
                    remoteEndpoint, boost::bind(&Server::handleSend, this,
                            boost::asio::placeholders::error));
            //start();
        }
    }
    void handleSend(const boost::system::error_code& /*error*/) {
        //cout << buf <<endl;
        cout << msgpools.front() <<endl;
        std::cout << "ThreadId : " << boost::this_thread::get_id() <<endl;
        cout << count++ <<endl;
        start();
    }
private:
    udp::socket sock;
    udp::endpoint remoteEndpoint;

    enum {
        BUF_SIZE = 512
    };
    //char buf[BUF_SIZE];
    //使用malloc来给buf分配地址空间
    int count = 0;
    
};


void listen_thread(){
    service.run();
}

void start_listen(int thread_count){
    for(int i = 0 ; i< thread_count ; i++ ){
        threads.create_thread(listen_thread);
    }
}

int main() {
    try {
        //boost::asio::io_service service;
        Server server(service);

        start_listen(8);
        threads.join_all();

        //service.run(); // 注意：一定要调用 run()函数
    } catch (std::exception& e) {
        std::cerr << e.what() << std::endl;
    }

    return 0;
}