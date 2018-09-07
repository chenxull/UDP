#include <iostream>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/thread/thread.hpp>

using namespace std;
using boost::asio::ip::udp;
static int j=0;
boost::thread_group threads;
boost::asio::io_service service;

class Client {
public:
    char *rand_str(char *str,const int len)
    {
    int i;
    str[0] = '0';
    str[1] = 'x';
    str[2] = '0'+j;
    j++;
    
    for(i=3;i<len;++i)
        str[i]='A'+rand()%26;
    
    str[++i]='\\0';
    return str;
    }

    Client(boost::asio::io_service& service, const udp::endpoint& remote) :
        remoteEndpoint(remote), sock(service, udp::v4()) {
        // sock.open(udp::v4());
        start();
    }

private:
    void start() {
        memset(buf, 0, BUF_SIZE);
       // cin.getline(buf, BUF_SIZE);
        rand_str(buf,BUF_SIZE-1);
        sock.async_send_to(boost::asio::buffer(buf, strlen(buf)),
                remoteEndpoint, boost::bind(&Client::handleSend, this,
                        boost::asio::placeholders::error));
    }
    void handleSend(const boost::system::error_code& error) {
        if (!error) {
            memset(buf, 0, BUF_SIZE);
            udp::endpoint local;
            sock.async_receive_from(boost::asio::buffer(buf, BUF_SIZE), local,
                    boost::bind(&Client::handleReceive, this,
                            boost::asio::placeholders::error));
        }
    }
    void handleReceive(const boost::system::error_code& error) {
        if (!error) {
            cout << buf << endl;
            std::cout << "ThreadId : " << boost::this_thread::get_id() <<endl;
            //cout<< Count++ <<endl;
            start();
        }
    }
private:
    udp::endpoint remoteEndpoint;
    udp::socket sock;
    enum {
        BUF_SIZE = 512
    };
    char buf[BUF_SIZE];
    int Count = 0;
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
        udp::resolver resolver(service);
        udp::resolver::query query(udp::v4(), "127.0.0.1", "9998");
        udp::endpoint receiverEndpoint = *resolver.resolve(query);

        cout << receiverEndpoint << endl;
//        cout << receiverEndpoint.address().to_string() << endl;
//        cout << receiverEndpoint.port() << endl;
       
        Client c(service, receiverEndpoint);
         
        start_listen(8);
        threads.join_all();
      //  service.run();
    } catch (exception& e) {
        cout << e.what() << endl;
    }
}


