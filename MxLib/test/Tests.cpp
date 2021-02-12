#include <gtest/gtest.h>
#include "../MxLib/MxLib.hpp"

#include <random>
#include <cassert>

#define ITERATION_RAND_ROW(nrow_max, ntest_row)                             \
    assert (nrow_max  >  0);                                                \
    assert (ntest_row >= 0);                                                \
                                                                            \
    std::uniform_int_distribution<> rand_row (1, nrow_max);                 \
                                                                            \
        for (size_t itest_row = 0; itest_row < ntest_row; ++itest_row) {    \
            genmx::dim_t nrow = rand_row (gen);

#define ITERATION_RAND_COL(ncol_max, ntest_col)                             \
    assert (ncol_max  >  0);                                                \
    assert (ntest_col >= 0);                                                \
                                                                            \
    std::uniform_int_distribution<> rand_col (1, ncol_max);                 \
                                                                            \
        for (size_t itest_col = 0; itest_col < ntest_col; ++itest_col) {    \
            genmx::dim_t ncol = rand_col (gen);

#define ITERATION_RAND_VAL(T, val_min, val_max, ntest_val)                  \
    assert (ntest_val >= 0);                                                \
                                                                            \
    std::uniform_int_distribution<> rand_val (val_min, val_max);            \
                                                                            \
        for (size_t itest_val = 0; itest_val < ntest_val; ++itest_val) {    \
            double val = rand_val (gen);

/* Template test ----------------------
TEST (ConstructTest, InitValDefault) {
    
    std::random_device rd;
    std::mt19937 gen (rd ());
    
    ITERATION_RAND_ROW (500, 10)
        ITERATION_RAND_COL (400, 15)
            // Test somebody
        }
    }
}
--------------------------------------- */

TEST (ConstructTest, InitValDefault) {
    
    std::random_device rd;
    std::mt19937 gen (rd ());
    
    ITERATION_RAND_ROW (500, 5)
        ITERATION_RAND_COL (500, 5)
        
            genmx::Matrix <char >   m_char  {nrow, ncol};
            genmx::Matrix <short>   m_short {nrow, ncol};
            genmx::Matrix <int  >   m_int   {nrow, ncol};
            genmx::Matrix <long >   m_long  {nrow, ncol};

            genmx::Matrix <unsigned char >  m_uchar  {nrow, ncol};
            genmx::Matrix <unsigned short>  m_ushort {nrow, ncol};
            genmx::Matrix <unsigned int  >  m_uint   {nrow, ncol};
            genmx::Matrix <unsigned long >  m_ulong  {nrow, ncol};
            
            genmx::Matrix <float>  m_float  {nrow, ncol};
            genmx::Matrix <double> m_double {nrow, ncol};

            for (int irow = 0; irow < nrow; ++irow) {
                for (int icol = 0; icol < ncol; ++icol) {
                    ASSERT_EQ (m_char. GetValue (irow, icol), char  {});
                    ASSERT_EQ (m_short.GetValue (irow, icol), short {});
                    ASSERT_EQ (m_int.  GetValue (irow, icol), int   {});
                    ASSERT_EQ (m_long. GetValue (irow, icol), long  {});

                    ASSERT_EQ (m_uchar. GetValue (irow, icol), (unsigned char)  {});
                    ASSERT_EQ (m_ushort.GetValue (irow, icol), (unsigned short) {});
                    ASSERT_EQ (m_uint.  GetValue (irow, icol), (unsigned int)   {});
                    ASSERT_EQ (m_ulong. GetValue (irow, icol), (unsigned long)  {});

                    ASSERT_EQ (m_float. GetValue (irow, icol), float  {});
                    ASSERT_EQ (m_double.GetValue (irow, icol), double {});
                }
            }
        }
    }
}

TEST (ConstructTest, InitValRandom) {

    const int ntest_val = 5;
    const int val_min = INT32_MIN,
              val_max = INT32_MAX;

    std::random_device rd;
    std::mt19937 gen (rd ());
    std::uniform_int_distribution<> rand_val (val_min, val_max);

    // Iteration over values ---------------------------------------------
    ITERATION_RAND_ROW (500, ntest_val)
        ITERATION_RAND_COL (500, ntest_val)
            ITERATION_RAND_VAL (int, INT32_MIN, INT32_MAX, 20)

                // Test correct init -------------------------------------
                genmx::Matrix <double> m {nrow, ncol, val};
                for (int irow = 0; irow < nrow; ++irow)
                    for (int icol = 0; icol < ncol; ++icol)
                        ASSERT_EQ (val, m.GetValue (irow, icol));

            }
        }
    }

}

TEST (ConstructTest, CopyTest_Negative) {
    std::random_device rd;
    std::mt19937 gen (rd ());
    
    // Iteration over values ---------------------------------------------
    ITERATION_RAND_ROW (500, 5)
        ITERATION_RAND_COL (400, 5)
            ITERATION_RAND_VAL (int, INT32_MIN, INT32_MAX, 10)

                // Test correct init -------------------------------------
                genmx::Matrix <double> m {nrow, ncol, val};
                genmx::Matrix <double> n = -m;
                
                for (int irow = 0; irow < nrow; ++irow)             // Negative
                    for (int icol = 0; icol < ncol; ++icol)
                        ASSERT_EQ (
                            m.GetValue (irow, icol), -n.GetValue (irow, icol)
                        );
            
            }
        }
    }
}


bool EpsEqual (double lhs, double rhs, double eps = 1e-3) {
    return fabs (lhs - rhs) <= eps;
}

TEST (DeterminantTest, RandExamples) {
    std::random_device rd;
    std::mt19937 gen (rd ());
    
    // Iteration over values ---------------------------------------------
    ITERATION_RAND_ROW (10, 100)
        genmx::Matrix <double> m {nrow, nrow, -10, 10, genmx::RAND_FILL::RAND};
        
        double fastDet = m.Determinant ();
        double simpleDet = m.Determinant_Simple ();
        
        ASSERT_TRUE (EpsEqual (fastDet, simpleDet, 1e-2)) << m
        << "m.Determinant (): "        << fastDet   << std::endl
        << "m.Determinant_Simple (): " << simpleDet << std::endl
        << "m.Determinant (): "        << m.Determinant ()   << std::endl
        << "m.Determinant_Simple (): " << m.Determinant_Simple () << std::endl; 
    }
}

TEST (DeterminantTest, Examples) {
    using M = genmx::Matrix <double>;

    for (int size = 1; size < 200; ++size) {
        M zero {size, size};
        ASSERT_TRUE (EpsEqual (0, zero.Determinant ()));

        auto I = M::eye (size);
        ASSERT_TRUE (EpsEqual (1, I.Determinant ()));
        
        I *= 2;
        ASSERT_TRUE (EpsEqual (pow (2, size), I.Determinant ()));
    }
}

int main (int argc, char* argv[]) {
    ::testing::InitGoogleTest (&argc, argv);
    return RUN_ALL_TESTS ();
}