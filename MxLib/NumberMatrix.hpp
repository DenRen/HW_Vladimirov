#pragma once

#include <cstdlib>
#include <cstring>
#include <cassert>
#include <stdexcept>
#include <iostream>

//#define MY_DEBUG

namespace genmx {       // General matrix (number, text, quaternion)

    enum class RAND_FILL {
        RAND
    };

    typedef int dim_t;

    template <typename T>
    class Matrix {
        T *buf_;
        dim_t nrow_, ncol_;
        
    #ifdef MY_DEBUG
        static int id_iter_;
        int id_;
    #endif

    public:
        Matrix () = delete;

        Matrix (dim_t nRow, dim_t nColumn, T val = T {}) :
            buf_ ((T *) new T[nRow * nColumn] {val}),
            nrow_ (nRow),
            ncol_ (nColumn)

        #ifdef MY_DEBUG
            ,id_ (id_iter_++)
        #endif
        {
            assert (nrow_ > 0);
            assert (ncol_ > 0);

        #ifdef MY_DEBUG
            std::cout << "id " << id_ << ": "
                      << "Matrix (dim_t nRow, dim_t nColumn, T val = T {})"
                      << std::endl;
        #endif
        }

        Matrix (dim_t nRow, dim_t nColumn, RAND_FILL) :
            buf_ ((T *) new T[nRow * nColumn]),
            nrow_ (nRow),
            ncol_ (nColumn)
            
        #ifdef MY_DEBUG
            ,id_ (id_iter_++)
        #endif
        {
            throw std::runtime_error ("Not realized");
            /*
                std::random_device rd;
                std::mt19937 gen(rd());
                std::uniform_int_distribution<> dis(-T_MAX, +T_MAX);
                
                // OR (https://software.intel.com/sites/landingpage/IntrinsicsGuide/)

                #include <immintrin.h>

                T temp;
                _rdrand$$sizeof (T)$$_step (&temp); // But only for number

            */
        }

        Matrix (const Matrix& matrix) :
            buf_ ((T *) new T[matrix.nrow_ * matrix.ncol_]),
            nrow_ (matrix.nrow_),
            ncol_ (matrix.ncol_)
            
        #ifdef MY_DEBUG
            ,id_ (id_iter_++)
        #endif
        {
            for (dim_t irow = 0; irow < nrow_; ++irow) {
                for (dim_t icol = 0; icol < ncol_; ++icol) {
                    *(buf_ + irow * ncol_ + icol) = 
                    *(matrix.buf_ + irow * ncol_ + icol);
                }
            }

        #ifdef MY_DEBUG
            std::cout << "id " << id_ << ": "
                      << "Matrix (const Matrix&)"
                      << std::endl;
        #endif
        }

        ~Matrix () {
            delete [] buf_;

        #ifdef MY_DEBUG
            std::cout << "id " << id_ << ": "
                      << "~Matrix ()"
                      << std::endl;
        #endif
        }
    };

#ifdef MY_DEBUG
    template <typename T>
    int Matrix <T>::id_iter_ = 0;
#endif
} // end namespace genmx