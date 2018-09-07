#include <iostream>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <sys/malloc.h>
#include <vector>
#include <boost/thread/thread.hpp>

#define BUF_SIZE 512
#define THREAD_SUM 8
using namespace std;
using boost::asio::ip::udp;
//boost::asio::io_service service;
//boost::thread_group threads;



class Server {
public:
    char* buf;  // 改为calloc
    vector <string> msgpools;

public:
    Server(boost::asio::io_service& service ,char* buff) :
        sock(service, udp::endpoint(udp::v4(), 4007)) ,buf(buff) { 
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
        //cout << msgpools.back() <<endl;
        std::cout << "ThreadId : " << boost::this_thread::get_id() <<endl;
        cout << count++ <<endl;
        start();
    }
private:

    udp::socket sock;
    udp::endpoint remoteEndpoint;


    int count = 0;
    
};




int main() {
    
        
        boost::asio::io_service service;
        char* packet_buf = (char*) calloc(BUF_SIZE*THREAD_SUM,sizeof(char));
        boost::thread *t[THREAD_SUM] ={0};
/*
        for(int i = 0 ;i < 8 ; i ++ ){
            boost::asio::io_service service[THREAD_SUM];
            Server server(service[i], &packet_buf[i * BUF_SIZE]);
            cout << "this" << endl;
            t[i] = new boost::thread(boost::bind(&boost::asio::io_service::run,&service[i]));
            cout << "this2" << endl;
        }

        for(int i = 0; i < THREAD_SUM; ++i)
        {
            t[i]->join();
        }
*/

        Server server(service,&packet_buf[2 * BUF_SIZE]);
        service.run();




        //start_listen(8);
        //service.run(); // 注意：一定要调用 run()函数
    

    return 0;
}