#include "CalcDet.hpp"

#include "MxLib/MxLib.hpp"
#include <iostream>

namespace cnslarg {

void Interactive_CalcDet (int argc, char* argv[]) {
    bool error = 0;
    double result = CalcDeterminant (argc, argv, error);
    if (error == false)
        std::cout << result;
}

int CalcDeterminant (int argc, char* argv[], bool& error) {
    if (argc == 1) {
        PrintCalcDetExample ();
        error = true;

        return 0;
    } else {
        int size = atoi (argv[1]);

        if (size * size != argc - 2) {    
            std::cout << "Wrong number of elements in the matrix." << std::endl;
            PrintCalcDetExample ();
            error = true;

            return 0;
        }

        genmx::Matrix <double> m {size, size};

        for (int irow = 0; irow < size; ++irow)
            for (int icol = 0; icol < size; ++icol)
                m.SetValue (irow, icol, atoi (argv[irow * size + icol + 2]));

        return m.D ();
    }
}

void PrintCalcDetExample () {
    std::cout << "Please enter size matrix and value of elements" << std::endl;
    std::cout << "For example: 2 1 0 0 1" << std::endl;
    std::cout << "             ^ \\-----/----< Elements 1 0" << std::endl;
    std::cout << "             |                       0 1" << std::endl;
    std::cout << "Size >-------/" << std::endl;
}

}