//go get the declaration of the nyan::Socket class from socket.h
#include "nyan/socket.h"

/*
Implement every Socket member in socket.h. Own one fd; -1 is empty and fd 0 is valid. Close owned fds on destruction/reset; release transfers ownership.
Move construction/assignment transfers ownership and empties the source. Handle self-move and reset(get()) safely. Never throw from cleanup or retry Linux close after an error.
Test with real descriptors: moves keep them usable; destruction/reset closes the intended fd exactly once; release leaves it open.
*/
// TODO: Implement Socket ownership and moves. See docs/roadmap.md.

/*The C++ header [<cerrno>](https://en.cppreference.com/cpp/header/cerrno) provides standardized error-handling macros and integers (like errno, EDOM, and ERANGE), serving as the modern C++ equivalent of the C library's <errno.h>.*/
//Tells why something failed. 
#include <cerrno> 
/*The directive #include <unistd.h> is a preprocessor command in C and C++ that imports the POSIX operating system API header file. The name stands for "Unix Standard." It acts as a gateway for your program to interact directly with the underlying operating system kernel.*/
//Gives you POSIX/Linux function
#include <unistd.h>

//implements the namespace nyan from the socket.h file
namespace nyan{
    //stores the descrpitor as is and 0 remaisn valid. 
    //constructor. Socket:Socket (belongs to the socket class); (int fd) = constructor accepts an integer; noexcept = (promises not to throw a exception); :fd_(fd) = member initializer list. 
    Socket::Socket(int fd) noexcept:fd_(fd){}
    //cals for reset so an owned descriptor is closed. 
    //destructor. 
    Socket::~Socket() noexcept{
        reset();
    }

    //move constructor
    //Socket(Socket&& other) = from header is move constructor. && means rvalue referene. 
    Socket::Socket(Socket&& other) noexcept:fd_(other.release()){}

    //move operator. 
    Socket& Socket::operator=(Socket&& other)noexcept{
        //check for self-move 
        if(this != &other){
            reset(other.release());
        }
        return *this;
    }

    //return the current socket. 
    int Socket::get() const noexcept{
        return fd_;
    }

    //reset the socket. Replaces the descriptor the object owns. 
    void Socket::reset(int fd) noexcept{
        if(fd_==fd){
            return;
        }

        if(fd_ != -1){
            //keep trying to close it until it works. 
            while(::close(fd_) == -1 && errno == EINTR){
                //Linux may report EINTR from close; retry as required
                //by this project's socket ownershpi contract.
            }
        } 
        fd_ = fd;
    }


    //gives the descriptor to somebody else without closing it. 
    int Socket::release() noexcept{
        const int fd = fd_;
        fd_ = -1;
        return fd;
    }
}

