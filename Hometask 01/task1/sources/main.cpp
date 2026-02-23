#include <iostream>
#include <string>
#include <cstdlib>

using namespace std;

struct CommandLineArgs
{
    string ip_address;
    int port = 0;
    string role;
    int index = -1;
    string library;
};

bool parseArgument(int argc, char* argv[], CommandLineArgs& args)
{
    for(int i = 1; i < argc; i++)
    {
        string flag = argv[i];

        if(flag == "-a" && (i+1) < argc)
        {
            args.ip_address = argv[++i];
        }
        else if(flag == "-p" && (i+1) < argc)
        {
            args.port = atoi(argv[++i]);
        }
        else if(flag == "-r" && (i+1) < argc)
        {
            args.role = argv[++i];
        }
        else if(flag == "-i" && (i+1) < argc)
        {
            args.index = atoi(argv[++i]);
        }
        else if(flag == "-L" && (i+1) < argc)
        {
            args.library = argv[++i];
        }
    }

    if(args.ip_address.empty() || args.port == 0 || 
       args.role.empty() || args.index == -1 || args.library.empty()) {
        return false;
    }

    return true;    
}

int main(int argc, char* argv[])
{
    CommandLineArgs args;

    if(parseArgument(argc, argv, args))
    {
        cout << "Arguments:\n"
             << "ip_address: " << args.ip_address << "\n"
             << "port: " << args.port << "\n"
             << "role: " << args.role << "\n"
             << "index: " << args.index << "\n"
             << "library: " << args.library << endl;
    }
    else
    {
        cout << "Usage: " << argv[0] 
             << " -a <ip> -p <port> -r <role> -i <index> -L <library>\n";
        return 1;
    }

    return 0;    
}