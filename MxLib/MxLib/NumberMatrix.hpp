#pragma once

#include <cstdlib>
#include <cstring>
#include <cassert>
#include <stdexcept>
#include <iostream>

//#define MXLIB_DEBUG

namespace genmx {       // General rhs (number, text, quaternion)

    enum class RAND_FILL {
        RAND
    };
    enum class NO_INIT {
        NOINIT
    };

    typedef unsigned dim_t; // Наколько хорошо в данном случае использовать unsigned вместо int?

    template <typename U>
    class Matrix {
        U *buf_;
        dim_t nrow_, ncol_;
        
    #ifdef MXLIB_DEBUG
        static int id_iter_;
        int id_;
    #endif

        inline void _dump_name_func (std::string str) {
    #ifdef MXLIB_DEBUG  
        std::cout << "id " << id_ << ": "
                  << str << std::endl;
    #endif
    }
        // Чтобы не делать U& GetValue (dim_t, dim_t) {}
        inline U* GetPointer (dim_t row, dim_t col) {
            return buf_ + row * ncol_ + col;
        }

        inline bool IsEqualSize          (const Matrix <U>& rhs) const {
            return nrow_ == rhs.nrow_ && ncol_ == rhs.ncol_;
        }
        inline bool IsMultSize           (const Matrix <U>& rhs) const {
            return ncol_ == rhs.nrow_;
        }
        inline bool IsEqualSquareSize    (const Matrix <U>& rhs) const {
            return     IsSquareSize ()  && 
                   rhs.IsSquareSize ()  &&
                   ncol_ == rhs.ncol_;
        }
        inline bool IsSquareSize         () const {
            return ncol_ == nrow_;
        }

        inline void CheckEqualSize       (const Matrix <U>& rhs) const {
            if (IsEqualSize (rhs) == false)
                throw std::runtime_error ("CheckEqualSize");
        }
        inline void CheckMultSize        (const Matrix <U>& rhs) const {
            if (IsMultSize (rhs) == false)
                throw std::runtime_error ("CheckMultSize");
        }
        inline void CheckEqualSquareSize (const Matrix <U>& rhs) const {
            if (IsEqualSquareSize (rhs) == false)
                throw std::runtime_error ("CheckEqualSquareSize");
        }
        inline void CheckSquareSize      () const {
            if (IsSquareSize () == false)
                throw std::runtime_error ("CheckSquareMatrix");
        }

        inline void Release () {
            delete[] (buf_);
        }
        inline U* GetRow (dim_t nrow) {
            if (nrow >= nrow_)
                std::runtime_error ("nrow is tol large");

            return buf_ + nrow * ncol_;
        }
    
    public:
        Matrix () = delete;

        Matrix (dim_t nRow, dim_t nColumn, U val = U {}) :
            buf_ ((U*) ::operator new[] (nRow * nColumn * sizeof (U))),   // Only for speed (1/2)
            nrow_ (nRow),
            ncol_ (nColumn)

        #ifdef MXLIB_DEBUG
            ,id_ (id_iter_++)
        #endif
        {
            const size_t full_size = nrow_ * ncol_;
            for (size_t i = 0; i < full_size; ++i)
                new (buf_ + i) U {val};  // Only for speed (2/2)

            _dump_name_func (__PRETTY_FUNCTION__);
        }
        Matrix (dim_t nRow, dim_t nColumn, NO_INIT) :
            buf_ ((U*) ::operator new[] (nRow * nColumn * sizeof (U))),   // Only for speed (1/2)
            nrow_ (nRow),
            ncol_ (nColumn)

        #ifdef MXLIB_DEBUG
            ,id_ (id_iter_++)
        #endif
        {
            _dump_name_func (__PRETTY_FUNCTION__);
        }
        Matrix (dim_t nRow, dim_t nColumn, RAND_FILL) :
            buf_ ((U*) ::operator new[] (nRow * nColumn * sizeof (U))),
            nrow_ (nRow),
            ncol_ (nColumn)
            
        #ifdef MXLIB_DEBUG
            ,id_ (id_iter_++)
        #endif
        {
            _dump_name_func ("Now you will be killed because method Matrix"
                             "Matrix (dim_t nRow, dim_t nColumn, RAND_FILL) has not yet been implemented");

            throw std::runtime_error ("Not realized");
            /*
                std::random_device rd;
                std::mt19937 gen(rd());
                std::uniform_int_distribution<> dis(-T_MAX, +T_MAX);
                
                // OR (https://software.intel.com/sites/landingpage/IntrinsicsGuide/)

                #include <immintrin.h>

                U temp;
                _rdrand$$sizeof (U)$$_step (&temp); // But only for number

            */
        }

