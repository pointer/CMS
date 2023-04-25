class tcp_connection : public boost::enable_shared_from_this<tcp_connection>
{
public:
    
    typedef boost::shared_ptr<tcp_connection> pointer;
    
    static pointer create(boost::asio::io_service& io_service)
    {
        return pointer(new tcp_connection(io_service));
    }
    
    tcp::socket& socket()
    {
        return socket_;
    }
    
    
    void startConnection()
    {
        start_read();
    }
    
    
    void stopConnection()
    {
        socket_.close();
    }
    
    
private:
    
    tcp_connection(boost::asio::io_service& io_service) : socket_(io_service)
    {
    }
    

    void start_read()
    {
        boost::asio::async_read_until(socket_, input_buffer_, '\n', boost::bind(&tcp_connection::handle_read, shared_from_this(), boost::asio::placeholders::error));
    }
    
    
    void handle_read(const boost::system::error_code& error)
    {
        if (!error)
        {
            boost::asio::streambuf::const_buffers_type bufs = input_buffer_.data();
            
            std::string msgstr(boost::asio::buffers_begin(bufs),
                               boost::asio::buffers_begin(bufs) +
                               input_buffer_.size());
            
            // Remove the first part of the buffer to prevent from accumulating
            input_buffer_.consume(input_buffer_.size());
            
            std::vector<std::string> msgVector;
            boost::split(msgVector, msgstr, boost::is_any_of(":"));
            
            if(msgVector.size() >= 2)
            {
                messageFromClient_ = msgVector[0];
                valueFromClient_ = msgVector[1];
                
                // HERE I HAVE CODE TO HANDLE MESSAGES FROM CLIENT
                
                boost::asio::async_write(socket_, 
boost::asio::buffer(messageToClient_), 
boost::bind(&tcp_connection::handle_write, shared_from_this(), 
boost::asio::placeholders::error, 
boost::asio::placeholders::bytes_transferred));
                
                start_read();
            }
        }
        else
        {   
            stopConnection();
        }
    }
    

    void handle_write(const boost::system::error_code& /*error*/,
                      size_t /*bytes_transferred*/)
    {
    }
    
    tcp::socket socket_;
    boost::asio::streambuf input_buffer_;
};


class tcp_server
{
public:
    
    tcp_server(boost::asio::io_service& io_service) : acceptor_(io_service, tcp::endpoint(tcp::v4(), tcpPort))
    {
        start_accept();
    }
    
    void start_accept()
    {
        
        tcp_connection::pointer new_connection = tcp_connection::create(acceptor_.get_io_service());
        
        acceptor_.async_accept(new_connection->socket(),
                               boost::bind(&tcp_server::handle_accept, this, new_connection, boost::asio::placeholders::error));
    }
    
    void handle_user_read(const boost::system::error_code& err, std::size_t bytes_transferred)
    {
    }
    
    void handle_accept(tcp_connection::pointer new_connection, const boost::system::error_code& error)
    {
        if (!error)
        {
            new_connection->startConnection();
            
            start_accept();
        }
    }
    
    tcp::acceptor acceptor_;
};

typedef boost::shared_ptr<tcp_connection> pointer;
struct conn_data
{
  pointer p;
  bool is_loop_callback = false;
};
std::vector<conn_data> connections;

...
	
void handle_accept(pointer new_connection,
                       const boost::system::error_code& error)
    {
        if (!error)
        {
            new_connection->startConnection();
            bool is_loop_callback = ...;
            connections.emplace_back(new_connection, is_loop_callback);
            start_accept();
        }
    }
connections[i]->start_listen();
void start_listen()
    {
        std::string listen_msg = "TEST";
        
        boost::asio::async_write(socket_, boost::asio::buffer(listen_msg),
                                 boost::bind(&tcp_connection::handle_write, shared_from_this(),
                                             boost::asio::placeholders::error,
                                             boost::asio::placeholders::bytes_transferred));
    }

