#include "main.h"

int main()
{
    try
    {
        Interface interface;
        interface.run();
    }
    catch (const boost::exception& boostException)
    {
		std::cerr << "boost error: " << boost::diagnostic_information(boostException) << '\n';
	}
    catch (const std::exception& e)
    {
        std::cerr << "std error: " << e.what() << '\n';
    }
    catch (...)
    {
		std::cerr << "Unknown error" << '\n';
	}

    return 0;
}