        Matrix (const Matrix& rhs) :
            buf_ ((U*) ::operator new[] (rhs.nrow_ * rhs.ncol_ * sizeof (U))),
            nrow_ (rhs.nrow_),
            ncol_ (rhs.ncol_)
            
        #ifdef MXLIB_DEBUG
            ,id_ (id_iter_++)
        #endif
        {
            const size_t full_size = nrow_ * ncol_;
            for (size_t i = 0; i < full_size; ++i)
                *(buf_ + i) = *(rhs.buf_ + i);

            _dump_name_func (__PRETTY_FUNCTION__);
        }
        Matrix (Matrix &&other) noexcept = default;
        static Matrix eye (dim_t nrow, dim_t ncol, U one = 1) {
            Matrix result {nrow, ncol, U {}};

            const dim_t min_size = std::min (nrow, ncol);
            
            for (dim_t i = 0; i < min_size; ++i)
                result.SetValue (i, i, one);

            return result;
        }

        ~Matrix () {
            Release ();

            _dump_name_func (__PRETTY_FUNCTION__);
        }
        
        Matrix& operator= (const Matrix& rhs) {
            if (IsEqualSize (rhs) == false) {
                Release ();
                nrow_ = rhs.nrow_;
                ncol_ = rhs.ncol_;
            }

            const size_t full_size = nrow_ * ncol_;
            for (size_t i = 0; i < full_size; ++i)
                *(buf_ + i) = *(rhs.buf_ + i);

            _dump_name_func (__PRETTY_FUNCTION__);

            return *this;
        }
        Matrix& operator= (Matrix&& rhs) {
            // CheckEqualSize (rhs); Not necessary
            
            std::swap (buf_,  rhs.buf_);
            std::swap (nrow_, rhs.nrow_);
            std::swap (ncol_, rhs.ncol_);

            _dump_name_func (__PRETTY_FUNCTION__);

            return *this;
        }

        Matrix& operator+= (const Matrix& rhs) {
            CheckEqualSize (rhs);

            for (dim_t irow = 0; irow < nrow_; ++irow)
                for (dim_t icol = 0; icol < ncol_; ++icol)
                    *GetPointer (irow, icol) += rhs.GetValue (irow, icol);

            return *this;
        }
        Matrix& operator-= (const Matrix& rhs) {
            CheckEqualSize (rhs);

            for (dim_t irow = 0; irow < nrow_; ++irow)
                for (dim_t icol = 0; icol < ncol_; ++icol)
                    *GetPointer (irow, icol) -= rhs.GetValue (irow, icol);

            return *this;
        }
        Matrix& operator*= (const Matrix& rhs) {
            CheckMultSize (rhs);

            Matrix <U> result {nrow_, rhs.ncol_, U {}};

            for (dim_t irow = 0; irow < nrow_; ++irow) {
                for (dim_t icol = 0; icol < rhs.ncol_; ++icol) {
                    U& cell = *result.GetPointer (irow, icol);

                    for (dim_t i = 0; i < ncol_; i++)
                        cell += GetValue (irow, i) * rhs.GetValue (i, icol);
                }
            }

            return *this = std::move (result);;
        }
        Matrix& operator/= (const Matrix& rhs) {
            CheckEqualSquareSize (rhs);

            throw std::runtime_error ("Not realized"); // TODO

            return *this;
        }
        template <typename N>
        Matrix& operator*= (N num) {
            const size_t full_size = nrow_ * ncol_;
            for (size_t i = 0; i < full_size; ++i)
                *(buf_ + i) *= num;

            return *this;
        }

        bool operator== (const Matrix& rhs) {
            return Equal (rhs);
        }
        bool operator!= (const Matrix& rhs) {
            return !Equal (rhs);
        }

        dim_t nrows () { return nrow_; }
        dim_t ncols () { return ncol_; }
        
        Matrix operator- () const {
            Matrix <U> result = Matrix <U> {*this};
            result.negate ();
            return result;
        }
        
        Matrix& operator++ (int) {      // M++
            Matrix old {*this};
            *this += eye (nrow_, ncol_);
            return old;
        }
        Matrix& operator++ () {         // ++M;
            *this += eye (nrow_, ncol_);
            return *this;
        }
        
        Matrix& operator-- (int) {      // M--;
            Matrix old {*this};
            *this -= eye (nrow_, ncol_);
            return old;
        }
        Matrix& operator-- () {         // --M;
            *this -= eye (nrow_, ncol_);
            return *this;
        }
        
