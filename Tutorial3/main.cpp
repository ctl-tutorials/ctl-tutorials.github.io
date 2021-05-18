//#include <QApplication>
//#include <ctl_qtgui.h>
#include <mat/mat.h>
#include <iostream>

using namespace CTL;

void tutorial3a();
void tutorial3b();
void tutorial3c();

int main(int argc, char *argv[])
{
    //QApplication a(argc, argv);

    //mat::Matrix<4, 32> m;
    //std::iota(m.begin(), m.end(), 0);
    //gui::plot(m);

    //tutorial3a();
    //tutorial3b();
    tutorial3c();

    //return a.exec();
}

void tutorial3c()
{
    // helper functions
    auto I = mat::eye<5>();
    auto D = mat::diag<4,6>({1,2,3,4});
    auto d = mat::diag(D);
    auto D2 = mat::diag<4,10>(d);
    auto R = mat::rotationMatrix({1,0,0});
    auto r = mat::rotationAxis(R);
    auto a = mat::rotationAngle(R);

    std::cout << I.info("eye ")
              << D.info("D ")
              << d.info("diag ")
              << D2.info("D2 ")
              << R.info("R ")
              << r.info("axis ")
              << a << " rad, " << a * 180/PI << " deg\n"
              << mat::det(R) << '\n'
                 ;

    // concatenation
    auto V = mat::vertcat(R, r.transposed());
    auto H = mat::horzcat(R, r, r, r, r, r); // C++14 only

    std::cout << V.info("V ")
              << H.info("H ");
}

void tutorial3b()
{
    mat::Matrix<2, 3> m(0);

    m(0,0) = -1; // no checks
    m.at(0,1) = -2; // run time check
    m.get<0,2>() = 3 * m.get<0,0>(); // compile time check

    //auto val = m[0,1]; // m[1] BAD!
    auto val = m[0][1]; // okay
    std::copy_n(m[0], 3, m[1]);

    auto row = m.row<0>();

    std::cout << m.info()
              << row.info("row ")
              << row.column<1>() << '\n'
              << row.subMat<0,1>().info("subMat ")
              << row.subMat<2,0>().info("reverse ")
              << m.subMat<0,1, 2,0>().info("fliprl ")
                 ;
}

void tutorial3a()
{
    mat::Matrix<1,3> v(1.0);
    mat::Matrix<2,3> m(1, 2, 3,
                       4, 5, 6);

    m.SEPARATOR_CHARACTER_FOR_INFO_STRING = ' ';

    std::cout << v.info("vector ") << '\n'
              << m.info("matrix ");

    // operations
    auto prod = m * v.transposed();
    std::cout << prod.info("prod ");

    auto m2 = 1.35 * m / 1.35;// + m;
    std::cout << m2.info("m2 ")
              << std::boolalpha
              << (m2 == m) << '\n'
              << (m2 - m).reshapedAsVector().norm() << '\n';
                 ;
}