        Matrix& negate () & {
            const size_t full_size = nrow_ * ncol_;
            for (size_t i = 0; i < full_size; ++i)
                *(buf_ + i) = -*(buf_ + i);
            
            return *this;
        }
        Matrix& transpose () & {
            Matrix temp {ncol_, nrow_, NO_INIT::NOINIT};

            for (dim_t irow = 0; irow < nrow_; ++irow)
                for (dim_t icol = 0; icol < ncol_; ++icol)
                    temp.SetValue (icol, irow, GetValue (irow, icol));
            
            return *this = std::move (temp);
        }
        inline U trace () const {
            CheckSquareSize ();

            U _trace = GetValue (0, 0);

            for (dim_t i = 1; i < nrow_; ++i)
                _trace += GetValue (i, i);

            return _trace;
        }
        Matrix& T () & {
            return transpose ();
        }
        U Tr () const {
            return trace ();
        }

        inline U GetValue (dim_t row, dim_t col) const {
            return *(buf_ + row * ncol_ + col);
        }
        inline void SetValue (dim_t row, dim_t col, const U& val) {
            *(buf_ + row * ncol_ + col) = val;
        }

        void Dump (std::ostream& os) const {
            for (dim_t irow = 0; irow < nrow_; ++irow) {
                for (dim_t icol = 0; icol < ncol_ - 1; ++icol) {
                    os << GetValue (irow, icol) << " ";
                }
                os << GetValue (irow, ncol_ - 1) << std::endl;
            }
        }
        bool Equal (const Matrix& other) const {
            if (IsEqualSize (other)) {
                
                const size_t full_size = nrow_ * ncol_;
                for (size_t i = 0; i < full_size; ++i)
                    if (*(buf_ + i) != *(other.buf_ + i))
                        return false;
                
                return true;

            } else
                return false;
        }
        void SwapRows (dim_t row_first, dim_t row_second) {
            if (row_first == row_second)
                return;

            for (int icol = 0; icol < ncol_; ++icol)
                std::swap (*GetPointer (row_first,  icol),
                           *GetPointer (row_second, icol));
        }

        U Determinant () {
            CheckSquareSize ();

            Matrix <U> save {*this};

            U result = U {};
            for (dim_t irow = 0; irow < nrow_; ++irow) {
                
                // Get nonzero on a_ij or det(A) == 0
                if (GetValue (irow, irow) == U {}) {

                    dim_t irow_swap = irow + 1;
                    for (; irow_swap < nrow_; ++irow_swap)
                        if (GetValue (irow_swap, irow) != U {})
                            break;
                    
                    if (irow_swap < nrow_)
                        SwapRows (irow, irow_swap);
                    else
                        return U {};
                }

                const U cur_diag = GetValue (irow, irow);
                if (irow == 0)
                    result  = cur_diag;
                else
                    result *= cur_diag;

                if (irow == nrow_ - 1)
                    break;

                for (dim_t irow_sub = irow + 1; irow_sub < nrow_; ++irow_sub) {
                    U koef = GetValue (irow_sub, irow) / cur_diag;

                    for (dim_t icol_sub = irow + 1; icol_sub < ncol_; ++icol_sub)
                        *GetPointer (irow_sub, icol_sub) -= koef * GetValue (irow, icol_sub);
                }
            }
            
            *this = std::move (save);

            return result;
        }
    
    private:
        class ProxyRow {
            U* row_;
            dim_t ncol_;
        public:
            ProxyRow (U* row, dim_t ncol) :
                row_ (row),
                ncol_ (ncol)
            {}

            const U& operator[] (dim_t nColumn) const {
                if (nColumn >= ncol_)
                    std::runtime_error ("nColumn is to large");
                
                return row_[nColumn];
            }
            U& operator[] (dim_t nColumn) {
                return const_cast <U&> (
                    static_cast <const ProxyRow&> (*this)[nColumn]
                );
            }
        };
    public:

        ProxyRow operator[] (dim_t nRow) {
            if (nRow >= nrow_)
                std::runtime_error ("nrow is to large");
            
            return ProxyRow (GetRow (nRow), ncol_);
        }

    };

    template <typename U> Matrix <U> operator+ (const Matrix <U>& lhs, const Matrix <U>& rhs) {
        Matrix <U> result {lhs};
        result += rhs;
        return result;
    }
    template <typename U> Matrix <U> operator- (const Matrix <U>& lhs, const Matrix <U>& rhs) {
        Matrix <U> result {lhs};
        result -= lhs;
        return result;
    }
    template <typename U> Matrix <U> operator* (const Matrix <U>& lhs, const Matrix <U>& rhs) {
        Matrix <U> result {lhs};
        result *= rhs;
        return result;
    }
    template <typename U> Matrix <U> operator/ (const Matrix <U>& lhs, const Matrix <U>& rhs) {
        Matrix <U> result {lhs};
        result /= rhs;
        return result;
    }

    template <typename U>
    std::ostream &operator<< (std::ostream &os, const Matrix <U>& rhs) {
        rhs.Dump (os);
        return os;
    }
#ifdef MXLIB_DEBUG
    template <typename U>
    int Matrix <U>::id_iter_ = 0;
#endif
} // end namespace genmx